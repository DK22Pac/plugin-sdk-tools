#pragma once
#include <filesystem>
#include "..\shared\Games.h"

using namespace std::experimental::filesystem;

class GvsMacroGenerator {
public:
    static bool Generate(path const &sdkpath, Games::IDs game);
};
