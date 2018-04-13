#include "Type.h"
#include "StringEx.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

Type::Type(Type const &rhs) {
    mName = rhs.mName;
    mIsNumber = rhs.mIsNumber;
    mIsEllipsis = rhs.mIsEllipsis;
    mIsVoid = rhs.mIsVoid;
    mIsPointerToFixedSizeArray = rhs.mIsPointerToFixedSizeArray;
    mArraySize[0] = rhs.mArraySize[0];
    mArraySize[1] = rhs.mArraySize[1];
    mIsConst = rhs.mIsConst;
    mIsInBuilt = rhs.mIsInBuilt;
    mIsCustom = rhs.mIsCustom;
    mIsRenderWare = rhs.mIsRenderWare;
    mPointers = rhs.mPointers;
    mFunctionOrArrayPointers = rhs.mFunctionOrArrayPointers;
    mIsFunction = rhs.mIsFunction;
    mFunctionCC = rhs.mFunctionCC;
    mFunctionParams = rhs.mFunctionParams;
    if (rhs.mFunctionRetType)
        mFunctionRetType = new Type(*rhs.mFunctionRetType);
    else
        mFunctionRetType = nullptr;
    mIsTemplate = rhs.mIsTemplate;
    mTemplateTypes = rhs.mTemplateTypes;
    mDbgTokens = rhs.mDbgTokens;
}

Type::~Type() {
    delete mFunctionRetType;
}

bool IsSpecialCharacter(char c) {
    return c == '*' || c == '&' || c == '<' || c == '>' || c == ',' || c == '(' || c == ')' || c == '[' || c == ']';
};

bool IsSpace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool IsFunctionCCToken(Token const &t) {
    return t.type == Token::CC_CDECL
        || t.type == Token::CC_STDCALL
        || t.type == Token::CC_FASTCALL
        || t.type == Token::CC_THISCALL;
}

// Rw, rw, _Rw, _rw, __Rw, __rw
bool IsRenderWareTypeName(string const &typeName) {
    int charIndex = -1;
    if (String::Compare(typeName, 0, 'R') || String::Compare(typeName, 0, 'r'))
        charIndex = 1;
    else if (String::Compare(typeName, 0, '_')) {
        if (String::Compare(typeName, 1, '_')) {
            if (String::Compare(typeName, 2, 'R') || String::Compare(typeName, 2, 'r'))
                charIndex = 3;
        }
        else if (String::Compare(typeName, 1, 'R') || String::Compare(typeName, 1, 'r'))
            charIndex = 2;
    }
    if (charIndex != -1) {
        return String::Compare(typeName, charIndex, 'w')
            || String::Compare(typeName, charIndex, 'p')
            || String::Compare(typeName, charIndex, 't')
            || String::Compare(typeName, charIndex, 'x')
            || String::Compare(typeName, charIndex, 's');
    }
    return false;
}

string GetCorrectTypeName(string const &typeName, bool &corrected) {
    corrected = true;
    if (String::Compare(typeName, 0, '_')) {
        if (typeName == "_BYTE")
            return "unsigned char";
        if (typeName == "_WORD")
            return "unsigned short";
        if (typeName == "_DWORD")
            return "unsigned int";
        if (typeName == "_QWORD")
            return "unsigned long long";
        if (typeName == "_BOOL1")
            return "bool";
        if (typeName == "_BOOL4")
            return "bool32";
        if (String::Compare(typeName, 1, '_')) {
            if (typeName == "__int8")
                return "char";
            if (typeName == "__int16")
                return "short";
            if (typeName == "__int32")
                return "int";
            if (typeName == "__int64")
                return "long long";
        }
    }
    else if (String::Compare(typeName, 0, 'u')) {
        if (typeName == "uchar")
            return "unsigned char";
        if (typeName == "ushort")
            return "unsigned short";
        if (typeName == "uint")
            return "unsigned int";
    }
    else if (typeName == "BYTE")
        return "unsigend char";
    else if (typeName == "WORD")
        return "unsigned short";
    else if (typeName == "DWORD")
        return "unsigned int";
    else if (typeName == "QWORD")
        return "long long";
    else if (typeName == "LONG")
        return "long";
    else if (typeName == "ULONG")
        return "unsigned long";
    else if (typeName == "FLOAT")
        return "float";
    corrected = false;
    return typeName;
}

bool ObtainRegularToken(string const &tokenWord, Token &outToken) {
    if (tokenWord == "...") {
        outToken = { tokenWord, Token::ELLIPSIS };
        return true;
    }
    if (tokenWord == "const") {
        outToken = { tokenWord, Token::KEYWORD_CONST };
        return true;
    }
    if (tokenWord == "void") {
        outToken = { tokenWord, Token::VOID_TYPE };
        return true;
    }
    if (tokenWord == "class") {
        outToken = { tokenWord, Token::KEYWORD_CLASS };
        return true;
    }
    if (tokenWord == "struct") {
        outToken = { tokenWord, Token::KEYWORD_STRUCT };
        return true;
    }
    if (tokenWord == "enum") {
        outToken = { tokenWord, Token::KEYWORD_ENUM };
        return true;
    }
    if (tokenWord.length() >= 7 && tokenWord[0] == '_' && tokenWord[1] == '_') {
        if (tokenWord.length() == 10) {
            if (!tokenWord.compare(2, 8, "thiscall")) {
                outToken = { tokenWord, Token::CC_THISCALL };
                return true;
            }
            if (!tokenWord.compare(2, 8, "fastcall")) {
                outToken = { tokenWord, Token::CC_FASTCALL };
                return true;
            }
        }
        else if (tokenWord.length() == 9 && !tokenWord.compare(2, 7, "stdcall")) {
            outToken = { tokenWord, Token::CC_STDCALL };
            return true;
        }
        else if (tokenWord.length() == 7 && !tokenWord.compare(2, 5, "cdecl")) {
            outToken = { tokenWord, Token::CC_CDECL };
            return true;
        }
    }
    return false;
}

void HandleSpecialCharacter(char c, vector<Token> &tokens, bool &prevWordSU, bool &prevWordLong) {
    string w;
    w += c;
    tokens.emplace_back(w, Token::SPECIAL_CHAR);
    prevWordSU = false;
    prevWordLong = false;
}

void HandleWord(string word, vector<Token> &tokens, bool &prevWordSU, bool &prevWordLong) {
    Token token;
    if (String::IsNumber(word)) {
        tokens.emplace_back(word, Token::NUMBER);
        prevWordSU = false;
        prevWordLong = false;
    }
    else if (ObtainRegularToken(word, token)) {
        tokens.push_back(token);
        prevWordSU = false;
        prevWordLong = false;
    }
    else {
        // special cases: unsigned/signed + char/short/int/long
        //                long + double
        if (word == "unsigned" || word == "signed") {
            prevWordSU = true;
            prevWordLong = false;
            tokens.emplace_back(word, Token::INBUILT_TYPE_NAME);
        }
        else if (word == "long") {
            if (prevWordSU) {
                Token &lastToken = tokens.back();
                lastToken.value += " long";
                prevWordSU = false;
                prevWordLong = true;
            }
            else if (prevWordLong) {
                Token &lastToken = tokens.back();
                lastToken.value += " long";
                prevWordLong = false;
            }
            else {
                prevWordLong = true;
                tokens.emplace_back(word, Token::INBUILT_TYPE_NAME);
            }
        }
        else {
            bool corrected = false;
            word = GetCorrectTypeName(word, corrected);
            if (word == "char" || word == "short" || word == "int" || word == "long long") {
                if (prevWordSU) {
                    Token &lastToken = tokens.back();
                    lastToken.value += " " + word;
                }
                else
                    tokens.emplace_back(word, Token::INBUILT_TYPE_NAME);
            }
            else if (word == "double") {
                if (prevWordLong) {
                    Token &lastToken = tokens.back();
                    lastToken.value += " " + word;
                }
                else
                    tokens.emplace_back(word, Token::INBUILT_TYPE_NAME);
            }
            else {
                if (corrected || word == "float" || word == "bool" || word == "wchar_t")
                    tokens.emplace_back(word, Token::INBUILT_TYPE_NAME);
                else
                    tokens.emplace_back(word, Token::CUSTOM_TYPE_NAME);
            }
            prevWordSU = false;
            prevWordLong = false;
        }
    }
}

vector<Token> GetTokens(string const &line) {
    vector<Token> result;
    string currentWord;
    bool readingWord = false;
    bool signedUnsignedFlag = false; // signed/unsigned
    bool longFlag = false;

    for (size_t i = 0; i < line.length(); i++) {
        auto c = line[i];
        if (!readingWord) {
            if (IsSpace(c))
                continue;
            if (IsSpecialCharacter(c)) {
                HandleSpecialCharacter(c, result, signedUnsignedFlag, longFlag);
                continue;
            }
            readingWord = true;
            currentWord += c;
        }
        else {
            if (IsSpace(c) || IsSpecialCharacter(c)) {
                HandleWord(currentWord, result, signedUnsignedFlag, longFlag);
                currentWord.clear();
                readingWord = false;
                if (IsSpecialCharacter(c))
                    HandleSpecialCharacter(c, result, signedUnsignedFlag, longFlag);
            }
            else
                currentWord += c;
        }
    }

    if (!currentWord.empty())
        HandleWord(currentWord, result, signedUnsignedFlag, longFlag);

    return result;
}

string Type::GetFullType(bool leaveSpaceAtTheEnd) const {
    return BeforeName(leaveSpaceAtTheEnd) + AfterName();
}

string Type::GetFullTypeRemovePointer() {
    string savedPtrs = mPointers;
    if (mPointers.size() > 0)
        mPointers.pop_back();
    string result = GetFullType();
    mPointers = savedPtrs;
    return result;
}

bool Type::IsPointer() const {
    return mPointers.size() > 0 || mIsPointerToFixedSizeArray;
}

bool Type::IsTemplate() const {
    return mTemplateTypes.size() > 0;
}

string Type::BeforeName(bool leaveSpaceAtTheEnd) const {
    string result;
    if (mIsFunction) {
        if (mFunctionRetType)
            result = mFunctionRetType->GetFullType(false);
        else
            result = "void";
    }
    else
        result = mName;
    if (IsTemplate()) {
        result += '<';
        for (size_t i = 0; i < mTemplateTypes.size(); i++) {
            if (i != 0)
                result += ' ' + ',';
            result += mTemplateTypes[i].GetFullType();
        }
        result += '>';
    }
    if (mIsConst)
        result += " const";
    if (mPointers.size() > 0)
        result += ' ' + mPointers;
    if (mIsPointerToFixedSizeArray)
        result += '(' + mFunctionOrArrayPointers;
    else if (mIsFunction) {
        result += '(';
        if (mFunctionCC != CC_CDECL) {
            if (mFunctionCC == CC_STDCALL)
                result += "__stdcall ";
            else if (mFunctionCC == CC_THISCALL)
                result += "__thiscall ";
            else if (mFunctionCC == CC_FASTCALL)
                result += "__fastcall ";
        }
        result += mFunctionOrArrayPointers;
    }
    if (leaveSpaceAtTheEnd && !mIsFunction && !mIsPointerToFixedSizeArray && mPointers.size() == 0)
        result += ' ';
    return result;
}

string Type::AfterName(bool includeArrays) const {
    string result;
    if (mIsPointerToFixedSizeArray)
        result += ')';
    if (includeArrays && mArraySize[0] > 0) {
        result += '[' + to_string(mArraySize[0]) + ']';
        if (mArraySize[1] > 0)
            result += '[' + to_string(mArraySize[1]) + ']';
    }
    if (mIsFunction) {
        result += ')';
        result += '(';
        for (size_t i = 0; i < mFunctionParams.size(); i++) {
            if (i != 0)
                result += ' ' + ',';
            result += mFunctionParams[i].GetFullType();
        }
        result += ')';
    }
    return result;
}

vector<vector<Token>> SplitTypes(vector<Token> const &tokens, size_t &counter) {
    vector<vector<Token>> result;
    counter += 1;
    if (tokens[counter].type == Token::SPECIAL_CHAR && (tokens[counter].value[0] == ')' || tokens[counter].value[0] == '>'))
        return result;
    unsigned int currBrace = 1;
    result.resize(1);
    for (; counter < tokens.size(); counter++) {
        Token const &t = tokens[counter];
        if (t.type == Token::SPECIAL_CHAR) {
            if (t.value[0] == ')' || t.value[0] == '>') {
                currBrace--;
                if (currBrace == 0) {
                    return result;
                }
            }
            else if (t.value[0] == '(' || t.value[0] == '<')
                currBrace++;
            else if (t.value[0] == ',' && currBrace == 1) {
                result.resize(result.size() + 1);
                continue;
            }
        }
        result.back().push_back(t);
    }
    result.clear();
    return result;
}

void Type::SetFunctionTypeFromToken(Token const &t) {
    if (t.type == Token::CC_STDCALL)
        mFunctionCC = CC_STDCALL;
    else if (t.type == Token::CC_FASTCALL)
        mFunctionCC = CC_FASTCALL;
    else if (t.type == Token::CC_THISCALL)
        mFunctionCC = CC_THISCALL;
    else
        mFunctionCC = CC_CDECL;
}

void Type::AddRetTypeForFunction() {
    delete mFunctionRetType;
    mFunctionRetType = new Type;
    mFunctionRetType->mName = mName;
    mName = "Function";
    mFunctionRetType->mPointers = mPointers;
    mPointers.clear();
    mFunctionRetType->mIsConst = mIsConst;
    mIsConst = false;
    mFunctionRetType->mIsInBuilt = mIsInBuilt;
    mIsInBuilt = false;
    mFunctionRetType->mIsEllipsis = mIsEllipsis;
    mIsEllipsis = false;
    mFunctionRetType->mIsRenderWare = mIsRenderWare;
    mIsRenderWare = false;
    mFunctionRetType->mIsVoid = mIsVoid;
    mIsVoid = false;
    mFunctionRetType->mIsTemplate = mIsTemplate;
    mIsTemplate = false;
    mFunctionRetType->mTemplateTypes = mTemplateTypes;
    mTemplateTypes.clear();
    mArraySize[0] = 0;
    mArraySize[1] = 0;
}

void Type::SetFromString(string const &str) {
    auto tokens = GetTokens(str);
    SetFromTokens(tokens);
}

void Type::SetFromTokens(vector<Token> const &tokens) {
    if (tokens.size() == 1 && tokens[0].type == Token::NUMBER) {
        mIsNumber = true;
        mName = tokens[0].value;
    }
    else {
        for (size_t i = 0; i < tokens.size(); i++) {
            Token const &t = tokens[i];

            size_t left = i;
            size_t right = tokens.size() - i - 1;
            bool last = right == 0;

            if (t.type == Token::KEYWORD_CONST)
                mIsConst = true;
            else if (t.type == Token::SPECIAL_CHAR) {
                if (!mName.empty()) {
                    if (!mIsFunction && !mIsPointerToFixedSizeArray && (t.value[0] == '*' || t.value[0] == '&'))
                        mPointers += t.value[0];
                    else if (t.value[0] == '[') {
                        unsigned int arySize = 0;
                        if (right >= 1) {
                            if (tokens[i + 1].type == Token::SPECIAL_CHAR && tokens[i + 1].value[0] == ']') {
                                // array size not defined!
                                arySize = 1;
                                i += 1;
                            }
                            else if (right >= 2 && tokens[i + 1].type == Token::NUMBER 
                                && tokens[i + 2].type == Token::SPECIAL_CHAR
                                && tokens[i + 2].value[0] == ']')
                            {
                                arySize = String::ToNumber(tokens[i + 1].value);
                                i += 2;
                            }
                            if (arySize > 0) {
                                if (mArraySize[0] == 0)
                                    mArraySize[0] = arySize;
                                else {
                                    if (mArraySize[1] == 0)
                                        mArraySize[1] = arySize;
                                }
                            }
                        }
                    }
                    else if (t.value[0] == '<') {
                        vector<vector<Token>> vttypes = SplitTypes(tokens, i);
                        mIsTemplate = true;
                        mTemplateTypes.resize(vttypes.size());
                        for (size_t vt = 0; vt < mTemplateTypes.size(); vt++)
                            mTemplateTypes[vt].SetFromTokens(vttypes[vt]);
                    }
                    else if (t.value[0] == '(') {
                        if (!mIsFunction && right >= 3 &&
                            tokens[i + 1].type == Token::SPECIAL_CHAR && tokens[i + 1].value[0] == '*' &&
                            tokens[i + 2].type == Token::SPECIAL_CHAR && tokens[i + 2].value[0] == ')' &&
                            tokens[i + 3].type == Token::SPECIAL_CHAR && tokens[i + 3].value[0] == '[')
                        {
                            mIsPointerToFixedSizeArray = true;
                            mFunctionOrArrayPointers = "*"; // maybe also add support for references?
                            i += 2;
                        }
                        else {
                            if (!mIsFunction) {
                                mIsFunction = true;
                                AddRetTypeForFunction();
                                // function types like 'void(int)' are also stored as function pointers
                                mFunctionOrArrayPointers = "*"; // maybe also add support for references?
                            }
                            bool scanParameters = true;
                            if (right >= 1) {
                                if (tokens[i + 1].type == Token::SPECIAL_CHAR && tokens[i + 1].value[0] == '*')
                                    scanParameters = false;
                                else if (IsFunctionCCToken(tokens[i + 1]))
                                    scanParameters = false;
                            }
                            if (scanParameters) {
                                vector<vector<Token>> vfptypes = SplitTypes(tokens, i);
                                mFunctionParams.resize(vfptypes.size());
                                for (size_t vt = 0; vt < mFunctionParams.size(); vt++)
                                    mFunctionParams[vt].SetFromTokens(vfptypes[vt]);
                            }
                        }
                    }
                }
            }
            else if (t.type == Token::ELLIPSIS) {
                mIsEllipsis = true;
                mName = t.value;
                mIsInBuilt = true;
            }
            else if (IsFunctionCCToken(t))
                SetFunctionTypeFromToken(t);
            else if (t.type == Token::INBUILT_TYPE_NAME || t.type == Token::CUSTOM_TYPE_NAME || t.type == Token::VOID_TYPE) {
                if (mName.empty()) {
                    mName = t.value;
                    mIsInBuilt = t.type == Token::INBUILT_TYPE_NAME || t.type == Token::VOID_TYPE;
                    mIsCustom = t.type == Token::CUSTOM_TYPE_NAME;
                    mIsVoid = t.type == Token::VOID_TYPE;
                    mIsRenderWare = IsRenderWareTypeName(mName);
                }
            }
        }
    }

    mDbgTokens = tokens;
}

void AddPointer(string &str, char ptrChar) {
    if (str.size() > 0)
        str.back() = '*';
    str += ptrChar;
}

Type Type::GetReference() {
    Type newType = *this;
    if (newType.mIsFunction) {
        if (newType.mArraySize[0] > 0) {
            AddPointer(newType.mFunctionOrArrayPointers, '*');
            newType.mArraySize[0] = 0;
            newType.mArraySize[1] = 0;
        }
        else
            AddPointer(newType.mFunctionOrArrayPointers, '&');
    }
    else if (newType.mIsPointerToFixedSizeArray)
        AddPointer(newType.mPointers, '&');
    else {
        if (newType.mArraySize[0] > 0) {
            if (newType.mArraySize[1] > 0) {
                // make a pointer to fixed-size array
                newType.mIsPointerToFixedSizeArray = true;
                newType.mFunctionOrArrayPointers = '*';
                newType.mArraySize[0] = newType.mArraySize[1];
                newType.mArraySize[1] = 0;
            }
            else {
                AddPointer(newType.mPointers, '*');
                newType.mArraySize[0] = 0;
            }
        }
        else
            AddPointer(newType.mPointers, '&');
    }
    return newType;
}

string StrOffset(size_t offset) {
    return string(offset * 4, ' ');
}

void Type::DbgPrint(size_t offset) const {
    if (mIsNumber) {
        cout << StrOffset(offset) << "Number '" << mName << "'" << endl;
    }
    else if (mIsEllipsis) {
        cout << StrOffset(offset) << "Type '...'" << endl;
    }
    else if (mIsFunction) {
        cout << StrOffset(offset) << "Function {" << endl;
        cout << StrOffset(offset + 1) << "callingcc: ";
        if (mFunctionCC == CC_STDCALL)
            cout << "__stdcall";
        else if (mFunctionCC == CC_THISCALL)
            cout << "__thiscall";
        else if (mFunctionCC == CC_FASTCALL)
            cout << "__fastcall";
        else
            cout << "__cdecl";
        cout << endl;
        cout << StrOffset(offset + 1) << "function pointers: " << mFunctionOrArrayPointers << endl;
        cout << StrOffset(offset + 1) << "isconst: " << boolalpha << mIsConst << endl;
        cout << StrOffset(offset + 1) << "isarray: " << boolalpha << (mArraySize[0] > 0) << endl;
        cout << StrOffset(offset + 1) << "arraylength: " << mArraySize[0];
        if (mArraySize[0] > 0 && mArraySize[1] > 0)
            cout << 'x' << mArraySize[1];
        cout << endl;

        cout << StrOffset(offset + 1) << "rettype {" << endl;
        mFunctionRetType->DbgPrint(offset + 2);
        cout << StrOffset(offset + 1) << "}" << endl;

        cout << StrOffset(offset + 1) << "functionparams {" << endl;
        for (Type const &fp : mFunctionParams)
            fp.DbgPrint(offset + 2);
        cout << StrOffset(offset + 1) << "}" << endl;
        cout << StrOffset(offset) << "}" << endl;
    }
    else {
        cout << StrOffset(offset) << "Type '" << mName << "'" << ' ';
        if (mIsPointerToFixedSizeArray)
            cout << "(Array Pointer) ";
        cout << '{' << endl;

        cout << StrOffset(offset + 1) << "isconst: " << boolalpha << mIsConst << endl;
        cout << StrOffset(offset + 1) << "inbuilt: " << boolalpha << mIsInBuilt << endl;
        cout << StrOffset(offset + 1) << "renderware: " << boolalpha << mIsRenderWare << endl;
        if (mPointers.size() > 0)
            cout << StrOffset(offset + 1) << "pointers: " << mPointers << endl;
        else
            cout << StrOffset(offset + 1) << "pointers: -" << endl;
        if (mIsPointerToFixedSizeArray)
            cout << StrOffset(offset + 1) << "array pointers: " << mFunctionOrArrayPointers << endl;
        cout << StrOffset(offset + 1) << "isarray: " << boolalpha << (mArraySize[0] > 0) << endl;
        cout << StrOffset(offset + 1) << "arraylength: " << mArraySize[0];
        if (mArraySize[0] > 0 && mArraySize[1] > 0)
            cout << 'x' << mArraySize[1];
        cout << endl;
        if (mIsTemplate) {
            cout << StrOffset(offset + 1) << "templateparams {" << endl;
            for (Type const &tt : mTemplateTypes)
                tt.DbgPrint(offset + 2);
            cout << StrOffset(offset + 1) << "}" << endl;
        }

        cout << StrOffset(offset) << "}" << endl;
    }
}
