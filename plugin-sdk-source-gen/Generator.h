#pragma once
#include "Module.h"
#include "..\shared\json\json.hpp"

using json = nlohmann::json;

enum eGame { GAME_SA, GAME_VC, GAME_3 };

class Generator {
public:
    static int readJsonNumber(json const &node, string const &key) {
        auto it = node.find(key);
        if (it == node.end())
            return 0;
        if ((*it).is_string()) {
            int value = 0;
            sscanf_s((*it).get<string>().c_str(), "0x%X", &value);
            return value;
        }
        return (*it).get<int>();
    }

    static string readJsonString(json const &node, string const &key) {
        return node.value(key, "");
    }

    static bool readJsonBool(json const &node, string const &key) {
        return node.value(key, false);
    }

    static void Generate(path const &sdkpath) {
        for (unsigned int i = 0; i < 3; i++) {

            string gameId;
            if (i == GAME_SA)
                gameId = "sa";
            else if (i == GAME_VC)
                gameId = "vc";
            else
                gameId = "iii";

            vector<Module> modules = ReadGame(sdkpath, i);

            WriteModules(sdkpath, gameId, modules);
        }

    }

    static vector<Module> ReadGame(path const &sdkpath, unsigned int game) {
        string gameFolder = "gtasa";
        if (game == GAME_VC)
            gameFolder = "gtavc";
        else if (game == GAME_3)
            gameFolder = "gta3";

        vector<Module> modules;

        // read enums
        for (const auto& p : recursive_directory_iterator(sdkpath / gameFolder / "enums")) {
            if (p.path().extension() == ".json") {
                ifstream enumFile(p.path().string());
                if (enumFile.is_open()) {
                    json j;
                    j << enumFile;
                    string moduleName = readJsonString(j, "module");
                    if (!moduleName.empty()) {
                        Module *m = Module::Find(modules, moduleName);
                        if (!m) {
                            Module tmp;
                            tmp.m_name = moduleName;
                            modules.push_back(tmp);
                            m = &modules.back();
                        }
                        Enum e;
                        e.m_name = readJsonString(j, "name");
                        e.m_moduleName = moduleName;
                        e.m_width = readJsonNumber(j, "width");
                        e.m_isClass = readJsonBool(j, "isClass");
                        e.m_isHexademical = readJsonBool(j, "isHexademical");
                        e.m_isSigned = readJsonBool(j, "isSigned");
                        e.m_isBitfield = readJsonBool(j, "isBitfield");
                        e.m_comment = readJsonString(j, "comment");
                        auto &members = j.find("members");
                        if (members != j.end()) {
                            for (auto const &jm : *members) {
                                Enum::Member m;
                                m.m_name = readJsonString(jm, "name");
                                m.m_value = readJsonNumber(jm, "value");
                                m.m_comment = readJsonString(jm, "comment");
                                e.m_members.push_back(m);
                            }
                        }
                        m->m_enums.push_back(e);
                    }
                }
            }
        }
        return modules;
    }

    static void WriteModules(path const &sdkpath, string const &gameId, vector<Module> modules) {
        path folder = sdkpath / "generated" / ("plugin_" + gameId) / ("game_" + gameId);
        if (!exists(folder))
            create_directories(folder);
        for (auto &module : modules) {
            module.Write(folder);
        }
    }
};
