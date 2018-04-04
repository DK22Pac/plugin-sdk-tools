#pragma once
#include "idp.hpp"

class Variable {
public:
    unsigned int m_address = 0;
    unsigned int m_size = 0;
    qstring m_name;
    qstring m_demangledName;
    qstring m_type;
    qstring m_rawType;
    qstring m_module;
    qstring m_defaultValues;
    qstring m_comment;
    bool m_isReadOnly = false;

    static unsigned int Find(qstring const &name, qvector<Variable> const &entries);

    static qvector<Variable> FromCSV(char const *filepath);
    static qvector<Variable> FromReferenceCSV(char const *filepath, qvector<Variable> const &baseVars);

    static bool ToCSV(qvector<Variable> const &entries, char const *filepath, char const *version);

    static bool ToReferenceCSV(qvector<Variable> const &baseEntries, char const *baseVersion,
        qvector<Variable> const &entries, char const *version, char const *filepath);

    static bool ToReferenceCSV(qvector<Variable> const &baseEntries, char const *baseVersion,
        qvector<unsigned int> const &addresses, char const *version, char const *filepath);
};
