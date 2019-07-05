#include "Paths.h"

path Paths::GetDatabaseDir(path const &sdkpath, Games::IDs game) {
    path p = sdkpath / "database" / Games::GetGameFolder(game);
    if (!exists(p))
        create_directories(p);
    return p;
}

path Paths::GetModulesDir(path const &sdkpath, Games::IDs game) {
    path p = sdkpath / "generated" / "modules" / Games::GetGameFolder(game);
    if (!exists(p / "meta"))
        create_directories(p / "meta");
    return p;
}

path Paths::GetOtherDir(path const &sdkpath, Games::IDs game) {
    path p = sdkpath / "generated" / "other" / Games::GetGameFolder(game);
    if (!exists(p))
        create_directories(p);
    return p;
}
