#pragma once
#include <string>

using namespace std;

class Games {
public:
    enum IDs {
        GTASA,
        GTAVC,
        GTA3
    };

    static IDs ToID(unsigned int intId);
    static string GetGameFolder(IDs game);
    static string GetGameAbbr(IDs game);
    static string GetGameAbbrLow(IDs game);
};
