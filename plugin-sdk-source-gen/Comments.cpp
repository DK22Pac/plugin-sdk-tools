#include "Comments.h"
#include "StringEx.h"

void WriteComment(ofstream &stream, string const &comment, tabs t, unsigned int pos) {
    if (comment.empty())
        return;
    auto lines = String::Split(comment, ";;");
    IterateFirstLast(lines, [&](string &line, bool first, bool last) {
        if (pos == 0)
            stream << t();
        else {
            if (!first)
                stream << t() << string(pos, ' ');
            stream << ' ';
        }
        stream << "//!";
        if (pos != 0)
            stream << '<';
        stream << ' ' << line;
        if (!last || pos == 0)
            stream << endl;
    });
}

std::string GetPluginSdkComment(Games::IDs game, bool isHeader) {
    std::string result = "/*";
    result += '\n';
    result += "    Plugin-SDK ";
    if (game != Games::Unknown) {
        result += '(';
        result += Games::GetGameFullName(game);
        result += ") ";
        if (isHeader)
            result += "header";
        else
            result += "source";
        result += " file";
    }
    result += '\n';
    result += "    Authors: GTA Community. See more here";
    result += '\n';
    result += "    https://github.com/DK22Pac/plugin-sdk";
    result += '\n';
    result += "    Do not delete this comment block. Respect others' work!";
    result += '\n';
    result +=  "*/";
    return result;
}
