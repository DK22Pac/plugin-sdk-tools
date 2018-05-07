#pragma once
#include <string>
#include <fstream>
#include "Tabs.h"
#include "ListEx.h"

using namespace std;

class EnumMember {
public:
    string mName;    // member name
    int mValue;      // member value
    string mComment; // member comment
};

class Enum {
public:
    string mName;        // enum name
    string mModuleName;  // module name
    string mScope;       // enum scope
    unsigned int mWidth; // enum width in bytes
    bool mIsClass;       // enum class
    bool mIsHexademical; // hexademical values
    bool mIsSigned;      // signed values
    bool mIsBitfield;    // bitfield
    string mComment;     // enum comment

    List<EnumMember> mMembers;

    string GetFullName() const; // combine name + scope
    void Write(ofstream &stream, tabs t);
};
