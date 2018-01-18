#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Enum.h"
#include "Struct.h"
#include "Variable.h"
#include "Function.h"
#include "Games.h"

using namespace std;
using namespace std::experimental::filesystem;

class Module {
public:
    Games::IDs mGame;
    string mName;
    vector<Enum> mEnums;
    vector<Struct> mStructs;
    vector<Variable> mVariables;
    vector<Function> mFunctions;

    vector<string> mErrors;
    vector<string> mWarnings;

    static Module *Find(vector<Module> &modules, string const &name);
    Struct *FindStruct(string const &name, bool bFullName = false);
    Struct *AddEmptyStruct(string const &name, string const &scope);

    void Write(path const &folder, vector<Module> const &allModules);
    bool WriteHeader(path const &folder, vector<Module> const &allModules);
    bool WriteSource(path const &folder, vector<Module> const &allModules);
};
