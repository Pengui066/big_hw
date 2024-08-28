#include "Lexer.h"
#include "Parser.h"

using std::cout; using std::endl;

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
        cout << tkLib[(int)tmp.tkType] << ", ;" << endl;
        break;
    case COLON:
        cout << tkLib[(int)tmp.tkType] << ", :" << endl;
        break;
    case QUESTIONMARK:
        cout << tkLib[(int)tmp.tkType] << ", ?" << endl;
        break;
    case EVA:
        cout << tkLib[(int)tmp.tkType] << ", =" << endl;
        break;
    case LBRACKET:
        cout << tkLib[(int)tmp.tkType] << ", (" << endl;
        break;
    case RBRACKET:
        cout << tkLib[(int)tmp.tkType] << ", )" << endl;
        break;
    case LBRACE:
        std::cout << tkLib[(int)tmp.tkType] << ", {" << std::endl;
        break;
    case RBRACE:
        std::cout << tkLib[(int)tmp.tkType] << ", }" << std::endl;
        break;
    case EOI:
        cout << tkLib[(int)tmp.tkType] << endl;
        break;
    case NONETK:
        cout << "none" << endl;

    default:
        break;
    }
    } while (tmp.tkType != EOI && !tmp.is_NONETK());
    return 0;
}