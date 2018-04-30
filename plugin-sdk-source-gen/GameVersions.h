#pragma once
#include <filesystem>
#include "..\shared\Games.h"

using namespace std::experimental::filesystem;

class GameVersions {
public:
    static bool GenerateMacroFile(path const &sdkpath, Games::IDs game);

    template<typename T>
    static string GetSupportedGameVersionsMacro(Games::IDs game, T *exeVersionInfos) {
        string result = "SUPPORTED";
        for (unsigned int i = 0; i < Games::GetGameVersionsCount(game); i++) {
            if (game == Games::IDs::GTASA && i == 1) // skip GTASA 1.0 US HoodLum
                continue;
            if (exeVersionInfos[i].mAddress != 0)
                result += "_" + String::ToUpper(Games::GetGameVersionName(game, i));
        }
        return result;
    }
};
