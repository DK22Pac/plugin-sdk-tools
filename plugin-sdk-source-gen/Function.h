#pragma once
#include <string>
#include <vector>
#include <functional>
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

    struct Reference {
        int mRefAddr = 0;
        int mGameVersion = -1;
        int mRefType = -1;
        int mRefObjectId = -1;
        int mRefIndexInObject = -1;
    };

    string mName; // only function name
    string mMangledName; // full name
    string mScope; // scope
    string mModuleName;
    CC mCC = CC_CDECL;
    Type mRetType;

    bool mIsConst = false;
    bool mIsEllipsis = false;
    bool mIsOverloaded = false;
    int mRVOParamIndex = -1;
    unsigned int mNumParamsToSkipForWrapper = 0;
    string mComment;
    string mType;
    unsigned int mPriority = 1;

    struct Parameter {
        string mName;
        Type mType;
    };

    vector<Parameter> mParameters;

    struct ExeVersionInfo {
        unsigned int mAddress = 0;
        string mRefsStr;
    };

    ExeVersionInfo mVersionInfo[Games::GetMaxGameVersions()];

    string GetFullName() const; // combine name + scope

    void ForAllParameters(std::function<void(Parameter &p, bool first, bool last)> callback, unsigned int startParam = 0);
    void ForAllParameters(std::function<void(Parameter &p)> callback, unsigned int startParam = 0);
    void ForAllParameters(std::function<void(Parameter &p, unsigned int index)> callback, unsigned int startParam = 0);

    void WriteDefinition(ofstream &stream, tabs t, Games::IDs game);
    void WriteDeclaration(ofstream &stream, tabs t, Games::IDs game);
    void WriteMeta(ofstream &stream, tabs t, Games::IDs game);

    string NameForWrapper(Games::IDs game, bool definition);
    string MetaDesc();
    string AddrOfMacro(bool global);
    string Addresses(Games::IDs game);
};
