#pragma once
#include <string>
#include <fstream>
#include "Tabs.h"
#include "ListEx.h"

using namespace std;

class EnumMember {
public:
    string mName;      // member name
    int mValue = 0;    // member value
    string mComment;   // member comment
    int mBitWidth = 0; // member bitfield width
};

class Enum {
public:
    string mName;             // enum name
    string mModuleName;       // module name
    string mScope;            // enum scope
    unsigned int mWidth = 0;  // enum width in bytes
    bool mIsClass = false;    // enum class
    bool mIsHexademical = false; // hexademical values
    bool mIsSigned = false;   // signed values
    bool mIsBitfield = false; // bitfield
    bool mIsAnonymous = false; // anonymous enum
    bool mUsedAsBitfieldMember = false; // used as bitfield member in struct
    string mStartWord;        // start word for enum members
    string mComment;          // enum comment

    List<EnumMember> mMembers;

    string GetFullName() const; // combine name + scope
    void Write(ofstream &stream, tabs t);
    void WriteBitfield(ofstream &stream, tabs t, bool isAnonymousMember);
};
