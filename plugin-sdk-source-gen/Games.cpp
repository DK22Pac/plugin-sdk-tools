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
