#include "JsonIO.h"

int JsonIO::readJsonNumber(json const &node, string const &key) {
    auto it = node.find(key);
    if (it == node.end())
        return 0;
    if ((*it).is_string()) {
        int value = 0;
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
