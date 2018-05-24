#pragma once

void process_vtables(char const *tablesFilePath, char const *methodsFilePath,
    bool createVTableStructs, bool VTableStructsOnlyUnique, bool VTableUnionForBaseClass);
