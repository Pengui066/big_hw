#include "Parser.h"
using namespace std;
Parser parser;

void ParseDefStat() {
Token expcDefId = lexer.getNextToken();
if (expcDefId.tkType == IDENTIFIER) {
    if (!strcmp(expcDefId.idName, "var")) {
        parser.parseVarDef();
        return;
    }
    else if (!strcmp(expcDefId.idName, "def")) {
        parser.parseFuncDef();
        return;
    }
}
RaiseErr(WRONG_GRAMMAR, "'var' or 'def", expcDefId.nameItself());
}

int main()
{
ParseDefStat();
ParseDefStat();
ParseDefStat();
return 0;
}