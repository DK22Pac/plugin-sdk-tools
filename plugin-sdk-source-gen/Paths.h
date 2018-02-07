#pragma once
#include <filesystem>
#include "..\shared\Games.h"

using namespace std::experimental::filesystem;

class Paths {
public:
    static inline path GetPluginDir(path const &sdkpath, Games::IDs game) {
        path p = sdkpath / "generated" / ("plugin_" + Games::GetGameAbbrLow(game));
        if (!exists(p))
            create_directories(p);
        return p;
    }

    static inline path GetPluginGameDir(path const &sdkpath, Games::IDs game) {
        path p = sdkpath / "generated" / ("plugin_" + Games::GetGameAbbrLow(game)) / ("game_" + Games::GetGameAbbrLow(game));
        if (!exists(p))
            create_directories(p);
        return p;
    }

    static inline path GetPluginBaseDir(path const &sdkpath, Games::IDs game) {
        path p = sdkpath / "generated" / ("plugin_" + Games::GetGameAbbrLow(game)) / "plbase";
        if (!exists(p))
            create_directories(p);
        return p;
    }
};
