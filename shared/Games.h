#pragma once
#include <string>
#include <algorithm>
#include "String.h"

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

    inline static Games::IDs Games::ToID(unsigned int intId) {
        return static_cast<IDs>(intId);
    }

    inline static string Games::GetGameFolder(IDs game) {
        if (game == GTAVC)
            return "gtavc";
        else if (game == GTA3)
            return "gta3";
        return "gtasa";
    }

    inline static string Games::GetGameAbbr(IDs game) {
        if (game == GTAVC)
            return "VC";
        else if (game == GTA3)
            return "III";
        return "SA";
    }

    inline static string Games::GetGameAbbrLow(IDs game) {
        if (game == GTAVC)
            return "vc";
        else if (game == GTA3)
            return "iii";
        return "sa";
    }

    inline static string Games::GetGameVersionName(IDs game, unsigned int version) {
        if (version < GetGameVersionsCount(game)) {
            if (game == GTASA)
                return VersionNamesSA[version];
            if (game == GTAVC)
                return VersionNamesVC[version];
            if (game == GTA3)
                return VersionNamesIII[version];
        }
        return "";
    }

    template<typename T>
    static string GetSupportedGameVersionsMacro(IDs game, T *exeVersionInfos) {
        string result = "SUPPORTED";
        for (unsigned int i = 0; i < GetGameVersionsCount(game); i++) {
            if (exeVersionInfos[i].mAddress != 0)
                result += "_" + String::ToUpper(GetGameVersionName(game, i));
        }
        return result;
    }
};
