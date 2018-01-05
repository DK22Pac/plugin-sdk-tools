#pragma once
#include "Enum.h"
#include "Comments.h"
#include "String.h"

void Enum::Write(ofstream &stream, tabs t) {
    WriteComment(stream, m_comment, t, 0);
    stream << t() << "enum" << ' ';
    if (m_isClass)
        stream << "class" << ' ';
    stream << "PLUGIN_API" << ' ' << m_name << ' ';
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
            enumBaseTypeName += "long long";
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
        stream << (m_isHexademical? String::ToHexString(m_members[i].m_value) : to_string(m_members[i].m_value));
        if (i != (m_members.size() - 1))
            stream << ',';
        WriteComment(stream, m_members[i].m_comment, t, stream.tellp() - pos);
        stream << endl;
    }
    t--;
    stream << t() << '}' << ';';
}
