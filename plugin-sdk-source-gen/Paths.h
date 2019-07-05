#pragma once
#include <filesystem>
#include "..\shared\Games.h"

using namespace std::experimental::filesystem;

class Paths {
public:
    static path GetDatabaseDir(path const &sdkpath, Games::IDs game);
    static path GetModulesDir(path const &sdkpath, Games::IDs game);
    static path GetOtherDir(path const &sdkpath, Games::IDs game);
};
