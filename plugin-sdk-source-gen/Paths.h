#pragma once
#include <filesystem>
#include "..\shared\Games.h"

using namespace std::experimental::filesystem;

class Paths {
public:
    static inline path GetDatabaseDir(path const &sdkpath, Games::IDs game) {
        path p = sdkpath / "database" / Games::GetGameFolder(game);
        if (!exists(p))
            create_directories(p);
        return p;
    }

    static inline path GetModulesDir(path const &sdkpath, Games::IDs game) {
        path p = sdkpath / "generated" / "modules" / Games::GetGameFolder(game);
        if (!exists(p / "meta"))
            create_directories(p / "meta");
        return p;
    }

    static inline path GetOtherDir(path const &sdkpath, Games::IDs game) {
        path p = sdkpath / "generated" / "other" / Games::GetGameFolder(game);
        if (!exists(p))
            create_directories(p);
        return p;
    }
};
