#include "ut_struct.h"

bool isSystemStruct(qstring const &name) {
    return name == "DSBUFFERDESC";
}

qstring structMemberErrorMessage(int errorCode) {
    switch (errorCode) {
    case -1:
        return "already has member with this name (bad name)";
    case -2:
        return "already has member at this offset";
    case -3:
        return "bad number of bytes or bad sizeof(type)";
    case -4:
        return "bad typeid parameter";
    case -5:
        return "bad struct id (the 1st argument)";
    case -6:
        return "unions can't have variable sized members";
    case -7:
        return "variable sized member should be the last member in the structure";
    case -8:
        return "recursive structure nesting is forbidden";
    }
    return "unknown";
}
