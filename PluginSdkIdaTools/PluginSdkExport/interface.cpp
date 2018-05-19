#include "ida.hpp"
#include "idp.hpp"
#include "name.hpp"
#include "export.h"
#include <Windows.h>
#include <filesystem>
#include "..\..\shared\Games.h"

#include "ut_options.h"

using namespace std::experimental::filesystem;

enum eInputField {
    FIELD_OUTPUTFOLDER = 1,
    FIELD_OPTIONS = 2,
    FIELD_EXPORTBUTTON = 3
};

int gSelectedGame;
unsigned short gSelectedVersion;
unsigned short gExportOptions;
char gOutputFolder[QMAXPATH];
const bool gDebugBuild = false;

static int idaapi exportcb(int, form_actions_t &) {
    if (gOutputFolder[0] == '\0') {
        warning("Output folder was not selected");
        return 0;
    }
    static char outputPathStr[QMAXPATH];
    ExpandEnvironmentStringsA(gOutputFolder, outputPathStr, QMAXPATH);
    path output = outputPathStr;
    if (!exists(output)) {
        warning("Output folder does not exist (%s)", output.string().c_str());
        return 0;
    }
    exportdb(gSelectedGame, gSelectedVersion, gExportOptions, output);
    return 0;
}

static int idaapi modcb(int fid, form_actions_t &fa) {
    if (fid == FIELD_OUTPUTFOLDER) {
        // path
        fa.get_path_value(FIELD_OUTPUTFOLDER, gOutputFolder, QMAXPATH);
    }
    else if (fid == FIELD_OPTIONS) {
        // selected options mask (eExportOptions)
        fa.get_cbgroup_value(FIELD_OPTIONS, &gExportOptions);
    }
    return 1;
}

void showform() {
    gExportOptions = 0xFFFF;
    gSelectedVersion = 0;

    qstring detectedGameAndVersion;

    char filename[QMAXPATH];
    get_root_filename(filename, QMAXPATH);

    gSelectedGame = -1;
    if (strstr(filename, "gta_sa") || strstr(filename, "gtasa") || strstr(filename, "gta-sa"))
        gSelectedGame = Games::GTASA;
    else if (strstr(filename, "gta_vc") || strstr(filename, "gtavc") || strstr(filename, "gta-vc"))
        gSelectedGame = Games::GTAVC;
    else if (strstr(filename, "gta_3") || strstr(filename, "gta3") || strstr(filename, "gta-3") || strstr(filename, "gta_iii"))
        gSelectedGame = Games::GTA3;
    else
        gSelectedGame = -1;

    if (gSelectedGame == -1)
        detectedGameAndVersion = "Unable to detect game version";
    else {
        detectedGameAndVersion = "GTA";
        detectedGameAndVersion += Games::GetGameAbbr(Games::ToID(gSelectedGame)).c_str();
        detectedGameAndVersion += ' ';

        bool detectedVersion = false;

        for (unsigned int i = Games::GetGameVersionsCount(Games::ToID(gSelectedGame)); i > 0; i--) {
            if (strstr(filename, Games::GetGameVersionName(Games::ToID(gSelectedGame), i - 1).c_str())) {
                gSelectedVersion = i - 1;
                detectedVersion = true;
                detectedGameAndVersion += Games::GetGameVersionName(Games::ToID(gSelectedGame), i - 1).c_str();
                break;
            }
        }

        if (!detectedVersion) {
            detectedGameAndVersion += "(defaulted to ";
            detectedGameAndVersion += Games::GetGameVersionName(Games::ToID(gSelectedGame), 0).c_str();
            detectedGameAndVersion += ')';
        }
    }

    qstrncpy(gOutputFolder, "%GTA_DATABASES%", 16);

    qstring formdef =
        "BUTTON NO NONE\n"
        "BUTTON YES NONE\n"
        "BUTTON CANCEL NONE\n"
        "Export to plugin-sdk\n"
        "\n"
        "%/"
        "Detected game: ";
    formdef += detectedGameAndVersion;
    formdef += "\n";
    formdef +=
        "<Output folder:F1::35:::>"
        "\n"
        "<Functions:C>"
        "<Variables:C>";
    if (gSelectedVersion == 0 || gDebugBuild) {
        formdef +=
            "<Structures:C>"
            "<Enums:C>";
    }
    formdef +=
        "2>"
        "\n"
        "<Export:B3:::::>\n"
        "\n";

#if (IDA_VER >= 70)
    ask_form(formdef.c_str(), modcb, gOutputFolder, &gExportOptions, exportcb);
#else
    AskUsingForm_c(formdef.c_str(), modcb, gOutputFolder, &gExportOptions, exportcb);
#endif
}
