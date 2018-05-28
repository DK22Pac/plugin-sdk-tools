#include "..\shared\Utility.h"
#include "Module.h"
#include <iostream>
#include "Comments.h"
#include "StringEx.h"
#include <unordered_set>

Module *Module::Find(List<Module> &modules, string const &name) {
    for (auto &m : modules) {
        if (m.mName == name)
            return &m;
        ;
    }
    return nullptr;
}

Struct *Module::FindStruct(string const &name, bool bFullName) {
    for (auto &s : mStructs) {
        string structName;
        if (bFullName)
            structName = s.GetFullName();
        else
            structName = s.mName;
        if (structName == name)
            return &s;
    }
    return nullptr;
}

Enum *Module::FindEnum(string const & name, bool bFullName) {
    for (auto &e : mEnums) {
        string enumName;
        if (bFullName)
            enumName = e.GetFullName();
        else
            enumName = e.mName;
        if (enumName == name)
            return &e;
    }
    return nullptr;
}

void AddScopeStruct(Module *m, string const &scope, Struct *struc) {
    if (scope.empty())
        return;
    string newScope, className;
    String::Break(scope, "::", newScope, className, true);
    Struct *ss = m->FindStruct(scope, true);
    if (!ss)
        ss = m->AddEmptyStruct(className, newScope);
    struc->SetEnclose(ss);
}

Struct *Module::AddEmptyStruct(string const &name, string const &scope) {
    Struct s;
    s.mName = name;
    if (s.mName.find('$') != string::npos)
        s.mIsAnonymous = true;
    s.mScope = scope;
    s.mModuleName = mName;
    s.mModule = this;
    mStructs.push_back(s);
    Struct *result = &mStructs.back();
    AddScopeStruct(this, scope, &mStructs.back());
    return result;
}

void Module::Write(path const &folder, List<Module> const &allModules, Games::IDs game) {
    WriteHeader(folder, allModules, game);
    if (mHasSourceFile)
        WriteSource(folder, allModules, game);
    if (mHasMetaFile)
        WriteMeta(folder / "meta", allModules, game);
}

bool Module::WriteHeader(path const &folder, List<Module> const &allModules, Games::IDs game) {
    path headerFilePath = folder / (mName + ".h");
    ofstream stream(headerFilePath);
    if (!stream.is_open()) {
        Message("Unable to open header file '%s'", headerFilePath.string().c_str());
        return false;
    }
    tabs t(0);
    // file header
    stream << GetPluginSdkComment(game, true) << endl;
    stream << "#pragma once" << endl << endl;
    // include files
    stream << "#include " << '"' << "PluginBase.h" << '"' << endl;
    // find all types in this module
    vector<pair<string, bool>> usedTypes;

    auto addUsedTypeName = [&](string const &typeName, bool needsHeader = true) {
        if (mForbiddenModules.find(typeName) != mForbiddenModules.end())
            needsHeader = false;
        bool found = false;
        for (auto &e : usedTypes) {
            if (e.first == typeName) {
                if (!e.second && needsHeader)
                    e.second = true;
                found = true;
                break;
            }
        }
        if (!found)
            usedTypes.emplace_back(typeName, needsHeader);
    };

    auto addUsedType = [&](Type &type, bool needsHeader = true) {
        if (type.mIsForwardDecl)
            addUsedTypeName(type.mName, false);
        else {
            if (type.mIsRenderWare)
                addUsedTypeName("RenderWare", needsHeader);
            else if (type.mIsCustom) {
                if (type.mIsTemplate) {
                    for (auto &t : type.mTemplateTypes) {
                        if (t.mIsCustom)
                            addUsedTypeName(t.mName, needsHeader);
                    }
                }
                else
                    addUsedTypeName(type.mName, needsHeader);
            }
            else if (type.mIsFunction) {
                for (auto &t : type.mFunctionParams) {
                    if (t.mIsCustom)
                        addUsedTypeName(t.mName, needsHeader);
                }
                if (type.mFunctionRetType && type.mFunctionRetType->mIsCustom)
                    addUsedTypeName(type.mFunctionRetType->mName, needsHeader);
            }
        }
    };

    auto numDefinedTypes = mStructs.size() + mEnums.size();
    for (auto &s : mStructs) // structs and enums - put them at begin
        addUsedTypeName(s.GetFullName(), false);
    for (auto &e : mEnums)
        addUsedTypeName(e.GetFullName(), false);
    for (auto &s : mStructs) {
        for (auto &m : s.mMembers) { // struct members
            if (!m.mIsVTable)
                addUsedType(m.mType);
        }
        for (auto &f : s.mFunctions) { // struct function parameters and return value
            addUsedType(f.mRetType);
            for (auto &p : f.mParameters)
                addUsedType(p.mType);
        }
        for (auto &v : s.mVariables) // struct variables
            addUsedType(v.mType);
    }
    for (auto &v : mVariables) // variables
        addUsedType(v.mType);
    for (auto &f : mFunctions) { // function parameters and return value
        addUsedType(f.mRetType);
        for (auto &p : f.mParameters)
            addUsedType(p.mType);
    }

    // required modules
    for (auto &req : mRequiredModules)
        addUsedTypeName(req);

    // print include headers
    for (size_t i = numDefinedTypes; i < usedTypes.size(); i++) {
        auto &t = usedTypes[i];
        if (t.second) {
            stream << "#include " << '"' << t.first << ".h" << '"' << endl;
            t.second = true;
        }
    }

    // print forward declarations
    int numForwardDeclarations = 0;
    for (size_t i = numDefinedTypes; i < usedTypes.size(); i++) {
        auto &t = usedTypes[i];
        if (!t.second) {
            stream << endl << "class " << t.first << ";";
            numForwardDeclarations++;
        }
    }

    if (numForwardDeclarations > 0)
        stream << endl;

    if (mStructs.size() > 0) {
        mStructs.sort([](Struct &s1, Struct &s2) {
            return !s1.ContainsType(s2.mName, false);
        });
    }

    bool makeNewLine = true;
    // enums
    for (auto &e : mEnums) {
        if (!e.mUsedAsBitfieldMember) {
            stream << endl;
            e.Write(stream, t);
            stream << endl;
        }
    }
    // structs
    for (auto &s : mStructs) {
        if (!s.mIsAnonymous && !s.mEncloseClass) {
            stream << endl;
            s.Write(stream, t, *this, allModules, game);
            stream << endl;
        }
    }
    // variables
    unsigned int numWrittenVars = 0;
    for (auto &v : mVariables) {
        if (makeNewLine) {
            stream << endl;
            makeNewLine = false;
        }
        v.WriteDeclaration(stream, t, game, false);
        stream << endl;
        numWrittenVars++;
    }
    makeNewLine = true;
    //functions
    for (auto &f : mFunctions) {
        if (makeNewLine) {
            stream << endl;
            makeNewLine = false;
        }
        f.WriteDeclaration(stream, t, game);
        stream << endl;
    }
    makeNewLine = true;
    // structs extra info
    for (auto &s : mStructs) {
        if (!s.mIsAnonymous && (s.mHasVTable || s.mSize > 0)) {
            if (makeNewLine) {
                stream << endl;
                makeNewLine = false;
            }
            s.WriteStructExtraInfo(stream);
        }
    }

    if (mHasMetaFile)
        stream << endl << "#include " << '"' << "meta/meta." << mName + ".h" << '"' << endl;

    return true;
}

bool Module::WriteSource(path const &folder, List<Module> const &allModules, Games::IDs game) {
    path sourceFilePath = folder / (mName + ".cpp");
    ofstream stream(sourceFilePath);
    if (!stream.is_open()) {
        Message("Unable to open source file '%s'", sourceFilePath.string().c_str());
        return false;
    }
    tabs t(0);
    // file header
    stream << GetPluginSdkComment(game, false) << endl;
    // include files
    stream << "#include " << '"' << mName + ".h" << '"' << endl << endl;
    // source macro
    stream << "PLUGIN_SOURCE_FILE" << endl << endl;

    unsigned int numWrittenVars = 0;
    // class variables
    for (auto &s : mStructs) {
        for (auto &v : s.mVariables) {
            v.WriteDefinition(stream, t, game);
            stream << endl;
            numWrittenVars++;
        }
    }
    // global variables
    for (auto &v : mVariables) {
        v.WriteDefinition(stream, t, game);
        stream << endl;
        numWrittenVars++;
    }
    int numWrittenFuncs = 0;
    // class functions
    for (auto &s : mStructs)
        numWrittenFuncs += s.WriteFunctions(stream, t, game, true, false, numWrittenFuncs > 0 || numWrittenVars > 0);
    //functions
    for (auto &f : mFunctions) {
        if (numWrittenFuncs > 0 || numWrittenVars > 0)
            stream << endl;
        f.WriteDefinition(stream, t, game);
        stream << endl;
        numWrittenFuncs++;
    }
    return true;
}

bool Module::WriteMeta(path const &folder, List<Module> const &allModules, Games::IDs game) {
    path metaFilePath = folder / ("meta." + mName + ".h");
    ofstream stream(metaFilePath);
    if (!stream.is_open()) {
        Message("Unable to open meta file '%s'", metaFilePath.string().c_str());
        return false;
    }
    tabs t(0);
    // file header
    stream << GetPluginSdkComment(game, true) << endl;
    // include files
    stream << "#include " << '"' << "PluginBase.h" << '"' << endl;
    stream << endl;
    stream << "namespace plugin {" << endl;
    // class functions
    for (auto &s : mStructs)
        s.WriteFunctions(stream, t, game, false, true, true);
    //functions
    for (auto &f : mFunctions) {
        stream << endl;
        f.WriteMeta(stream, t, game);
        stream << endl;
    }
    for (auto &s : mStructs)
        s.WriteGeneratedConstruction(stream, t, game);
    stream << endl << "}" << endl;
    return true;
}

void Module::AddFunction(Function const &fn) {
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

Variable *Module::GetVariable(unsigned int baseAddress) {
    for (auto &v : mVariables) {
        if (v.mVersionInfo[0].mAddress == baseAddress)
            return &v;
    }
    for (auto &s : mStructs) {
        for (auto &sv : s.mVariables) {
            if (sv.mVersionInfo[0].mAddress == baseAddress)
                return &sv;
        }
    }
    return nullptr;
}

Function *Module::GetFunction(unsigned int baseAddress) {
    for (auto &f : mFunctions) {
        if (f.mVersionInfo[0].mAddress == baseAddress)
            return &f;
    }
    for (auto &s : mStructs) {
        for (auto &sf : s.mFunctions) {
            if (sf.mVersionInfo[0].mAddress == baseAddress)
                return &sf;
        }
    }
    return nullptr;
}
