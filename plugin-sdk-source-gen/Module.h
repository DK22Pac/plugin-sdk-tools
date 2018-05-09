#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include "Enum.h"
#include "Struct.h"
#include "Variable.h"
#include "Function.h"
#include "..\shared\Games.h"
#include "ListEx.h"

using namespace std;
using namespace std::experimental::filesystem;

class Module {
public:
    Games::IDs mGame;
    string mName;
    List<Enum> mEnums;
    List<Struct> mStructs;
    List<Variable> mVariables;
    List<Function> mFunctions;

    bool mHasSourceFile = false;
    bool mHasMetaFile = false;

    Set<string> mRequiredModules;
    Set<string> mForbiddenModules;

    List<string> mErrors;
    List<string> mWarnings;

    static Module *Find(List<Module> &modules, string const &name);
    Struct *FindStruct(string const &name, bool bFullName = false);
    Struct *AddEmptyStruct(string const &name, string const &scope);
    void AddFunction(Function const &fn);

    void Write(path const &folder, List<Module> const &allModules, Games::IDs game);
    bool WriteHeader(path const &folder, List<Module> const &allModules, Games::IDs game);
    bool WriteSource(path const &folder, List<Module> const &allModules, Games::IDs game);
    bool WriteMeta(path const &folder, List<Module> const &allModules, Games::IDs game);
    Variable *GetVariable(unsigned int baseAddress);
    Function *GetFunction(unsigned int baseAddress);
};
