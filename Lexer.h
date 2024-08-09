#pragma once
#include<iostream>
#include<string>
#include<cctype>
#include<cstring>
#define UNFINISHED 0
#define ASSERT(x) if (!(x)) __debugbreak();

enum OPTYPE // 记录操作符种类的枚举类型
{
    NONEOP, 
    EQUAL, GREATER, LESS,
    PLUS, MINUS, TIMES, DIVIDEBY, MODULUS, 
    AND, OR, NOT
};

enum TKTYPE // 记录token种类的枚举类型
{
    NONETK,
    INTEGER, IDENTIFIER, OPERATOR,
    SEMICOLON, QUESTIONMARK, COLON, EVA, LBRACKET, RBRACKET, EOI
};

struct Token
{
    TKTYPE tkType;
    union 
    {
        int intValue;
        char idName[20];
        OPTYPE opType;
    };
    Token():tkType(TKTYPE::NONETK), intValue(0) {}
    bool is_NONETK() { return this->tkType == NONETK; } // 判断是否为空字符的函数
};

class Lexer
{
private:
    char preChar;
    char opBuffer;
    void skipBlank();
    bool isalphaUl(char _char) {
        return isalpha(_char) || _char == '_';
    }
    bool isalnumUl(char _char) {
        return isalnum(_char) || _char == '_';
    }
    bool issymbol(char _char) {
        return _char == '+'
        || _char == '-'
        || _char == '*'
        || _char == '/'
        || _char == '%'
        || _char == '>'
        || _char == '<'
        || _char == '!'
        || _char == '='
        || _char == '|'
        || _char == '&'
        || _char == ';'
        || _char == '?'
        || _char == ':'
        || _char == '('
        || _char == ')';
    }
public:
    Token getToken();
    Lexer();
};