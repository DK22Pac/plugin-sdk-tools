#include "Games.h"

Games::IDs Games::ToID(unsigned int intId) {
    return static_cast<IDs>(intId);
}

string Games::GetGameFolder(IDs game) {
    if (game == GTAVC)
        return "gtavc";
    else if (game == GTA3)
        return "gta3";
    return "gtasa";
}

string Games::GetGameAbbr(IDs game) {
    if (game == GTAVC)
        return "VC";
    else if (game == GTA3)
        return "III";
    return "SA";
}

string Games::GetGameAbbrLow(IDs game) {
    if (game == GTAVC)
        return "vc";
    else if (game == GTA3)
        return "iii";
    return "sa";
}

string Games::GetGameVersionName(IDs game, unsigned int version) {
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
