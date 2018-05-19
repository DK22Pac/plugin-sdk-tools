#include "ida.hpp"
#include "idp.hpp"
#include "name.hpp"
#include "import.h"
#include <Windows.h>
#include <filesystem>
#include "..\..\shared\Games.h"

#include "ut_options.h"

using namespace std::experimental::filesystem;

enum eInputField {
    FIELD_INPUTFOLDER = 1,
    FIELD_OPTIONS = 2,
    FIELD_IMPORTBUTTON = 3
};

int gSelectedGame;
unsigned short gSelectedVersion;
unsigned short gImportOptions;
char gInputFolder[QMAXPATH];

static int idaapi importcb(int, form_actions_t &) {
    if (gInputFolder[0] == '\0') {
        warning("Input folder was not selected");
        return 0;
    }
    static char inputPathStr[QMAXPATH];
    ExpandEnvironmentStringsA(gInputFolder, inputPathStr, QMAXPATH);
    path input = inputPathStr;
    if (!exists(input)) {
        warning("Input folder does not exist (%s)", input.string().c_str());
        return 0;
    }
    importdb(gSelectedGame, gSelectedVersion, gImportOptions, input);
    return 0;
}

static int idaapi modcb(int fid, form_actions_t &fa) {
    if (fid == FIELD_INPUTFOLDER) {
        // path
        fa.get_path_value(FIELD_INPUTFOLDER, gInputFolder, QMAXPATH);
    }
    else if (fid == FIELD_OPTIONS) {
        // selected options mask (eExportOptions)
        fa.get_cbgroup_value(FIELD_OPTIONS, &gImportOptions);
    }
    return 1;
}

void showform() {
    gImportOptions = 0xFFFF;
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

    qstrncpy(gInputFolder, "%GTA_DATABASES%", 16);

    qstring formdef =
        "BUTTON NO NONE\n"
        "BUTTON YES NONE\n"
        "BUTTON CANCEL NONE\n"
        "Import from plugin-sdk\n"
        "\n"
        "%/"
        "Detected game: ";
    formdef += detectedGameAndVersion;
    formdef += "\n";
    formdef +=
        "<Input folder:F1::35:::>"
        "\n"
        "<Functions:C>"
        "<Variables:C>"
        "<Structures:C>"
        "<Enums:C>2>"
        "\n"
        "<Import:B3:::::>\n"
        "\n";
#if (IDA_VER >= 70)
    ask_form(formdef.c_str(), modcb, gInputFolder, &gImportOptions, importcb);
#else
    AskUsingForm_c(formdef.c_str(), modcb, gInputFolder, &gImportOptions, importcb);
#endif
}
