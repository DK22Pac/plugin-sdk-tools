#pragma once
#include <string>

using namespace std;

class tabs {
    unsigned int mCount;
public:
    tabs();
    tabs(unsigned int count);
    string operator()();
    void operator+=(unsigned int const &count);
    void operator-=(unsigned int const &count);
    tabs &operator++();
    tabs &operator--();
    tabs operator++(int);
    tabs operator--(int);
    unsigned int count();
};
