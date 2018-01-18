#pragma once
#include <string>
#include <vector>
#include "Type.h"
#include "Games.h"

using namespace std;

class Function {
public:
    enum CC {
        CC_CDECL,
        CC_STDCALL,
        CC_FASTCALL,
        CC_THISCALL
    };

    string mName; // only function name
    string mMangledName; // full name
    string mScope; // scope
    string mModuleName;
    CC mCC = CC_CDECL;
    Type mRetType;
    bool mIsConst = false;
    string mComment;

    struct Parameter {
        string mName;
        Type mType;
        string mComment;
    };

    vector<Parameter> mParameters;

    struct ExeVersionInfo {
        unsigned int mAddress = 0;
    };

    ExeVersionInfo mVersionInfo[Games::GetMaxGameVersions()];

    string GetFullName(); // combine name + scope
};
