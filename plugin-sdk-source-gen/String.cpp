#include "String.h"

vector<string> String::Split(string const &str, string const &delim) {
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

void String::Replace(string &str, const string &what, const string &to) {
    for (size_t pos = 0; ; pos += to.length()) {
        pos = str.find(what, pos);
        if (pos == string::npos)
            break;
        str.erase(pos, what.length());
        str.insert(pos, to);
    }
}

bool String::StartsWith(string const &str, string const &what) {
    return !str.compare(0, what.size(), what);
}

bool String::Compare(string const &str, size_t index, char c) {
    return str.length() > index && str[index] == c;
}

bool String::IsNumber(string const &str) {
    if (str.empty())
        return false;
    for (char c : str) {
        if (!isdigit(c))
            return false;
    }
    return true;
}

void String::Trim(string &str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start != string::npos)
        str = str.substr(start);
    size_t end = str.find_last_not_of(" \t\r\n");
    if (end != string::npos)
        str = str.substr(0, end + 1);
}
