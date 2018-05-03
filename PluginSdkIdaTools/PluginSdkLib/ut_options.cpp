#include "ut_options.h"

void getOptionsAndComment(qstring const &line, qvector<qstring> const &options, qstring &realComment, qstring &optionStr) {
    optionStr.clear();
    realComment.clear();
    bool hasOptions = false;
    qstring lineUpper = toUpper(line);
    for (auto const &opt : options) {
        if (startsWith(lineUpper, toUpper(opt))) {
            hasOptions = true;
            break;
        }
    }
    auto lineEnd = line.find('\n');
    if (hasOptions) {
        if (lineEnd != qstring::npos) {
            optionStr = line.substr(0, lineEnd);
            if (line.length() > lineEnd + 1)
                realComment = line.substr(lineEnd + 1);
        }
        else
            optionStr = line;
    }
    else
        realComment = line;
}

void getOptionValue(qstring const &optionStr, qstring const &fieldName, qstring &out) {
    out = "";
    qstring _fieldName = fieldName + ":";
    auto fieldpos = optionStr.find(_fieldName);
    if (fieldpos != qstring::npos) {
        size_t optionStrLen = optionStr.length();
        size_t valuepos_start = fieldpos + _fieldName.length();
        if (valuepos_start < optionStrLen) {
            if (optionStr[valuepos_start] == '"')
                out = optionStr.substr(valuepos_start + 1, optionStr.find('"', valuepos_start + 1));
            else
                out = optionStr.substr(valuepos_start, optionStr.find(' ', valuepos_start));
        }
    }
}

void getOptionValue(qstring const &optionStr, qstring const &fieldName, int &out) {
    qstring opt;
    getOptionValue(optionStr, fieldName, opt);
    if (opt.empty())
        out = 0;
    else
        out = atoi(opt.c_str());
}

void getOptionValue(qstring const &optionStr, qstring const &fieldName, bool &out) {
    qstring opt;
    getOptionValue(optionStr, fieldName, opt);
    out = opt == "true";
}

void getOptionValue(qstring const &optionStr, qstring const &fieldName) {}

void setOptionDefaultValue(qstring &opt) {
    opt = "";
}

void setOptionDefaultValue(int &opt) {
    opt = 0;
}

void setOptionDefaultValue(bool &opt) {
    opt = false;
}

void getOptionValues(qstring const &optionsStr, qvector<qstring> const &options, unsigned int &optionsCounter) {}

void getStructExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outScope, bool &outIsStruct, bool &outIsAnonymous,
    bool &outIsCoreClass)
{
    getExtraInfo(line, outComment, getStructOptions(), outModuleName, outScope, outIsStruct, outIsAnonymous, outIsCoreClass);
}

void getStructMemberExtraInfo(qstring const &line, qstring &outComment, qstring &outRawType, bool &outIsAnonymous, bool &outIsBase) {
    getExtraInfo(line, outComment, getStructMemberOptions(), outRawType, outIsAnonymous, outIsBase);
}

void getEnumExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outScope, bool &outIsClass) {
    getExtraInfo(line, outComment, getEnumOptions(), outModuleName, outScope, outIsClass);
}

void getEnumMemberExtraInfo(qstring const &line, qstring &outComment, int &outBitWidth, bool &outIsCounter) {
    getExtraInfo(line, outComment, getEnumMemberOptions(), outBitWidth, outIsCounter);
}

void getFunctionExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outRetType, qstring &outPriority, bool &outIsConst) {
    getExtraInfo(line, outComment, getFunctionOptions(), outModuleName, outRetType, outPriority, outIsConst);
}

void getFunctionArgumentExtraInfo(qstring const &line, qstring const &argName, qstring &outRawType) {
    auto lineEnd = line.find('\n');
    if (lineEnd != qstring::npos)
        getOptionValue(line.substr(0, lineEnd), argName, outRawType);
    else
        getOptionValue(line, argName, outRawType);
}

void getVariableExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outRawType) {
    getExtraInfo(line, outComment, getVariableOptions(), outModuleName, outRawType);
}

qvector<qstring> getFunctionOptions() {
    qvector<qstring> options;
    options.push_back("module");
    options.push_back("rettype");
    options.push_back("priority");
    options.push_back("isconst");
    return options;
}

qvector<qstring> getVariableOptions() {
    qvector<qstring> options;
    options.push_back("module");
    options.push_back("rawtype");
    return options;
}

qvector<qstring> getEnumOptions() {
    qvector<qstring> options;
    options.push_back("module");
    options.push_back("scope");
    options.push_back("isclass");
    return options;
}

qvector<qstring> getEnumMemberOptions() {
    qvector<qstring> options;
    options.push_back("bitwidth");
    options.push_back("iscounter");
    return options;
}

qvector<qstring> getStructOptions() {
    qvector<qstring> options;
    options.push_back("module");
    options.push_back("scope");
    options.push_back("isstruct");
    options.push_back("isanonymous");
    options.push_back("iscore");
    return options;
}

qvector<qstring> getStructMemberOptions() {
    qvector<qstring> options;
    options.push_back("rawtype");
    options.push_back("isanonymous");
    options.push_back("isbase");
    return options;
}
