#include "statement.h"

MEM_manager memManager;

int assignment::execute() // 赋值语句的执行
{
    int pos = memManager.findPos(this->varName);
    mem[pos] = exp->evaluate();
    // 每个expression *exp 指向的都是expression类的子类对象，每个evaluate函数的执行方式都不一样，体现一种多态
    // delete exp; // 这里不能直接析构，因为表达式可能会二次调用，比如在while循环当中，同一个指针指向的表达式会多次求值
    return 0;
}

int outputStat::execute() // 输出语句的执行
{
    std::cout << exp->evaluate() << std::endl;
    return 0;
}

int func_definition::execute() // 函数定义
{
    FuncBody* funcbody = new FuncBody;
    funcbody->LocVarMap = this->LocVarMap;
    funcbody->statList = this->statList;
    funcbody->LocVarNum = this->LocVarNum;
    funcList[this->funcName] = funcbody;
    return 0;
}

int var_declaration::execute() // 变量声明语句（并初始化为0）
{
    if (!memManager.funcCallStk.empty()) { // 当前函数调用栈非空，说明正有函数在调用
        int pos = memManager.findPos(this->varName);
        mem[pos] = 0;
    } else {
        globVarMap[this->varName] = memManager.globVarPtr;
        memManager.globVarPtr++;
        int pos = globVarMap[this->varName];
        mem[pos] = 0;
    }
    return 0;
}

int ifStat::execute() // 条件语句
{
    int retStatus;
    if (condExpr->evaluate()) {
        retStatus = ifBranchStat->execute(); 
        if (retStatus == 1) {
            return 1;
        }
    }
    else {
        retStatus = elseBranchStat->execute(); 
        if (retStatus == 1) {
            return 1;
        }
    }
    return 0;
}

int whileStat::execute() // 循环语句
{
    int retStatus;
    while (condExpr->evaluate()) {
        retStatus = bodyStat->execute(); 
        if (retStatus == 1) {
            return 1;
        }
    }
    return 0;
}

void compoundStat::addStatement(statement *_statPtr)
{
    statList.push_back(_statPtr);
}

int compoundStat::execute() // 复合语句
{
    int retStatus;
    for (auto stat : this->statList) {
        retStatus = stat->execute();
        if (retStatus == 1) {
            return 1;
        }
    }
    return 0;
}

int inputStat::execute() // 输入语句的执行
{
    int pos = memManager.findPos(this->varName);
    std::cin >> mem[pos];
    return 0;
}

int returnStat::execute() // 返回指令
{
    if (memManager.funcCallStk.empty()) RaiseErr(RUN_ERROR, "cannot return outside of function definition. ");
    mem[memManager.SP-memManager.funcCallStk.top()->LocVarNum] = this->expr->evaluate(); //求值并放入返回位置
    memManager.EndFuncCall();
    return 1;
}

int voidStat::execute() // 空指令
{
    // do nothing
    return 0;
}

int funcCall::execute() // 函数调用指令
{
    auto it = funcList.find(funcName);
    if (it == funcList.end()) RaiseErr(UNDEFINED_FUNC, funcName); // 寻找当前函数是否定义
    // 如果定义，拿到当前函数代码的结构位置
    int retStatus;
    FuncBody* curFuncBodyPtr = funcList[funcName];
    memManager.newFuncCall(curFuncBodyPtr); // 初始化，移动栈顶指针
    for (auto stat : curFuncBodyPtr->statList ) {
        retStatus = stat->execute();
        if (retStatus == 1) break;
    }
    if (retStatus == 0) {
    memManager.EndFuncCall(); } // 函数调用结束之后善后，移回栈顶指针，并把当前函数调用置空
    return 0;
}
