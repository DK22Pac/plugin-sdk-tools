#include "..\shared\Utility.h"
#include "GvsMacroGenerator.h"
#include "Paths.h"
#include "Comments.h"
#include <fstream>

bool GvsMacroGenerator::Generate(path const &sdkpath, Games::IDs game) {
    path filePath = Paths::GetOtherDir(sdkpath, game) / "VersionsMacro.h";
    ofstream stream(filePath);
    if (!stream.is_open()) {
        Message("Unable to open VersionsMacro file '%s'", filePath.string().c_str());
        return false;
    }
    stream << GetPluginSdkComment(game, true) << endl;
    stream << "#pragma once" << endl << endl;
    // how many combinations do we have
    unsigned int NumCombinations = 1 << Games::GetGameVersionsCount(game);
    // for all combinations...
    for (unsigned int c = 0; c < NumCombinations; c++) {
        stream << "#if";
        // for all game versions...
        for (unsigned int v = 0; v < Games::GetGameVersionsCount(game); v++) {
            stream << ' ';
            if (v != 0)
                stream << "&& ";
            if (!(c & (1 << v)))
                stream << '!';
            stream << "defined(PLUGIN_SGV_" << String::ToUpper(Games::GetGameVersionName(game, v)) << ')';
        }
        stream << endl;
        // for all combinations
        for (unsigned int nc = 1; nc < NumCombinations; nc++) {
            stream << "#define SUPPORTED";
            // for all game versions...
            for (unsigned int nv = 0; nv < Games::GetGameVersionsCount(game); nv++) {
                if (nc & (1 << nv)) {
                    stream << '_';
                    stream << String::ToUpper(Games::GetGameVersionName(game, nv));
                }
            }
            stream << ' ';
            if ((c & nc) != c) {
                stream << " [[deprecated(\"not all .exe versions are supported:";
                // for all game versions...
                for (unsigned int mv = 0; mv < Games::GetGameVersionsCount(game); mv++) {
                    if ((c & (1 << mv)) && !(nc & (1 << mv)))
                        stream << "\\n    usage for " << Games::GetGameVersionDetailedName(game, mv) << " exe is not supported";
                }
                stream << "\")]]";
            }
            stream << endl;
        }

        stream << "#endif" << endl;
    }
    return true;
}
