#pragma once
#include "ida.hpp"

class AddressRange {
    unsigned int m_start, m_end;
public:
    AddressRange(unsigned int start, unsigned int end);
    unsigned int Start() const;
    unsigned int End() const;
};

bool IsInRange(unsigned int value, AddressRange const &range);

bool IsInRange(unsigned int value, qvector<AddressRange> const &ranges);

void AddRange(qvector<AddressRange> &ranges, unsigned int start, unsigned int end);
