#include "Parser.h"

std::map<std::string, int> varMap; // 用来记录

Lexer lexer;

void assignment::execute()
{
    varMap[varName] = exp->evaluate(); 
    // 每个expression *exp 指向的都是expression类的子类对象，每个evaluate函数的执行方式都不一样，体现一种多态
    delete exp;
}

void output::execute()
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
            std::cerr << "error: form func \"parseExpr\", missing expected colon. " << std::endl;
            ASSERT(0);
            return nullptr;
        }
        expression* branch2 = parseDosExpr(6);
        expression* condExp = new tresExp(expr1, branch1, branch2);
        return condExp; // 后置条件：在TokenBuffer里面留下一个非operator的Token，这个Token很可能是“右括号”或者“分号”
    }
    else { return expr1; } // 读完一个完整的表达式1后返回的后置条件：在TokenBuffer里面留下一个非比较符号的Token，当然也可以知道这个符号非“加减乘除模”
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
    // TODO: 完善以上四种情况的分支 先不加与或非
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
    std::cerr << "error: from func \"dosExp::evaluate()\", unknown operator in dosExp. " << std::endl;
    ASSERT(0);
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
