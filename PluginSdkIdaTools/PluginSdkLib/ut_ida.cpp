#include "ut_ida.h"
#include "ut_string.h"

bool isFunctionPrefixReserved(qstring const &name) {
    return startsWith(name, "sub_") ||
        startsWith(name, "_sub_") ||
        startsWith(name, "j_sub_") ||
        startsWith(name, "nullsub_") ||
        startsWith(name, "_nullsub_") ||
        startsWith(name, "j_nullsub_") ||
        startsWith(name, "dummy_") ||
        startsWith(name, "_dummy_") ||
        startsWith(name, "j_dummy_") ||
        startsWith(name, "unknown_libname_") ||
        startsWith(name, "_unknown_libname_") ||
        startsWith(name, "j_unknown_libname_") ||
        startsWith(name, "locret_") ||
        startsWith(name, "loc_") ||
        startsWith(name, "unk_");
}

bool isDataPrefixReserved(qstring const &name) {
    return startsWith(name, "off_") ||
        startsWith(name, "seg_") ||
        startsWith(name, "asc_") ||
        startsWith(name, "byte_") ||
        startsWith(name, "word_") ||
        startsWith(name, "dword_") ||
        startsWith(name, "qword_") ||
        startsWith(name, "byte3_") ||
        startsWith(name, "xmmword_") ||
        startsWith(name, "ymmword_") ||
        startsWith(name, "packreal_") ||
        startsWith(name, "flt_") ||
        startsWith(name, "dbl_") ||
        startsWith(name, "tbyte_") ||
        startsWith(name, "stru_") ||
        startsWith(name, "custdata_") ||
        startsWith(name, "algn_") ||
        startsWith(name, "unk_");
}

bool isPrefixReserved(qstring const &name) {
    return isFunctionPrefixReserved(name) || isDataPrefixReserved(name);
}
