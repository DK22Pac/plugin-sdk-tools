#include "Variable.h"
#include "Comments.h"

string Variable::GetFullName() {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}

string Variable::GetNameWithType(bool bFullName) {
    return mType.BeforeName() + (bFullName ? GetFullName() : mName) + mType.AfterName();
}

string Variable::GetNameWithRefType(bool bFullName) {
    Type refType = mType.GetReference();
    return refType.BeforeName() + (bFullName ? GetFullName() : mName) + refType.AfterName();
}

void Variable::WriteDefinition(ofstream &stream, tabs t) {
    stream << t() << GetNameWithRefType(true) << ';';
}

void Variable::WriteDeclaration(ofstream &stream, tabs t) {
    WriteComment(stream, mComment, t, 0);
    stream << t() << GetNameWithRefType(false) << ';';
}
