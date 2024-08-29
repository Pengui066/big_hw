#ifndef __PARSER_H__
#define __PARSER_H__

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "statement.h"
#include "expression.h"
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

extern Lexer lexer;

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