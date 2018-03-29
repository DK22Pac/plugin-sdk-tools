#include "Struct.h"
#include "Comments.h"
#include "StringEx.h"

bool Struct::Member::IsPadding() {
    return !strncmp(mName.c_str(), "_pad", 4) || !strncmp(mName.c_str(), "__pad", 5);
}

void Struct::Write(ofstream &stream, tabs t, Module const &myModule, vector<Module> const &allModules, Games::IDs game) {
    WriteComment(stream, mComment, t, 0);
    stream << t();
    Access access = Access::Public;
    if (mKind == Kind::Struct)
        stream << "struct";
    else if (mKind == Kind::Union)
        stream << "union";
    else {
        stream << "class";
        access = Access::Private;
    }
    stream << ' ';
    if (!mIsAnonymous)
        stream << "PLUGIN_API" << ' ' << mName << ' ';
    if (!mParent.empty())
        stream << ": public " << mParent << ' ';
    stream << '{' << endl;
    if (access == Access::Private) {
        stream << "public:" << endl;
        access = Access::Public;
    }
    ++t;
    unsigned int numWrittenMembers = 0;
    for (unsigned int i = 0; i < mMembers.size(); i++) {
        if (!mParent.empty() && i == 0)
            continue;
        Struct::Member &m = mMembers[i];
        if (m.IsPadding()) {
            if (access == Access::Public) {
                stream << endl << "private:";
                access = Access::Private;
            }
        }
        else {
            if (access == Access::Private) {
                stream << endl << "public:";
                access = Access::Public;
            }
        }
        if (numWrittenMembers != 0)
            stream << endl;
        stream << t();
        auto pos = stream.tellp();
        stream << m.mType.BeforeName() << m.mName << m.mType.AfterName() << ';';
        WriteComment(stream, m.mComment, t, stream.tellp() - pos);
        ++numWrittenMembers;
    }
    if (access == Access::Private) {
        stream << endl << "public:" << endl;
        access = Access::Public;
    }
    if (numWrittenMembers > 0)
        stream << endl;
    // variables
    if (mVariables.size() > 0) {
        for (unsigned int i = 0; i < mVariables.size(); i++) {
            mVariables[i].WriteDeclaration(stream, t, game, true);
            stream << endl;
            ++numWrittenMembers;
        }
    }
    if (numWrittenMembers > 0)
        stream << endl;
    // functions
    if (mFunctions.size() > 0) {
        for (unsigned int i = 0; i < mFunctions.size(); i++) {
            mFunctions[i].WriteDeclaration(stream, t, game);
            stream << endl;
            ++numWrittenMembers;
        }
    }

    --t;
    stream << t() << '}' << ';';
    if (mSize > 0) {
        stream << endl;
        stream << endl;
        stream << "VALIDATE_SIZE(" << mName << ',' << ' ' << String::ToHexString(mSize) << ')' << ';';
    }
}

bool TypePresentCB(Type const &t, string const &typeName) {
    if (t.mIsCustom && !t.mIsRenderWare && t.mName == typeName)
        return true;
    if (t.IsTemplate()) {
        for (Type const &tt : t.mTemplateTypes) {
            if (TypePresentCB(tt, typeName))
                return true;
        }
    }
    if (t.mIsFunction) {
        for (Type const &fp : t.mFunctionParams) {
            if (TypePresentCB(fp, typeName))
                return true;
        }
    }
    return false;
}

bool Struct::ContainsType(string const &typeName, bool withPointers) const {
    for (Member const &m : mMembers) {
        if (m.mType.mIsCustom && !m.mType.mIsRenderWare && m.mType.mName == typeName) {
            if (withPointers || !m.mType.IsPointer())
                return true;
        }
        if (withPointers && TypePresentCB(m.mType, typeName))
            return true;
    }
    return false;
}

string Struct::GetFullName() const {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}

void Struct::AddFunction(Function const &fn) {
    bool overloaded = false;
    for (auto &f : mFunctions) {
        if (f.GetFullName() == fn.GetFullName()) {
            if (!f.mIsOverloaded)
                f.mIsOverloaded = true;
            overloaded = true;
            break;
        }
    }
    Function &addedFn = mFunctions.emplace_back(fn);
    if (overloaded)
        addedFn.mIsOverloaded = true;
}
