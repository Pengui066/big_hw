#include "Lexer.h"

void Lexer::skipBlank()
{
    char ch = preChar;
    while (isspace(ch)) { // 如果prechar是空白字符，再尝试去接着吃空白字符，如果不是就不管
        ch = std::cin.get();
    }
    preChar = ch; // 在skipBlank函数之后一定会在preChar里面留下来一个非空白字符（或者EOI）
}

Token Lexer::getToken() // 读取数字字母下划线混合，并判断是否以数字开头
// 注意，它最后留在preChar里面的字符一定不是数字字母下划线
{
    Token tk_tmp;
    std::string id_tmp;
    char ch = 0;
    skipBlank(); // 在skipBlank之后一定是会在preChar里面留东西的
    if (isdigit(preChar)) { // 先前字符是数字
        id_tmp += preChar;
        while (isdigit(ch = std::cin.get())) {
            id_tmp += ch;
        }
        if (isalphaUl(ch)) {
            std::cout << "Numbers cannot be at the beginning of an identifier.\n";
            while(isalnumUl(ch = std::cin.get())){}
            preChar = ch; // 最后一个不是数字字母下划线的字符留在preChar里面
            return Token(); // 返回空Token
        }
        // 其余情况，在数字后面不是字母下划线，说明之前这个数字token是合法的，即使后面的是未知字符，它也会在下一次getToken被拿下
        preChar = ch;
        tk_tmp.tkType = INTEGER;
        int value_tmp = 0;
        try {
            value_tmp = std::stoi(id_tmp); 
        } // 按理来讲第一个错误是不会产生的，第二个错误会产生，如何handle？
        // 也有另外一个思路，直接用stoi函数返回的pos值判断是不是合法的数字输入
        catch(const std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            return Token();
        }
        catch(const std::out_of_range& e) {
            std::cerr <<e.what() << '\n';
            return Token();
        }
        tk_tmp.intValue = value_tmp;
        return tk_tmp; // 返回integer类型的token
    }
    if (isalphaUl(preChar)) { //先前字符是字母下划线
        id_tmp += preChar;
        while (isalnumUl(ch = std::cin.get())) {
            id_tmp += ch;
        }
        preChar = ch;
        tk_tmp.tkType = IDENTIFIER;
        strcpy(tk_tmp.idName,id_tmp.c_str());
        return tk_tmp;
    }
    if (preChar == EOF) {
    tk_tmp.tkType = EOI;
    return tk_tmp;
    }
    if (issymbol(preChar)) {
        if (preChar == '=') {
            ch = std::cin.get();
            if (ch != '=') {
                preChar = ch; // 这个时候preChar里面是单等号后面的一个字符
                tk_tmp.tkType = EVA;
                return tk_tmp;
            }
            else {
                preChar = std::cin.get(); // preChar里面是双等号后面的一个字符
                tk_tmp.tkType = OPERATOR;
                tk_tmp.opType = EQUAL;
                return tk_tmp;
            }
        }
        if (preChar == '|') {
            ch = std::cin.get();
            if (ch != '|') {
                preChar = ch;
                std::cout << "unknown tokn: '|', wrong input.\n";
                return Token(); // 输入错误，返回空Token
            }
            else {
                preChar = std::cin.get(); // prechar拿到双竖线后面的字符
                tk_tmp.tkType = OPERATOR;
                tk_tmp.opType = OR;
                return tk_tmp;
            }
        }
        if (preChar == '&') {
            ch = std::cin.get();
            if (ch != '&') {
                preChar = ch;
                std::cout << "unknown tokn: '&', wrong input.\n";
                return Token(); // 输入错误，返回空Token
            }
            else {
                preChar = std::cin.get(); // prechar拿到双竖线后面的字符
                tk_tmp.tkType = OPERATOR;
                tk_tmp.opType = AND;
                return tk_tmp;
            }
        }
        if (preChar == ';') {preChar = std::cin.get(); tk_tmp.tkType = SEMICOLON; return tk_tmp;}
        if (preChar == '?') {preChar = std::cin.get(); tk_tmp.tkType = QUESTIONMARK; return tk_tmp;}
        if (preChar == ':') {preChar = std::cin.get(); tk_tmp.tkType = COLON; return tk_tmp;}
        if (preChar == '(') {preChar = std::cin.get(); tk_tmp.tkType = LBRACKET; return tk_tmp;}
        if (preChar == ')') {preChar = std::cin.get(); tk_tmp.tkType = RBRACKET; return tk_tmp;}
        tk_tmp.tkType = OPERATOR;
        if (preChar == '+') {preChar = std::cin.get(); tk_tmp.opType = PLUS; return tk_tmp;}
        if (preChar == '-') {preChar = std::cin.get(); tk_tmp.opType = MINUS; return tk_tmp;}
        if (preChar == '*') {preChar = std::cin.get(); tk_tmp.opType = TIMES; return tk_tmp;}
        if (preChar == '/') {preChar = std::cin.get(); tk_tmp.opType = DIVIDEBY; return tk_tmp;}
        if (preChar == '%') {preChar = std::cin.get(); tk_tmp.opType = MODULUS; return tk_tmp;}
        if (preChar == '>') {preChar = std::cin.get(); tk_tmp.opType = GREATER; return tk_tmp;}
        if (preChar == '<') {preChar = std::cin.get(); tk_tmp.opType = LESS; return tk_tmp;}
        if (preChar == '!') {preChar = std::cin.get(); tk_tmp.opType = NOT; return tk_tmp;}
        ASSERT(0);
    }
    std::cerr << "ERROR: Invalid input: '" << preChar << "'.\n";
    return Token();
};

Lexer::Lexer(): preChar(' ') {}


