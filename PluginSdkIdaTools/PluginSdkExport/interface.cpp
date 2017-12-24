#include "ida.hpp"
#include "idp.hpp"
#include "export.h"
#include <Windows.h>
#include <filesystem>

#include "ut_options.h"

using namespace std::experimental::filesystem;

enum eInputField {
    FIELD_GAME = 1,
    FIELD_GAMEVERSION = 2,
    FIELD_OUTPUTFOLDER = 3,
    FIELD_OPTIONS = 4,
    FIELD_EXPORTBUTTON = 5
};

int gSelectedGame;
unsigned short gSelectedVersion;
unsigned short gExportOptions;
char gOutputFolder[QMAXPATH];

static int idaapi exportcb(int, form_actions_t &) {
    if (gSelectedGame == -1) {
        warning("Game was not selected");
        return 0;
    }
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
    if (fid == FIELD_GAME) {
        // selected index from eGtaGame (-1 = none)
        fa.get_combobox_value(FIELD_GAME, &gSelectedGame);
    }
    else if (fid == FIELD_GAMEVERSION) {
        // selected index from eGtaGameVersion
        fa.get_radiobutton_value(FIELD_GAMEVERSION, &gSelectedVersion);
    }
    else if (fid == FIELD_OUTPUTFOLDER) {
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
    static const char formdef[] =
        "BUTTON NO NONE\n"
        "BUTTON YES NONE\n"
        "BUTTON CANCEL NONE\n"
        "Export to plugin-sdk\n"
        "\n"
        "%/"
        "<Game:b1:0:::>"
        "<10us:R><10eu:R><11us:R><11eu:R><Steam:R>2>\n"
        "<Output folder:F3::35:::>"
        "\n"
        "<Functions:C>"
        "<Variables:C>"
        "<Structures:C>"
        "<Enums:C>4>"
        "\n"
        "<Export:B5:::::>\n"
        "\n";

    qstrvec_t gameNames;
    gameNames.push_back("GTA SA");
    gameNames.push_back("GTA VC");
    gameNames.push_back("GTA 3");

    gExportOptions = 0xFFFF;

    char filename[QMAXPATH];
    get_root_filename(filename, QMAXPATH);
    gSelectedGame = -1;
    if (strstr(filename, "gta_sa") || strstr(filename, "gtasa") || strstr(filename, "gta-sa"))
        gSelectedGame = 0;
    else if (strstr(filename, "gta_vc") || strstr(filename, "gtavc") || strstr(filename, "gta-vc"))
        gSelectedGame = 1;
    else if (strstr(filename, "gta_3") || strstr(filename, "gta3") || strstr(filename, "gta-3") || strstr(filename, "gta_iii"))
        gSelectedGame = 2;

    gSelectedVersion = 0;

    qstrncpy(gOutputFolder, "%PLUGIN_SDK_DIR%", 17);

    ask_form(formdef, modcb, &gameNames, &gSelectedGame, &gSelectedVersion, gOutputFolder, &gExportOptions, exportcb);
}
