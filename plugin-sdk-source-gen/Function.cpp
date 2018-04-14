#include "Function.h"
#include "Comments.h"
#include "StringEx.h"
#include <sstream>

string Function::GetFullName() const {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}

void Function::ForAllParameters(std::function<void(Parameter &p, bool first, bool last)> callback, unsigned int startParam) {
    size_t length = mParameters.size();
    for (size_t i = startParam; i < length; i++)
        callback(mParameters[i], i == startParam, i == (length - 1));
}

void Function::ForAllParameters(std::function<void(Parameter &p)> callback, unsigned int startParam) {
    for (size_t i = startParam; i < mParameters.size(); i++)
        callback(mParameters[i]);
}

void Function::ForAllParameters(std::function<void(Parameter &p, unsigned int index)> callback, unsigned int startParam) {
    for (size_t i = startParam; i < mParameters.size(); i++)
        callback(mParameters[i], i - startParam);
}

string Function::NameForWrapper(Games::IDs game, bool definition) {
    string result;
    bool isStatic = mCC != CC_THISCALL;
    if (!definition) {
        result += Games::GetSupportedGameVersionsMacro(game, mVersionInfo) + " ";
        if (isStatic) // MAYBE also check for first parameter type
            result += "static ";
    }
    if (mRVOParamIndex != -1)
        result += mParameters[mRVOParamIndex].mType.GetFullTypeRemovePointer();
    else
        result += mRetType.GetFullType();
    if (!definition)
        result += mName;
    else
        result += GetFullName();
    result += "(";
    ForAllParameters([&](Parameter &p, unsigned int index) {
        if (index != 0)
            result += ", ";
        result += p.mType.BeforeName() + p.mName + p.mType.AfterName();
    }, mNumParamsToSkipForWrapper);
    result += ")";
    return result;
}

void Function::WriteDefinition(ofstream &stream, tabs t, Games::IDs game) {
    stream << t() << "int " << AddrOfMacro(false) << " = ADDRESS_BY_VERSION(" << Addresses(game) << ");" << endl;
    stream << t() << "int " << AddrOfMacro(true) << " = GLOBAL_ADDRESS_BY_VERSION(" << Addresses(game) << ");" << endl;
    stream << endl;
    stream << t() << NameForWrapper(game, true) << " {" << endl;
    ++t;
    bool noReturn = mRetType.mIsVoid;
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
        ForAllParameters([&](Parameter &p, unsigned int index) {
            if (index != 0 || !noReturn)
                stream << ", ";
            stream << p.mType.GetFullType(false);
        });
        stream << ">(";
    }
    else
        stream << "(";
    stream << AddrOfMacro(true);
    ForAllParameters([&](Parameter &p, unsigned int index) {
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
    stream << t() << "using mv_addresses_t = MvAddresses<" << Addresses(game) << ">;" << endl;
    stream << t() << "using refs_t = RefList<";
    vector<Reference> refs;
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
        unsigned int last = refs.size() - 1;
        for (unsigned int i = 0; i < refs.size(); i++) {
            stream << endl << t();
            Reference &ref = refs[i];
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
            if (i != last)
                stream << ",";
        }
        t--;
    }
    stream << ">;" << endl;
    stream << t() << "using def_t = ";
    if (mRVOParamIndex != -1 || mRetType.mIsVoid)
        stream << "void";
    else
        stream << mRetType.GetFullType(false);
    stream << "(";
    ForAllParameters([&](Parameter &p, unsigned int index) {
        if (index != 0)
            stream << ", ";
        stream << p.mType.GetFullType(false);
    }, mNumParamsToSkipForWrapper);
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
    ForAllParameters([&](Parameter &p, unsigned int index) {
        if (index != 0)
            stream << ",";
        stream << p.mType.GetFullType(false);
    }, mNumParamsToSkipForWrapper);
    stream << ">";
    ForAllParameters([&](Parameter &p, unsigned int index) {
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
        ForAllParameters([&](Parameter &p, unsigned int index) {
            if (index != 0)
                result += ", ";
            result += p.mType.GetFullType(false);
        }, mNumParamsToSkipForWrapper);
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
