#include "Lexer.h"

const char* opLib[] = {"NONEOP", "EQUAL", "GREATER", "LESS", "PLUS", "MINUS", "TIMES", "DIVIDEBY", "MODULUS", "AND", "OR", "NOT"};
const char* tkLib[] = {"NONETK", "INTEGER", "IDENTIFIER", "OPERATOR", "SEMICOLON", "QUESTIONMARK", "COLON", "EVA", "LBRACKET", "RBRACKET", "LBRACE", "RBRACE", "EOI"};

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
    clearTokenBuffer();
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
        return value_tmp; // 返回integer类型的token
    }
    if (isalphaUl(preChar)) { //先前字符是字母下划线
        id_tmp += preChar;
        while (isalnumUl(ch = std::cin.get())) {
            id_tmp += ch;
        }
        preChar = ch;
        return id_tmp.c_str(); // 返回identifier类型的Token
    }
    if (preChar == EOF) { // 先前字符是End Of File
    return EOI;
    }
    if (issymbol(preChar)) {
        if (preChar == '=') {
            ch = std::cin.get();
            if (ch != '=') {
                preChar = ch; // 这个时候preChar里面是单等号后面的一个字符
                return EVA;
            }
            else {
                preChar = std::cin.get(); // preChar里面是双等号后面的一个字符
                return EQUAL;
            }
        }
        if (preChar == '|') {
            ch = std::cin.get();
            if (ch != '|') {
                preChar = ch;
                std::cout << "unknown token: '|', invalid input.\n";
                return Token(); // 输入错误，返回空Token
            }
            else {
                preChar = std::cin.get(); // prechar拿到双竖线后面的字符
                return OR;
            }
        }
        if (preChar == '&') {
            ch = std::cin.get();
            if (ch != '&') {
                preChar = ch;
                std::cout << "unknown token: '&', invalid input.\n";
                return Token(); // 输入错误，返回空Token
            }
            else {
                preChar = std::cin.get(); // prechar拿到双&后面的字符
                return AND;
            }
        }
        if (preChar == ';') {preChar = std::cin.get(); return SEMICOLON; }
        if (preChar == '?') {preChar = std::cin.get(); return QUESTIONMARK; }
        if (preChar == ':') {preChar = std::cin.get(); return COLON; }
        if (preChar == '(') {preChar = std::cin.get(); return LBRACKET; }
        if (preChar == ')') {preChar = std::cin.get(); return RBRACKET; }
        if (preChar == '{') {preChar = std::cin.get(); return LBRACE; }
        if (preChar == '}') {preChar = std::cin.get(); return RBRACE; }
        if (preChar == '+') {preChar = std::cin.get(); return PLUS; }
        if (preChar == '-') {preChar = std::cin.get(); return MINUS; }
        if (preChar == '*') {preChar = std::cin.get(); return TIMES; }
        if (preChar == '/') {preChar = std::cin.get(); return DIVIDEBY; }
        if (preChar == '%') {preChar = std::cin.get(); return MODULUS; }
        if (preChar == '>') {preChar = std::cin.get(); return GREATER; }
        if (preChar == '<') {preChar = std::cin.get(); return LESS; }
        if (preChar == '!') {preChar = std::cin.get(); return NOT; }
        ASSERT(0);
    }
    std::cerr << "ERROR: Invalid input: '" << preChar << "'.\n";
    return Token(); // 注意：遇到不认识的字符，返回空Token，但是preChar里面的未知字符会保留。
}
Token Lexer::getNextToken()
{
    if (this->tkBuffer.is_NONETK()) { tkBuffer = getToken(); }
    return tkBuffer;
};

Lexer::Lexer(): preChar(' ') {}

bool is_cmpOP(OPTYPE _op)
{
    return _op == EQUAL || _op == GREATER || _op == LESS;
}

void Lexer::speakNextToken()
{
    Token tmp;
    tmp = getToken();
    switch (tmp.tkType)
    {
    case IDENTIFIER:
        std::cout << "identifier, " << tmp.idName << std::endl;
        break;
    case INTEGER:
        std::cout << "integer, " << tmp.intValue << std::endl;
        break;
    case OPERATOR:
        std::cout << "operator, " << opLib[(int)tmp.opType] << std::endl;
        break;
    case SEMICOLON:
        std::cout << tkLib[(int)tmp.tkType] << ", ;" << std::endl;
        break;
    case COLON:
        std::cout << tkLib[(int)tmp.tkType] << ", :" << std::endl;
        break;
    case QUESTIONMARK:
        std::cout << tkLib[(int)tmp.tkType] << ", ?" << std::endl;
        break;
    case EVA:
        std::cout << tkLib[(int)tmp.tkType] << ", =" << std::endl;
        break;
    case LBRACKET:
        std::cout << tkLib[(int)tmp.tkType] << ", (" << std::endl;
        break;
    case RBRACKET:
        std::cout << tkLib[(int)tmp.tkType] << ", )" << std::endl;
        break;
    case LBRACE:
        std::cout << tkLib[(int)tmp.tkType] << ", {" << std::endl;
        break;
    case RBRACE:
        std::cout << tkLib[(int)tmp.tkType] << ", }" << std::endl;
        break;
    case EOI:
        std::cout << tkLib[(int)tmp.tkType] << std::endl;
        break;
    case NONETK:
        std::cout << "none" << std::endl;
    default:
        break;
    }
}

Token::Token(const Token &_tk) // 拷贝构造函数
{
    this->tkType = _tk.tkType;
    if (_tk.tkType == IDENTIFIER) {strcpy(this->idName, _tk.idName);}
    else if (_tk.tkType == OPERATOR) {this->opType = _tk.opType;}
    else {this->intValue = _tk.intValue;}
} 
// 自己写不明白，以为普通的符号Token类型不需要拷贝union中的内容，其实不是
// c4 = -2 * 4 * a; 这句导致了ParseExpr2函数中nextTk虽然tkType是SEMICOLON但是union中的opType是TIMIES的情况，被while循环识别为乘号继续执行

