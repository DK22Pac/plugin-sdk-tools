#include "Struct.h"
#include "Comments.h"
#include "StringEx.h"
#include "Module.h"
#include "GameVersions.h"

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

void Struct::Write(ofstream &stream, tabs t, Module &myModule, List<Module> const &allModules, Games::IDs game) {
    WriteComment(stream, mComment, t, 0);
    
    bool isPacked = (mSize % 4) != 0;
    if (isPacked)
        stream << t() << "#pragma pack(push, 1)" << endl;
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
    if (UsesCustomConstruction() || mHasVTableMember) {
        SetAccess(stream, t, access, Access::Private);
        stream << t() << "PLUGIN_NO_DEFAULT_CONSTRUCTION";
        if (mHasVTableMember)
            stream << "_VIRTUALBASE";
        stream << "(" << mName << ")" << endl;
        ++numWrittenMembers;
        makeNewLine = true;
    }

    auto numNamedNestedClasses = count_if(mNestedClasses.begin(), mNestedClasses.end(), [](Struct *s) { return !s->mIsAnonymous; });

    if (numNamedNestedClasses > 0) {
        if (makeNewLine) {
            stream << endl;
            makeNewLine = false;
        }
        SetAccess(stream, t, access, Access::Public);
        for (auto nested : mNestedClasses) {
            if (!nested->mIsAnonymous) {
                nested->Write(stream, t, myModule, allModules, game);
                stream << endl;
                ++numWrittenMembers;
                makeNewLine = true;
            }
        }
    }
    
    bool writingBitfield = false;
    IterateFirstLast(mMembers, [&](StructMember &m, bool first, bool last) {
        if (m.mIsBase || m.mIsVTable)
            return;
        if (makeNewLine) {
            stream << endl;
            makeNewLine = false;
        }
        if (m.mIsPadding)
            SetAccess(stream, t, access, Access::Private);
        else
            SetAccess(stream, t, access, Access::Public);
        auto pos = stream.tellp();
        if (m.mBitfield) {
            //if (!writingBitfield) {
                stream << t() << "struct {" << endl;
                ++t;
            //}
            m.mBitfield->WriteBitfield(stream, t, m.mIsAnonymous);

            // write bitfield ending
            --t;
            stream << t() << '}';
            if (!m.mIsAnonymous)
                stream << ' ' << m.mName;
            stream << ';';
            WriteComment(stream, m.mComment, t, stream.tellp() - pos);
            writingBitfield = true;
        }
        else {
            bool anonymousType = false;
            if (m.mType.mIsCustom) {
                auto typeStruct = myModule.FindStruct(m.mType.mName, true);
                if (typeStruct && typeStruct->mIsAnonymous) {
                    typeStruct->Write(stream, t, myModule, allModules, game);
                    anonymousType = true;
                }
            }
            if (!anonymousType)
                stream << t() << m.mType.BeforeName();
            else if (!m.mIsAnonymous)
                stream << ' ';
            if (!m.mIsAnonymous)
                stream << m.mName;
            stream << m.mType.AfterName() << ';';
            WriteComment(stream, m.mComment, t, stream.tellp() - pos);
            writingBitfield = false;
        }
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
    stream << t() << '}';
    if (!mIsAnonymous)
        stream << ';';

    if (isPacked)
        stream << endl << t() << "#pragma pack(pop)";
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
    bool definitions, bool metadata, bool makeNewLine, Function::Flags flags = Function::Flags())
{
    StartFunction(stream, numWrittenFunctions, definitions, metadata, makeNewLine);
    if (metadata)
        fn->WriteMeta(stream, t, game);
    else if (definitions)
        fn->WriteDefinition(stream, t, game, flags);
    else
        fn->WriteDeclaration(stream, t, game, flags);
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
    unsigned int &numWrittenFunctions, bool definitions, bool metadata, bool makeNewLine, Function::Flags flags = Function::Flags())
{
    if (fn && (flags.Empty() || !fn->mWrittenToSource)) {
        StartBlock(stream, numWrittenBlocks, definitions, metadata);
        WriteOneFunction(fn, stream, t, game, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine, flags);
        EndBlock(numWrittenBlocks);
    }
}

void WriteBlock(List<Function *> funcs, ofstream &stream, tabs t, Games::IDs game, unsigned int &numWrittenBlocks,
    unsigned int &numWrittenFunctions, bool definitions, bool metadata, bool makeNewLine, Function::Flags flags = Function::Flags())
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
                WriteOneFunction(fn, stream, t, game, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine, flags);
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
        IterateIndex(mVTable, [&](VTableMethod &vm, int index) {
            Function *funcToWrite = nullptr;
            string funcReplacement;
            Struct *oldClass = nullptr; string oldScope, oldFullClassName, oldShortClassName, oldRetTypeName;
            bool resetCloneFunc = false;

            if (vm.mUnique || (!mParent || index >= static_cast<int>(mParent->mVTableSize))) {
                if (vm.mFunc) {
                    if (vm.mFunc->IsDestructor() && !definitions)
                        funcReplacement = "destructor";
                    else {
                        funcToWrite = vm.mFunc;
                        if (vm.mPureOverriden) {
                            oldScope = funcToWrite->mScope;
                            oldClass = funcToWrite->mClass;
                            oldFullClassName = funcToWrite->mFullClassName;
                            oldShortClassName = funcToWrite->mShortClassName;
                            funcToWrite->mScope = GetFullName();
                            funcToWrite->mClass = this;
                            funcToWrite->mFullClassName = GetFullName();
                            funcToWrite->mShortClassName = mName;
                            if (funcToWrite->mName == "Clone") {
                                oldRetTypeName = funcToWrite->mRetType.mName;
                                funcToWrite->mRetType.mName = GetFullName();
                                resetCloneFunc = true;
                            }
                        }
                    }
                }
                else
                    funcReplacement = "not found";
            }
            else
                funcReplacement = "not overriden";
            
            if (funcToWrite || (!definitions && !funcReplacement.empty())) {
                if (numWrittenVirtualFuncs == 0)
                    StartBlock(stream, numWrittenBlocks, definitions, metadata);
                if (funcToWrite) {
                    Function::Flags flags;
                    if (vm.mPureOverriden)
                        flags.OverridenVirtualFunc = true;
                    WriteOneFunction(funcToWrite, stream, t, game, numWrittenFunctions, definitions, metadata,
                        numWrittenBlocks == 0 && makeNewLine, flags);
                }
                else {
                    StartFunction(stream, numWrittenFunctions, definitions, metadata, numWrittenBlocks == 0 && makeNewLine);
                    if (numWrittenVirtualFuncs != 0)
                        stream << endl;
                    stream << t() << "// virtual function #" << index << " (" << funcReplacement << ")" << endl;
                    EndFunction(stream, numWrittenFunctions);
                }
                numWrittenVirtualFuncs++;
            }

            if (funcToWrite) {
                if (vm.mPureOverriden) {
                    funcToWrite->mScope = oldScope;
                    funcToWrite->mClass = oldClass;
                    funcToWrite->mFullClassName = oldFullClassName;
                    funcToWrite->mShortClassName = oldShortClassName;
                }
                if (resetCloneFunc)
                    funcToWrite->mRetType.mName = oldRetTypeName;
            }
        });
        if (numWrittenVirtualFuncs > 0)
            EndBlock(numWrittenBlocks);

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

        // Write WS functions
        List<Function *> wsFunctions;
        for (auto &f : mFunctions) {
            if (f.mHasWSParameters)
                wsFunctions.push_back(&f);
        }
        Function::Flags flags;
        flags.OverloadedWideStringFunc = true;
        WriteBlock(wsFunctions, stream, t, game, numWrittenBlocks, numWrittenFunctions, definitions, metadata, makeNewLine, flags);
    }
    return numWrittenFunctions;
}

void Struct::WriteStackObjectFunction(ofstream & stream, tabs t, Games::IDs game, Function *fn) {
    stream << t();
    string fnName;
    if (fn->IsDestructor())
        fnName = "~";
    fnName += "stack_object";
    stream << fn->NameForWrapper(game, false, fnName) << " {" << endl;
    ++t;
    fn->WriteFunctionCall(stream, t, game, true, Function::SpecialCall::StackObject);
    --t;
    stream << t() << "}" << endl;
}

void AddOperatorNewToUniqueList(List<Function *> &uniqueList, Function *fn) {
    // check if we already have function with such signature (actually, we check parameters only)

    auto CompareFunctions = [](Function *f1, Function *f2) {
        if (f1->mParameters.size() == f2->mParameters.size()) {
            for (size_t i = 0; i < f1->mParameters.size(); i++) {
                if (f1->mParameters[i].mType.GetFullType(false) != f2->mParameters[i].mType.GetFullType(false))
                    return false;
            }
            return true;
        }
        return false;
    };

    for (auto f : uniqueList) {
        if (CompareFunctions(f, fn))
            return;
    }

    uniqueList.push_back(fn);
}

void Struct::WriteCustomOperatorNewFunction(ofstream & stream, tabs t, Games::IDs game, Function *ctor, bool writeArrayNew) {
    for (int i = 0; i < (writeArrayNew + 1); i++) {
        bool isArray = i == 1;
        List<Function *> opNewList;
        bool hasDefaultOperatorNew = false;
        Function *Struct::*pmDefaultOperatorNew;
        List<Function *> Struct::*pmOperatorsNew;
        if (isArray) {
            pmDefaultOperatorNew = &Struct::mDefaultOperatorNewArray;
            pmOperatorsNew = &Struct::mOperatorsNewArray;
        }
        else {
            pmDefaultOperatorNew = &Struct::mDefaultOperatorNew;
            pmOperatorsNew = &Struct::mOperatorsNew;
        }
        if (this->*pmDefaultOperatorNew) {
            AddOperatorNewToUniqueList(opNewList, this->*pmDefaultOperatorNew);
            hasDefaultOperatorNew = true;
        }
        for (auto n : this->*pmOperatorsNew)
            AddOperatorNewToUniqueList(opNewList, this->*pmDefaultOperatorNew);
        // now for all hierarchy, find non-overloaded operators
        Struct *parent = mParent;
        while (parent) {
            if (parent->*pmDefaultOperatorNew) {
                AddOperatorNewToUniqueList(opNewList, parent->*pmDefaultOperatorNew);
                if (!hasDefaultOperatorNew)
                    hasDefaultOperatorNew = true;
            }
            for (auto n : parent->*pmOperatorsNew)
                AddOperatorNewToUniqueList(opNewList, n);
            parent = parent->mParent;
        }
        if (!hasDefaultOperatorNew)
            opNewList.push_back(nullptr);

        for (auto n : opNewList) {
            stream << t() << "template <>" << endl;
            stream << t();
            if (n)
                stream << GameVersions::GetSupportedGameVersionsMacro(game, ctor->mVersionInfo, n->mVersionInfo);
            else
                stream << GameVersions::GetSupportedGameVersionsMacro(game, ctor->mVersionInfo);
            stream << " inline " << GetFullName() << " *operator_new";
            if (isArray)
                stream << "_array";
            stream << "<" << GetFullName() << ">(";
            bool firstParam = true;
            if (isArray) {
                stream << "unsigned int objCount";
                firstParam = false;
            }
            if (n && n->mParameters.size() > 1) {
                // custom operator new parameters
                Iterate(n->mParameters, [&](FunctionParameter &p) {
                    if (firstParam)
                        firstParam = false;
                    else
                        stream << ", ";
                    stream << p.mType.BeforeName() << p.mName << p.mType.AfterName();
                }, 1);
            }
            if (ctor->mParameters.size() > 1) {
                // custom constructor parameters
                Iterate(ctor->mParameters, [&](FunctionParameter &p) {
                    if (firstParam)
                        firstParam = false;
                    else
                        stream << ", ";
                    stream << p.mType.BeforeName() << p.mName << p.mType.AfterName();
                }, 1);
            }
            stream << ") {" << endl;
            ++t;
            // allocate data
            stream << t() << "void *objData = ";
            if (n) {
                Function::SpecialData spdata;
                spdata.mClassNameForOpNewDelete = GetFullName();
                n->WriteFunctionCall(stream, tabs(0), game, false, isArray ?
                    Function::SpecialCall::Custom_Array_OperatorNew : Function::SpecialCall::Custom_OperatorNew, spdata);
            }
            else {
                stream << "operator new(sizeof(" << GetFullName() << ")";
                if (isArray)
                    stream << " * objCount + 4";
                stream << "); " << endl;
            }
            // construct object (s)
            if (isArray) {
                stream << t() << "*reinterpret_cast<unsigned int *>(objData) = objCount;" << endl;
                stream << t() << GetFullName() << " *objArray = reinterpret_cast<" << GetFullName() <<
                    " *>(reinterpret_cast<unsigned int>(objData) + 4);" << endl;
                stream << t() << "for (unsigned int i = 0; i < objCount; i++)" << endl;
                ++t;
            }
            else
                stream << t() << GetFullName() << " *obj = reinterpret_cast<" << GetFullName() << " *>(objData);" << endl;
            ctor->WriteFunctionCall(stream, t, game, false, isArray?
                Function::SpecialCall::Custom_Array_Constructor : Function::SpecialCall::Custom_Constructor);
            if (isArray)
                --t;
            stream << t() << "return obj";
            if (isArray)
                stream << "Array";
            stream << ";" << endl;
            --t;
            stream << t() << "}" << endl;
        }
    }
}

void Struct::WriteCustomOperatorDeleteFunction(ofstream & stream, tabs t, Games::IDs game) {
    Function *dtor = mDeletingDestructor;
    if (!dtor)
        dtor = mBaseDestructor;
    Function *opDelete = mDefaultOperatorDelete;
    bool usesDeletingDestructor = dtor == mDeletingDestructor;
    if (!usesDeletingDestructor) {
        Struct *parent = mParent;
        while (!opDelete && parent) {
            opDelete = parent->mDefaultOperatorDelete;
            parent = parent->mParent;
        }
    }
    stream << t() << "template <>" << endl;
    stream << t();
    if (!usesDeletingDestructor && opDelete)
        stream << GameVersions::GetSupportedGameVersionsMacro(game, dtor->mVersionInfo, opDelete->mVersionInfo);
    else
        stream << GameVersions::GetSupportedGameVersionsMacro(game, dtor->mVersionInfo);
    stream << " inline void operator_delete<";
    stream << GetFullName() << ">(" << GetFullName() << " *obj) {" << endl;
    ++t;
    stream << t() << "if (obj == nullptr) return;" << endl;
    if (usesDeletingDestructor)
        dtor->WriteFunctionCall(stream, t, game, false, Function::SpecialCall::Custom_DeletingDestructor);
    else {
        dtor->WriteFunctionCall(stream, t, game, false, Function::SpecialCall::Custom_BaseDestructor);
        if (opDelete)
            opDelete->WriteFunctionCall(stream, t, game, false, Function::SpecialCall::Custom_OperatorDelete);
        else
            stream << t() << "operator delete(obj);" << endl;
    }
    --t;
    stream << t() << "}" << endl;
}

void Struct::WriteCustomOperatorDeleteArrayFunction(ofstream & stream, tabs t, Games::IDs game) {
    Function *dtor = mDeletingDestructor;
    if (!dtor)
        dtor = mBaseDestructor;
    bool usesDeletingDestructor = dtor == mDeletingDestructor && mHasVectorDeletingDestructor;
    Function *opDeleteArray = mDefaultOperatorDeleteArray;
    if (!usesDeletingDestructor) {
        Struct *parent = mParent;
        while (!opDeleteArray && parent) {
            opDeleteArray = parent->mDefaultOperatorDeleteArray;
            parent = parent->mParent;
        }
    }
    stream << t() << "template <>" << endl;
    stream << t();
    if (!usesDeletingDestructor && opDeleteArray)
        stream << GameVersions::GetSupportedGameVersionsMacro(game, dtor->mVersionInfo, opDeleteArray->mVersionInfo);
    else
        stream << GameVersions::GetSupportedGameVersionsMacro(game, dtor->mVersionInfo);
    stream << " inline void operator_delete_array<";
    stream << GetFullName() << ">(" << GetFullName() << " *objArray) {" << endl;
    ++t;
    stream << t() << "if (objArray == nullptr) return;" << endl;
    if (usesDeletingDestructor)
        dtor->WriteFunctionCall(stream, t, game, false, Function::SpecialCall::Custom_Array_DeletingArrayDestructor);
    else {
        stream << t() << "void *objData = reinterpret_cast<void *>(reinterpret_cast<char *>(objArray) - 4);" << endl;
        stream << t() << "unsigned int arraySize = *reinterpret_cast<unsigned int *>(objData);" << endl;
        stream << t() << "for (unsigned int i = 0; i < arraySize; i++)" << endl;
        ++t;
        dtor->WriteFunctionCall(stream, t, game, false, dtor == mDeletingDestructor ?
            Function::SpecialCall::Custom_Array_DeletingDestructor :
            Function::SpecialCall::Custom_Array_BaseDestructor);
        --t;
        if (opDeleteArray)
            opDeleteArray->WriteFunctionCall(stream, t, game, false, Function::SpecialCall::Custom_Array_OperatorDelete);
        else
            stream << t() << "operator delete(objData);" << endl;
    }
    --t;
    stream << t() << "}" << endl;
}

void Struct::WriteGeneratedConstruction(ofstream & stream, tabs t, Games::IDs game) {
    if (!UsesCustomConstruction() || mIsAbstractClass)
        return;
    bool hasStackObject = false;
    if (mDefaultConstructor || !mCustomConstructors.empty() || !mCopyConstructors.empty() || mBaseDestructor) {
        stream << endl;
        stream << t() << "template<>" << endl;
        stream << t() << "struct stack_object<" << GetFullName() << "> : stack_object_no_default<" << GetFullName() << "> {" << endl;
        ++t;
        if (mDefaultConstructor)
            WriteStackObjectFunction(stream, t, game, mDefaultConstructor);
        for (auto fn : mCustomConstructors)
            WriteStackObjectFunction(stream, t, game, fn);
        for (auto fn : mCopyConstructors)
            WriteStackObjectFunction(stream, t, game, fn);
        if (mBaseDestructor)
            WriteStackObjectFunction(stream, t, game, mBaseDestructor);

        // maybe also implement overloaded operators?
        --t;
        stream << t() << "};" << endl;
        hasStackObject = true;
    }
    if (mDefaultConstructor || !mCustomConstructors.empty() || !mCopyConstructors.empty() || mBaseDestructor || mDeletingDestructor) {
        stream << endl;
        if (mDefaultConstructor)
            WriteCustomOperatorNewFunction(stream, t, game, mDefaultConstructor, true);
        for (auto c : mCustomConstructors)
            WriteCustomOperatorNewFunction(stream, t, game, c, false);
        for (auto c : mCopyConstructors)
            WriteCustomOperatorNewFunction(stream, t, game, c, false);
        if (mBaseDestructor || mDeletingDestructor) {
            WriteCustomOperatorDeleteFunction(stream, t, game);
            WriteCustomOperatorDeleteArrayFunction(stream, t, game);
        }
    }
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

    CreateVTable();
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
    else if (fn.mUsage == Function::Usage::DefaultOperatorNew || fn.mUsage == Function::Usage::CustomOperatorNew) {
        mOperatorsNew.push_back(&fn);
        if (fn.mUsage == Function::Usage::DefaultOperatorNew && !mDefaultOperatorNew)
            mDefaultOperatorNew = &fn;
    }
    else if (fn.mUsage == Function::Usage::DefaultOperatorNewArray || fn.mUsage == Function::Usage::CustomOperatorNewArray) {
        mOperatorsNewArray.push_back(&fn);
        if (fn.mUsage == Function::Usage::DefaultOperatorNewArray && !mDefaultOperatorNewArray)
            mDefaultOperatorNewArray = &fn;
    }
    else if (fn.mUsage == Function::Usage::DefaultOperatorDelete || fn.mUsage == Function::Usage::CustomOperatorDelete) {
        mOperatorsDelete.push_back(&fn);
        if (fn.mUsage == Function::Usage::DefaultOperatorDelete && !mDefaultOperatorDelete)
            mDefaultOperatorDelete = &fn;
    }
    else if (fn.mUsage == Function::Usage::DefaultOperatorDeleteArray || fn.mUsage == Function::Usage::CustomOperatorDeleteArray) {
        mOperatorsDeleteArray.push_back(&fn);
        if (fn.mUsage == Function::Usage::DefaultOperatorDeleteArray && !mDefaultOperatorDeleteArray)
            mDefaultOperatorDeleteArray = &fn;
    }
    else if (fn.mUsage == Function::Usage::Operator)
        mOperators.push_back(&fn);
}

void Struct::SetParent(Struct *parent) {
    mParent = parent;
    if (parent)
        parent->mChilds.push_back(this);
}

void Struct::SetEnclose(Struct * enclose) {
    mEncloseClass = enclose;
    if (enclose)
        enclose->mNestedClasses.push_back(this);
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
    if (mDefaultConstructor || !mCustomConstructors.empty() || !mCopyConstructors.empty() || mBaseDestructor || mDeletingDestructor) {
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

void Struct::WriteStructExtraInfo(ofstream & stream) {
    if (mHasVTable)
        stream << "VTABLE_DESC(" << GetFullName() << ", " << String::ToHexString(mVTableAddress) << ", " << mVTableSize << ");" << endl;
    if (mSize > 0)
        stream << "VALIDATE_SIZE(" << GetFullName() << ", " << String::ToHexString(mSize) << ");" << endl;
}

void Struct::CreateVTable() {
    if (mCreatedOrCheckedVTable)
        return;
    if (mParent)
        mParent->CreateVTable();
    if (mVTableSize > 0) {
        if (mParent && mParent->mVTable.size() > 0) {
            mVTable = mParent->mVTable;
            for (auto &vm : mVTable)
                vm.mUnique = false;
            if (mVTableSize > mParent->mVTable.size())
                mVTable.resize(mVTableSize);
        }
        else
            mVTable.resize(mVTableSize);
        for (auto vf : mVirtualFunctions) {
            unsigned int vtIndex = static_cast<unsigned int >(vf->mVTableIndex);
            mVTable[vtIndex].mFunc = vf;
            mVTable[vtIndex].mUnique = true;
            auto parent = mParent;
            while (parent && parent->mVTable.size() > vtIndex && parent->mVTable[vtIndex].mFunc == nullptr) {
                parent->mVTable[vtIndex].mFunc = vf;
                parent->mVTable[vtIndex].mPureOverriden = true;
                parent = parent->mParent;
            }
        }
    }
    mCreatedOrCheckedVTable = true;
}
