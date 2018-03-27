#pragma once
#include "ida.hpp"

#define IDA_TOOLS

#include "..\..\shared\json\json.hpp"

using json = nlohmann::json;

qstring csvvalue(qstring const &value);

// example usage:
//    readcsv(csvline, outParam1, outParam2, ...);

template <typename T>
void readcsvparameter(qstring const &line, unsigned int &currIndex, T &param) {
    qstring tmp;
    bool startedQuotes = false;
    bool inQuotes = false;
    while (currIndex < line.length()) {
        auto c = line[currIndex];
        if (c == '\r' || c == '\n') {
            currIndex = line.length();
            break;
        }
        if (!startedQuotes) {
            if (c == '"')
                startedQuotes = true;
            else if (!inQuotes && c == ',') {
                currIndex++;
                break;
            }
            else
                tmp.append(c);
        }
        else {
            if (c != '"') {
                inQuotes = !inQuotes;
                if (!inQuotes && c == ',') {
                    currIndex++;
                    break;
                }
            }
            startedQuotes = false;
            tmp.append(c);
        }
        currIndex++;
    }
    param = tmp;
    param.trim2(' ');
}

template <typename First, typename... Rest>
void readcsvparameter(qstring const &line, unsigned int &currIndex, First &firstParam, Rest&... otherParams) {
    readcsvparameter(line, currIndex, firstParam);
    readcsvparameter(line, currIndex, otherParams...);
}

template<typename... ArgTypes>
void readcsv(qstring const &line, ArgTypes&... args) {
    unsigned int currentIndex = 0;
    readcsvparameter(line, currentIndex, args...);
}

bool startsWith(qstring const &strToCheck, qstring const &strStart);

void addValueToStringLineList(qstring &line, qstring const &value);

void addValueCSVLine(qstring &line, qstring const &value);

bool isValidCharacterForFileName(char c);

qstring getValidFileName(qstring const &oldFileName);

void startWritingToJson();

void endWritingToJson();

char const *jsonOrderedName(char const *name);

void jsonRemoveOrderingSigns(qstring &str);

template<typename ...ArgTypes>
qstring format(const qstring &format, ArgTypes... args) {
    static char buf[1024];
    qsnprintf(buf, 1024, format.c_str(), args...);
    return buf;
}

template <typename T>
qstring toHexString(T intVal, bool prefix = true) {
    if (prefix)
        return format("0x%X", intVal);
    return format("%X", intVal);
}

template <typename T>
qstring toString(T intVal) {
    return format("%d", intVal);
}


int toNumber(qstring const &str);

bool contains(qstring const &str, qstring const &substr);
