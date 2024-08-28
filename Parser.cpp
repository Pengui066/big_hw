#include "Parser.h"

std::map<std::string, int> varMap; // 用来记录变量
std::map<std::string, int> funcList; 

Lexer lexer;

void assignment::execute()
{
    varMap[varName] = exp->evaluate(); 
    // 每个expression *exp 指向的都是expression类的子类对象，每个evaluate函数的执行方式都不一样，体现一种多态
    delete exp;
}

void outputStat::execute()
{
    std::cout << exp->evaluate() << std::endl;
    delete exp;
}

expression *Parser::parseTresExpr()
{
    expression *expr1 = parseDosExpr(6); // 它应该从流里面吞掉一个完整的表达式1，保证下一个token是operator
    Token tk1 = lexer.getNextToken();
    if (tk1.tkType == QUESTIONMARK) {
        expression* branch1 = parseDosExpr(6);
        Token tk2 = lexer.getNextToken();
        if (tk2.tkType != COLON) {
            std::string errMessage = "from ternary expression, missing expected colon, but found ";
            errMessage += tk2.nameItself();
            RaiseErr(WRONG_GRAMMAR, errMessage);
            return nullptr;
        }
        expression* branch2 = parseDosExpr(6);
        expression* condExp = new tresExp(expr1, branch1, branch2);
        return condExp; // 后置条件：在TokenBuffer里面留下一个非operator的Token，这个Token很可能是“右括号”或者“分号”
    }
    else { return expr1; } 
}

// SEQNUM代表当前parse的表达式的运算优先级，数字越小优先级越高
// 递归地parse优先级更高的表达式
expression* Parser::parseDosExpr(int SEQNUM) { 
    ASSERT(SEQNUM >=0 && SEQNUM <= 6);
    if (SEQNUM > 0 && SEQNUM <= 6) {
        expression *expr = parseDosExpr(SEQNUM - 1);
        Token nextTk = lexer.getNextToken();
        while(judgeFuncList[SEQNUM](nextTk))
        {
            expression *expr2 = parseDosExpr(SEQNUM - 1);
            // 根据expr nextTk expr2构造得到二元表达式
            expr = new dosExp(expr, expr2, nextTk.opType);
            nextTk = lexer.getNextToken();
        }
        return expr; // return的后置条件：下一个operator不是当前层级的运算符
    }
    if (SEQNUM == 0) {
        return parseUnoExpr();
    }
    return nullptr;
}

expression *Parser::parseUnoExpr()
{
    Token tkAfterOp = lexer.getToken();
    ASSERT(tkAfterOp.opType == MINUS || tkAfterOp.tkType == IDENTIFIER || tkAfterOp.tkType == INTEGER || tkAfterOp.tkType == LBRACKET || tkAfterOp.opType == NOT);
    if (tkAfterOp.tkType == IDENTIFIER || tkAfterOp.tkType == INTEGER) { // 标识符或者整数的情况，这种情况TokenBuffer被清空，之所以放前面是因为它的覆盖面最广，可以避免integer值等于5然后被误判为减号的情况
        expression *expr = new baseExp(tkAfterOp);
        return expr;
    }
    if (tkAfterOp.opType == MINUS) {
        expression *expr = parseUnoExpr();
        expr = new unoExp(expr, MINUS);
        return expr;
    }
    if (tkAfterOp.opType == NOT) {
        expression* expr = parseUnoExpr();
        expr = new unoExp(expr, NOT);
        return expr;
    }
    if (tkAfterOp.tkType == LBRACKET) { // 括号情况
        expression *expr = parseTresExpr(); // 吞掉完整表达式，后面会不会在TokenBuffer里面留下有用的信息？
        Token potentialRBracket = lexer.getNextToken();
        lexer.clearTokenBuffer();
        ASSERT(potentialRBracket.tkType == RBRACKET);
        return expr;
    }
    ASSERT(0);
    return nullptr;
}

statement* Parser::parseFuncDef()
{
    Token firstTk = lexer.getNextToken();
    if (firstTk.tkType == IDENTIFIER) {
        if (!(strcmp(firstTk.idName, "def"))) { // 函数定义语句
            Token funcName = lexer.getTk_ensureType(IDENTIFIER);
            lexer.getTk_ensureType(LBRACKET);
            lexer.getTk_ensureType(RBRACKET);
            statement* body = UNFINISHED;
            return new func_definition(funcName.idName, body);
        }
    }
    RaiseErr(WRONG_GRAMMAR, "'def'", firstTk.nameItself());
    return nullptr;
}

statement* Parser::parseVarDef()
{
    Token firstTk = lexer.getNextToken();
    if (firstTk.tkType != IDENTIFIER || strcmp(firstTk.idName, "var")) { // 变量定义语句
    Token expectedVarName = lexer.getTk_ensureType(IDENTIFIER);
    lexer.getTk_ensureType(SEMICOLON);
    return new var_definition(expectedVarName.idName);
    }
    RaiseErr(WRONG_GRAMMAR, "'var'", firstTk.nameItself());
    return nullptr;
}

statement* Parser::parseNotDefStat() // 读取分析不是变量声明/函数定义的语句
{
    Token firstTk = lexer.getNextToken();
    if (firstTk.tkType == IDENTIFIER) {
        if (!(strcmp(firstTk.idName, "if"))) { // 条件语句
            lexer.getTk_ensureType(LBRACKET); // 拿左括号
            expression* condExpr = parseTresExpr(); // 判断条件表达式，后置条件是在TokenBuffer里面留下一个右括号
            lexer.getNxtTk_ensureType(RBRACKET); // 拿右括号
            statement* ifBranch = parseNotDefStat(); // if分支
            Token expectedElse = lexer.getNextToken(); 
            if (expectedElse.tkType != IDENTIFIER || strcmp(expectedElse.idName, "else")) {
                statement* elseBranch = new voidStat;
                return new ifStat(condExpr, ifBranch, elseBranch); // 后置条件：在TokenBuffer里面留下一个非else的Token
            }
            lexer.clearTokenBuffer();
            statement* elseBranch = parseNotDefStat(); // 后置条件视情况而定
            return new ifStat(condExpr, ifBranch, elseBranch);
        }
        if (!(strcmp(firstTk.idName, "while"))) { // 循环语句
            lexer.getTk_ensureType(LBRACKET); // 拿左括号
            expression* condExpr = parseTresExpr(); // 循环条件，后置条件是在TokenBuffer里面留下一个右括号
            lexer.getNxtTk_ensureType(RBRACKET); // 拿右括号
            statement* bodyStat = parseNotDefStat(); // 后置条件视情况而定
            return new whileStat(condExpr, bodyStat);
        }
        if (!(strcmp(firstTk.idName, "output"))) { // 输出语句
            expression* expr = parseTresExpr();
            lexer.getNxtTk_ensureType(SEMICOLON); // getNxtTk_ensureType之后默认清空TokenBuffer，所以后置条件是在TokenBuffer留下NONETK
            return new outputStat(expr);
        }
        if (!(strcmp(firstTk.idName, "input"))) { // 输入语句
            Token expcIdTk = lexer.getTk_ensureType(IDENTIFIER);
            lexer.getTk_ensureType(SEMICOLON); // 后置条件TokenBuffer是NONETK
            return new inputStat(expcIdTk.idName);
        }
        if (!(strcmp(firstTk.idName, "return"))) { // 返回语句

        }
        Token SecondTk = lexer.getToken();
        if (SecondTk.tkType == LBRACKET) {
            lexer.getTk_ensureType(RBRACKET);
            lexer.getTk_ensureType(SEMICOLON);
            auto it = funcList.find(firstTk.idName);
            if (it == funcList.end()) RaiseErr(UNDEFINED_FUNC, firstTk.idName);
            return new funcCall(firstTk.idName); int x = UNFINISHED;
        }
        if (SecondTk.tkType == EVA) {
            auto it = varMap.find(firstTk.idName);
            if (it == varMap.end()) RaiseErr(UNDEFINED_VAR, firstTk.idName);
            expression* expr = parseTresExpr();
            lexer.getNxtTk_ensureType(SEMICOLON);
            return new assignment(expr, firstTk.idName);
        }
        RaiseErr(WRONG_GRAMMAR, "wrong input, neither function call, nor variable assignment. ");
    }
    else if (firstTk.tkType == LBRACE) {
        lexer.clearTokenBuffer();
        compoundStat* compoundStatPtr = new compoundStat; // 先创建一个复合语句的对象
        do {
            statement* statPtr = parseNotDefStat();
            compoundStatPtr->addStatement(statPtr);
        } while (lexer.getNextToken().tkType != RBRACE); // 退出的时候TokenBuffer留一个右大括号
        lexer.clearTokenBuffer(); // 后置条件：TokenBuffer置空
        return compoundStatPtr;
    }
    RaiseErr(WRONG_GRAMMAR, "statement identifiers or '{'", firstTk.nameItself());
    return nullptr;
}

int unoExp::evaluate()
{
    if (this->op == NONEOP) return Opnd->evaluate();
    if (this->op == MINUS) return -Opnd->evaluate();
    if (this->op == NOT) return !(Opnd->evaluate());
    ASSERT(0);
    return 0;
}

unoExp::~unoExp()
{
    delete this->Opnd;
}

int dosExp::evaluate()
{
    if (this->op == PLUS) return ((lOpnd->evaluate()) + (rOpnd->evaluate()));
    if (this->op == MINUS) return ((lOpnd->evaluate()) - (rOpnd->evaluate()));
    if (this->op == TIMES) return ((lOpnd->evaluate()) * (rOpnd->evaluate()));
    if (this->op == DIVIDEBY) return ((lOpnd->evaluate()) / (rOpnd->evaluate()));
    if (this->op == MODULUS) return ((lOpnd->evaluate()) % (rOpnd->evaluate()));
    if (this->op == EQUAL) return ((lOpnd->evaluate()) == (rOpnd->evaluate()));
    if (this->op == GREATER) return ((lOpnd->evaluate()) > (rOpnd->evaluate()));
    if (this->op == LESS) return ((lOpnd->evaluate()) < (rOpnd->evaluate()));
    if (this->op == AND) return ((lOpnd->evaluate()) && (rOpnd->evaluate()));
    if (this->op == OR) return ((lOpnd->evaluate()) || (rOpnd->evaluate()));
    ASSERT(0); // unknown operator in dosExp.
    return 0;
}

dosExp::~dosExp()
{
    delete this->lOpnd;
    delete this->rOpnd;
}

int tresExp::evaluate()
{
    if(Opnd1->evaluate()) return Opnd2->evaluate(); 
    return Opnd3->evaluate(); 
}

tresExp::~tresExp()
{
    delete this->Opnd1;
    delete this->Opnd2;
    delete this->Opnd3;
}

baseExp::baseExp(Token _tk)
{
    ASSERT(_tk.tkType == IDENTIFIER || _tk.tkType == INTEGER);
    baseTk = _tk;
}

int baseExp::evaluate()
{
    ASSERT(this->baseTk.tkType == IDENTIFIER || this->baseTk.tkType == INTEGER);
    if (this->baseTk.tkType == IDENTIFIER) {
        auto it = varMap.find(baseTk.idName);
        if (it == varMap.end()) varMap[baseTk.idName] = 0; //! 没找到就分配为0，后面得改！！！
        return varMap[baseTk.idName];
    }
    if (this->baseTk.tkType == INTEGER) {
        return this->baseTk.intValue;
    }
    return 0;
}

baseExp::~baseExp() {}

expression::~expression() {}

inline bool isL0op(const Token& _tk) { // - !
    return (_tk.tkType == OPERATOR) && (_tk.opType == MINUS || _tk.opType == NOT);
}
inline bool isL1op(const Token& _tk) { // * / %
    return (_tk.tkType == OPERATOR) && (_tk.opType == TIMES || _tk.opType == DIVIDEBY || _tk.opType == MODULUS);
}
inline bool isL2op(const Token& _tk) { // + -
    return (_tk.tkType == OPERATOR) && (_tk.opType == PLUS || _tk.opType == MINUS);
}
inline bool isL3op(const Token& _tk) { // > <
    return (_tk.tkType == OPERATOR) && (_tk.opType == GREATER || _tk.opType == LESS);
}
inline bool isL4op(const Token& _tk) { // ==
    return (_tk.tkType == OPERATOR) && (_tk.opType == EQUAL);
}
inline bool isL5op(const Token& _tk) { // &&
    return (_tk.tkType == OPERATOR) && (_tk.opType == AND);
}
inline bool isL6op(const Token& _tk) { // ||
    return (_tk.tkType == OPERATOR) && (_tk.opType == OR);
}

judgeOpFP judgeFuncList[] = {isL0op, isL1op, isL2op, isL3op, isL4op, isL5op, isL6op};

void func_definition::execute()
{
}

void var_definition::execute()
{
}

void ifStat::execute()
{
}

void whileStat::execute()
{
}

void compoundStat::addStatement(statement *_statPtr)
{
    statList.push_back(_statPtr);
}

void compoundStat::execute()
{
}

void inputStat::execute()
{
}

void returnStat::execute()
{
}

void voidStat::execute()
{
}

void funcCall::execute()
{
}
