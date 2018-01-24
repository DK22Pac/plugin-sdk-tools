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
