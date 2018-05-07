#include "Struct.h"
#include "Comments.h"
#include "StringEx.h"
#include "Module.h"

void SetAccess(ofstream &stream, tabs t, Struct::Access &accessVar, Struct::Access newAccess) {
    if (accessVar != newAccess) {
        if (t.count() > 0)
            t--;
        stream << t();
        if (newAccess == Struct::Access::Private)
            stream << "private:";
        else if (newAccess == Struct::Access::Public)
            stream << "public:";
        if (newAccess == Struct::Access::Protected)
            stream << "protected:";
        stream << endl;
        accessVar = newAccess;
    }
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
    ++t;
    unsigned int numWrittenMembers = 0;
    bool makeNewLine = false;
    if (UsesCustomConstruction()) {
        SetAccess(stream, t, access, Access::Private);
        stream << t() << "PLUGIN_NO_DEFAULT_CONSTRUCTION(" << mName << ")" << endl;
        ++numWrittenMembers;
        makeNewLine = true;
    }
    
    IterateFirstLast(mMembers, [&](StructMember &m, bool first, bool last) {
        if (m.mIsBase)
            return;
        if (makeNewLine) {
            stream << endl;
            makeNewLine = false;
        }
        if (m.mIsVTable) {
            SetAccess(stream, t, access, Access::Private);
            stream << t() << "void *vtable;" << endl;
            ++numWrittenMembers;
            return;
        }
        if (m.mIsPadding)
            SetAccess(stream, t, access, Access::Private);
        else
            SetAccess(stream, t, access, Access::Public);
        stream << t();
        auto pos = stream.tellp();
        stream << m.mType.BeforeName() << m.mName << m.mType.AfterName() << ';';
        WriteComment(stream, m.mComment, t, stream.tellp() - pos);
        stream << endl;
        ++numWrittenMembers;
    });
    SetAccess(stream, t, access, Access::Public);
    // static variables
    unsigned int numWrittenVariables = 0;
    IterateFirstLast(mVariables, [&](Variable &v, bool first, bool last) {
        if (first && numWrittenMembers > 0)
            stream << endl;
        v.WriteDeclaration(stream, t, game, true);
        stream << endl;
        ++numWrittenVariables;
    });
    // functions
    WriteFunctions(stream, t, game, false, false, numWrittenMembers > 0 || numWrittenVariables > 0);
    --t;
    stream << t() << '}' << ';';

    if (mHasVTable) {
        stream << endl;
        stream << endl;
        stream << "VTABLE_DESC(" << GetFullName() << ", " << String::ToHexString(mVTableAddress) << ", " << mVTableSize << ");";
    }

    if (mSize > 0) {
        stream << endl;
        stream << endl;
        stream << "VALIDATE_SIZE(" << GetFullName() << ", " << String::ToHexString(mSize) << ");";
    }
}

void StartFunction(ofstream &stream, unsigned int &numWrittenFunctions, bool definitions, bool metadata, bool makeNewLine) {
    if (numWrittenFunctions == 0) {
        if (makeNewLine)
            stream << endl;
    }
    else if (definitions || metadata)
        stream << endl;
}

void EndFunction(ofstream &stream, unsigned int &numWrittenFunctions) {
    stream << endl;
    numWrittenFunctions++;
}

void WriteOneFunction(Function *fn, ofstream &stream, tabs t, Games::IDs game, unsigned int &numWrittenFunctions,
    bool definitions, bool metadata, bool makeNewLine)
{
    StartFunction(stream, numWrittenFunctions, definitions, metadata, makeNewLine);
    if (metadata)
        fn->WriteMeta(stream, t, game);
    else if (definitions)
        fn->WriteDefinition(stream, t, game);
    else
        fn->WriteDeclaration(stream, t, game);
    fn->mWrittenToSource = true;
    EndFunction(stream, numWrittenFunctions);
}

void StartBlock(ofstream &stream, unsigned int &numWrittenBlocks, bool definitions, bool metadata) {
    if (!definitions && !metadata && numWrittenBlocks > 0)
        stream << endl;
}

void EndBlock(unsigned int &numWrittenBlocks) {
    numWrittenBlocks++;
}

void WriteBlock(Function *fn, ofstream &stream, tabs t, Games::IDs game, unsigned int &numWrittenBlocks,
    unsigned int &numWrittenFunctions, bool definitions, bool metadata, bool makeNewLine)
{
    if (fn && !fn->mWrittenToSource) {
        StartBlock(stream, numWrittenBlocks, definitions, metadata);
        WriteOneFunction(fn, stream, t, game, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
        EndBlock(numWrittenBlocks);
    }
}

void WriteBlock(List<Function *> funcs, ofstream &stream, tabs t, Games::IDs game, unsigned int &numWrittenBlocks,
    unsigned int &numWrittenFunctions, bool definitions, bool metadata, bool makeNewLine)
{
    if (funcs.size() > 0) {
        unsigned int writtenCount = 0;
        bool first = true;
        for (auto fn : funcs) {
            if (!fn->mWrittenToSource) {
                if (first) {
                    StartBlock(stream, numWrittenBlocks, definitions, metadata);
                    first = false;
                }
                WriteOneFunction(fn, stream, t, game, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                writtenCount++;
            }
        }
        if (writtenCount > 0)
            EndBlock(numWrittenBlocks);
    }
}

unsigned int Struct::WriteFunctions(ofstream &stream, tabs t, Games::IDs game, bool definitions, bool metadata, bool makeNewLine) {
    for (auto &f : mFunctions)
        f.mWrittenToSource = false;
    unsigned int numWrittenFunctions = 0;
    unsigned int numWrittenBlocks = 0;
    if (metadata) {
        StartBlock(stream, numWrittenBlocks, definitions, metadata);
        for (auto &f : mFunctions)
            WriteOneFunction(&f, stream, t, game, numWrittenFunctions, definitions, metadata, makeNewLine);
        EndBlock(numWrittenBlocks);
    }
    else {
        if (mConstruction == Construction::Default || definitions) {
            WriteBlock(mDefaultConstructor, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
            WriteBlock(mCustomConstructors, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
            WriteBlock(mCopyConstructors, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
            WriteBlock(mBaseDestructor, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
            WriteBlock(mOperatorsNew, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
            WriteBlock(mOperatorsNewArray, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
            WriteBlock(mOperatorsDelete, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
            WriteBlock(mOperatorsDeleteArray, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
        }
        WriteBlock(mOperators, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
        // vtable
        unsigned int numWrittenVirtualFuncs = 0;
        if (mVTableSize > 0) {
            Vector<Function *> vtable(mVTableSize, nullptr);
            for (auto f : mVirtualFunctions)
                vtable[f->mVTableIndex] = f;
            if (!definitions) {
                StartBlock(stream, numWrittenBlocks, definitions, metadata);
                stream << t() << "/* virtual functions */" << endl;
                EndBlock(numWrittenBlocks);
            }
            unsigned int currIndex = 0;
            IterateIndex(vtable, [&](Function *f, int index) {
                if (f && f->IsDestructor()) {
                    if (!definitions) {
                        if (numWrittenVirtualFuncs == 0)
                            StartBlock(stream, numWrittenBlocks, definitions, metadata);
                        StartFunction(stream, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                        stream << t() << "// vtable function #" << index << " (destructor)";
                        EndFunction(stream, numWrittenFunctions);
                        numWrittenVirtualFuncs++;
                    }
                    else {
                        if (numWrittenVirtualFuncs == 0)
                            StartBlock(stream, numWrittenBlocks, definitions, metadata);
                        WriteOneFunction(f, stream, t, game, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                        numWrittenVirtualFuncs++;
                    }
                }
                else if (mParent && index < mParent->mVTableSize) {
                    if (f) {
                        if (numWrittenVirtualFuncs == 0)
                            StartBlock(stream, numWrittenBlocks, definitions, metadata);
                        WriteOneFunction(f, stream, t, game, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                        numWrittenVirtualFuncs++;
                    }
                    else if (!definitions) {
                        if (numWrittenVirtualFuncs == 0)
                            StartBlock(stream, numWrittenBlocks, definitions, metadata);
                        StartFunction(stream, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                        stream << t() << "// vtable function #" << index << " (not overloaded)";
                        EndFunction(stream, numWrittenFunctions);
                        numWrittenVirtualFuncs++;
                    }
                }
                else {
                    if (f) {
                        if (numWrittenVirtualFuncs == 0)
                            StartBlock(stream, numWrittenBlocks, definitions, metadata);
                        WriteOneFunction(f, stream, t, game, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                        numWrittenVirtualFuncs++;
                    }
                    else if (!definitions) {
                        // try to find this function in child classes
                        function<Function *(Struct *, int)> FindVFunc;
                        FindVFunc = [&](Struct *s, int vfi) -> Function * {
                            if (s != this) {
                                for (auto vf : s->mVirtualFunctions) {
                                    if (vf->mVTableIndex > vfi)
                                        break;
                                    else if (vf->mVTableIndex == vfi) {
                                        return vf;
                                    }
                                }
                            }
                            for (Struct *child : s->mChilds) {
                                Function *result = FindVFunc(child, vfi);
                                if (result)
                                    return result;
                            }
                            return nullptr;
                        };
                        Function *vf = FindVFunc(this, index);
                        if (vf) {
                            if (numWrittenVirtualFuncs == 0)
                                StartBlock(stream, numWrittenBlocks, definitions, metadata);
                            WriteOneFunction(f, stream, t, game, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                            numWrittenVirtualFuncs++;
                        }
                        else {
                            if (numWrittenVirtualFuncs == 0)
                                StartBlock(stream, numWrittenBlocks, definitions, metadata);
                            StartFunction(stream, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                            stream << t() << "// vtable function #" << index << " (not found)";
                            EndFunction(stream, numWrittenFunctions);
                            numWrittenVirtualFuncs++;
                        }
                    }
                }
            });
            if (numWrittenVirtualFuncs > 0)
                EndBlock(numWrittenBlocks);
            if (!definitions) {
                StartBlock(stream, numWrittenBlocks, definitions, metadata);
                stream << t() << "/* virtual functions - end */" << endl;
                EndBlock(numWrittenBlocks);
            }
        }
        for (auto sf : mNonStaticFunctions) {
            if (sf->mUsage != Function::Usage::Default)
                sf->mWrittenToSource = true;
        }
        WriteBlock(mNonStaticFunctions, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
        for (auto sf : mStaticFunctions) {
            if (sf->mUsage != Function::Usage::Default)
                sf->mWrittenToSource = true;
        }
        WriteBlock(mStaticFunctions, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine);
    }
    return numWrittenFunctions;
}

void Struct::WriteGeneratedConstruction(ofstream & stream, tabs t, Games::IDs game) {
    if (!UsesCustomConstruction())
        return;
    if (mDefaultConstructor || !mCustomConstructors.empty() || !mCopyConstructors.empty() || mBaseDestructor) {
        stream << t() << "template<>" << endl;
        stream << t() << "struct stack_object<" << GetFullName() << "> : stack_object_no_default<" << GetFullName() << "> {" << endl;
        ++t;
        
        // TODO: implement overloaded operators?
        --t;
    }

    template <>
    struct stack_object<CColBox> : stack_object_no_default<CColBox> {
        stack_object() {
            cout << "calling EXE CColBox::CColBox" << endl;
        }

        stack_object(void *pValue) {
            cout << "calling EXE CColBox::CColBox(void *)" << endl;
        }
    };
}

void Struct::OnUpdateStructs(List<Module> &modules) {
    auto SortByParameterCount = [](Function *f1, Function *f2) { return f1->mParameters.size() < f2->mParameters.size(); };
    auto SortByName = [](Function *f1, Function *f2) { return f1->mName < f2->mName; };
    auto SortByVTableIndex = [](Function *f1, Function *f2) { return f1->mVTableIndex < f2->mVTableIndex; };

    if (mCustomConstructors.size() > 0)
        mCustomConstructors.sort(SortByParameterCount);
    if (mCopyConstructors.size() > 0)
        mCopyConstructors.sort(SortByParameterCount);
    if (mOperatorsNew.size() > 0)
        mOperatorsNew.sort(SortByParameterCount);
    if (mOperatorsNewArray.size() > 0)
        mOperatorsNewArray.sort(SortByParameterCount);
    if (mOperatorsDelete.size() > 0)
        mOperatorsDelete.sort(SortByParameterCount);
    if (mOperatorsDeleteArray.size() > 0)
        mOperatorsDeleteArray.sort(SortByParameterCount);
    if (mOperators.size() > 0)
        mOperators.sort(SortByName);
    if (mVirtualFunctions.size() > 0)
        mVirtualFunctions.sort(SortByVTableIndex);
    if (mNonStaticFunctions.size() > 0)
        mNonStaticFunctions.sort(SortByName);
    if (mStaticFunctions.size() > 0)
        mStaticFunctions.sort(SortByName);

    for (auto &m : mMembers) {
        if (!m.mIsVTable && m.mType.mIsCustom && m.mType.mPointers.size() == 0 && !m.mType.mIsFunction && !m.mType.mIsTemplate) {
            Struct *ms = nullptr;
            for (auto &mdl : modules) {
                ms = mdl.FindStruct(m.mType.mName, true);
                if (ms)
                    break;
            }
            if (ms && ms != this)
                mMemberClasses.insert(ms);
        }
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
    for (StructMember &m : mMembers) {
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

void Struct::AddFunction(Function &func_to_add) {
    bool overloaded = false;
    for (auto &f : mFunctions) {
        if (f.GetFullName() == func_to_add.GetFullName()) {
            if (!f.mIsOverloaded)
                f.mIsOverloaded = true;
            overloaded = true;
            break;
        }
    }
    Function &fn = mFunctions.emplace_back(func_to_add);
    if (overloaded)
        fn.mIsOverloaded = true;

    fn.mClass = this;
    if (fn.mIsVirtual) {
        if (!mHasVirtualFunctions)
            mHasVirtualFunctions = true;
        if (fn.IsDestructor())
            mHasVirtualDestructor = true;
        mVirtualFunctions.push_back(&fn);
    }
    if (fn.mIsStatic)
        mStaticFunctions.push_back(&fn);
    else
        mNonStaticFunctions.push_back(&fn);
    if (fn.mUsage == Function::Usage::DefaultConstructor) {
        if (!mDefaultConstructor)
            mDefaultConstructor = &fn;
    }
    else if (fn.mUsage == Function::Usage::CustomConstructor)
        mCustomConstructors.push_back(&fn);
    else if (fn.mUsage == Function::Usage::CopyConstructor)
        mCopyConstructors.push_back(&fn);
    else if (fn.mUsage == Function::Usage::BaseDestructor) {
        if (!mBaseDestructor)
            mBaseDestructor = &fn;
    }
    else if (fn.mUsage == Function::Usage::DeletingDestructor) {
        if (!mDeletingDestructor)
            mDeletingDestructor = &fn;
    }
    else if (fn.mUsage == Function::Usage::DefaultOperatorNew || fn.mUsage == Function::Usage::CustomOperatorNew)
        mOperatorsNew.push_back(&fn);
    else if (fn.mUsage == Function::Usage::DefaultOperatorNewArray || fn.mUsage == Function::Usage::CustomOperatorNewArray)
        mOperatorsNewArray.push_back(&fn);
    else if (fn.mUsage == Function::Usage::DefaultOperatorDelete || fn.mUsage == Function::Usage::CustomOperatorDelete)
        mOperatorsDelete.push_back(&fn);
    else if (fn.mUsage == Function::Usage::DefaultOperatorDeleteArray || fn.mUsage == Function::Usage::CustomOperatorDeleteArray)
        mOperatorsDeleteArray.push_back(&fn);
    else if (fn.mUsage == Function::Usage::Operator)
        mOperators.push_back(&fn);
}

void Struct::SetParent(Struct *parent) {
    mParent = parent;
    if (parent)
        parent->mChilds.push_back(this);
}

bool Struct::UsesCustomConstruction() {
    if (mConstruction != Construction::Unknown)
        return mConstruction == Construction::Custom;
    if (mHasVirtualFunctions) {
        mConstruction = Construction::Custom;
        return true;
    }
    if (mIsCoreClass) {
        mConstruction = Construction::Default;
        return false;
    }
    if (mDefaultConstructor || mCustomConstructors.size() > 0 || mCopyConstructors.size() > 0 || mBaseDestructor || mDeletingDestructor) {
        mConstruction = Construction::Custom;
        return true;
    }
    bool parentUsesCustomConstructor = mParent && mParent->UsesCustomConstruction();
    if (parentUsesCustomConstructor)
        return true;
    bool memberUsesCustomConstructor = false;
    for (auto ms : mMemberClasses) {
        memberUsesCustomConstructor = ms->UsesCustomConstruction();
        if (memberUsesCustomConstructor)
            return true;
    }
    return false;
}
