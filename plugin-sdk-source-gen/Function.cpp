#include "Function.h"
#include "Comments.h"

string Function::GetFullName() const {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}

void Function::ForAllParameters(std::function<void(Parameter &p, bool first, bool last)> callback) {
    size_t length = mParameters.size();
    for (size_t i = 0; i < length; i++)
        callback(mParameters[i], i == 0, i == (length - 1));
}

void Function::ForAllParameters(std::function<void(Parameter &p)> callback) {
    for (size_t i = 0; i < mParameters.size(); i++)
        callback(mParameters[i]);
}

void Function::ForAllParameters(std::function<void(Parameter &p, unsigned int index)> callback) {
    for (size_t i = 0; i < mParameters.size(); i++)
        callback(mParameters[i], i);
}

//string Function::GetPtrTypeStr() const {
//    string result = mRetType.GetFullType() + "(";
//    bool first 
//    for (size_t i = startParam; i < mParameters.size(); i++) {
//        auto const &p = mParameters[i];
//        if (!first)
//            stream << ", ";
//        else
//            first = false;
//        result += p.mType.GetFullType();
//    }
//    return result;
//}

void Function::WriteDefinition(ofstream &stream, tabs t, Games::IDs game) {
    
}

void Function::WriteDeclaration(ofstream &stream, tabs t, Games::IDs game) {
    WriteComment(stream, mComment, t, 0);
    stream << t();
    stream << Games::GetSupportedGameVersionsMacro(game, mVersionInfo) << ' ';
    bool isStatic = mCC != CC_THISCALL;
    if (isStatic) // MAYBE also check for first parameter type
        stream << "static" << ' ';
    stream << mRetType.GetFullType() << mName << '(';
    size_t startParam = 0;
    if (!isStatic)
        startParam = 1;

    bool first = true;
    for (size_t i = startParam; i < mParameters.size(); i++) {
        auto const &p = mParameters[i];
        if (!first)
            stream << ", ";
        else
            first = false;
        stream << p.mType.BeforeName() << p.mName << p.mType.AfterName();
    }
    stream << ");";
}

void Function::WriteMeta(ofstream &stream, tabs t, Games::IDs game) {
    stream << t() << "META_BEGIN(" << GetFullName() << ")" << endl;
    t++;
    stream << t() << "static int address;" << endl;
    stream << t() << "static const int id = " << String::ToHexString(mVersionInfo[0].mAddress) << ";" << endl;

    t--;
    stream << t() << "META_END";
}
