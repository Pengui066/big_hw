#include "Parser.h"

using namespace std;

Parser parser;

int main() {
for( Token mainTk = lexer.getToken(); 
    mainTk.tkType == IDENTIFIER; 
    mainTk = lexer.getToken() )
{
    if (!strcmp(mainTk.idName, "output")) {
        outputStat outputObj(parser.parseTresExpr());
        Token expectedSemicolon = lexer.getNextToken();
        if (expectedSemicolon.tkType != SEMICOLON) {
            RaiseErr(WRONG_GRAMMAR, "from output statement, missing semicolon, but found " + expectedSemicolon.nameItself());
            break;
        }
        outputObj.execute();
    }
    else if (lexer.getToken().tkType == EVA) {
        assignment assignObj(parser.parseTresExpr(), mainTk.idName);
        Token expectedSemicolon = lexer.getNextToken();
        if (expectedSemicolon.tkType != SEMICOLON) {
        RaiseErr(WRONG_GRAMMAR, "from eva statement, missing semicolon, but found " + expectedSemicolon.nameItself());
        break;
        }
        assignObj.execute();
    }
    else {
        RaiseErr(WRONG_GRAMMAR, "Invalid statement. ");
    }
}
cout << "over.";
return 0;
}