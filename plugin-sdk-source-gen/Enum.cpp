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
    IterateFirstLast(mMembers, [&](EnumMember &member, bool first, bool last) {
        stream << t();
        auto pos = stream.tellp();
        stream << member.mName << ' ' << '=' << ' ';
        stream << (mIsHexademical? String::ToHexString(member.mValue) : to_string(member.mValue));
        if (!last)
            stream << ',';
        WriteComment(stream, member.mComment, t, stream.tellp() - pos);
        stream << endl;
    });
    --t;
    stream << t() << '}' << ';';
}

string GetNameForBitField(Enum &en, EnumMember &m) {
    string name;
    if (en.mStartWord.empty())
        name = m.mName;
    else {
        if (String::StartsWith(m.mName, en.mStartWord)) {
            name = m.mName.substr(en.mStartWord.length());
            if (name.length() > 0 && name[0] == '_')
                name = name.substr(1);
        }
    }
    auto nameParts = String::Split(name, "_");
    if (!any_of(nameParts.begin(), nameParts.end(), [](string &str) { return any_of(str.begin(), str.end(), [](char c) { return islower(c); }); })) {
        string result;
        for (auto const &s : nameParts) {
            if (!s.empty()) {
                auto fixedStr = String::ToLower(s);
                fixedStr[0] = toupper(fixedStr[0]);
                result += fixedStr;
            }
        }
        return result;
    }
    return name;
}

void Enum::WriteBitfield(ofstream &stream, tabs t, bool isAnonymousMember) {
    string bitFieldType;
    if (!mIsSigned)
        bitFieldType = "unsigned ";
    switch (mWidth) {
    case 2:
        bitFieldType += "short";
        break;
    case 4:
        bitFieldType += "int";
        break;
    case 8:
        bitFieldType += "long long";
        break;
    default:
        bitFieldType += "char";
        break;
    }
    unsigned int totalWidth = mWidth * 8;
    unsigned int currentWidth = 0;
    Iterate(mMembers, [&](EnumMember &member) {
        if (currentWidth != 0 && (currentWidth % 8) == 0)
            stream << endl;
        stream << t();
        auto pos = stream.tellp(); 
        stream << bitFieldType << ' ';
        string bfName = GetNameForBitField(*this, member);
        int width = (member.mBitWidth == 0 ? 1 : member.mBitWidth);
        if (!bfName.empty()) {
            if (isAnonymousMember)
                stream << "m_";
            stream << (width == 1 ? 'b' : 'n') << bfName << ' ';
        }
        stream << ": " << width << ';';
        WriteComment(stream, member.mComment, t, stream.tellp() - pos);
        stream << endl;
        currentWidth += width;
    });
    if (currentWidth < totalWidth) {
        stream << t() << bitFieldType << " : " << (totalWidth - currentWidth) << ';' << endl;
    }
    else if (currentWidth > totalWidth) {
        // bitfield error
    }
}
