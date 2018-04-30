#pragma once
#include <string>
#include "ListEx.h"
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

    string mName;                 // class name
    string mModuleName;           // module name
    Kind mKind = Kind::Struct;    // class/struct/union
    unsigned int mSize = 0;       // class size in bytes
    unsigned int mAlignment = 0;  // alignment in bytes
    bool mIsAnonymous = false;    // is anonymous class (class has no name)
    bool mHasVTable = false;      // has vtable in exe
    bool mHasVirtualFunctions = false; // has virtual functions in mFunctions
    bool mHasVirtualDestructor = false; // has virtual destructor in mFunctions
    bool mHasDefaultConstructor = false; // has default constructor
    bool mHasCustomConstructors = false; // has custom constructors
    unsigned int mVTableAddress = 0; // virtual table address
    int mVTableSize = 0;          // count of methods in virtual table
    string mComment;              // class comment
    string mScope;                // class scope
    string mParentName;           // parent class name

    // class hierarchy
    Struct *mParent = nullptr;    // parent class
    List<Struct *> mChilds;       // child classes

    // special functions
    Function *mDestructor = nullptr; // destructor
    Function *mDeletingDestructor = nullptr; // deleting destructor

    class Member {
    public:
        string mName;             // member name
        Type mType;               // member type
        unsigned int mOffset = 0; // member offse in bytes
        unsigned int mSize = 0;   // member size in bytes
        string mComment;          // member comment
        bool mIsBase = false;     // is base class
        bool mIgnore = false;     // don't write to source files (vtable)

        bool IsPadding();         // checks if member has "_pad" in its name
    };

    List<Member> mMembers;        // class fields
    List<Variable> mVariables;    // variables
    List<Function> mFunctions;    // functions

    string GetFullName();         // combine name + scope

    void Write(ofstream &stream, tabs t, Module const &myModule, List<Module> const &allModules, Games::IDs game);

    bool ContainsType(string const &typeName, bool withPointers = true);

    void AddFunction(Function &fn);

    void SetParent(Struct *parent);
};
