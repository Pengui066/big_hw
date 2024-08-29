#include "errRaiser.h"

const char* errLib[] = {
    "Integer out of range", "invalid input", 
    "wrong grammar", "missing return value", 
    "cannot find main function", "undefined function", "undefined variable",
    "stack overflow", "run error"
};

void RaiseErr(ERRORTYPE _type, const std::string& message)
{
    std::cerr << "ERROR: " << errLib[(int)_type] << std::endl;
    if (_type == UNDEFINED_FUNC) {
        std::cerr << "function '" << message << "' is not defined. " << std::endl; 
    }
    else if (_type == UNDEFINED_VAR) {
        std::cerr << "variable '" << message << "' is not defined. " << std::endl; 
    }
    else if (_type == RUN_ERROR) {
        std::cerr << message << std::endl;
    }
    else std::cerr << message << std::endl;
    exit(0);
}

void RaiseErr(ERRORTYPE _type, const std::string& expected, std::string but_found)
{
    std::cerr << "ERROR: " << errLib[(int)_type] << std::endl;
    std::cerr << "expected " << expected << ", but found " << but_found << std::endl;
    exit(0);
}
