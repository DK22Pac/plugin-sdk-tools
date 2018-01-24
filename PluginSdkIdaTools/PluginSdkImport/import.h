#pragma once
#include <filesystem>

using namespace std::experimental::filesystem;

void importdb(int selectedGame, unsigned short selectedVersion, unsigned short options, path const &input);
