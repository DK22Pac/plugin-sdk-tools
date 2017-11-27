#include "ut_string.h"
#include "idp.hpp"

qstring csvvalue(qstring const &value) {
    qstring retval = value;
    if (value.find(',') != qstring::npos) {
        retval.insert(0, '"');
        retval.append('"');
    }
    return retval;
}

void splitComment(qstring const &comment, qstring &realComment, qstring &optionStr) {
    auto lineEnd = comment.find('\n');
    qstring firstLine, otherLines;
    if (lineEnd != qstring::npos) {
        firstLine = comment.substr(0, lineEnd);
        if (comment.length() > lineEnd + 1)
            otherLines = comment.substr(lineEnd + 1);
    }
    else
        firstLine = comment;
    if (firstLine.find("module:") != qstring::npos || firstLine.find("attributes:") != qstring::npos) {
        optionStr = firstLine;
        realComment = otherLines;
    }
    else {
        optionStr.clear();
        realComment = comment;
    }
    realComment.replace("\n", "$n$");
}

qvector<qstring> extractValuesFromComment(qstring const &optionStr, qstring const &fieldName) {
    qvector<qstring> result;
    qstring _fieldName = fieldName;
    _fieldName += ":";
    auto fieldpos = optionStr.find(_fieldName);
    if (fieldpos != qstring::npos) {
        size_t valuepos = fieldpos + _fieldName.length();
        qstring str;
        for (size_t i = valuepos; i < optionStr.length(); i++) {
            if (optionStr[i] == '\r' || optionStr[i] == '\n' || optionStr[i] == ' ' || optionStr[i] == '\t') {
                qstring endChar;
                endChar.append(optionStr[i]);
                if (!str.empty()) {
                    result.push_back(str);
                    str.clear();
                }
                break;
            }
            if (optionStr[i] == ',' && !str.empty()) {
                result.push_back(str);
                str.clear();
            }
            str.append(optionStr[i]);
        }
        if (!str.empty())
            result.push_back(str);
    }
    return result;
}

qstring readMemberComment(qstring const &commentLine) {
    qstring result = commentLine;
    result.replace("\n", "$n$");
    return result;
}

bool startsWith(qstring const &strToCheck, qstring const &strStart) {
    return strToCheck.substr(0, strStart.length()) == strStart;
}

void addValueToStringLineList(qstring &line, qstring const &value) {
    if (!line.empty())
        line.append(' ');
    line.append(value);
}

bool isValidCharacterForFileName(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}

qstring getValidFileName(qstring const &oldFileName) {
    qstring newFileName;
    for (size_t i = 0; i < oldFileName.length(); i++) {
        auto c = oldFileName[i];
        if (isValidCharacterForFileName(c))
            newFileName.append(c);
        else
            newFileName.append('_');
    }
    return newFileName;
}

unsigned int &OrderingSignsCount() {
    static unsigned int count = 0;
    return count;
}

void startWritingToJson() {
    OrderingSignsCount() = 0;
}

char const *jsonOrderedName(char const *name) {
    static char orderedName[32];
    qsnprintf(orderedName, 32, "$ORD%010d$%s", OrderingSignsCount(), name);
    OrderingSignsCount()++;
    return orderedName;
}

void jsonRemoveOrderingSigns(qstring &str) {
    qstring newstr;
    size_t b = 0;
    size_t e = str.find("$ORD");
    while (e != qstring::npos) {
        if (e > b)
            newstr.append(str.substr(b, e));
        b = str.find("$", e + 4);
        if (b == qstring::npos)
            break;
        b += 1;
        e = str.find("$ORD", b);
    }
    if (b != qstring::npos) {
        auto len = str.length();
        if (b < len)
            newstr.append(str.substr(b));
    }
    str = newstr;
}

void endWritingToJson() {
    OrderingSignsCount() = 0;
}
