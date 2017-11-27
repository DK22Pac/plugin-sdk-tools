#include "ut_range.h"

unsigned int AddressRange::Start() const { return m_start; }
unsigned int AddressRange::End() const { return m_end; }
AddressRange::AddressRange(unsigned int start, unsigned int end) : m_start(start), m_end(end) {}

bool IsInRange(unsigned int value, AddressRange const &range) {
    return value >= range.Start() && value <= range.End();
}

bool IsInRange(unsigned int value, qvector<AddressRange> const &ranges) {
    for (auto const &range : ranges) {
        if (IsInRange(value, range))
            return true;
    }
    return false;
}

void AddRange(qvector<AddressRange> &ranges, unsigned int start, unsigned int end) {
    AddressRange range = { start, end };
    ranges.push_back(range);
}
