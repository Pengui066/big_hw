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

expression *Parser::parseExpr()
{
    expression *expr1 = parseExpr1(); // 它应该从流里面吞掉一个完整的表达式1，保证下一个token是operator
    Token tk1 = lexer.getNextToken();
    if (is_cmpOP(tk1.opType))
    {
        expression* expr2 = parseExpr1(); // 比较表达式的右侧运算分量
        Token tk2 = lexer.getNextToken();
        //! 规定中一个比较表达式求值似乎不能单独出现
        //! 所以a = (1+2 == 3)?1:2;的写法是非法的
        //todo: 在函数指针版本的parseExpr里面把这个改掉，让比较表达式可以单独出现，并且让三目的条件里面可以方任何表达式
        if (tk2.tkType != QUESTIONMARK) {
            std::cerr << "error: form func \"parseExpr\", missing expected questionMark. " << std::endl;
            ASSERT(0);
            return nullptr;
        }
        expression* branch1 = parseExpr1();
        tk2 = lexer.getNextToken();
        if (tk2.tkType != COLON) {
            std::cerr << "error: form func \"parseExpr\", missing expected colon. " << std::endl;
            ASSERT(0);
            return nullptr;
        }
        expression* branch2 = parseExpr1();
        expression* cmpExp = new dosExp(expr1, expr2, tk1.opType);
        expression* condExp = new tresExp(cmpExp, branch1, branch2);
        return condExp; // 后置条件：在TokenBuffer里面留下一个非“加减乘除模”的Token，这个Token很可能是“右括号”或者“分号”
    }
    else { return expr1; } // 读完一个完整的表达式1后返回的后置条件：在TokenBuffer里面留下一个非比较符号的Token，当然也可以知道这个符号非“加减乘除模”
}

expression *Parser::parseExpr1()
{
    expression *expr = parseExpr2();
    Token nextTk = lexer.getNextToken();
    while(nextTk.opType == PLUS || nextTk.opType == MINUS) /* nextTk is "+" or "-" */
    {
        expression *expr2 = parseExpr2();
        // 根据expr nextTk expr2构造得到的加法、减法表达式
        expr = new dosExp(expr, expr2, nextTk.opType);
        nextTk = lexer.getNextToken();
    }
    return expr; // 后置条件：在TokenBuffer里面留下一个非“加减”的Token，当然可以推断出来这个Token也非“乘除模”
}

expression *Parser::parseExpr2()
{
    expression *expr = parseExpr3();
    Token nextTk = lexer.getNextToken();
    while(nextTk.opType == TIMES||nextTk.opType == DIVIDEBY||nextTk.opType == MODULUS) /* nextTk is "*" or "/" */
    {
        expression *expr2 = parseExpr3();
        // 根据expr nextTk expr2构造得到的乘法、除法表达式
        expr = new dosExp(expr, expr2, nextTk.opType);
        nextTk = lexer.getNextToken();
    }
    return expr; // 什么时候返回？在扫描到第一个非“*/%”的Token之后，并且这个Token会被存在TokenBuffer里面
}

expression *Parser::parseExpr3()
{
    Token tkAfterOp = lexer.getToken();
    ASSERT(tkAfterOp.opType == MINUS || tkAfterOp.tkType == IDENTIFIER || tkAfterOp.tkType == INTEGER || tkAfterOp.tkType == LBRACKET);
    // TODO: 完善以上四种情况的分支 先不加与或非
    if (tkAfterOp.tkType == IDENTIFIER || tkAfterOp.tkType == INTEGER) { // 标识符或者整数的情况，这种情况TokenBuffer被清空，之所以放前面是因为它的覆盖面最广，可以避免integer值等于5然后被误判为减号的情况
        expression *expr = new baseExp(tkAfterOp);
        return expr;
    }
    if ((tkAfterOp.opType == MINUS)) {
        expression *expr = parseExpr3();
        expr = new unoExp(expr, MINUS);
        return expr;
    }
    if (tkAfterOp.tkType == LBRACKET) { // 括号情况
        expression *expr = parseExpr(); // 吞掉完整表达式，后面会不会在TokenBuffer里面留下有用的信息？
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
