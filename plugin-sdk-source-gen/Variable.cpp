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
    bool isConst = mType.mIsConst;
    mType.mIsConst = false;
    stream << t() << GetNameWithRefType(true) << ';';
    mType.mIsConst = isConst;
}

void Variable::WriteDeclaration(ofstream &stream, tabs t, bool isStatic, Games::IDs game) {
    WriteComment(stream, mComment, t, 0);
    stream << t();
    if (isStatic)
        stream << "static ";
    else
        stream << "extern ";
    stream << Games::GetSupportedGameVersionsMacro(game, mVersionInfo) << ' ';
    string originalVarTypeAndName = GetNameWithType(false);
    bool isConst = mType.mIsConst;
    mType.mIsConst = false;
    stream << GetNameWithRefType(false) << ';';
    string additionalComment;
    if (mType.mArraySize[0] > 0) {
        if (isStatic)
            additionalComment += "static ";
        additionalComment += originalVarTypeAndName;
    }
    if (isConst && mDefaultValues.length() > 0 && mType.mIsInBuilt) {
        if (mType.mArraySize[0] > 0)
            additionalComment += " = ";
        additionalComment += mDefaultValues;
    }
    if (additionalComment.length() > 0)
        stream << " // " << additionalComment;
    mType.mIsConst = isConst;
}
