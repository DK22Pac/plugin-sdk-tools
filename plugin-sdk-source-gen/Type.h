#pragma once
#include <string>
#include <vector>

using namespace std;

struct Token {
    enum Type {
        SPECIAL_CHAR,
        ELLIPSIS,
        KEYWORD_CONST,
        INBUILT_TYPE_NAME,
        CUSTOM_TYPE_NAME,
        VOID_TYPE,
        CC_CDECL,
        CC_STDCALL,
        CC_THISCALL,
        CC_FASTCALL,
        NUMBER,
        KEYWORD_CLASS,
        KEYWORD_STRUCT,
        KEYWORD_ENUM
    };

    string value;
    Type type;

    Token() = default;
    Token(string const &_value, Type _type) : value(_value), type(_type) {}
};

class Type {
public:
    enum FunctionCC {
        CC_CDECL,
        CC_THISCALL,
        CC_STDCALL,
        CC_FASTCALL
    };

    string mName;

    // only for template types...
    bool mIsNumber = false;

    bool mIsEllipsis = false;

    bool mIsVoid = false;

    // array
    bool mIsPointerToFixedSizeArray = false;
    unsigned int mArraySize[2] = { 0, 0 }; // 2D arrays are supported
    
    // const
    bool mIsConst = false;
    
    // type
    bool mIsInBuilt = false;
    bool mIsCustom = false;
    bool mIsRenderWare = false;

    // pointer
    string mPointers;

    string mFunctionOrArrayPointers;

    // function
    bool mIsFunction = false;
    FunctionCC mFunctionCC = CC_CDECL;
    vector<Type> mFunctionParams;
    Type *mFunctionRetType = nullptr;

    // template
    bool mIsTemplate = false;
    vector<Type> mTemplateTypes;

    // debug
    vector<Token> mDbgTokens;

    Type() = default;
    Type(Type const &rhs);
    ~Type();
    
    void SetFromString(string const &str);
    void SetFromTokens(vector<Token> const &tokens);
    string GetFullType(bool leaveSpaceAtTheEnd = true) const;
    string GetFullTypeRemovePointer();
    string BeforeName(bool leaveSpaceAtTheEnd = true) const;
    string AfterName(bool includeArrays = true) const;

    bool IsPointer() const;
    bool IsTemplate() const;

    void SetFunctionTypeFromToken(Token const &t);

    void AddRetTypeForFunction();

    void DbgPrint(size_t offset = 0) const;

    Type GetReference();
};
