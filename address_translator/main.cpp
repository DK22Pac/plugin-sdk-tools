#include "..\shared\Utility.h"
#include "..\shared\Games.h"
#include "CSV.h"
#include "StringEx.h"
#include "..\shared\translator.h"
#include <filesystem>

using namespace std::experimental::filesystem;

// usage:
//     address_translator %PLUGIN_SDK_DIR% gtasa 10eu

int main(int argc, char *argv[]) {
    if (argc < 4)
        return ErrorCode(1, "Error: Not enough parameters (%d, expected 4)", argc);
    path sdkpath = argv[1]; // plugin-sdk folder;

    if (!exists(sdkpath))
        return ErrorCode(2, "Error: plugin-sdk folder does not exist (%s)", sdkpath.string().c_str());

    string gameNameStr = argv[2];
    string gameVerStr = argv[3];

    Games::IDs game;
    if (gameNameStr == Games::GetGameFolder(Games::GTA3))
        game = Games::GTA3;
    else if (gameNameStr == Games::GetGameFolder(Games::GTAVC))
        game = Games::GTAVC;
    else if (gameNameStr == Games::GetGameFolder(Games::GTASA))
        game = Games::GTASA;
    else
        return ErrorCode(3, "Error: Unknown game name (%s)", gameNameStr.c_str());

    unsigned int numVersions = Games::GetGameVersionsCount(game);
    int gameVer = -1;
    for (unsigned int i = 0; i < numVersions; i++) {
        if (gameVerStr == Games::GetGameVersionName(game, i)) {
            gameVer = i;
            break;
        }
    }

    if (gameVer == -1)
        return ErrorCode(4, "Error: Unknown game version (%s)", gameVerStr.c_str());
    
    path dbGamePath = sdkpath / "database" / Games::GetGameFolder(game);

    auto WriteReferenceFile = [=](bool bFunctions) {
        string refType = bFunctions ? "functions" : "variables";
        path baseFilePath = dbGamePath / (string("plugin-sdk.") + Games::GetGameAbbrLow(game) + "." + refType + "." +
            Games::GetGameVersionName(game, 0) + ".csv");

        std::ifstream baseFile(baseFilePath.string());
        if (!baseFile.is_open())
            return ErrorCode(5, "Error: Unable to open base file %s", baseFilePath.string().c_str());

        auto baseLines = CSV::ReadLines(baseFile);

        baseFile.close();

        path refFilePath = dbGamePath / (string("plugin-sdk.") + Games::GetGameAbbrLow(game) + "." + refType + "." +
            Games::GetGameVersionName(game, gameVer) + ".csv");

        std::ofstream refFile(refFilePath.string());
        if (!refFile.is_open())
            return ErrorCode(6, "Error: Unable to open reference file %s", refFilePath.string().c_str());

        refFile << Games::GetGameVersionName(game, 0) << "," << Games::GetGameVersionName(game, gameVer);
        if (bFunctions)
            refFile << ",RefList";
        refFile << ",NameComment" << endl;

        for (auto &line : baseLines) {
            string csvAddr, csvModule, csvName, csvDemangledName, csvType, csvCC, csvRetType, csvParameters, csvIsConst, csvRefs;
            if (bFunctions)
                CSV::Read(line, csvAddr, csvModule, csvName, csvDemangledName, csvType, csvCC, csvRetType, csvParameters, csvIsConst, csvRefs);
            else
                CSV::Read(line, csvAddr, csvModule, csvName, csvDemangledName);
            refFile << csvAddr << ",";
            int addr = String::ToNumber(csvAddr);
            if (addr == 0)
                refFile << "0";
            else
                refFile << String::ToHexString(translateAddr(game, gameVer, addr));
            if (bFunctions)
                refFile << "," << CSV::Value(csvRefs);
            refFile << "," << CSV::Value(csvDemangledName) << endl;
        }

        refFile.close();
        return 0;
    };

    auto errc = WriteReferenceFile(true);
    if (errc != 0)
        return errc;
    errc = WriteReferenceFile(false);
    if (errc != 0)
        return errc;

    return 0;
}
