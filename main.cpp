#include "Parser.h"

using namespace std;

Parser parser;

int main() {
for( Token mainTk = lexer.getToken(); 
    mainTk.tkType == IDENTIFIER; 
    mainTk = lexer.getToken() )
{
    if (!strcmp(mainTk.idName, "output")) {
        output outputObj(parser.parseTresExpr());
        if (lexer.getNextToken().tkType != SEMICOLON) {
            cerr << "error: output: missing semicolon. \n";
            break;
        }
        outputObj.execute();
    }
    else if (lexer.getToken().tkType == EVA) {
        assignment assignObj(parser.parseTresExpr(), mainTk.idName);
        if (lexer.getNextToken().tkType != SEMICOLON) {
        cerr << "error: eva: missing semicolon. \n";
        break;
        }
        assignObj.execute();
    }
    else {
        std::cerr << "error: from func \"main\", not \'=\' after identifier. \n";
    }
}
cout << "over.";
return 0;
}