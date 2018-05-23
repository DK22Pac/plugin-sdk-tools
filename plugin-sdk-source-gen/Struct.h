#pragma once
#include <string>
#include "ListEx.h"
#include <fstream>
#include "Tabs.h"
#include "Type.h"
#include "Function.h"
#include "Variable.h"
#include "Enum.h"

using namespace std;

class Module;

class StructMember {
public:
    string mName;             // member name
    Type mType;               // member type
    unsigned int mOffset = 0; // member offse in bytes
    unsigned int mSize = 0;   // member size in bytes
    string mComment;          // member comment
    bool mIsBase = false;     // is base class
    bool mIsPadding = false;  // if member has "_pad" in its name
    bool mIsVTable = false;   // is vtable pointer
    bool mIsAnonymous = false; // is anonymous member (has no name)
    Enum *mBitfield = nullptr; // associated bitfield
};

class Struct {
public:
    enum class Kind { Class, Struct, Union };

    enum class Access { Public, Private, Protected };

    enum class Construction { Unknown, Default, Custom };

    string mName;                 // class name
    string mModuleName;           // module name
    Module *mModule = nullptr;
    Kind mKind = Kind::Class;    // class/struct/union
    unsigned int mSize = 0;       // class size in bytes
    unsigned int mAlignment = 0;  // alignment in bytes
    bool mIsAnonymous = false;    // is anonymous class (class has no name)
    bool mIsCoreClass = false;    // is core class
    bool mIsAbstractClass = false; // this class has pure virtual functions; NOTE: not used yet
    bool mHasVTable = false;      // has vtable in exe
    bool mHasVirtualFunctions = false; // has virtual functions in mFunctions
    bool mHasVirtualDestructor = false; // has virtual destructor in mFunctions
    bool mHasVectorDeletingDestructor = false; // can we use deleting destructor to delete an array?
    unsigned int mVTableAddress = 0; // virtual table address
    int mVTableSize = 0;          // count of methods in virtual table
    Construction mConstruction = Construction::Unknown; // hide class constructors and destructor
    string mComment;              // class comment
    string mScope;                // class scope
    string mParentName;           // parent class name

    // class hierarchy
    Struct *mParent = nullptr;    // parent (base) class
    List<Struct *> mChilds;       // child classes

    Struct *mEncloseClass = nullptr; // enclose class
    List<Struct *> mNestedClasses; // nested classes

    Set<Struct *> mMemberClasses;

    List<StructMember> mMembers;  // class fields
    List<Variable> mVariables;    // variables
    List<Function> mFunctions;    // all functions

    Function *mDefaultConstructor = nullptr;
    List<Function *> mCustomConstructors;
    List<Function *> mCopyConstructors;
    Function *mBaseDestructor = nullptr;
    Function *mDeletingDestructor = nullptr;
    List<Function *> mOperatorsNew; // note: may include mDefaultOperatorNew
    List<Function *> mOperatorsNewArray; // note: may include mDefaultOperatorNewArray
    List<Function *> mOperatorsDelete; // note: may include mDefaultOperatorDelete
    List<Function *> mOperatorsDeleteArray; // note: may include mDefaultOperatorDeleteArray
    List<Function *> mOperators;
    List<Function *> mVirtualFunctions;
    List<Function *> mNonStaticFunctions;
    List<Function *> mStaticFunctions;

    Function *mDefaultOperatorNew = nullptr;
    Function *mDefaultOperatorDelete = nullptr;
    Function *mDefaultOperatorNewArray = nullptr;
    Function *mDefaultOperatorDeleteArray = nullptr;

    void OnUpdateStructs(List<Module> &modules); // update things before we write to source files
    string GetFullName();         // combine name + scope
    void Write(ofstream &stream, tabs t, Module &myModule, List<Module> const &allModules, Games::IDs game);
    unsigned int WriteFunctions(ofstream &stream, tabs t, Games::IDs game, bool definitions, bool metadata, bool makeNewLine);
    void WriteGeneratedConstruction(ofstream &stream, tabs t, Games::IDs game);
    void WriteStackObjectFunction(ofstream & stream, tabs t, Games::IDs game, Function *fn);
    void WriteCustomOperatorNewFunction(ofstream & stream, tabs t, Games::IDs game, Function *ctor);
    void WriteCustomOperatorDeleteFunction(ofstream & stream, tabs t, Games::IDs game);
    void WriteCustomOperatorDeleteArrayFunction(ofstream & stream, tabs t, Games::IDs game);
    bool ContainsType(string const &typeName, bool withPointers = true);
    void AddFunction(Function &func_to_add);
    void SetParent(Struct *parent);
    void SetEnclose(Struct *enclose);
    bool UsesCustomConstruction();
    void WriteStructExtraInfo(ofstream &stream);
};
