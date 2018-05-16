#pragma once

/*
    https://github.com/DK22Pac/plugin-sdk-tools
    Dmitri K. / DK22Pac ©
*/

// Most of RW functions and variables are not 'translated'!

inline unsigned int translate_gta3_11en_to_steam(unsigned int address) {
    if (address == 0)
        return 0;

    // .bss

    if (address >= 0x663EC0)
        return address + 0x10140;
    // other RW data
    if (address >= 0x661018)
        return 0;
    if (address >= 0x661010)
        return address + 0x10010;
    if (address >= 0x660FF8)
        return address + 0xFF88;
    if (address >= 0x660F78)
        return address + 0x10020;
    if (address >= 0x64C618)
        return address + 0x10008;
    if (address >= 0x621000)
        return address + 0x10000;

    // .data

    // other RW data
    if (address >= 0x618638)
        return 0;
    if (address >= 0x618520)
        return address + 0xD000;
    if (address >= 0x617A38)
        return address + 0xCFF8;
    if (address >= 0x6178F0)
        return address + 0xCE08;
    if (address >= 0x617700)
        return address + 0xD140;
    if (address >= 0x5F3E10)
        return address + 0xCFF8;
    if (address >= 0x5EA000)
        return address + 0xD000;

    // .text

    // other RW functions
    if (address >= 0x59E8B0)
        return 0;
    if (address >= 0x59CF50)
        return address - 0x110;
    if (address >= 0x59BC70)
        return address - 0x2F10;
    if (address >= 0x598E70)
        return address + 0x11D0;
    if (address >= 0x581FD0)
        return address - 0x110;
    if (address >= 0x567240)
        return address - 0x100;
    if (address >= 0x5670D0)
        return address - 0x80;
    if (address >= 0x53A090)
        return address - 0x50;
    if (address >= 0x48AF50)
        return address - 0x70;
    if (address >= 0x48AEE0 && address < 0x48AF50)
        return 0;

    return address;
}
