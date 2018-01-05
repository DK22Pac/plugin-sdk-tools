#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "Tabs.h"
#include "Type.h"

using namespace std;

class Struct {
public:
    enum class Kind { Class, Struct, Union };

    enum class Access { Public, Private, Protected };

    string m_name;
    string m_moduleName;
    Kind m_kind;
    unsigned int m_size;
    unsigned int m_alignment;
    bool m_isAnonymous;
    bool m_isPacked;
    string m_comment;
    string m_parent;

    class Member {
    public:
        string m_name;
        string m_fullType;
        Type m_type;
        unsigned int m_offset;
        unsigned int m_size;
        bool m_isAnonymous;
        string m_comment;

        bool IsPadding();
    };

    vector<Member> m_members;

    void Write(ofstream &stream, tabs t);

    bool ContainsType(string const &typeName, bool withPointers = true);
};
