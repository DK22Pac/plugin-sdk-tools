#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Tabs.h"
#include "Type.h"
#include "Function.h"
#include "Variable.h"

using namespace std;

class Module;

class Struct {
public:
    enum class Kind { Class, Struct, Union };

    enum class Access { Public, Private, Protected };

    string mName;
    string mModuleName;
    Kind mKind = Kind::Struct;
    unsigned int mSize = 0;
    unsigned int mAlignment = 0;
    bool mIsAnonymous = false;
    string mComment;
    string mScope;
    string mParent;

    class Member {
    public:
        string mName;
        string mFullType;
        Type mType;
        unsigned int mOffset;
        unsigned int mSize;
        bool mIsAnonymous;
        string mComment;

        bool IsPadding();
    };

    vector<Member> mMembers;

    vector<Variable> mVariables;
    vector<Function> mFunctions;


    string GetFullName() const; // combine name + scope

    void Write(ofstream &stream, tabs t, Module const &myModule, vector<Module> const &allModules, Games::IDs game);

    bool ContainsType(string const &typeName, bool withPointers = true) const;

    void AddFunction(Function const &fn);
};
