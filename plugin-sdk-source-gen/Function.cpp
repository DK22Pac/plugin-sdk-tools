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
        if (mUsage != Function::Usage::DeletingDestructor)
            result += GameVersions::GetSupportedGameVersionsMacro(game, mVersionInfo) + " ";
        if (mIsStatic)
            result += "static ";
        else if (mClass && (mIsVirtual || (IsDestructor() && mClass->mHasVirtualDestructor))
            && (!mClass->mParent || mVTableIndex >= mClass->mParent->mVTableSize))
        {
            result += "virtual ";
        }
    }
    if (!IsConstructor() && !IsDestructor()) {
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
        IterateIndex(mParameters, [&](Parameter &p, unsigned int index) {
            if (index != 0)
                result += ", ";
            result += p.mType.BeforeName() + p.mName + p.mType.AfterName();
        }, mNumParamsToSkipForWrapper);
    }
    result += ")";
    return result;
}

void Function::WriteDefinition(ofstream &stream, tabs t, Games::IDs game) {
    if (mUsage != Function::Usage::DeletingDestructor) {
        stream << t() << "int " << AddrOfMacro(false) << " = ADDRESS_BY_VERSION(" << Addresses(game) << ");" << endl;
        stream << t() << "int " << AddrOfMacro(true) << " = GLOBAL_ADDRESS_BY_VERSION(" << Addresses(game) << ");" << endl;
        stream << endl;
    }
    stream << t() << NameForWrapper(game, true) << " {" << endl;
    ++t;
    if (mUsage != Function::Usage::DeletingDestructor) {
        bool noReturn = mRetType.mIsVoid || IsConstructor() || IsDestructor();
        if (mRVOParamIndex != -1) {
            Parameter &retParam = mParameters[mRVOParamIndex];
            stream << t() << retParam.mType.GetFullTypeRemovePointer() << retParam.mName << ";" << endl;
            noReturn = true;
        }
        stream << t();
        if (!noReturn)
            stream << "return ";
        stream << "plugin::Call";
        if (mCC == CC_THISCALL)
            stream << "Method";
        if (!noReturn)
            stream << "AndReturn";
        stream << "DynGlobal";
        if (mParameters.size() > 0 || !noReturn) {
            stream << "<";
            if (!noReturn)
                stream << mRetType.GetFullType(false);
            IterateIndex(mParameters, [&](Parameter &p, unsigned int index) {
                if (index != 0 || !noReturn)
                    stream << ", ";
                stream << p.mType.GetFullType(false);
            });
            stream << ">(";
        }
        else
            stream << "(";
        stream << AddrOfMacro(true);
        IterateIndex(mParameters, [&](Parameter &p, unsigned int index) {
            stream << ", ";
            if (index == mRVOParamIndex)
                stream << "&";
            stream << p.mName;
        });
        stream << ");" << endl;
        if (mRVOParamIndex != -1) {
            Parameter &retParam = mParameters[mRVOParamIndex];
            stream << t() << "return " << retParam.mName << ";" << endl;
        }
    }
    else
        stream << t() << "// no base destructor present" << endl;
    --t;
    stream << t() << "}";
}

void Function::WriteDeclaration(ofstream &stream, tabs t, Games::IDs game) {
    WriteComment(stream, mComment, t, 0);
    stream << t() << NameForWrapper(game, false) << ";";
}

void Function::WriteMeta(ofstream &stream, tabs t, Games::IDs game) {
    stream << t() << "META_BEGIN";
    if (mIsOverloaded)
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
    List<Reference> refs;
    for (unsigned int i = 0; i < Games::GetGameVersionsCount(game); i++) {
        std::istringstream ss(mVersionInfo[i].mRefsStr);
        std::string token;
        while (ss >> token) {
            Reference ref;
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
        IterateFirstLast(refs, [&](Reference &ref, bool first, bool last) {
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
    IterateIndex(mParameters, [&](Parameter &p, unsigned int index) {
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
    IterateIndex(mParameters, [&](Parameter &p, unsigned int index) {
        if (index != 0)
            stream << ",";
        stream << p.mType.GetFullType(false);
    }, 0, IsDestructor() ? 0 : -1);
    stream << ">";
    IterateIndex(mParameters, [&](Parameter &p, unsigned int index) {
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
    string result = GetFullName();
    if (mIsOverloaded) {
        result += ", " + mRetType.GetFullType() + "(";
        if (mCC == CC_THISCALL)
            result += mScope + "::";
        result += "*)(";
        if (!IsDestructor()) {
            IterateIndex(mParameters, [&](Parameter &p, unsigned int index) {
                if (index != 0)
                    result += ", ";
                result += p.mType.GetFullType(false);
            }, mNumParamsToSkipForWrapper);
        }
        result += ")";
    }
    return result;
}

string Function::AddrOfMacro(bool global) {
    string result;
    if (global)
        result = "g";
    result += "addrof";
    if (mIsOverloaded)
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

bool Function::IsDestructor() {
    return mUsage == Function::Usage::BaseDestructor || mUsage == Function::Usage::DeletingDestructor;
}

bool Function::IsConstructor() {
    return mUsage == Function::Usage::DefaultConstructor || mUsage == Function::Usage::CustomConstructor;
}
