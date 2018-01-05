#pragma once
#include <string>
#include <vector>

using namespace std;

class String {
public:
    template <typename T, typename t = enable_if_t<is_integral_v<T>>>
    static string ToHexString(T intVal) {
        static char strhex[32];
        snprintf(strhex, 32, "0x%X", intVal);
        return strhex;
    }

    static vector<string> Split(string const &str, string const &delim);

    static void Replace(string &str, const string &what, const string &to);

    static bool StartsWith(string const &str, string const &with);

    static bool Compare(string const &str, size_t index, char c);

    static bool IsNumber(string const &str);

    static void Trim(string &str);
};
