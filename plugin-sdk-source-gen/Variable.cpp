#include "Variable.h"
#include "Comments.h"
#include "StringEx.h"

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

void Variable::WriteDefinition(ofstream &stream, tabs t, Games::IDs game) {
    bool isConst = mType.mIsConst;
    if (mIsReadOnly)
        mType.mIsConst = true;
    else
        mType.mIsConst = false;
    stream << t() << "PLUGIN_VARIABLE" << ' ' << GetNameWithRefType(true) << " = ";
    if (mType.mArraySize[0] == 0)
        stream << '*';
    stream << "reinterpret_cast<";
    stream << mType.BeforeName() << '*' << mType.AfterName(false);
    stream << ">(";
    stream << "GLOBAL_ADDRESS_BY_VERSION(";
    bool first = true;
    for (unsigned int i = 0; i < Games::GetGameVersionsCount(game); i++) {
        if (game == Games::IDs::GTASA && i == 1) // skip GTASA 1.0 US HoodLum
            continue;
        if (!first)
            stream << ", ";
        else
            first = false;
        stream << String::ToHexString(mVersionInfo[i].mAddress);
    }
    stream << "));";
    mType.mIsConst = isConst;
}

void Variable::WriteDeclaration(ofstream &stream, tabs t, Games::IDs game, bool isStatic) {
    WriteComment(stream, mComment, t, 0);
    stream << t();
    stream << Games::GetSupportedGameVersionsMacro(game, mVersionInfo) << ' ';
    if (isStatic)
        stream << "static";
    else
        stream << "extern";
    stream << ' ';
    string originalVarTypeAndName = GetNameWithType(false);
    bool isConst = mType.mIsConst;
    if (mIsReadOnly)
        mType.mIsConst = true;
    else
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
