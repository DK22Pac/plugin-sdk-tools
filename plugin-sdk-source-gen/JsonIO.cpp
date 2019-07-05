#include "JsonIO.h"
#include <fstream>

int JsonIO::readJsonNumber(json const &node, string const &key) {
    auto it = node.find(key);
    if (it == node.end())
        return 0;
    if ((*it).is_string()) {
        unsigned int value = 0;
        sscanf_s((*it).get<string>().c_str(), "0x%X", &value);
        return value;
    }
    return (*it).get<int>();
}

string JsonIO::readJsonString(json const &node, string const &key) {
    return node.value(key, "");
}

bool JsonIO::readJsonBool(json const &node, string const &key) {
    return node.value(key, false);
}

json JsonIO::parse(path const &filepath) {
    ifstream file(filepath);
    if (file.is_open()) {
        try {
            json j = json::parse(file);
            return j;
        }
        catch (...) {}
    }
    return json();
}
