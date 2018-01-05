#include "Struct.h"
#include "Comments.h"
#include "String.h"

bool Struct::Member::IsPadding() {
    return !strncmp(m_name.c_str(), "_pad", 4) || !strncmp(m_name.c_str(), "__pad", 5);
}

void Struct::Write(ofstream &stream, tabs t) {
    if (!m_parent.empty()) {

    }
    WriteComment(stream, m_comment, t, 0);
    stream << t();
    Access access = Access::Public;
    if (m_kind == Kind::Struct)
        stream << "struct";
    else if (m_kind == Kind::Union)
        stream << "union";
    else {
        stream << "class";
        access = Access::Private;
    }
    stream << ' ';
    if (!m_isAnonymous)
        stream << "PLUGIN_API" << ' ' << m_name << ' ';
    if (!m_parent.empty())
        stream << ": public " << m_parent << ' ';
    stream << '{' << endl;
    if (access == Access::Private) {
        stream << "public:" << endl;
        access = Access::Public;
    }
    t++;
    for (unsigned int i = 0; i < m_members.size(); i++) {
        if (!m_parent.empty() && i == 0)
            continue;
        Struct::Member &m = m_members[i];
        if (m.IsPadding()) {
            if (access == Access::Public) {
                stream << "private:" << endl;
                access = Access::Private;
            }
        }
        else {
            if (access == Access::Private) {
                stream << "public:" << endl;
                access = Access::Public;
            }
        }
        stream << t();
        auto pos = stream.tellp();
        stream << m.m_type.BeforeName() << ' ' << m.m_name << m.m_type.AfterName() << ';';
        WriteComment(stream, m.m_comment, t, stream.tellp() - pos);
        stream << endl;
    }
    if (access == Access::Private) {
        stream << "public:" << endl;
        access = Access::Public;
    }
    // functions and other things
    t--;
    stream << t() << '}' << ';' << endl;
    stream << endl;
    stream << "VALIDATE_SIZE(" << m_name << ',' << ' ' << String::ToHexString(m_size) << ')' << ';';
}

bool Struct::ContainsType(string const &typeName, bool withPointers) {
    //for (Member const &m : m_members) {
    //    if (m.m_type.mName == typeName) {
    //        if (withPointers || !m.m_type.IsPointer())
    //    }
    //}
}
