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

int jsonReadNumber(json const &node, qstring const &key) {
    std::string strKey = key.c_str();
    auto it = node.find(strKey);
    if (it == node.end())
        return 0;
    if ((*it).is_string()) {
        unsigned int value = 0;
        sscanf_s((*it).get<std::string>().c_str(), "0x%X", &value);
        return value;
    }
    return (*it).get<int>();
}

qstring jsonReadString(json const &node, qstring const &key) {
    std::string strKey = key.c_str();
    return qstring(node.value(strKey, "").c_str());
}

bool jsonReadBool(json const &node, qstring const &key) {
    return node.value(key.c_str(), false);
}

json jsonReadFromFile(char const *filepath) {
    json result;
    FILE *f = qfopen(filepath, "rb");
    if (f) {
        qfseek(f, 0, SEEK_END);
        auto length = qftell(f);
        qfseek(f, 0, SEEK_SET);
        auto buffer = new char[length + 1];
        qfread(f, buffer, length);
        buffer[length] = '\0';
        qfclose(f);
        try {
            result = json::parse(buffer);
        }
        catch (std::exception &e) {
            warning("Unable to read json from file \n%s\n%s", filepath, e.what());
        }
        delete[] buffer;
    }
    return result;
}

bool jsonWriteToFile(json const &j, char const *filepath) {
    auto outFile = qfopen(filepath, "wt");
    if (outFile) {
        bool result = true;
        try {
            qstring jsonStr = j.dump(4).c_str();
            jsonRemoveOrderingSigns(jsonStr);
            qfputs(jsonStr.c_str(), outFile);
        }
        catch (std::exception &e) {
            warning("Unable to write json data to file\n%s\n%s", filepath, e.what());
            result = false;
        }
        qfclose(outFile);
        return result;
    }
    warning("Unable to open '%s'", filepath);
    return false;
}

int toNumber(qstring const &str) {
    return (startsWith(str, "0x") ? strtol(str.substr(2).c_str(), nullptr, 16) : strtol(str.c_str(), nullptr, 10));
}

bool contains(qstring const &str, qstring const &substr) {
    return str.find(substr) != qstring::npos;
}
