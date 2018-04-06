#include "..\shared\Utility.h"
#include "Module.h"
#include <iostream>
#include "Comments.h"
#include <unordered_set>

Module *Module::Find(vector<Module> &modules, string const &name) {
    for (unsigned int i = 0; i < modules.size(); i++) {
        if (modules[i].mName == name)
            return &modules[i];
        ;
    }
    return nullptr;
}

Struct *Module::FindStruct(string const &name, bool bFullName) {
    for (unsigned int i = 0; i < mStructs.size(); i++) {
        string structName;
        if (bFullName)
            structName = mStructs[i].GetFullName();
        else
            structName = mStructs[i].mName;
        if (structName == name)
            return &mStructs[i];
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

void Module::Write(path const &folder, vector<Module> const &allModules, Games::IDs game) {
    WriteHeader(folder, allModules, game);
    WriteSource(folder, allModules, game);
    WriteMeta(folder / "meta", allModules, game);
}

bool Module::WriteHeader(path const &folder, vector<Module> const &allModules, Games::IDs game) {
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

    auto addUsedType = [&](Type const &type, bool needsHeader = true) {
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
    for (auto &s : mStructs) { // struct members
        for (auto &m : s.mMembers)
            addUsedType(m.mType);
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
        sort(mStructs.begin(), mStructs.end(), [](Struct const &s1, Struct const &s2) {
            return !s1.ContainsType(s2.mName, false);
        });
    }

    // enums
    if (mEnums.size() > 0) {
        for (unsigned int i = 0; i < mEnums.size(); i++) {
            stream << endl;
            mEnums[i].Write(stream, t);
            stream << endl;
        }
    }
    // structs
    if (mStructs.size() > 0) { 
        for (unsigned int i = 0; i < mStructs.size(); i++) {
            if (!mStructs[i].mIsAnonymous) {
                stream << endl;
                mStructs[i].Write(stream, t, *this, allModules, game);
                stream << endl;
            }
        }
    }
    // variables
    if (mVariables.size() > 0) {
        for (unsigned int i = 0; i < mVariables.size(); i++) {
            stream << endl;
            mVariables[i].WriteDeclaration(stream, t, game, false);
            stream << endl;
        }
    }
    //functions
    if (mFunctions.size() > 0) {
        for (unsigned int i = 0; i < mFunctions.size(); i++) {
            stream << endl;
            mFunctions[i].WriteDeclaration(stream, t, game);
            stream << endl;
        }
    }

    stream << endl << "#include " << '"' << "meta/meta." << mName + ".h" << '"' << endl;

    return true;
}

bool Module::WriteSource(path const &folder, vector<Module> const &allModules, Games::IDs game) {
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
    // class variables
    if (mStructs.size() > 0) {
        for (unsigned int i = 0; i < mStructs.size(); i++) {
            if (mStructs[i].mVariables.size() > 0) {
                for (unsigned int j = 0; j < mStructs[i].mVariables.size(); j++) {
                    mStructs[i].mVariables[j].WriteDefinition(stream, t, game);
                    stream << endl;
                }
            }
        }
    }
    // global variables
    if (mVariables.size() > 0) {
        for (unsigned int i = 0; i < mVariables.size(); i++) {
            stream << endl;
            mVariables[i].WriteDefinition(stream, t, game);
            stream << endl;
        }
    }
    // class functions
    if (mStructs.size() > 0) {
        for (unsigned int i = 0; i < mStructs.size(); i++) {
            if (mStructs[i].mFunctions.size() > 0) {
                for (unsigned int j = 0; j < mStructs[i].mFunctions.size(); j++) {
                    stream << endl;
                    mStructs[i].mFunctions[j].WriteDefinition(stream, t, game);
                    stream << endl;
                }
            }
        }
    }
    //functions
    if (mFunctions.size() > 0) {
        for (unsigned int i = 0; i < mFunctions.size(); i++) {
            stream << endl;
            mFunctions[i].WriteDefinition(stream, t, game);
            stream << endl;
        }
    }
    return true;
}

bool Module::WriteMeta(path const &folder, vector<Module> const &allModules, Games::IDs game) {
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
    if (mStructs.size() > 0) {
        for (unsigned int i = 0; i < mStructs.size(); i++) {
            if (mStructs[i].mFunctions.size() > 0) {
                for (unsigned int j = 0; j < mStructs[i].mFunctions.size(); j++) {
                    stream << endl;
                    mStructs[i].mFunctions[j].WriteMeta(stream, t, game);
                    stream << endl;
                }
            }
        }
    }
    //functions
    if (mFunctions.size() > 0) {
        for (unsigned int i = 0; i < mFunctions.size(); i++) {
            stream << endl;
            mFunctions[i].WriteMeta(stream, t, game);
            stream << endl;
        }
    }
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
