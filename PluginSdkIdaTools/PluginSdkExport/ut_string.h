#pragma once
#include "ida.hpp"

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

void splitComment(qstring const &comment, qstring &realComment, qstring &optionStr);

qvector<qstring> extractValuesFromComment(qstring const &optionStr, qstring const &fieldName);

qstring readMemberComment(qstring const &commentLine);

bool startsWith(qstring const &strToCheck, qstring const &strStart);

void addValueToStringLineList(qstring &line, qstring const &value);

bool isValidCharacterForFileName(char c);

qstring getValidFileName(qstring const &oldFileName);

void startWritingToJson();

void endWritingToJson();

char const *jsonOrderedName(char const *name);

void jsonRemoveOrderingSigns(qstring &str);
