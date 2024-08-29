#ifndef __MEM_MANAGER_H__
#define __MEM_MANAGER_H__

#include <map>
#include <vector>
#include <stack>
#include "errRaiser.h"
#define MAX_MEM_SIZE 10000
#define STACK_BEGIN_POS 1000
#define MAX_STACK_SIZE 1000

/*
MEM[0-999] 全局变量
MEM[1000-1999] 栈区
*/

class statement;

struct FuncBody
{
    std::vector<statement*> statList;
    std::map<std::string, unsigned int> LocVarMap;
    unsigned LocVarNum;
};

extern std::map<std::string, int> globVarMap;
extern std::map<std::string, FuncBody*> funcList; 
extern int mem[MAX_MEM_SIZE];

class MEM_manager
{
private:
    const unsigned StackTop; // 1999
public:
    MEM_manager(): 
        StackTop(STACK_BEGIN_POS+MAX_STACK_SIZE-1),
        SP(STACK_BEGIN_POS), globVarPtr(0) {}
    std::stack<FuncBody*> funcCallStk; // 函数的调用堆栈，按照LIFO顺序存放函数代码Struct的指针
    int SP; // 栈顶指针
    int globVarPtr;
    void newFuncCall(FuncBody* _curFuncBodyPtr);
    void EndFuncCall();
    int findPos(std::string varName);
};

#endif