#pragma once
#include "idp.hpp"
#include "struct.hpp"

class Struct {
public:
    enum Kind {
        STRT_STRUCT,
        STRT_CLASS,
        STRT_UNION
    };

    class Member {
    public:
        qstring m_name;
        qstring m_type;
        qstring m_rawType;
        qstring m_comment;
        unsigned int m_size = 0;
        unsigned int m_offset = 0;
        bool m_isAnonymous = false;
        bool m_isString = false;

        Member() = default;
    };

    qstring m_name;
    qstring m_module;
    qstring m_scope;
    qstring m_comment;
    unsigned int m_size = 0;
    unsigned int m_alignment = 0;
    Kind m_kind = STRT_STRUCT;
    bool m_isAnonymous = false;

    qvector<Member> m_members;

    // ida struct
    struc_t *m_idaStruct = nullptr;
    tid_t m_idaStructId = 0;

    Struct() = default;
};

bool isSystemStruct(qstring const &s);

qstring structMemberErrorMessage(int errorCode);
