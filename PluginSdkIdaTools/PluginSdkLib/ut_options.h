#pragma once
#include "ut_string.h"

void getOptionsAndComment(qstring const &line, qvector<qstring> const &options, qstring &realComment, qstring &optionStr);
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
    qstring optionsStr;
    getOptionsAndComment(inCommentStr, options, outComment, optionsStr);
    if (optionsStr.size() > 0) {
        unsigned int counter = 0;
        getOptionValues(optionsStr, options, counter, out...);
    }
    else {
        setOptionDefaultValue(out...);
        outComment = inCommentStr;
    }
    outComment.replace("\n", ";;");
}

void getStructExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outScope, bool &outIsStruct,
    bool &outIsAnonymous, bool &outIsCoreClass, bool &outIsAbstractClass, bool &outHasVectorDeletingDtor);
void getStructMemberExtraInfo(qstring const &line, qstring &outComment, qstring &outRawType, bool &outIsAnonymous, bool &outIsBase);
void getEnumExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outScope, bool &outIsClass);
void getEnumMemberExtraInfo(qstring const &line, qstring &outComment, int &outBitWidth, bool &outIsCounter);
void getFunctionExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outRetType,
    qstring &outPriority, bool &outIsConst);
void getFunctionArgumentExtraInfo(qstring const &line, qstring const &argName, qstring &outRawType, qstring &outDefValue);
void getVariableExtraInfo(qstring const &line, qstring &outComment, qstring &outModuleName, qstring &outRawType);

qvector<qstring> getFunctionOptions();
qvector<qstring> getVariableOptions();
qvector<qstring> getEnumOptions();
qvector<qstring> getEnumMemberOptions();
qvector<qstring> getStructOptions();
qvector<qstring> getStructMemberOptions();
