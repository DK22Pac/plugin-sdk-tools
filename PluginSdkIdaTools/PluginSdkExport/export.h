#pragma once
#include <filesystem>

using namespace std::experimental::filesystem;

void exportdb(int selectedGame, unsigned short selectedVersion, unsigned short options, path const &output);
