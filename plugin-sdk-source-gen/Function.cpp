#include "Function.h"
#include "Comments.h"
#include "StringEx.h"

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
    stream << t() << NameForWrapper(game, true) << " {" << endl;
    ++t;
    bool isMethod = mCC == CC_THISCALL;
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
    if (isMethod)
        stream << "Method";
    if (!noReturn)
        stream << "AndReturn";
    stream << "Dyn";
    if (mParameters.size() > 0) {
        stream << "<";
        ForAllParameters([&](Parameter &p, unsigned int index) {
            if (index != 0)
                stream << ", ";
            stream << p.mType.GetFullType();
        });
        stream << ">(";
    }
    else
        stream << "(";
    stream << "addrof";
    if (mIsOverloaded)
        stream << "_o";
    stream << "(" << GetFullName();
    if (mIsOverloaded) {
        stream << ", " << mRetType.GetFullType() << "(";
        if (isMethod)
            stream << mScope << "::";
        stream << "*)(";
        ForAllParameters([&](Parameter &p, unsigned int index) {
            if (index != 0)
                stream << ", ";
            stream << p.mType.GetFullType();
        }, mNumParamsToSkipForWrapper);
        stream << ")";
    }
    stream << ")";
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
    stream << t() << "META_BEGIN(" << GetFullName() << ")" << endl;
    t++;
    stream << t() << "static int address;" << endl;
    stream << t() << "static const int id = " << String::ToHexString(mVersionInfo[0].mAddress) << ";" << endl;

    t--;
    stream << t() << "META_END";
}
