#include "Function.h"

string Function::GetFullName() {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}

void Function::WriteDefinition(ofstream &stream, tabs t, Games::IDs game) {
    
}

void Function::WriteDeclaration(ofstream &stream, tabs t, Games::IDs game, bool isStatic) {
    
}
