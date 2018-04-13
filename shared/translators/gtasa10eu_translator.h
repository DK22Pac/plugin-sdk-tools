#pragma once

/*
    https://github.com/CookiePLMonster/RawMouse/blob/master/RawMouse/MemoryMgr.h
    https://github.com/thelink2012/modloader/blob/master/src/translator/gta3/sa/10eu.hpp

*/

inline unsigned int translate_gtasa_10eu(unsigned int address) {
    if (address >= 0x746720 && address < 0x857000) {
        if (address >= 0x7BA940)
            return address + 0x40;
        return address + 0x50;
	}
    return address;
}
