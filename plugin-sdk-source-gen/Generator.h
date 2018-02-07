#pragma once
#include <vector>
#include <filesystem>
#include "Module.h"
#include "..\shared\Games.h"

using namespace std;
using namespace std::experimental::filesystem;

class Generator {
public:
    static void Generate(path const &sdkpath);
    static void ReadGame(vector<Module> &modules, path const &sdkpath, Games::IDs game);
    static void WriteModules(path const &sdkpath, Games::IDs game, vector<Module> &modules);
    static void ReadHierarchy(path const &sdkpath, Games::IDs game, vector<Module> &modules);
};
