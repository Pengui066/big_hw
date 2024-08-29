#include "Parser.h"
using namespace std;
Parser parser;

statement* ParseStat() {
Token expcDefId = lexer.getNextToken();
if (expcDefId.tkType == IDENTIFIER) {
    if (!strcmp(expcDefId.idName, "var")) {
        return parser.parseVarDef();
    }
    else if (!strcmp(expcDefId.idName, "def")) {
        return parser.parseFuncDef();
    }
    else {
        return parser.parseNotDefStat();
    }
}
if (expcDefId.tkType == EOI) {
    std::cout << "found END OF INPUT, run over." << endl;
    return nullptr;
}
RaiseErr(WRONG_GRAMMAR, "statement identifiers", expcDefId.nameItself());
return nullptr;
}

int main() {
    cout << ">>> ";
    statement* stat = ParseStat();
    while (stat != nullptr) {
        stat->execute();
        cout << ">>> ";
        stat = ParseStat();
        cout << "\t\t\t\t looping...\n";
    }
    auto it = funcList.begin();
    while (it != funcList.end()){
        delete it->second;
        ++it;
    }
    return 0;
}