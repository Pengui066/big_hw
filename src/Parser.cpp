#include "Parser.h"

Lexer lexer;

// 后置条件：在TokenBuffer里面留下一个非operator的Token，这个Token很可能是“右括号”或者“分号”
// ------
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
        return condExp;
    }
    else { return expr1; } 
}

// SEQNUM代表当前parse的表达式的运算优先级，数字越小优先级越高
// 递归地parse优先级更高的表达式
// ------
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
    if (tkAfterOp.tkType == INTEGER) { // 标识符或者整数的情况，这种情况TokenBuffer被清空，之所以放前面是因为它的覆盖面最广，可以避免integer值等于5然后被误判为减号的情况
        expression *expr = new baseExp(tkAfterOp);
        return expr;
    }
    if (tkAfterOp.tkType == IDENTIFIER) {
        Token expcLbracket = lexer.getNextToken();
        expression *expr;
        if (expcLbracket.tkType == LBRACKET) {
            lexer.getTk_ensureType(RBRACKET);
            expr = new baseExp(tkAfterOp, 1);
        } else {
            expr = new baseExp(tkAfterOp, 0); }
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
        if (eqID(firstTk, "def")) { // 函数定义语句
            Token funcName = lexer.getTk_ensureType(IDENTIFIER);
            lexer.getTk_ensureType(LBRACKET);
            lexer.getTk_ensureType(RBRACKET);
            lexer.getTk_ensureType(LBRACE);
            func_definition* funcDefPtr = new func_definition(funcName.idName);
            
            Token expcVar = lexer.getNextToken();
            unsigned int idxNum = 0;
            while (eqID(expcVar, "var")) {
                Token expectedVarName = lexer.getTk_ensureType(IDENTIFIER);
                lexer.getTk_ensureType(SEMICOLON);
                funcDefPtr->addLocVar(expectedVarName.idName, idxNum);
                idxNum++;
                expcVar = lexer.getNextToken();
            } funcDefPtr->setLocVarNum(idxNum);

            Token expcRbrace = lexer.getNextToken();
            while (expcRbrace.tkType != RBRACE) {
                statement* statPtr = parseNotDefStat();
                funcDefPtr->addStatment(statPtr);
                expcRbrace = lexer.getNextToken();
            }

            lexer.clearTokenBuffer();
            return funcDefPtr;
        }
    }
    RaiseErr(WRONG_GRAMMAR, "'def'", firstTk.nameItself());
    return nullptr;
}

statement* Parser::parseVarDef()
{
    Token firstTk = lexer.getNextToken();
    if (eqID(firstTk, "var")) { // 变量定义语句
    Token expectedVarName = lexer.getTk_ensureType(IDENTIFIER);
    lexer.getTk_ensureType(SEMICOLON);
    return new var_declaration(expectedVarName.idName);
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
            if (!eqID(expectedElse.idName, "else")) {
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
            expression* expr = parseTresExpr();
            lexer.getNxtTk_ensureType(SEMICOLON);
            return new returnStat(expr);
        }
        Token SecondTk = lexer.getToken();
        if (SecondTk.tkType == LBRACKET) {
            lexer.getTk_ensureType(RBRACKET);
            lexer.getTk_ensureType(SEMICOLON);
            return new funcCall(firstTk.idName);
        }
        if (SecondTk.tkType == EVA) {
            expression* expr = parseTresExpr();
            lexer.getNxtTk_ensureType(SEMICOLON);
            return new assignment(expr, firstTk.idName);
        }
        RaiseErr(WRONG_GRAMMAR, "wrong input, neither function call, nor variable assignment, but found " + SecondTk.nameItself() + " after " + firstTk.nameItself());
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
    else if (firstTk.tkType == SEMICOLON) {
        lexer.clearTokenBuffer();
        return new voidStat;
    }
    RaiseErr(WRONG_GRAMMAR, "statements or Lbrace or semicolon", firstTk.nameItself());
    return nullptr;
}

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