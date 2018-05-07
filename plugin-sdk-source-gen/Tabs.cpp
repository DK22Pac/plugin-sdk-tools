#include "Tabs.h"

tabs::tabs() {
    mCount = 0;
}

tabs::tabs(unsigned int count) {
    mCount = count;
}

string tabs::operator()() {
    if (mCount > 0)
        return string(mCount * 4, ' ');
    return "";
}

void tabs::operator+=(unsigned int const &count) {
    mCount += count;
}

void tabs::operator-=(unsigned int const &count) {
    if (mCount < count)
        mCount = 0;
    else
        mCount -= count;
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

unsigned int tabs::count() {
    return mCount;
}
