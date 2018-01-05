#include "Comments.h"
#include "String.h"

void WriteComment(ofstream &stream, string const &comment, tabs t, unsigned int pos) {
    if (comment.empty())
        return;
    auto lines = String::Split(comment, ";;");
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
