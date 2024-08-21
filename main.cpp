#include "Lexer.h"

using std::cout; using std::endl;
Lexer lexer;
const char* opLib[] = {"NONEOP", "EQUAL", "GREATER", "LESS", "PLUS", "MINUS", "TIMES", "DIVIDEBY", "MODULUS", "AND", "OR", "NOT"};
const char* tkLib[] = {"NONETK", "INTEGER", "IDENTIFIER", "OPERATOR", "SEMICOLON", "QUESTIONMARK", "COLON", "EVA", "LBRACKET", "RBRACKET", "EOI"};


int main() {
    Token tmp;
    do {
    tmp = lexer.getToken();
    switch (tmp.tkType)
    {
    case IDENTIFIER:
        cout << "identifier, " << tmp.idName << endl;
        break;
    case INTEGER:
        cout << "integer, " << tmp.intValue << endl;
        break;
    case OPERATOR:
        cout << "operator, " << opLib[(int)tmp.opType] << endl;
        break;
    case SEMICOLON:
        cout << tkLib[(int)tmp.tkType] << " ;" << endl;
        break;
    case COLON:
        cout << tkLib[(int)tmp.tkType] << " :" << endl;
        break;
    case QUESTIONMARK:
        cout << tkLib[(int)tmp.tkType] << " ?" << endl;
        break;
    case EVA:
        cout << tkLib[(int)tmp.tkType] << " =" << endl;
        break;
    case LBRACKET:
        cout << tkLib[(int)tmp.tkType] << " (" << endl;
        break;
    case RBRACKET:
        cout << tkLib[(int)tmp.tkType] << " )" << endl;
        break;
    case EOI:
        cout << tkLib[(int)tmp.tkType] << endl;
        break;
    case NONETK:
        cout << "none" << endl;

    default:
        break;
    }
    } while (tmp.tkType != EOI && tmp.tkType != NONETK);
    return 0;
}