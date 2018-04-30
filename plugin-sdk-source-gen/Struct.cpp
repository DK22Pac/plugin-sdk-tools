#include "Struct.h"
#include "Comments.h"
#include "StringEx.h"

bool Struct::Member::IsPadding() {
    return !strncmp(mName.c_str(), "_pad", 4) || !strncmp(mName.c_str(), "__pad", 5);
}

void Struct::Write(ofstream &stream, tabs t, Module const &myModule, List<Module> const &allModules, Games::IDs game) {
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
    if (!mParentName.empty())
        stream << ": public " << mParentName << ' ';
    stream << '{' << endl;
    if (mHasDefaultConstructor) {
        stream << "protected:" << endl;
        ++t;
        stream << t() << mName << "(plugin::dummy_func_t) ";
        if (!mParentName.empty())
            stream << ": " << mParentName << "(plugin::dummy) ";
        stream << "{}" << endl;
        --t;
        access = Access::Protected;
    }
    if (access != Access::Public) {
        stream << "public:" << endl;
        access = Access::Public;
    }
    ++t;
    unsigned int numWrittenMembers = 0;
    IterateFirstLast(mMembers, [&](Struct::Member &m, bool first, bool last) {
        if (m.mIgnore || (!mParentName.empty() && first))
            return;
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
    });
    if (access == Access::Private) {
        stream << endl << "public:" << endl;
        access = Access::Public;
    }
    if (numWrittenMembers > 0)
        stream << endl;
    // variables
    numWrittenMembers = 0;
    if (mVariables.size() > 0) {
        for (auto &v : mVariables) {
            v.WriteDeclaration(stream, t, game, true);
            stream << endl;
            ++numWrittenMembers;
        }
    }
    if (numWrittenMembers > 0)
        stream << endl;
    // functions
    numWrittenMembers = 0;
    if (mFunctions.size() > 0) {
        for (auto &f : mFunctions) {
            if (!f.mIgnore) {
                f.WriteDeclaration(stream, t, game);
                stream << endl;
                ++numWrittenMembers;
            }
        }
    }

    --t;
    stream << t() << '}' << ';';

    if (mHasVTable) {
        stream << endl;
        stream << endl;
        stream << "VTABLE_DESC(" << mName << ", " << String::ToHexString(mVTableAddress) << ", " << mVTableSize << ");";
    }

    if (mSize > 0) {
        stream << endl;
        stream << endl;
        stream << "VALIDATE_SIZE(" << mName << ", " << String::ToHexString(mSize) << ");";
    }
}

bool TypePresentCB(Type &t, string const &typeName) {
    if (t.mIsCustom && !t.mIsRenderWare && t.mName == typeName)
        return true;
    if (t.IsTemplate()) {
        for (Type &tt : t.mTemplateTypes) {
            if (TypePresentCB(tt, typeName))
                return true;
        }
    }
    if (t.mIsFunction) {
        for (Type &fp : t.mFunctionParams) {
            if (TypePresentCB(fp, typeName))
                return true;
        }
    }
    return false;
}

bool Struct::ContainsType(string const &typeName, bool withPointers) {
    for (Member &m : mMembers) {
        if (m.mType.mIsCustom && !m.mType.mIsRenderWare && m.mType.mName == typeName) {
            if (withPointers || !m.mType.IsPointer())
                return true;
        }
        if (withPointers && TypePresentCB(m.mType, typeName))
            return true;
    }
    return false;
}

string Struct::GetFullName() {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}

void Struct::AddFunction(Function &fn) {
    bool overloaded = false;
    for (auto &f : mFunctions) {
        if (f.GetFullName() == fn.GetFullName()) {
            if (!f.mIsOverloaded)
                f.mIsOverloaded = true;
            overloaded = true;
            break;
        }
    }
    if (fn.mIsVirtual) {
        if (!mHasVirtualFunctions)
            mHasVirtualFunctions = true;
        if (fn.IsDestructor())
            mHasVirtualDestructor = true;
    }
    Function &addedFn = mFunctions.emplace_back(fn);
    if (overloaded)
        addedFn.mIsOverloaded = true;
    addedFn.mClass = this;
    if (addedFn.mUsage == Function::Usage::DefaultConstructor)
        mHasDefaultConstructor = true;
    else if (addedFn.mUsage == Function::Usage::CustomConstructor)
        mHasCustomConstructors = true;
    else if (addedFn.mUsage == Function::Usage::BaseDestructor) {
        mDestructor = &addedFn;
        if (mDeletingDestructor)
            mDeletingDestructor->mIgnore = true;
    }
    else if (addedFn.mUsage == Function::Usage::DeletingDestructor) {
        mDeletingDestructor = &addedFn;
        if (mDestructor) {
            mDeletingDestructor->mIgnore = true;
        }
    }
}

void Struct::SetParent(Struct *parent) {
    mParent = parent;
    if (parent)
        parent->mChilds.push_back(this);
}
