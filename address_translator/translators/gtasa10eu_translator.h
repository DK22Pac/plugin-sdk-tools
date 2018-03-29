#pragma once

/*
    https://github.com/CookiePLMonster/RawMouse/blob/master/RawMouse/MemoryMgr.h
*/

inline unsigned int translate_gtasa_10eu(unsigned int address) {
    if (address > 0x7466D0) {
        if (address < 0x7BA940)
            return address + 0x50;
        return address + 0x40;
	}
    return address;
}
