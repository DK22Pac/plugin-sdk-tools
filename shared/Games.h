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

    static constexpr unsigned int GetGameVersionsCount(IDs game) {
        if (game == GTASA)
            return sizeof(VersionNamesSA) / sizeof(char*);
        if (game == GTAVC)
            return sizeof(VersionNamesVC) / sizeof(char*);
        if (game == GTA3)
            return sizeof(VersionNamesIII) / sizeof(char*);
        return 0;
    }

    static constexpr unsigned int GetMaxGameVersions() {
        return max(GetGameVersionsCount(GTASA), max(GetGameVersionsCount(GTAVC), GetGameVersionsCount(GTA3)));
    }

    inline static Games::IDs ToID(unsigned int intId) {
        return static_cast<IDs>(intId);
    }

    inline static string GetGameFolder(IDs game) {
        if (game == GTAVC)
            return "gtavc";
        else if (game == GTA3)
            return "gta3";
        return "gtasa";
    }

    inline static string GetGameAbbr(IDs game) {
        if (game == GTAVC)
            return "VC";
        else if (game == GTA3)
            return "III";
        return "SA";
    }

    inline static string GetGameAbbrLow(IDs game) {
        if (game == GTAVC)
            return "vc";
        else if (game == GTA3)
            return "iii";
        return "sa";
    }

    inline static string GetGameFullName(IDs game) {
        if (game == GTAVC)
            return "Grand Theft Auto: Vice City";
        else if (game == GTA3)
            return "Grand Theft Auto 3";
        return "Grand Theft Auto: San Andreas";
    }

    inline static string GetGameVersionName(IDs game, unsigned int version) {
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
