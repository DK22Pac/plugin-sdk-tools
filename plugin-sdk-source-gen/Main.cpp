#include "..\shared\Utility.h"
#include "Generator.h"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2)
        return ErrorCode(1, "Error: Not enough parameters (%d, expected 2)", argc);
    path sdkpath = argv[1]; // plugin-sdk folder;
    Generator::Generate(sdkpath);

    return 0;
}
