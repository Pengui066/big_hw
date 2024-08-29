#ifndef __STATEMENT_H__
#define __STATEMENT_H__

#include <string>
#include <vector>
#include "expression.h"

extern MEM_manager memManager;

class statement // statement 虚基类
{
public:
    virtual int execute() = 0;
};

class voidStat: public statement // 空语句
{
public:
    voidStat() {}
    int execute();
};

class func_definition: public statement // 函数定义语句
{
private:
    std::string funcName;
    std::vector<statement*> statList;
    unsigned int LocVarNum;
    std::map<std::string, unsigned int> LocVarMap;
public:
    func_definition(const std::string& _name): 
        funcName(_name) {}
    void addLocVar(const std::string& _varName, unsigned int _idxNum) {
        LocVarMap[_varName] = _idxNum;
    }
    void addStatment(statement* _statPtr) {
        statList.push_back(_statPtr);
    }
    void setLocVarNum(int _num) {
        LocVarNum = _num;
    }
    int execute();
};

class var_declaration: public statement // 变量声明语句
{
private:
    std::string varName;
public:
    var_declaration(const char* _varName):
        varName(_varName) {}
    int execute();
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
    int execute();
};

class whileStat: public statement // 循环语句
{
private:
    expression* condExpr;
    statement* bodyStat;
public:
    whileStat(expression* _cond, statement* _body):
        condExpr(_cond), bodyStat(_body) {}
    int execute();
};

class compoundStat: public statement // 复合语句
{
private:
    std::vector<statement*> statList;
public: 
    void addStatement(statement* _statPtr);
    int execute();
};

class assignment: public statement // 赋值语句
{
private:
    std::string varName;
    expression* exp;
public:
    assignment(expression* _exp, const std::string& _name): 
        varName(_name), exp(_exp) {}
    int execute();
};

class funcCall: public statement // 函数调用语句
{
private:
    std::string funcName;
public:
    funcCall(const std::string& _name):
        funcName(_name) {}
    int execute();
};

class outputStat: public statement // 输出语句
{
private:
    expression* exp;
public:
    outputStat(expression* _exp):
        exp(_exp) {}
    int execute();
};

class inputStat: public statement // 输入语句
{
private:
    std::string varName;
public:
    inputStat(const char* _name): // 使用常量字符指针来初始化string
        varName(_name) {}
    int execute();
};

class returnStat: public statement // 返回语句
{
private:
    expression* expr;
public:
    returnStat(expression *_expr):
        expr(_expr) {}
    int execute();
};


#endif