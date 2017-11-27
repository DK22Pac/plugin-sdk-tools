#pragma once

enum eGtaGame {
    GTA_SA = 0,
    GTA_VC = 1,
    GTA_3 = 2
};

enum eGtaGameVersion {
    GAMEVERSION_10US = 0,
    GAMEVERSION_10EU = 1,
    GAMEVERSION_11US = 2,
    GAMEVERSION_11EU = 3,
    GAMEVERSION_STEAM = 4
};

enum eExportOptions {
    OPTION_FUNCTIONS = 1,
    OPTION_VARIABLES = 2,
    OPTION_STRUCTURES = 4,
    OPTION_ENUMS = 8
};
