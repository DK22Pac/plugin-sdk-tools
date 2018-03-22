#pragma once
#include <string>
#include <vector>
#include "Type.h"
#include "..\shared\Games.h"
#include "Tabs.h"

using namespace std;

class Function {
public:
    enum CC {
        CC_CDECL,
        CC_STDCALL,
        CC_FASTCALL,
        CC_THISCALL,
        CC_UNKNOWN = 255
    };

    string mName; // only function name
    string mMangledName; // full name
    string mScope; // scope
    string mModuleName;
    CC mCC = CC_CDECL;
    Type mRetType;
    bool mIsConst = false;
    bool mIsEllipsis = false;
    string mRefsStr;
    string mComment;

    struct Parameter {
        string mName;
        Type mType;
    };

    vector<Parameter> mParameters;

    struct ExeVersionInfo {
        unsigned int mAddress = 0;
    };

    ExeVersionInfo mVersionInfo[Games::GetMaxGameVersions()];

    string GetFullName(); // combine name + scope

    void WriteDefinition(ofstream &stream, tabs t, Games::IDs game);
    void WriteDeclaration(ofstream &stream, tabs t, Games::IDs game, bool isStatic);
};
