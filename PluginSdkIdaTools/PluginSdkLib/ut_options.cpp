#include "ut_options.h"

void getOptionsAndComment(qstring const &line, qstring &realComment, qstring &optionStr) {
    auto lineEnd = line.find('\n');
    if (lineEnd != qstring::npos) {
        optionStr = line.substr(0, lineEnd);
        if (line.length() > lineEnd + 1)
            realComment = line.substr(lineEnd + 1);
        else
            realComment = "";
    }
    else {
        optionStr = line;
        realComment = "";
    }
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

void getStructExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outScope, bool &outIsStruct, bool &outIsAnonymous) {
    qvector<qstring> options;
    options.push_back("module");
    options.push_back("scope");
    options.push_back("isstruct");
    options.push_back("isanonymous");
    getExtraInfo(line, outComment, options, outModuleName, outScope, outIsStruct, outIsAnonymous);
}

void getStructMemberExtraInfo(qstring const &line, qstring &outComment, qstring &outRawType, bool &outIsAnonymous) {
    qvector<qstring> options;
    options.push_back("rawtype");
    options.push_back("isanonymous");
    getExtraInfo(line, outComment, options, outRawType, outIsAnonymous);
}

void getEnumExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outScope, bool &outIsClass) {
    qvector<qstring> options;
    options.push_back("module");
    options.push_back("scope");
    options.push_back("isclass");
    getExtraInfo(line, outComment, options, outModuleName, outScope, outIsClass);
}

void getEnumMemberExtraInfo(qstring const &line, qstring &outComment, int &outBitWidth, bool &outIsCounter) {
    qvector<qstring> options;
    options.push_back("bitwidth");
    options.push_back("iscounter");
    getExtraInfo(line, outComment, options, outBitWidth, outIsCounter);
}

void getFunctionExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outRetType, qstring &outPriority, bool &outIsConst) {
    qvector<qstring> options;
    options.push_back("module");
    options.push_back("rettype");
    options.push_back("priority");
    options.push_back("isconst");
    getExtraInfo(line, outComment, options, outModuleName, outRetType, outIsConst);
}

void getFunctionArgumentExtraInfo(qstring const &line, qstring const &argName, qstring &outRawType) {
    auto lineEnd = line.find('\n');
    if (lineEnd != qstring::npos)
        getOptionValue(line.substr(0, lineEnd), argName, outRawType);
    else
        getOptionValue(line, argName, outRawType);
}

void getVariableExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outRawType) {
    qvector<qstring> options;
    options.push_back("module");
    options.push_back("rawtype");
    getExtraInfo(line, outComment, options, outModuleName, outRawType);
}
