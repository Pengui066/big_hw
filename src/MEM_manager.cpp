#include "MEM_manager.h"

std::map<std::string, int> globVarMap; // 用来记录全局变量和它们的地址之间的映射关系
std::map<std::string, FuncBody*> funcList; // 用来记录函数和它们对应的代码块Struct指针之间的映射关系
int mem[MAX_MEM_SIZE];

void MEM_manager::newFuncCall(FuncBody* _curFuncBodyPtr)
{
    funcCallStk.push(_curFuncBodyPtr); 
    unsigned int numOfLocalVars = _curFuncBodyPtr->LocVarNum; // 获取函数局部变量的数量
    if (SP + numOfLocalVars + 1 > StackTop) { // 栈溢出检查
        RaiseErr(STACK_OVERFLOW, "stack overflows. "); return; } 
    SP += numOfLocalVars + 1; // 把原先栈顶指针往上移动numOfLocalVars+1个位置
    return;
}
void MEM_manager::EndFuncCall()
{
    ASSERT(!this->funcCallStk.empty());
    FuncBody* curFuncBodyPtr = this->funcCallStk.top();
    SP -= curFuncBodyPtr->LocVarNum + 1;
    this->funcCallStk.pop();
}

int MEM_manager::findPos(std::string varName)
{
    if (!this->funcCallStk.empty()) { // 如果当前有函数正在调用
        FuncBody* curFuncBodyPtr = funcCallStk.top();
        auto it = curFuncBodyPtr->LocVarMap.find(varName); // 先在局部变量里面找
        if (it == curFuncBodyPtr->LocVarMap.end()) { // 在局部变量当中找不到
            auto it = globVarMap.find(varName); // 去全局变量里面找
            if (it == globVarMap.end()) 
                { RaiseErr(UNDEFINED_VAR, varName); return 0; } // 全局变量里面也找不到，抛出错误
            else { return globVarMap[varName]; } // 返回全局变量中的位置
        }
        else { return SP - (curFuncBodyPtr->LocVarMap[varName]); } // 返回局部变量中的位置
    }
    else {
        auto it = globVarMap.find(varName);
        if (it == globVarMap.end()) {
        RaiseErr(UNDEFINED_VAR, varName); return 0; }
        else {
            return globVarMap[varName];
        }
    }
}
