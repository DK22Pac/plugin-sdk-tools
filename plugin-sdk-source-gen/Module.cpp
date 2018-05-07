#include "..\shared\Utility.h"
#include "Module.h"
#include <iostream>
#include "Comments.h"
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

Struct *Module::AddEmptyStruct(string const &name, string const &scope) {
    Struct s;
    s.mName = name;
    s.mScope = scope;
    s.mModuleName = mName;
    mStructs.push_back(s);
    return &mStructs.back();
}

void Module::Write(path const &folder, List<Module> const &allModules, Games::IDs game) {
    WriteHeader(folder, allModules, game);
    WriteSource(folder, allModules, game);
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
        if (type.mIsRenderWare)
            addUsedTypeName("RenderWare", needsHeader);
        else if (type.mIsCustom) {
            addUsedTypeName(type.mName, needsHeader && type.IsPointer());
            if (type.mIsTemplate) {
                for (auto &t : type.mTemplateTypes) {
                    if (t.mIsCustom)
                        addUsedTypeName(t.mName, false);
                }
            }
            if (type.mIsFunction) {
                for (auto &t : type.mFunctionParams) {
                    if (t.mIsCustom)
                        addUsedTypeName(t.mName, false);
                }
                if (type.mFunctionRetType)
                    addUsedTypeName(type.mFunctionRetType->mName, false);
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
        for (auto &f : s.mFunctions) { // function parameters
            for (auto &p : f.mParameters)
                addUsedType(p.mType, false);
        }
    }
    for (auto &v : mVariables) // variables
        addUsedType(v.mType);
    for (auto &f : mFunctions) { // function parameters
        for (auto &p : f.mParameters)
            addUsedType(p.mType, false);
    }

    // print include headers
    for (size_t i = numDefinedTypes; i < usedTypes.size(); i++) {
        auto &t = usedTypes[i];
        // TODO: check if we can include this header here
        {
            stream << "#include " << '"' << t.first << ".h" << '"' << endl;
            t.second = true;
        }
    }

    // print forward declarations
    int numForwardDeclarations = 0;
    for (size_t i = numDefinedTypes; i < usedTypes.size(); i++) {
        auto &t = usedTypes[i];
        if (!t.second) {
            stream << endl << "class " << t.first;
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

    // enums
    for (auto &e : mEnums) {
        stream << endl;
        e.Write(stream, t);
        stream << endl;
    }
    // structs
    for (auto &s : mStructs) {
        if (!s.mIsAnonymous) {
            stream << endl;
            s.Write(stream, t, *this, allModules, game);
            stream << endl;
        }
    }
    // variables
    for (auto &v : mVariables) {
        stream << endl;
        v.WriteDeclaration(stream, t, game, false);
        stream << endl;
    }
    //functions
    for (auto &f : mFunctions) {
        stream << endl;
        f.WriteDeclaration(stream, t, game);
        stream << endl;
    }

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
