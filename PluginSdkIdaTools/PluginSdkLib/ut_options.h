#pragma once
#include "ut_string.h"

void getOptionsAndComment(qstring const &line, qstring &realComment, qstring &optionStr);
void getOptionValue(qstring const &optionStr, qstring const &fieldName, qstring &out);
void getOptionValue(qstring const &optionStr, qstring const &fieldName, int &out);
void getOptionValue(qstring const &optionStr, qstring const &fieldName, bool &out);
void getOptionValue(qstring const &optionStr, qstring const &fieldName);
void setOptionDefaultValue(qstring &opt);
void setOptionDefaultValue(int &opt);
void setOptionDefaultValue(bool &opt);

template <typename T, typename... Other>
void setOptionDefaultValue(T &opt, Other&... other) {
    setOptionDefaultValue(opt);
    setOptionDefaultValue(other...);
}

void getOptionValues(qstring const &optionsStr, qvector<qstring> const &options, unsigned int &optionsCounter);

template <typename T, typename... Other>
void getOptionValues(qstring const &optionsStr, qvector<qstring> const &options, unsigned int &optionsCounter, T &opt, Other&... other) {
    getOptionValue(optionsStr, options[optionsCounter++], opt);
    getOptionValues(optionsStr, options, optionsCounter, other...);
}

template<typename ...Types>
void getExtraInfo(qstring const &inCommentStr, qstring &outComment, qvector<qstring> const &options, Types&... out) {
    bool hasOptions = false;
    for (auto const &opt : options) {
        if (startsWith(inCommentStr, opt)) {
            hasOptions = true;
            break;
        }
    }
    if (!hasOptions) {
        setOptionDefaultValue(out...);
        outComment = inCommentStr;
    }
    else {
        qstring optionsStr;
        getOptionsAndComment(inCommentStr, outComment, optionsStr);
        unsigned int counter = 0;
        getOptionValues(optionsStr, options, counter, out...);
    }
    outComment.replace("\n", ";;");
}

void getStructExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outScope, bool &outIsStruct, bool &outIsAnonymous);
void getStructMemberExtraInfo(qstring const &line, qstring &outComment, qstring &outRawType, bool &outIsAnonymous);
void getEnumExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outScope, bool &outIsClass);
void getEnumMemberExtraInfo(qstring const &line, qstring &outComment, int &outBitWidth, bool &outIsCounter);
void getFunctionExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outRetType, qstring &outPriority, bool &outIsConst);
void getFunctionArgumentExtraInfo(qstring const &line, qstring const &argName, qstring &outRawType);
void getVariableExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outRawType);
