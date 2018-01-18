#pragma once
#include <string>
#include <algorithm>

using namespace std;

char const * const VersionNamesSA[] = { "10us", "10eu", "11us", "11eu", "sr2", "sr2lv" };
char const * const VersionNamesVC[] = { "10en", "11en", "steam" };
char const * const VersionNamesIII[] = { "10en", "11en", "steam" };

class Games {
public:
    enum IDs {
        GTASA,
        GTAVC,
        GTA3
    };

    static constexpr unsigned int Games::GetGameVersionsCount(IDs game) {
        if (game == GTASA)
            return sizeof(VersionNamesSA) / sizeof(char*);
        if (game == GTAVC)
            return sizeof(VersionNamesVC) / sizeof(char*);
        if (game == GTA3)
            return sizeof(VersionNamesIII) / sizeof(char*);
        return 0;
    }

    static constexpr unsigned int Games::GetMaxGameVersions() {
        return max(GetGameVersionsCount(GTASA), max(GetGameVersionsCount(GTAVC), GetGameVersionsCount(GTA3)));
    }

    static IDs ToID(unsigned int intId);
    static string GetGameFolder(IDs game);
    static string GetGameAbbr(IDs game);
    static string GetGameAbbrLow(IDs game);
    static string GetGameVersionName(IDs game, unsigned int version);
};
