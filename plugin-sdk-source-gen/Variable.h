#pragma once
#include <string>
#include "Type.h"
#include "..\shared\Games.h"
#include "Tabs.h"

using namespace std;

class Variable {
public:
    string mName;        // variable name (without class name and scope)
    string mMangledName; // mangled name
    string mScope;       // scope (scope::function_name)
    string mModuleName;  // module name
    Type mType;          // variable type
    string mDefaultValues; // default values (for example: { 0, 1 })
    string mComment;     // variable comment
    unsigned int mSize;  // variable size in bytes
    bool mIsReadOnly;    // is read-only? (may be used for 'const' attribute)

    struct ExeVersionInfo {
        unsigned int mAddress = 0;
    };

    ExeVersionInfo mVersionInfo[Games::GetMaxGameVersions()];

    string GetFullName(); // combine name + scope
    string GetNameWithType(bool bFullName = false);
    string GetNameWithRefType(bool bFullName = false);
    void WriteDefinition(ofstream &stream, tabs t, Games::IDs game);
    void WriteDeclaration(ofstream &stream, tabs t, Games::IDs game, bool isStatic);
};
