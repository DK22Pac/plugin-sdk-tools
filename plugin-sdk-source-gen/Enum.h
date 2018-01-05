#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Tabs.h"

using namespace std;

class Enum {
public:
    string m_name;
    string m_moduleName;
    unsigned int m_width;
    bool m_isClass;
    bool m_isHexademical;
    bool m_isSigned;
    bool m_isBitfield;
    string m_comment;

    class Member {
    public:
        string m_name;
        int m_value;
        string m_comment;
    };

    vector<Member> m_members;

    void Write(ofstream &stream, tabs t);
};
