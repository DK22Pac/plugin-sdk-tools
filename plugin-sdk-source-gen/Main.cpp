#include "..\shared\Utility.h"
#include "Generator.h"
#include "Type.h"
#include <iostream>
#include <iomanip>

char const *tokenTypeNames[] = {
    "SPECIAL_CHAR",
    "EPSILON",
    "CONST",
    "INBUILT_TYPE_NAME",
    "CUSTOM_TYPE_NAME",
    "VOID_TYPE",
    "CC_CDECL",
    "CC_STDCALL",
    "CC_THISCALL",
    "CC_FASTCALL",
    "NUMBER",
    "CLASS",
    "STRUCT",
    "ENUM"
};

void PrintType(string const &typeStr) {
    Type testType;
    testType.SetFromString(typeStr);
    std::cout << "input: " << typeStr << std::endl << std::endl;
    testType.DbgPrint(0);
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2)
        return ErrorCode(1, "Error: Not enough parameters (%d, expected 2)", argc);
    path sdkpath = argv[1]; // plugin-sdk folder;
    Generator::Generate(sdkpath);

    //PrintType(" __int8 ** (__thiscall *[1000])(CPool<CVehicle, Array<5000>>, ...)");

    //getchar();

    return 0;
}
