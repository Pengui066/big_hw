#ifndef __LEXER_H__
#define __LEXER_H__
#include<iostream>
#include<string>
#include<cctype>
#include<cstring>
#define UNFINISHED 0
#define ASSERT(x) if (!(x)) __debugbreak()

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
    SEMICOLON, QUESTIONMARK, COLON, EVA, LBRACKET, RBRACKET, LBRACE, RBRACE, EOI
};

extern const char* opLib[];
extern const char* tkLib[];

struct Token
{
    TKTYPE tkType;
    union 
    {
        int intValue;
        char idName[20];
        OPTYPE opType;
    };
    Token():tkType(TKTYPE::NONETK), intValue(0) {} // 默认构造函数
    Token(int _value): tkType(INTEGER), intValue(_value) {} // 整数类型Token的构造函数
    Token(OPTYPE _opType): tkType(OPERATOR), opType(_opType) {} // 运算符类型Token的构造函数
    Token(const char * _idName): tkType(IDENTIFIER) {strcpy(idName, _idName);} // 标识符类型的构造函数
    Token(TKTYPE _tkType): tkType(_tkType), intValue(0) {} // 其它类型的构造函数
    Token(const Token& _tk);
    bool is_NONETK() const { return this->tkType == NONETK; } // 判断是否为空字符的函数
};

class Lexer
{
private:
    Token tkBuffer;
    char preChar;
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
        || _char == ')'
        || _char == '{'
        || _char == '}';
    }
public:
    void clearTokenBuffer() {
        tkBuffer.tkType = NONETK;
    }
    Token getToken();
    Token getNextToken();
    Lexer();
    void speakNextToken();
};

inline bool is_cmpOP(OPTYPE _op);


#endif