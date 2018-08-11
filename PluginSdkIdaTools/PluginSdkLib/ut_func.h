#pragma once
#include "idp.hpp"

class Function {
public:
    class Param {
    public:
        qstring m_name;
        qstring m_type;
        qstring m_defValue;
        bool m_rawType = false;
    };

    unsigned int m_address = 0;
    qstring m_name;
    qstring m_demangledName;
    qstring m_type;
    qstring m_module;
    qstring m_retType;
    qstring m_cc;
    qvector<Param> m_params;
    bool m_rawRetType = false;
    bool m_isConst = false;
    bool m_forceOverloaded = false;
    qstring m_refsStr;
    qstring m_comment;
    unsigned int m_priority = 1;
    int m_vtableIndex = -1;

    static Function const *Find(qstring const &name, qvector<Function> const &entries);

    static qvector<Function> FromCSV(char const *filepath);
    static qvector<Function> FromReferenceCSV(char const *filepath, qvector<Function> const &baseFuncs);

    static bool ToCSV(qvector<Function> const &entries, char const *filepath, char const *version);
    static bool ToReferenceCSV(qvector<Function> const &baseEntries, char const *baseVersion,
        qvector<Function> const &entries, char const *version, char const *filepath);
};
