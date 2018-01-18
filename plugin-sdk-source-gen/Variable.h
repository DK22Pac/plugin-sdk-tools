#pragma once
#include <string>
#include <vector>
#include "Type.h"
#include "Games.h"
#include "Tabs.h"

using namespace std;

class Variable {
public:
    string mName; // only variable name
    string mMangledName; // full name
    string mScope; // scope
    string mModuleName;
    Type mType;
    string mComment;

    struct ExeVersionInfo {
        unsigned int mAddress = 0;
    };

    ExeVersionInfo mVersionInfo[Games::GetMaxGameVersions()];

    string GetFullName(); // combine name + scope
    string GetNameWithType(bool bFullName = false);
    string GetNameWithRefType(bool bFullName = false);
    void WriteDefinition(ofstream &stream, tabs t);
    void WriteDeclaration(ofstream &stream, tabs t);
};
