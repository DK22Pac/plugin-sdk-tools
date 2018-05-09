#pragma once
#include <string>
#include "ListEx.h"

using namespace std;

class String {
public:
    static Vector<string> Split(string const &str, string const &delim);
    static void Break(string const &str, string const &breakWord, string &outLeft, string &outRight, bool reverseSearch);
    static void Replace(string &str, const string &what, const string &to);
    static bool StartsWith(string const &str, string const &with);
    static bool EndsWith(string const &str, string const &with);
    static bool Compare(string const &str, size_t index, char c);
    static bool IsNumber(string const &str);
    static int ToNumber(string const &str);
    static string ToUpper(string const &str);
    static string ToLower(string const &str);

    static void Trim(string &str);

    template <typename T, typename t = enable_if_t<is_integral_v<T>>>
    static string ToHexString(T intVal, bool prefix = true) {
        if (intVal == 0)
            return "0";
        if (prefix)
            return Format("0x%X", intVal);
        return Format("%X", intVal);
    }

    template<typename ...ArgTypes>
    static string Format(const string &format, ArgTypes... args) {
        static char buf[1024];
        snprintf(buf, 1024, format.c_str(), args...);
        return buf;
    }
};
