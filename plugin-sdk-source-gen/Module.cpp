#include "Module.h"

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

void Module::Write(path const &folder, vector<Module> const &allModules) {
    WriteHeader(folder, allModules);
}

bool Module::WriteHeader(path const &folder, vector<Module> const &allModules) {
    ofstream stream(folder / (mName + ".h"));
    if (!stream.is_open()) {
        return false;
    }
    tabs t(0);
    // file header
    stream << "/*" << endl;
    stream << "    Plugin-SDK (Grand Theft Auto) header file" << endl;
    stream << "    Authors: GTA Community. See more here" << endl;
    stream << "    https://github.com/GTAmodding/plugin-sdk" << endl;
    stream << "    Do not delete this comment block. Respect others' work!" << endl;
    stream << "*/" << endl;
    stream << "#pragma once" << endl << endl;
    // include files
    stream << "#include " << '"' << ("plbase/PluginBase_" + Games::GetGameAbbr(mGame) + ".h") << '"' << endl;
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
                mStructs[i].Write(stream, t, *this, allModules);
                stream << endl;
            }
        }
    }
    // variables
    if (mVariables.size() > 0) {
        for (unsigned int i = 0; i < mVariables.size(); i++) {
            if (mVariables[i].mScope.empty()) {
                stream << endl;
                stream << t() << "extern ";
                tabs t0(0);
                mVariables[i].WriteDeclaration(stream, t0, false);
                stream << endl;
            }
        }
    }

    //functions

    return true;
}

bool Module::WriteSource(path const &folder, vector<Module> const &allModules) {
    return false;
}
