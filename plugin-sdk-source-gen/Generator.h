#pragma once
#include "ListEx.h"
#include <filesystem>
#include "Module.h"
#include "..\shared\Games.h"

using namespace std;
using namespace std::experimental::filesystem;

class Generator {
public:
    static void Generate(path const &sdkpath);
    static void ReadGame(List<Module> &modules, path const &sdkpath, Games::IDs game);
    static void WriteModules(path const &sdkpath, Games::IDs game, List<Module> &modules);
    static void UpdateStructs(List<Module> &modules);
};
