#include <idc.idc>

static recolor(vtableStart, vtableEnd) {
    auto currentAddr = vtableStart;
    while (currentAddr <= vtableEnd) {
        auto funcAddr = Dword(currentAddr);
        SetFunctionAttr(funcAddr, FUNCATTR_COLOR, 0xcfffbc);
        currentAddr = currentAddr + 4;
    }
}

static main (void) {
    recolor(0x871680, 0x871798);
}