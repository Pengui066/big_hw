#ifndef __PARSER_H__
#define __PARSER_H__

#include <map>
#include <string>
#include <iostream>
#include "Lexer.h"

/*
运算符优先级：
* ()
* - !
* * / %
* + -
* > <
* ==
* &&
* ||
* ?:
* =
*/

extern std::map<std::string, int> varMap; // 用来记录
extern Lexer lexer;

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
    baseExp(Token tk);
    int evaluate();
    ~baseExp();
};

class Parser
{
public:
    expression* parseTresExpr();
    expression* parseDosExpr(int SEQNUM); // 接受一个参数用来表示当前二元表达式的优先层级
    expression* parseUnoExpr();
};

class statement // statement 虚基类
{
public:
    virtual void execute() = 0;
};

class assignment: public statement // 赋值语句
{
private:
    std::string varName;
    expression* exp;
public:
    assignment(expression* _exp, std::string _name): 
        varName(_name), exp(_exp) {}
    void execute();
};

class output: public statement // 输出语句
{
private:
    expression* exp;
public:
    output(expression* _exp):
        exp(_exp) {}
    void execute();
};

inline bool isL0op(const Token& _tk); // - !
inline bool isL1op(const Token& _tk); // * / %
inline bool isL2op(const Token& _tk); // + -
inline bool isL3op(const Token& _tk); // > <
inline bool isL4op(const Token& _tk); // ==
inline bool isL5op(const Token& _tk); // &&
inline bool isL6op(const Token& _tk); // ||

typedef bool (*judgeOpFP) (const Token&);
extern judgeOpFP judgeFuncList[];
#endif