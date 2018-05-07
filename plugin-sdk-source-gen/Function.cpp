#include "Function.h"
#include "Struct.h"
#include "Comments.h"
#include "StringEx.h"
#include <sstream>
#include "GameVersions.h"

string Function::GetFullName() const {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}

string Function::NameForWrapper(Games::IDs game, bool definition) {
    string result;
    if (!definition) {
        result += GameVersions::GetSupportedGameVersionsMacro(game, mVersionInfo) + " ";
        if (mIsStatic)
            result += "static ";
    }
    if (mUsage != Function::Usage::DefaultConstructor && !IsDestructor()) {
        if (mRVOParamIndex != -1)
            result += mParameters[mRVOParamIndex].mType.GetFullTypeRemovePointer();
        else
            result += mRetType.GetFullType();
    }
    if (!definition)
        result += mName;
    else
        result += GetFullName();
    result += "(";
    if (!IsDestructor()) {
        IterateIndex(mParameters, [&](FunctionParameter &p, unsigned int index) {
            if (index != 0)
                result += ", ";
            result += p.mType.BeforeName() + p.mName + p.mType.AfterName();
        }, mNumParamsToSkipForWrapper);
    }
    result += ")";
    return result;
}

void Function::WriteFunctionCall(ofstream &stream, tabs t, Games::IDs game) {
    bool noReturn = mRetType.mIsVoid || IsConstructor() || IsDestructor();
    if (mRVOParamIndex != -1) {
        FunctionParameter &retParam = mParameters[mRVOParamIndex];
        stream << t() << retParam.mType.GetFullTypeRemovePointer() << retParam.mName << ";" << endl;
        noReturn = true;
    }
    stream << t();
    if (!noReturn)
        stream << "return ";
    stream << "plugin::Call";
    if (mIsVirtual)
        stream << "Virtual";
    if (mCC == CC_THISCALL)
        stream << "Method";
    if (!noReturn)
        stream << "AndReturn";
    if (!mIsVirtual)
        stream << "DynGlobal";
    if (mParameters.size() > 0 || !noReturn || mIsVirtual) {
        stream << "<";
        if (!noReturn) {
            stream << mRetType.GetFullType(false);
            if (mIsVirtual)
                stream << ", " << mVTableIndex;
        }
        else if (mIsVirtual)
            stream << mVTableIndex;
        IterateFirstLast(mParameters, [&](FunctionParameter &p, bool first, bool last) {
            if (!first || !noReturn || mIsVirtual)
                stream << ", ";
            stream << p.mType.GetFullType(false);
        });
        stream << ">(";
    }
    else
        stream << "(";
    if (!mIsVirtual)
        stream << AddrOfMacro(true);
    IterateIndex(mParameters, [&](FunctionParameter &p, unsigned int index) {
        if (!mIsVirtual || index != 0)
            stream << ", ";
        if (index == mRVOParamIndex)
            stream << "&";
        stream << p.mName;
    });
    stream << ");" << endl;
    if (mRVOParamIndex != -1) {
        FunctionParameter &retParam = mParameters[mRVOParamIndex];
        stream << t() << "return " << retParam.mName << ";" << endl;
    }
}

void Function::WriteDefinition(ofstream &stream, tabs t, Games::IDs game) {
    stream << t() << "int " << AddrOfMacro(false) << " = ADDRESS_BY_VERSION(" << Addresses(game) << ");" << endl;
    stream << t() << "int " << AddrOfMacro(true) << " = GLOBAL_ADDRESS_BY_VERSION(" << Addresses(game) << ");";
    if (mClass && mClass->UsesCustomConstruction() &&
        (IsConstructor() || IsDestructor() || mUsage == Usage::CopyConstructor || IsOperatorNewDelete()))
    {
        return;
    }
    stream << endl << endl;
    stream << t() << NameForWrapper(game, true) << " {" << endl;
    ++t;
    WriteFunctionCall(stream, t, game);
    --t;
    stream << t() << "}";
}

void Function::WriteDeclaration(ofstream &stream, tabs t, Games::IDs game) {
    WriteComment(stream, mComment, t, 0);
    stream << t() << NameForWrapper(game, false) << ";";
}

void Function::WriteMeta(ofstream &stream, tabs t, Games::IDs game) {
    stream << t();
    if (mUsage == Usage::DefaultConstructor || mUsage == Usage::CustomConstructor || mUsage == Usage::CopyConstructor)
        stream << "CTOR_";
    else if (mUsage == Usage::BaseDestructor)
        stream << "DTOR_";
    else if (mUsage == Usage::DeletingDestructor)
        stream << "DEL_DTOR_";
    else if (mUsage == Usage::DefaultOperatorNew || mUsage == Usage::CustomOperatorNew)
        stream << "OP_NEW_";
    else if (mUsage == Usage::DefaultOperatorNewArray || mUsage == Usage::CustomOperatorNewArray)
        stream << "OP_NEW_ARRAY_";
    else if (mUsage == Usage::DefaultOperatorDelete || mUsage == Usage::CustomOperatorDelete)
        stream << "OP_DELETE_";
    else if (mUsage == Usage::DefaultOperatorDeleteArray || mUsage == Usage::CustomOperatorDeleteArray)
        stream << "OP_DELETE_ARRAY_";
    stream << "META_BEGIN";
    if (UsesOverloadedMetaMacro())
        stream << "_OVERLOADED";
    stream << "(" << MetaDesc() << ")" << endl;
    t++;
    stream << t() << "static int address;" << endl;
    stream << t() << "static int global_address;" << endl;
    stream << t() << "static const int id = " << String::ToHexString(mVersionInfo[0].mAddress) << ";" << endl;
    stream << t() << "static const bool is_virtual = " << (mIsVirtual ? "true" : "false") << ";" << endl;
    stream << t() << "static const int vtable_index = " << mVTableIndex << ";" << endl;
    stream << t() << "using mv_addresses_t = MvAddresses<" << Addresses(game) << ">;" << endl;
    stream << t() << "using refs_t = RefList<";
    List<FunctionReference> refs;
    for (unsigned int i = 0; i < Games::GetGameVersionsCount(game); i++) {
        std::istringstream ss(mVersionInfo[i].mRefsStr);
        std::string token;
        while (ss >> token) {
            FunctionReference ref;
            ref.mGameVersion = i;
            ref.mRefAddr = String::ToNumber(token);
            ss >> token;
            ref.mRefType = String::ToNumber(token);
            ss >> token;
            ref.mRefObjectId = String::ToNumber(token);
            ss >> token;
            ref.mRefIndexInObject = String::ToNumber(token);
            refs.push_back(ref);
        }
    }
    if (refs.size() > 0) {
        t++;
        IterateFirstLast(refs, [&](FunctionReference &ref, bool first, bool last) {
            stream << endl << t();
            stream << String::ToHexString(ref.mRefAddr) << ", ";
            stream << "GAME_";
            if (game == Games::IDs::GTASA && ref.mGameVersion == 0)
                stream << "10US_COMPACT";
            else if (game == Games::IDs::GTASA && ref.mGameVersion == 1)
                stream << "10US_HOODLUM";
            else
                stream << String::ToUpper(Games::GetGameVersionName(game, ref.mGameVersion));
            stream << ", ";
            if (ref.mRefType == 0)
                stream << "H_CALL";
            else if (ref.mRefType == 1)
                stream << "H_JUMP";
            else if (ref.mRefType == 2)
                stream << "H_CALLBACK";
            stream << ", ";
            stream << String::ToHexString(ref.mRefObjectId) << ", ";
            stream << ref.mRefIndexInObject;
            if (!last)
                stream << ",";
        });
        t--;
    }
    stream << ">;" << endl;
    stream << t() << "using def_t = " << mRetType.GetFullType(false) << "(";
    IterateIndex(mParameters, [&](FunctionParameter &p, unsigned int index) {
        if (index != 0)
            stream << ", ";
        stream << p.mType.GetFullType(false);
    }, 0, IsDestructor() ? 0 : -1);
    stream << ");" << endl;
    stream << t() << "static const int cb_priority = PRIORITY_";
    if (mPriority == 1)
        stream << "AFTER";
    else
        stream << "BEFORE";
    stream << "; " << endl;
    stream << t() << "using calling_convention_t = CallingConventions::";
    if (mCC == Function::CC_CDECL)
        stream << "Cdecl";
    else if (mCC == Function::CC_THISCALL)
        stream << "Thiscall";
    stream << ";" << endl;
    stream << t() << "using args_t = ArgPick<ArgTypes<";
    IterateIndex(mParameters, [&](FunctionParameter &p, unsigned int index) {
        if (index != 0)
            stream << ",";
        stream << p.mType.GetFullType(false);
    }, 0, IsDestructor() ? 0 : -1);
    stream << ">";
    IterateIndex(mParameters, [&](FunctionParameter &p, unsigned int index) {
        if (index == 0)
            stream << ", ";
        else
            stream << ",";
        stream << index;
    }, mNumParamsToSkipForWrapper);
    stream << ">;" << endl;
    t--;
    stream << t() << "META_END";
}

string Function::MetaDesc() {
    string result;
    if (mUsage == Usage::Default || mUsage == Usage::Operator)
        result = GetFullName();
    else
        result = mClassName;
    if (UsesOverloadedMetaMacro()) {
        result += ", ";
        if (mUsage == Usage::DefaultConstructor || mUsage == Usage::CustomConstructor || mUsage == Usage::CopyConstructor)
            result += "void";
        else
            result += mRetType.GetFullType();
        result += "(";
        if (mUsage == Usage::Default || mUsage == Usage::Operator) {
            if (mCC == CC_THISCALL)
                result += mScope + "::";
            result += "*)(";
        }
        if (!IsDestructor()) {
            IterateIndex(mParameters, [&](FunctionParameter &p, unsigned int index) {
                if (index != 0)
                    result += ", ";
                result += p.mType.GetFullType(false);
            }, mNumParamsToSkipForWrapper);
        }
        result += ")";
    }
    return result;
}

bool Function::UsesOverloadedMetaMacro() {
    if (mUsage == Usage::DefaultConstructor ||
        mUsage == Usage::DefaultOperatorNew ||
        mUsage == Usage::DefaultOperatorNewArray ||
        mUsage == Usage::DefaultOperatorDelete ||
        mUsage == Usage::DefaultOperatorDeleteArray
        || IsDestructor())
    {
        return false;
    }
    if (mUsage == Usage::CustomConstructor ||
        mUsage == Usage::CopyConstructor ||
        mUsage == Usage::CustomOperatorNew ||
        mUsage == Usage::CustomOperatorNewArray ||
        mUsage == Usage::CustomOperatorDelete ||
        mUsage == Usage::CustomOperatorDeleteArray)
    {
        return true;
    }
    return mIsOverloaded;
}

string Function::AddrOfMacro(bool global) {
    string result;
    if (global)
        result = "g";
    result += "addrof";
    if (UsesOverloadedMetaMacro())
        result += "_o";
    result += "(" + MetaDesc() + ")";
    return result;
}

string Function::Addresses(Games::IDs game) {
    string result;
    bool first = true;
    for (unsigned int i = 0; i < Games::GetGameVersionsCount(game); i++) {
        if (game == Games::IDs::GTASA && i == 1) // skip GTASA 1.0 US HoodLum
            continue;
        if (!first)
            result += ", ";
        else
            first = false;
        result += String::ToHexString(mVersionInfo[i].mAddress);
    }
    return result;
}

bool Function::HasDefaultOpNewDeleteParams() {
    return mParameters.size() == 1 &&
        (mParameters[0].mType.mName == "int"
            || mParameters[0].mType.mName == "unsigned int"
            || mParameters[0].mType.mName == "size_t"
            || mParameters[0].mType.mName == "long"
            || mParameters[0].mType.mName == "unsigned long");
}

bool Function::IsDestructor() {
    return mUsage == Function::Usage::BaseDestructor || mUsage == Function::Usage::DeletingDestructor;
}

bool Function::IsConstructor() {
    return mUsage == Function::Usage::DefaultConstructor || mUsage == Function::Usage::CustomConstructor;
}

bool Function::IsOperatorNew() {
    return mUsage == Function::Usage::DefaultOperatorNew || mUsage == Function::Usage::CustomOperatorNew
        || mUsage == Function::Usage::DefaultOperatorNewArray || mUsage == Function::Usage::CustomOperatorNewArray;
}

bool Function::IsOperatorDelete() {
    return mUsage == Function::Usage::DefaultOperatorDelete || mUsage == Function::Usage::CustomOperatorDelete
        || mUsage == Function::Usage::DefaultOperatorDeleteArray || mUsage == Function::Usage::CustomOperatorDeleteArray;
}

bool Function::IsOperatorNewDelete() {
    return IsOperatorNew() || IsOperatorDelete();
}
