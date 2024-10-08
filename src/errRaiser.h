#ifndef __ERRRAISER_H__
#define __ERRRAISER_H__

#include<cstdlib>
#include<string>
#include<iostream>

#define UNFINISHED 0
#define ASSERT(x) if (!(x)) __debugbreak()

enum ERRORTYPE
{
INT_OUT_OF_RANGE, INVALID_INPUT, WRONG_GRAMMAR, 
MISSING_RETURN_VAL, MISSING_MAIN_FUNC, UNDEFINED_FUNC, UNDEFINED_VAR,
STACK_OVERFLOW, RUN_ERROR
};

extern const char* errLib[];

void RaiseErr(ERRORTYPE _type, const std::string& message);
void RaiseErr(ERRORTYPE _type, const std::string& expected, std::string but_found);
#endif