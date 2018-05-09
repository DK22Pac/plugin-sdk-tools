#include "StringEx.h"
#include <algorithm>

Vector<string> String::Split(string const &str, string const &delim) {
    Vector<string> result;
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

void String::Break(string const &str, string const &breakWord, string &outLeft, string &outRight, bool reverseSearch) {
    string stringToBreak = str;
    size_t wordPos;
    if (reverseSearch)
        wordPos = stringToBreak.rfind(breakWord);
    else
        wordPos = stringToBreak.find(breakWord);
    if (wordPos != string::npos) {
        outLeft = stringToBreak.substr(0, wordPos);
        outRight = stringToBreak.substr(wordPos + breakWord.length());
    }
    else {
        outLeft.clear();
        outRight = stringToBreak;
    }
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

bool String::EndsWith(string const &str, string const &what) {
    auto strLen = str.length();
    auto whatLen = what.length();
    if (whatLen > strLen)
        return false;
    return str.substr(strLen - whatLen) == what;
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

int String::ToNumber(string const &str) {
    return (StartsWith(str, "0x") ? strtol(str.substr(2).c_str(), nullptr, 16) : strtol(str.c_str(), nullptr, 10));
}

void String::Trim(string &str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start != string::npos)
        str = str.substr(start);
    size_t end = str.find_last_not_of(" \t\r\n");
    if (end != string::npos)
        str = str.substr(0, end + 1);
}

string String::ToUpper(string const &str) {
    string result;
    for (size_t i = 0; i < str.length(); i++)
        result += toupper(static_cast<unsigned char>(str[i]));
    return result;
}

string String::ToLower(string const &str) {
    string result;
    for (size_t i = 0; i < str.length(); i++)
        result += tolower(static_cast<unsigned char>(str[i]));
    return result;
}
