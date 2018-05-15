#pragma once

/*
    https://github.com/spaceeinstein/gtalc-dinput8/blob/master/vcversion/src/VerAdjust.cpp
*/

inline unsigned int translate_gtavc_steam(unsigned int address) {
    if (address < 0x41CC07)
        return address - 0x0;
    else if (address < 0x41CC20)
        return address - 0x37;
    else if (address < 0x434730)
        return address - 0x30;
    else if (address < 0x435029)
        return address - 0x40;
    else if (address < 0x435040)
        return address - 0x83;
    else if (address < 0x443E00)
        return address - 0x90;
    else if (address < 0x4518D3)
        return address - 0xF0;
    else if (address < 0x4518E0)
        return address - 0x124;
    else if (address < 0x489D10)
        return address - 0x120;
    else if (address < 0x489D82)
        return address - 0x113;
    else if (address < 0x489E10)
        return address - 0x112;
    else if (address < 0x48E6FD)
        return address - 0x110;
    else if (address < 0x48E830)
        return address - 0x10E;
    else if (address < 0x48EC30)
        return address - 0xF2;
    else if (address < 0x498FF3)
        return address - 0xF0;
    else if (address < 0x49B993)
        return address - 0xDF;
    else if (address < 0x49BB91)
        return address - 0xF6;
    else if (address < 0x49BD00)
        return address - 0x131;
    else if (address < 0x4A4360)
        return address - 0x141;
    else if (address < 0x4A448A)
        return address - 0x140;
    else if (address < 0x4A4586)
        return address - 0x18D;
    else if (address < 0x4A4670)
        return address - 0x186;
    else if (address < 0x4A4734)
        return address - 0x180;
    else if (address < 0x4A47B0)
        return address - 0x144;
    else if (address < 0x4A4BE3)
        return address - 0x140;
    else if (address < 0x4A4CF3)
        return address - 0x139;
    else if (address < 0x4A4FD0)
        return address - 0x133;
    else if (address < 0x4C4230)
        return address - 0x130;
    else if (address < 0x516F87)
        return address - 0x140;
    else if (address < 0x517060)
        return address - 0x111;
    else if (address < 0x57BF40)
        return address - 0x110;
    else if (address < 0x5CC921)
        return address - 0x1D0;
    else if (address < 0x5CC9E0)
        return address - 0x225;
    else if (address < 0x5D7270)
        return address - 0x230;
    else if (address < 0x5D7A12)
        return address - 0x2D0;
    else if (address < 0x5D7A40)
        return address - 0x3A2;
    else if (address < 0x600FB0)
        return address - 0x3A0;
    else if (address < 0x602000)
        return address - 0x390;
    else if (address < 0x61D3D0)
        return address - 0x3E0;
    else if (address < 0x62121B)
        return address - 0x3C0;
    else if (address < 0x621230)
        return address - 0x3AF;
    else if (address < 0x626F80)
        return address - 0x3B0;
    else if (address < 0x62721B)
        return address - 0x3B0;
    else if (address < 0x6272E0)
        return address - 0x381;
    else if (address < 0x6274D0)
        return address - 0x380;
    else if (address < 0x627570)
        return address - 0x370;
    else if (address < 0x627590)
        return address - 0x34A;
    else if (address < 0x63762B)
        return address - 0x340;
    else if (address < 0x639268)
        return address - 0x341;
    else if (address < 0x67E000)
        return address - 0x1050;
    else if (address < 0x683000)
        return address - 0x1000;
    else if (address < 0x68DD7C)
        return address - 0x1000;
    else if (address < 0x69A020)
        return address - 0xFF8;
    else if (address < 0x69A0C8)
        return address - 0x1004;
    else if (address < 0x6A5580)
        return address - 0x1000;
    else if (address < 0x6A5740)
        return address - 0x1004;
    else if (address < 0x6D668C)
        return address - 0x1008;
    else if (address < 0x6D6B28)
        return address - 0x1034;
    else if (address < 0x6DB8F0)
        return address - 0x1030;
    else if (address < 0x6DB924)
        return address - 0x1038;
    else if (address < 0x6DCAE0)
        return address - 0x1018;
    else if (address < 0x6F23C0)
        return address - 0x1068;
    else if (address < 0x6F2434)
        return address - 0x1008;
    else if (address < 0x6F245C)
        return address - 0x100C;
    else if (address < 0x6F7000)
        return address - 0x1010;
    else if (address < 0x786BA8)
        return address - 0x1000;
    else if (address < 0x786D40)
        return address - 0xFFC;
    else if (address < 0xA10B20)
        return address - 0xFF8;
    else if (address < 0xA10B4E)
        return address - 0xFF7;
    else
        return address - 0xFF6;
}
