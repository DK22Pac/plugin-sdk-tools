#include "ida.hpp"
#include "idp.hpp"
#include "process_vtables.h"

enum eInputField {
    FIELD_TABLESFILE = 1,
    FIELD_METHODSFILE = 2,
    FIELD_CREATEVTABLESTRUCTS = 3,
    FIELD_ONLYUNIQUEVTABLESTRUCTS = 4,
    FIELD_CREATEUNIONFORBASECLASS = 4,
    FIELD_CBGROUP = 6,
    FIELD_PROCESS_BUTTON = 7
};

enum eVTableStructsFlags {
    VSF_CREATE = 1,
    VSF_ONLYUNIQUE = 2,
    VSF_UNIONFORBASECLASS = 4
};

char gTablesFile[QMAXPATH];
char gMethodsFile[QMAXPATH];
unsigned short gVTableStructsFlags = VSF_CREATE|VSF_UNIONFORBASECLASS;

static int idaapi process(int, form_actions_t &) {
    if (gTablesFile[0] == '\0') {
        warning("Tables file was not selected");
        return 0;
    }
    process_vtables(gTablesFile, gMethodsFile[0] == '\0' ? NULL : gMethodsFile,
        gVTableStructsFlags & VSF_CREATE,
        gVTableStructsFlags & VSF_ONLYUNIQUE,
        gVTableStructsFlags & VSF_UNIONFORBASECLASS);
    return 0;
}

static int idaapi modcb(int fid, form_actions_t &fa) {
    if (fid == -1 || fid == FIELD_CBGROUP) {
        if (fid == FIELD_CBGROUP)
            fa.get_cbgroup_value(FIELD_CBGROUP, &gVTableStructsFlags);
        fa.enable_field(FIELD_ONLYUNIQUEVTABLESTRUCTS, gVTableStructsFlags & VSF_CREATE);
        fa.enable_field(FIELD_CREATEUNIONFORBASECLASS, gVTableStructsFlags & VSF_CREATE);
    }
    else if (fid == FIELD_TABLESFILE)
        fa.get_path_value(FIELD_TABLESFILE, gTablesFile, QMAXPATH);
    else if (fid == FIELD_METHODSFILE)
        fa.get_path_value(FIELD_METHODSFILE, gMethodsFile, QMAXPATH);
    return 1;
}

void showform() {
    qstring formdef =
        "BUTTON NO NONE\n"
        "BUTTON YES NONE\n"
        "BUTTON CANCEL NONE\n"
        "Process VTables\n"
        "\n"
        "%/"
        "<Tables file:f1::60:::>"
        "\n"
        "<Methods file:f2::59:::>"
        "\n"
        "<Create VTable structs:C3>"
        "<Only unique VTable structs:C4>"
        "<Create union for base vtable:C5>6>"
        "\n"
        "<Process:B7:::::>\n"
        "\n";

#if (IDA_VER >= 70)
    ask_form(formdef.c_str(), modcb, gTablesFile, gMethodsFile, &gVTableStructsFlags, process);
#else
    AskUsingForm_c(formdef.c_str(), modcb, gTablesFile, gMethodsFile, &gVTableStructsFlags, process);
#endif
}
