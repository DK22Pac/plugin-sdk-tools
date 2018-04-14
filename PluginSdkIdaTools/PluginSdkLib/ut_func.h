#pragma once
#include "idp.hpp"

class Function {
public:
    class Param {
    public:
        qstring m_name;
        qstring m_type;
        bool m_rawType = false;
    };

    unsigned int m_address = 0;
    qstring m_name;
    qstring m_demangledName;
    qstring m_type;
    qstring m_module;
    qstring m_retType;
    bool m_rawRetType = false;
    qstring m_cc;
    qvector<Param> m_params;
    bool m_isConst = false;
    qstring m_refsStr;
    qstring m_comment;
    unsigned int m_priority = 1;

    static Function const *Find(qstring const &name, qvector<Function> const &entries);

    static qvector<Function> FromCSV(char const *filepath);
    static qvector<Function> FromReferenceCSV(char const *filepath, qvector<Function> const &baseFuncs);

    static bool ToCSV(qvector<Function> const &entries, char const *filepath, char const *version);
    static bool ToReferenceCSV(qvector<Function> const &baseEntries, char const *baseVersion,
        qvector<Function> const &entries, char const *version, char const *filepath);
    static bool ToReferenceCSV(qvector<Function> const &baseEntries, char const *baseVersion,
        qvector<Function const *> const &refEntries, char const *version, char const *filepath);
};
