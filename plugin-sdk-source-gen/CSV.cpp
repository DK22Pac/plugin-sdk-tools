#include "CSV.h"

void CSV::ReadParam(string const &line, unsigned int &currIndex, string &param) {
    string tmp;
    bool startedQuotes = false;
    bool inQuotes = false;
    while (currIndex < line.length()) {
        auto c = line[currIndex];
        if (c == '\r' || c == '\n') {
            currIndex = line.length();
            break;
        }
        if (!startedQuotes) {
            if (c == '"')
                startedQuotes = true;
            else if (!inQuotes && c == ',') {
                currIndex++;
                break;
            }
            else
                tmp += c;
        }
        else {
            if (c != '"') {
                inQuotes = !inQuotes;
                if (!inQuotes && c == ',') {
                    currIndex++;
                    break;
                }
            }
            startedQuotes = false;
            tmp += c;
        }
        currIndex++;
    }
    param = tmp;
    String::Trim(param);
}

vector<string> CSV::ReadLines(ifstream &file) {
    // comments are not supported
    // empty lines are not ignored
    vector<string> result;
    bool firstLine = true;
    for (string line; getline(file, line);) {
        if (!firstLine)
            result.push_back(line);
        else
            firstLine = false;
    }
    return result;
}

string CSV::Value(string const &value) {
    string retval = value;
    if (value.find(',') != string::npos) {
        retval.insert(0, "\"");
        retval.append("\"");
    }
    return retval;
}
