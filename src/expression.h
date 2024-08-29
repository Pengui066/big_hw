#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "Lexer.h"
#include "MEM_manager.h"

class expression // expression 虚基类
{
public:
    virtual int evaluate() = 0;
    virtual ~expression() = 0;
};

class unoExp: public expression // 单目运算表达式
{
private:
    OPTYPE op;
    expression* Opnd;
public:
    unoExp(expression* _Opnd, OPTYPE _op): op(_op), Opnd(_Opnd) {}
    int evaluate();
    ~unoExp();
};

class dosExp: public expression // 二目运算表达式
{
private:
    OPTYPE op;
    expression *lOpnd, *rOpnd;
public:
    dosExp(expression* _lOpnd, expression* _rOpnd, OPTYPE _op):
        op(_op), lOpnd(_lOpnd), rOpnd(_rOpnd) {}
    int evaluate();
    ~dosExp();
};

class tresExp: public expression // 三目运算表达式
{
private:
    expression *Opnd1, *Opnd2, *Opnd3;
public:
    tresExp(expression* _Opnd1, expression* _Opnd2, expression* _Opnd3): 
        Opnd1(_Opnd1), Opnd2(_Opnd2), Opnd3(_Opnd3) {}
    int evaluate();
    ~tresExp();
};

class baseExp: public expression // 基本表达式
{
private:
    Token baseTk;
public:
    bool isFunc;
    baseExp(Token tk, bool _isFunc = 0);
    int evaluate();
    ~baseExp();
};

#endif