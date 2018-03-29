#pragma once
#include "StringEx.h"
#include <fstream>

class CSV {
    static void ReadParam(string const &line, unsigned int &currIndex, string &param);

    template <typename First, typename... Rest>
    static void ReadParam(string const &line, unsigned int &currIndex, First &firstParam, Rest&... otherParams) {
        ReadParam(line, currIndex, firstParam);
        ReadParam(line, currIndex, otherParams...);
    }

public:
    template<typename... ArgTypes>
    static void Read(string const &line, ArgTypes&... args) {
        unsigned int currentIndex = 0;
        ReadParam(line, currentIndex, args...);
    }

    static vector<string> ReadLines(ifstream &file);

    static string Value(string const &value);
};
