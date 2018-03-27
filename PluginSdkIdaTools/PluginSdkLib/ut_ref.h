#pragma once
#include "ida.hpp"

struct XRef {
    enum Type { // SDK's eHookType
        Call = 0,
        Jump = 1,
        Callback = 2
    };

    unsigned int m_address;
    unsigned int m_type;
    unsigned int m_objectid;
    unsigned int m_index;
};

qvector<XRef> getXrefToAddress(ea_t ea, bool isFunc = true);
qstring getXrefsToAddressAsString(ea_t ea);
