#include "ut_enum.h"

bool isDefaultEnum(qstring const &enumName) {
    return startsWith(enumName, "MACRO") ||
        startsWith(enumName, "D3D") ||
        startsWith(enumName, "_D3D") ||
        startsWith(enumName, "Rw") ||
        startsWith(enumName, "Rp") ||
        startsWith(enumName, "Rs") ||
        startsWith(enumName, "EAX");
}

qstring enumMemberErrorMessage(int errorCode) {
    switch (errorCode) {
    case 1:
        return "already have member with this name (bad name)";
    case 2:
        return "already have 256 members with this value";
    case 3:
        return "bad enum id";
    case 4:
        return "bad bmask";
    case 5:
        return "bad bmask and value combination (~bmask & value != 0)";
    }
    return "unknown";
}
