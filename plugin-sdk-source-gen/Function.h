#pragma once
#include <string>
#include <functional>
#include "Type.h"
#include "..\shared\Games.h"
#include "Tabs.h"
#include "ListEx.h"

// TODO: function parameter default value CVector:arg(CVector())

using namespace std;

class Struct;

struct FunctionParameter {
    enum class WSType {
        None, In, Out, InOut
    };

    string mName;
    Type mType;
    string mDefValue;
    WSType mWSType = WSType::None;
};

struct FunctionReference {
    int mRefAddr = 0;
    int mGameVersion = -1;
    int mRefType = -1;
    int mRefObjectId = -1;
    int mRefIndexInObject = -1;
};

class Function {
public:
    enum class Usage {
        Default,
        DefaultConstructor,
        CustomConstructor,
        BaseDestructor,
        DeletingDestructor,
        CopyConstructor,
        Operator,
        DefaultOperatorNew,
        CustomOperatorNew,
        DefaultOperatorNewArray,
        CustomOperatorNewArray,
        DefaultOperatorDelete,
        CustomOperatorDelete,
        DefaultOperatorDeleteArray,
        CustomOperatorDeleteArray
    };

    enum CC {
        CC_CDECL,
        CC_STDCALL,
        CC_FASTCALL,
        CC_THISCALL,
        CC_UNKNOWN = 255
    };

    enum class VTState {
        Default,
        Overloaded,
        Pure
    };

    enum class SpecialCall {
        None,
        StackObject,
        Custom_Constructor,
        Custom_OperatorNew,
        Custom_Array_Constructor,
        Custom_Array_OperatorNew,
        Custom_DeletingDestructor,
        Custom_BaseDestructor,
        Custom_OperatorDelete,
        Custom_Array_DeletingDestructor,
        Custom_Array_DeletingArrayDestructor,
        Custom_Array_BaseDestructor,
        Custom_Array_OperatorDelete
    };

    Struct *mClass = nullptr;      // ptr to class (nullptr if there's no class)

    string mName;                  // function name (without class name and scope)
    string mMangledName;           // mangled name
    string mScope;                 // scope (scope::function_name)
    string mClassName;             // class name
    string mModuleName;            // module name
    CC mCC = CC_CDECL;             // calling convention
    Type mRetType;                 // function return type

    bool mIsConst = false;         // has 'const' attribute
    bool mIsEllipsis = false;      // variable parameter count ('...')
    bool mIsOverloaded = false;    // is overloaded (one or more functions with same name in the class)
    bool mIsStatic = false;        // static function (function with __cdecl calling convention inside class)
    int mRVOParamIndex = -1;       // where's RVO parameter placed
    int mNumParamsToSkipForWrapper = 0; // count of 'special' parameters (this pointer, RVO parameter)
    string mComment;               // function comment
    string mType;                  // function declaration
    unsigned int mPriority = 1;    // priority for events (before/after)
    Usage mUsage = Usage::Default; // constructor/destructor/etc.
    int mVTableIndex = -1;         // function index in virtual table
    bool mIsVirtual = false;       // function is virtual (placed in virtual table)
    bool mWrittenToSource = false; // a temporary flag to detect if we already written this function to source file
    bool mHasWSParameters = false; // do we have wide-string parameters

    Vector<FunctionParameter> mParameters;

    struct ExeVersionInfo {
        unsigned int mAddress = 0;
        string mRefsStr;
    };

    ExeVersionInfo mVersionInfo[Games::GetMaxGameVersions()];

    string GetFullName() const; // combine name + scope
    void WriteFunctionCall(ofstream &stream, tabs t, Games::IDs game, bool writeReturn = true,
        SpecialCall specialType = SpecialCall::None, bool wsFuncs = false);
    void WriteDefinition(ofstream &stream, tabs t, Games::IDs game, bool wsFuncs = false);
    void WriteDeclaration(ofstream &stream, tabs t, Games::IDs game, bool wsFuncs = false);
    void WriteMeta(ofstream &stream, tabs t, Games::IDs game);
    string NameForWrapper(Games::IDs game, bool definition, string const &customName = string(), bool wsFuncs = false);
    string MetaDesc();
    string AddrOfMacro(bool global);
    string Addresses(Games::IDs game);
    string GetSpecialMetaWord();

    bool HasDefaultOpNewParams();
    bool HasDefaultOpDeleteParams();
    bool UsesOverloadedMetaMacro();

    bool IsConstructor();
    bool IsDestructor();
    bool IsOperatorNew();
    bool IsOperatorDelete();
    bool IsOperatorNewDelete();
};
