#pragma once
#include <vector>
#include <filesystem>
#include "Module.h"
#include "Games.h"

using namespace std;
using namespace std::experimental::filesystem;

class Generator {
public:
    static void Generate(path const &sdkpath);
    static vector<Module> ReadGame(path const &sdkpath, Games::IDs game);
    static void WriteModules(path const &sdkpath, Games::IDs game, vector<Module> modules);
};
