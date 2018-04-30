#pragma once
#include <string>
#include <fstream>
#include "Tabs.h"
#include "ListEx.h"

using namespace std;

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

    class Member {
    public:
        string mName;    // member name
        int mValue;      // member value
        string mComment; // member comment
    };

    List<Member> mMembers;

    string GetFullName() const; // combine name + scope

    void Write(ofstream &stream, tabs t);
};
