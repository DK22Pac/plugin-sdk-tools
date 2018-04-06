#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Tabs.h"

using namespace std;

class Enum {
public:
    string mName;
    string mModuleName;
    string mScope;
    unsigned int mWidth;
    bool mIsClass;
    bool mIsHexademical;
    bool mIsSigned;
    bool mIsBitfield;
    string mComment;

    class Member {
    public:
        string mName;
        int mValue;
        string mComment;
    };

    vector<Member> mMembers;

    string GetFullName() const; // combine name + scope

    void Write(ofstream &stream, tabs t);
};
