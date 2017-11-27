#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <filesystem>

using namespace std;
using namespace std::experimental::filesystem;

class tabs {
    unsigned int m_count;
public:
    tabs() : m_count(0) {}
    tabs(unsigned int count) : m_count(count) {}
    string operator()() { return string(m_count * 4, ' '); }
    void operator+=(unsigned int const &count) { m_count += count; }
    void operator-=(unsigned int const &count) { if (m_count < count) m_count = 0; else m_count -= count; }
    tabs &operator++() { operator+=(1); return *this; }
    tabs &operator--() { operator-=(1); return *this; }
    tabs operator++(int) { tabs copy(*this); operator+=(1); return copy; }
    tabs operator--(int) { tabs copy(*this); operator-=(1); return copy; }
};

vector<string> stringSplit(string const &str, string const &delim) {
    vector<string> result;
    if (str.length() > 0) {
        size_t b = 0;
        size_t e = str.find(delim);
        while (e != std::string::npos) {
            result.push_back(str.substr(b, e - b));
            b = e + delim.length();
            e = str.find(delim, b);
        }
        result.push_back(str.substr(b));
    }
    return result;
}

void WriteComment(ofstream &stream, string const &comment, tabs t, unsigned int pos) {
    if (comment.empty())
        return;
    auto lines = stringSplit(comment, "$n$");
    for (unsigned int i = 0; i < lines.size(); i++) {
        if (pos == 0)
            stream << t();
        else {
            if (i != 0)
                stream << t() << string(pos, ' ');
            stream << ' ';
        }
        stream << "//!";
        if (pos != 0)
            stream << '<';
        stream << ' ' << lines[i];
        if ((i + 1) != lines.size() || pos == 0)
            stream << endl;
    }
}

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

    void Write(ofstream &stream, tabs t) {
        WriteComment(stream, m_comment, t, 0);
        stream << t() << "enum" << ' ';
        if (m_isClass)
            stream << "class" << ' ';
        stream << m_name << ' ';
        if (m_width == 1 || m_width == 2 || m_width == 4 || m_width == 8) {
            stream << ':' << ' ';
            string enumBaseTypeName;
            if (!m_isSigned)
                enumBaseTypeName = "unsigned ";
            switch (m_width) {
            case 1:
                enumBaseTypeName += "char";
                break;
            case 2:
                enumBaseTypeName += "short";
                break;
            case 4:
                enumBaseTypeName += "int";
                break;
            case 8:
                enumBaseTypeName += "__int64";
                break;
            }
            stream << enumBaseTypeName << ' ';
        }
        stream << '{' << endl;
        t++;
        for (unsigned int i = 0; i < m_members.size(); i++) {
            stream << t();
            auto pos = stream.tellp();
            stream << m_members[i].m_name << ' ' << '=' << ' ';
            if (m_isHexademical) {
                static char strhex[32];
                snprintf(strhex, 32, "0x%X", m_members[i].m_value);
                stream << strhex;
            }
            else
                stream << m_members[i].m_value;
            if (i != (m_members.size() - 1))
                stream << ',';
            WriteComment(stream, m_members[i].m_comment, t, stream.tellp() - pos);
            stream << endl;
        }
        t--;
        stream << t() << '}' << ';';
    }
};

class Module {
public:
    string m_name;
    vector<Enum> m_enums;

    static Module *Find(vector<Module> &modules, string const &name) {
        for (unsigned int i = 0; i < modules.size(); i++) {
            if (modules[i].m_name == name)
                return &modules[i];
;        }
        return nullptr;
    }

    void Write(path const &folder) {
        WriteHeader(folder);
    }

    bool WriteHeader(path const &folder) {
        ofstream stream(folder / (m_name + ".h"));
        if (!stream.is_open()) {
            return false;
        }
        tabs t(0);
        // file header
        stream << "/*" << endl;
        stream << "    Plugin-SDK (Grand Theft Auto) header file" << endl;
        stream << "    Authors: GTA Community. See more here" << endl;
        stream << "    https://github.com/GTAmodding/plugin-sdk" << endl;
        stream << "    Do not delete this comment block. Respect others' work!" << endl;
        stream << "*/" << endl;
        stream << "#pragma once" << endl;
        // include files
        // enums
        if (m_enums.size() > 0) {
            stream << endl;
            for (unsigned int i = 0; i < m_enums.size(); i++) {
                m_enums[i].Write(stream, t);
                stream << endl;
            }
        }
        return true;
    }

    bool WriteSource(path const &folder) {
        return false;
    }
};
