#include <idc.idc>

static main (void) {
    auto vfAry = CreateArray("VfNames");
    auto readingVTables = 0;
    auto numMethods = 0;
    auto fileHandle = fopen("D:\\Projects\\fx.txt","r");
    auto line = readstr(fileHandle);
    while (line != -1) {
        auto len = strlen(line);
        if (len > 0) {
            if (line[len-1] == "\n") {
                line = substr(line, 0, len-1);
            }
            if (readingVTables == 0) {
                if (len == 1) {
                    readingVTables = 1;
                }
                else {
                    SetArrayString(vfAry, numMethods, line);
                    print(sprintf("METHOD %d: %s", numMethods, line));
                    numMethods++;
                }
            }
            else {
                auto pos = strstr(line, " ");
                if (pos > 0) {
                    auto addr = xtol(substr(line, 0, pos));
                    auto className = substr(line, pos+1, -1);
                    print(sprintf("VTABLE: Class '%s' at 0x%X", className, addr));
                    auto i;
                    for (i = 0; i < numMethods; i++) {
                        auto funcAddr = Dword(addr + i * 4);
                        auto idaFnName = GetFunctionName(funcAddr);
                        if (idaFnName != "__purecall" && substr(idaFnName, 0, 3) != "_ZN") {
                            auto methodName = GetArrayElement(AR_STR, vfAry, i);
                            auto funcName = sprintf("_ZN%d%s%s", strlen(className), className, methodName);
                            print(sprintf("    *0x%X: %s", funcAddr, funcName));
                            MakeName(funcAddr, funcName);
                        }
                        else {
                            print(sprintf("    0x%X: %s", funcAddr, idaFnName));
                        }
                        SetFunctionAttr(funcAddr, FUNCATTR_COLOR, 0xcfffbc);
                        SetFunctionCmt(funcAddr, "", 1);
                    }
                    auto destructorAddr = Dword(addr);
                    auto destructorName = GetFunctionName(destructorAddr);
                    if (destructorName != "__purecall") {
                        auto destructorCode = Dword(destructorAddr);
                        if (destructorCode == 0xE8F18B56) {
                            auto nvDestructorOffset = Dword(destructorAddr + 4);
                            if (nvDestructorOffset != -1) {
                                auto nvDestructorAddr = destructorAddr + 8 + nvDestructorOffset;
                                auto idaNvDestructorName = GetFunctionName(nvDestructorAddr);
                                if (substr(idaNvDestructorName, 0, 3) != "_ZN") {
                                    auto nvDestructorName = sprintf("_ZN%d%sD2Ev", strlen(className), className);
                                    print(sprintf("    *Destructor: 0x%X: %s", nvDestructorAddr, nvDestructorName));
                                    MakeName(nvDestructorAddr, nvDestructorName);
                                    SetFunctionCmt(nvDestructorAddr, "", 1);
                                }
                                else {
                                    print(sprintf("    Destructor: 0x%X: %s", nvDestructorAddr, idaNvDestructorName));
                                }
                            }
                        }
                    }
                }
            }
        }
        line = readstr(fileHandle);
    }
    DeleteArray(vfAry);
    fclose(fileHandle);
}