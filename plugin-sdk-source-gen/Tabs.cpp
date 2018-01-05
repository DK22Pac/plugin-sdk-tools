#include "Tabs.h"

tabs::tabs() {
    m_count = 0;
}

tabs::tabs(unsigned int count) {
    m_count = count;
}

string tabs::operator()() {
    return string(m_count * 4, ' ');
}

void tabs::operator+=(unsigned int const &count) {
    m_count += count;
}

void tabs::operator-=(unsigned int const &count) {
    if (m_count < count)
        m_count = 0;
    else
        m_count -= count;
}

tabs &tabs::operator++() {
    operator+=(1);
    return *this;
}

tabs &tabs::operator--() {
    operator-=(1);
    return *this;
}

tabs tabs::operator++(int) {
    tabs copy(*this);
    operator+=(1);
    return copy;
}

tabs tabs::operator--(int) {
    tabs copy(*this);
    operator-=(1);
    return copy;
}
