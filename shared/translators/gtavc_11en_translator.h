#pragma once

/*
    https://github.com/spaceeinstein/gtalc-dinput8/blob/master/vcversion/src/VerAdjust.cpp
*/

inline unsigned int translate_gtavc_11en(unsigned int address) {
    if (address < 0x489D0C)
        return address;
    else if (address < 0x489D10)
        return address + 0x04;
    else if (address < 0x489D82)
        return address + 0x0D;
    else if (address < 0x489E0D)
        return address + 0x0E;
    else if (address < 0x498FF3)
        return address + 0x10;
    else if (address < 0x4A4353)
        return address + 0x21;
    else if (address < 0x600FB0)
        return address + 0x20;
    else if (address < 0x601FF9) {
        if (address < 0x601E83)
            return address + 0x30;
        else if (address < 0x601EAB)
            return address + 0x2E;
        else if (address < 0x601EAD)
            return address + 0x2C;
        else if (address < 0x601EDC)
            return address + 0x30;
        else if (address < 0x601EDE)
            return address + 0x2E;
        else if (address < 0x601F0E)
            return address + 0x30;
        else if (address < 0x601F10)
            return address + 0x2E;
        else if (address < 0x601F48)
            return address + 0x30;
        else if (address < 0x601F50)
            return address + 0x2E;
        else if (address < 0x601F5F)
            return address + 0x28;
        else if (address < 0x601F8C)
            return address + 0x26;
        else
            return address + 0x24;
    }
    else if (address < 0x626F79)
        return address - 0x20;
    else if (address < 0x62758D) {
        if (address < 0x626F7F)
            return address - 0x1B;
        else if (address < 0x6271C9)
            return address - 0x20;
        else if (address < 0x6271D8)
            return address - 0x19;
        else if (address < 0x6271ED)
            return address - 0x20;
        else if (address < 0x6271F7)
            return address - 0x14;
        else if (address < 0x627203)
            return address - 0x02;
        else if (address < 0x62720A)
            return address + 0x20;
        else if (address < 0x627214)
            return address - 0x01;
        else if (address < 0x62721B)
            return address + 0x08;
        else if (address < 0x6272DB)
            return address + 0x0F;
        else if (address < 0x6273D1)
            return address + 0x10;
        else if (address < 0x627448)
            return address + 0x23;
        else if (address < 0x627470)
            return address + 0x28;
        else if (address < 0x6274B6)
            return address + 0x26;
        else if (address < 0x6274CC)
            return address + 0x2A;
        else if (address < 0x627570)
            return address + 0x20;
        else
            return address + 0x46;
    }
    else if (address < 0x67E000)
        return address + 0x50;
    else if (address < 0x6D6688)
        return address;
    else if (address < 0x6D6B28)
        return address - 0x2C;
    else if (address == 0x6DB8E8)
        return 0x786D44;
    else if (address < 0x6DB8F0)
        return address - 0x28;
    else if (address < 0x6DB924)
        return address - 0x30;
    else if (address < 0x6E0000)
        return address - 0x10;
    else if (address < 0x786BA8)
        return address;
    else if (address < 0x786D40)
        return address + 0x04;
    else if (address < 0xA10B56)
        return address + 0x08;
    else if (address < 0xA12000)
        return address + 0x09;
    else
        return address;
}
