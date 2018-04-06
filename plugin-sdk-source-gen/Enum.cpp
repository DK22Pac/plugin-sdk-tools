#pragma once
#include "Enum.h"
#include "Comments.h"
#include "StringEx.h"

string Enum::GetFullName() const {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}

void Enum::Write(ofstream &stream, tabs t) {
    WriteComment(stream, mComment, t, 0);
    stream << t() << "enum" << ' ';
    if (mIsClass)
        stream << "class" << ' ';
    stream << "PLUGIN_API" << ' ' << mName << ' ';
    if (mWidth == 1 || mWidth == 2 || mWidth == 4 || mWidth == 8) {
        stream << ':' << ' ';
        string enumBaseTypeName;
        if (!mIsSigned)
            enumBaseTypeName = "unsigned ";
        switch (mWidth) {
        case 1:
            enumBaseTypeName += "char";
            break;
        case 2:
            enumBaseTypeName += "short";
            break;
        case 4:
            enumBaseTypeName += "int";
            break;
        case 8:
            enumBaseTypeName += "long long";
            break;
        }
        stream << enumBaseTypeName << ' ';
    }
    stream << '{' << endl;
    ++t;
    for (unsigned int i = 0; i < mMembers.size(); i++) {
        stream << t();
        auto pos = stream.tellp();
        stream << mMembers[i].mName << ' ' << '=' << ' ';
        stream << (mIsHexademical? String::ToHexString(mMembers[i].mValue) : to_string(mMembers[i].mValue));
        if (i != (mMembers.size() - 1))
            stream << ',';
        WriteComment(stream, mMembers[i].mComment, t, stream.tellp() - pos);
        stream << endl;
    }
    --t;
    stream << t() << '}' << ';';
}
