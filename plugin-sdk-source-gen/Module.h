#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Enum.h"
#include "Struct.h"
#include "Games.h"

using namespace std;
using namespace std::experimental::filesystem;

class Module {
public:
    Games::IDs m_game;
    string m_name;
    vector<Enum> m_enums;
    vector<Struct> m_structs;

    static Module *Find(vector<Module> &modules, string const &name);

    void Write(path const &folder);
    bool WriteHeader(path const &folder);
    bool WriteSource(path const &folder);
};
