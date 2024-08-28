#ifndef __PARSER_H__
#define __PARSER_H__

#include <map>
#include <string>
#include <vector>
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

class statement // statement 虚基类
{
public:
    int x = UNFINISHED; // 需要一个用来生成汇编代码的接口
    virtual void execute() = 0;
};

class voidStat: public statement // 空语句
{
public:
    voidStat() {}
    void execute();
};

class func_definition: public statement // 函数定义语句
{
private:
    std::string funcName;
    statement * body;
public:
    func_definition(std::string _name, statement* _body): 
        funcName(_name), body(_body) {}
    void execute();
};

class var_definition: public statement // 变量定义语句
{
private:
    std::string varName;
public:
    var_definition(const char* _varName):
        varName(_varName) {}
    void execute();
};

class ifStat: public statement // 条件语句
{
private:
    expression* condExpr;
    statement* ifBranchStat;
    statement* elseBranchStat;
public:
    ifStat(expression* _cond, statement* _if, statement* _else = nullptr):
        condExpr(_cond), ifBranchStat(_if), elseBranchStat(_else) {}
    void execute();
};

class whileStat: public statement // 循环语句
{
private:
    expression* condExpr;
    statement* bodyStat;
public:
    whileStat(expression* _cond, statement* _body):
        condExpr(_cond), bodyStat(_body) {}
    void execute();
};

class compoundStat: public statement // 复合语句
{
private:
    std::vector<statement*> statList;
public: 
    void addStatement(statement* _statPtr);
    void execute();
};

class assignment: public statement // 赋值语句
{
private:
    std::string varName;
    expression* exp;
public:
    assignment(expression* _exp, const std::string& _name): 
        varName(_name), exp(_exp) {}
    void execute();
};

class funcCall: public statement // 函数调用语句
{
private:
    std::string funcName;
public:
    funcCall(const std::string& _name):
        funcName(_name) {}
    void execute();
};

class outputStat: public statement // 输出语句
{
private:
    expression* exp;
public:
    outputStat(expression* _exp):
        exp(_exp) {}
    void execute();
};

class inputStat: public statement // 输入语句
{
private:
    std::string varName;
public:
    inputStat(const char* _name): // 使用常量字符指针来初始化string
        varName(_name) {}
    void execute();
};

class returnStat: public statement // 返回语句
{
private:
    expression* expr;
public:
    returnStat(expression *_expr):
        expr(_expr) {}
    void execute();
};

class Parser
{
public:
    expression* parseTresExpr();
    expression* parseDosExpr(int SEQNUM); // 接受一个参数用来表示当前二元表达式的优先层级
    expression* parseUnoExpr();
    statement* parseFuncDef(); // 分析函数定义类型的语句
    statement* parseVarDef(); // 分析变量定义类型的语句（变量定义只能在全局区域或者函数定义的语句块之内）
    statement* parseNotDefStat(); // 分析复合语句
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