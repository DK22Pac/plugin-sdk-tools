#include "ut_string.h"
#include "idp.hpp"

qstring csvvalue(qstring const &value) {
    qstring retval = value;
    if (value.find(',') != qstring::npos) {
        retval.insert(0, '"');
        retval.append('"');
    }
    return retval;
}

bool startsWith(qstring const &strToCheck, qstring const &strStart) {
    return strToCheck.substr(0, strStart.length()) == strStart;
}

void addValueToStringLineList(qstring &line, qstring const &value) {
    if (!line.empty())
        line.append(' ');
    line.append(value);
}

void addValueCSVLine(qstring &line, qstring const &value) {
    if (!line.empty())
        line.append(", ");
    line.append(value);
}

bool isValidCharacterForFileName(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}

qstring getValidFileName(qstring const &oldFileName) {
    qstring newFileName;
    for (size_t i = 0; i < oldFileName.length(); i++) {
        auto c = oldFileName[i];
        if (isValidCharacterForFileName(c))
            newFileName.append(c);
        else
            newFileName.append('_');
    }
    return newFileName;
}

unsigned int &OrderingSignsCount() {
    static unsigned int count = 0;
    return count;
}

void startWritingToJson() {
    OrderingSignsCount() = 0;
}

char const *jsonOrderedName(char const *name) {
    static char orderedName[32];
    qsnprintf(orderedName, 32, "$ORD%010d$%s", OrderingSignsCount(), name);
    OrderingSignsCount()++;
    return orderedName;
}

void jsonRemoveOrderingSigns(qstring &str) {
    qstring newstr;
    size_t b = 0;
    size_t e = str.find("$ORD");
    while (e != qstring::npos) {
        if (e > b)
            newstr.append(str.substr(b, e));
        b = str.find("$", e + 4);
        if (b == qstring::npos)
            break;
        b += 1;
        e = str.find("$ORD", b);
    }
    if (b != qstring::npos) {
        auto len = str.length();
        if (b < len)
            newstr.append(str.substr(b));
    }
    str = newstr;
}

void endWritingToJson() {
    OrderingSignsCount() = 0;
}

int toNumber(qstring const &str) {
    return (startsWith(str, "0x") ? strtol(str.substr(2).c_str(), nullptr, 16) : strtol(str.c_str(), nullptr, 10));
}

bool contains(qstring const &str, qstring const &substr) {
    return str.find(substr) != qstring::npos;
}
