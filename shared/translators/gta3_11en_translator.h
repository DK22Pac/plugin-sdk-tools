#pragma once

/*
    https://github.com/DK22Pac/plugin-sdk-tools
    Dmitri K. / DK22Pac ©
*/

inline unsigned int translate_gta3_11en(unsigned int address) {
    // .bss
    if (address >= 0x95CDAE)
        return address + 0x1B8;
    if (address == 0x95CDAD)
        return 0x5FDB84;
    if (address >= 0x95CD95)
        return address + 0x1B9;
    if (address >= 0x8F6000)
        return address + 0x1B8;
    if (address >= 0x8E2A6C)
        return address + 0xB4;
    if (address >= 0x8472E0)
        return address - 0x50;
    // .CRT
    if (address >= 0x61C000)
        return address;
    // .data
    if (address >= 0x618038)
        return address - 0x938;
    if (address >= 0x618034 && address < 0x618038)
        return 0;
    if (address >= 0x618030)
        return address - 0x934;
    if (address >= 0x617EAC)
        return address - 0x948;
    if (address >= 0x617DA4 && address < 0x617EAC)
        return 0;
    if (address >= 0x61167C)
        return address - 0x840;
    if (address >= 0x611574 && address < 0x61167C)
        return 0;
    if (address >= 0x602A60)
        return address - 0x738;
    if (address >= 0x602958 && address < 0x602A60)
        return 0;
    if (address >= 0x60269C)
        return address - 0x630;
    if (address >= 0x602698)
        return address - 0x634;
    if (address >= 0x60217C)
        return address - 0x638;
    if (address >= 0x602070 && address < 0x60217C)
        return 0;
    if (address >= 0x601E10)
        return address - 0x530;
    if (address >= 0x601D08 && address < 0x601E10)
        return 0;
    if (address >= 0x601CE8)
        return address - 0x428;
    if (address >= 0x601BE0 && address < 0x601CE8)
        return 0;
    if (address >= 0x6011A4)
        return address - 0x320;
    if (address >= 0x60109C && address < 0x6011A4)
        return 0;
    if (address >= 0x5FDDE0)
        return address - 0x218;
    if (address >= 0x5FDD9C)
        return address - 0x214;
    if (address >= 0x5F5714)
        return address - 0x218;
    if (address >= 0x5F560C && address < 0x5F5714)
        return 0;
    if (address >= 0x5F4C0C)
        return address - 0x110;
    if (address >= 0x5F4C08 && address < 0x5F4C0C)
        return 0;
    if (address >= 0x5F3F4C)
        return address - 0x10C;
    if (address >= 0x5F3F48 && address < 0x5F3F4C)
        return 0;
    if (address >= 0x5F0A58)
        return address - 0x108;
    if (address >= 0x5F094C && address < 0x5F0A58)
        return 0;
    // _rwcseg
    if (address >= 0x5E4000)
        return address;
    // .text
    if (address >= 0x5E3220)
        return 0;
    if (address >= 0x598B80)
        return address + 0x2C0;
    if (address >= 0x598B30)
        return address + 0x280;
    if (address >= 0x597560)
        return address + 0x270;
    if (address >= 0x597420)
        return address + 0x280;
    if (address >= 0x5973F0 && address < 0x597420)
        return 0;
    if (address >= 0x591910)
        return address + 0x2B0;
    if (address >= 0x590A00)
        return address + 0x2C0;
    if (address >= 0x5909D0 && address < 0x590A00)
        return 0;
    if (address >= 0x58B7A0)
        return address + 0x2F0;
    if (address >= 0x581460)
        return address + 0x340;
    if (address >= 0x566F50)
        return address + 0x350;
    if (address >= 0x566F20)
        return address + 0x320;
    if (address >= 0x566DC0)
        return address + 0x310;
    if (address >= 0x564D00)
        return address + 0x140;
    if (address >= 0x554FE0)
        return address + 0x130;
    if (address >= 0x554EA0)
        return address + 0x140;
    if (address >= 0x554E70 && address < 0x554EA0)
        return 0;
    if (address >= 0x554CA0)
        return address + 0x170;
    if (address >= 0x54E100)
        return address + 0x140;
    if (address >= 0x54E000)
        return address + 0x150;
    if (address >= 0x54DFD0 && address < 0x54E000)
        return 0;
    if (address >= 0x54A9B0)
        return address + 0x180;
    if (address >= 0x54A980)
        return address + 0x190;
    if (address >= 0x54A950 && address < 0x54A980)
        return 0;
    if (address >= 0x54A8A0)
        return address + 0x1C0;
    if (address >= 0x54A640)
        return address + 0x1D0;
    if (address >= 0x54A610 && address < 0x54A640)
        return 0;
    if (address >= 0x545760)
        return address + 0x200;
    if (address >= 0x5454D0)
        return address + 0x210;
    if (address >= 0x5454A0 && address < 0x5454D0)
        return 0;
    if (address >= 0x521770)
        return address + 0x240;
    if (address >= 0x518130)
        return address + 0x230;
    if (address >= 0x511B90)
        return address + 0x210;
    if (address >= 0x50D140)
        return address + 0x1F0;
    if (address >= 0x50B850)
        return address + 0xF0;
    if (address >= 0x4F6CE0)
        return address + 0xE0;
    if (address >= 0x4DD5D0)
        return address + 0xB0;
    if (address >= 0x4C1B50)
        return address + 0xA0;
    if (address >= 0x499890)
        return address + 0xF0;
    if (address >= 0x48DD60)
        return address + 0xC0;
    if (address >= 0x48DA50)
        return address + 0xD0;
    if (address >= 0x48DA20 && address < 0x48DA50)
        return 0;
    if (address >= 0x48C4B0)
        return address + 0x100;
    if (address >= 0x48AE60)
        return address + 0xF0;
    if (address >= 0x489270)
        return address + 0x110;
    if (address >= 0x488420)
        return address + 0x100;
    if (address >= 0x47F2B0)
        return address + 0xD0;
    if (address >= 0x47A440)
        return address - 0x10;
    if (address >= 0x4731C0)
        return address;
    if (address >= 0x46BEB0)
        return address - 0x20;
    if (address >= 0x46B1F0)
        return address - 0x30;
    if (address >= 0x46B100)
        return address - 0x20;
    if (address >= 0x46B0D0 && address < 0x46B100)
        return 0;
    if (address >= 0x461730)
        return address + 0x10;
    return address;
}
