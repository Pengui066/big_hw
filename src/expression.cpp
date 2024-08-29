#include "expression.h"
#include "statement.h"

int unoExp::evaluate()
{
    if (this->op == NONEOP) return Opnd->evaluate();
    if (this->op == MINUS) return -Opnd->evaluate();
    if (this->op == NOT) return !(Opnd->evaluate());
    ASSERT(0);
    return 0;
}

unoExp::~unoExp()
{
    delete this->Opnd;
}

int dosExp::evaluate()
{
    if (this->op == PLUS) return ((lOpnd->evaluate()) + (rOpnd->evaluate()));
    if (this->op == MINUS) return ((lOpnd->evaluate()) - (rOpnd->evaluate()));
    if (this->op == TIMES) return ((lOpnd->evaluate()) * (rOpnd->evaluate()));
    if (this->op == DIVIDEBY) return ((lOpnd->evaluate()) / (rOpnd->evaluate()));
    if (this->op == MODULUS) return ((lOpnd->evaluate()) % (rOpnd->evaluate()));
    if (this->op == EQUAL) return ((lOpnd->evaluate()) == (rOpnd->evaluate()));
    if (this->op == GREATER) return ((lOpnd->evaluate()) > (rOpnd->evaluate()));
    if (this->op == LESS) return ((lOpnd->evaluate()) < (rOpnd->evaluate()));
    if (this->op == AND) return ((lOpnd->evaluate()) && (rOpnd->evaluate()));
    if (this->op == OR) return ((lOpnd->evaluate()) || (rOpnd->evaluate()));
    ASSERT(0); // unknown operator in dosExp.
    return 0;
}

dosExp::~dosExp()
{
    delete this->lOpnd;
    delete this->rOpnd;
}

int tresExp::evaluate()
{
    if(Opnd1->evaluate()) return Opnd2->evaluate(); 
    return Opnd3->evaluate(); 
}

tresExp::~tresExp()
{
    delete this->Opnd1;
    delete this->Opnd2;
    delete this->Opnd3;
}

baseExp::baseExp(Token _tk, bool _isFunc): isFunc(_isFunc)
{
    ASSERT(_tk.tkType == IDENTIFIER || _tk.tkType == INTEGER);
    baseTk = _tk;
}

int baseExp::evaluate()
{
    ASSERT(this->baseTk.tkType == IDENTIFIER || this->baseTk.tkType == INTEGER);
    if (this->baseTk.tkType == IDENTIFIER) {
        if (isFunc) {
            auto it = funcList.find(baseTk.idName);
            if (it == funcList.end()) RaiseErr(UNDEFINED_FUNC, baseTk.idName);
            funcCall obj = funcCall(this->baseTk.idName);
            obj.execute();
            return mem[memManager.SP + 1];
        }
        return mem[memManager.findPos(this->baseTk.idName)];
    }
    if (this->baseTk.tkType == INTEGER) {
        return this->baseTk.intValue;
    }
    return 0;
}

baseExp::~baseExp() {}

expression::~expression() {}
