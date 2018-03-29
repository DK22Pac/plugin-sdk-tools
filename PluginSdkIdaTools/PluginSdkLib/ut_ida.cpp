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
        startsWith(name, "unk_") ||
        startsWith(name, "GUID_") ||
        startsWith(name, "IID_") ||
        startsWith(name, "CLSID_");
}

bool isPrefixReserved(qstring const &name) {
    return isFunctionPrefixReserved(name) || isDataPrefixReserved(name);
}

bool isDataSegment(qstring const &name) {
    return name == ".data" || name == ".rdata" || name == ".bss";
}

bool parseType(qstring const &typeName, tinfo_t &out, bool silent) {
    qstring fixedTypeName = typeName;
    if (typeName.last() != ';')
        fixedTypeName += ";";
    qstring outTypeName;
#if (IDA_VER >= 70)
    if (parse_decl(&out, &outTypeName, NULL, fixedTypeName.c_str(), silent ? PT_SIL : 0)) {
#else
    if (parse_decl2(idati, fixedTypeName.c_str(), &outTypeName, &out, PT_TYP | (silent ? PT_SIL : 0))) {
#endif
        return true;
    }
    return false;
}

bool setType(ea_t ea, qstring const &typeName, bool silent) {
    tinfo_t tif;
    if (!typeName.empty()) {
        if (!parseType(typeName, tif, silent))
            return false;
    }
#if (IDA_VER >= 70)
    return apply_tinfo(ea, tif, TINFO_DEFINITE);
#else
    return apply_tinfo2(ea, tif, TINFO_DEFINITE);
#endif
}

bool setType(struc_t *struc, size_t offset, qstring const &typeName, bool silent) {
    auto member = get_member(struc, offset);
    if (!member)
        return false;
    return setType(struc, member, offset, typeName, silent);
}

bool setType(struc_t *struc, member_t *member, size_t offset, qstring const &typeName, bool silent) {
    tinfo_t tif;
    if (!typeName.empty()) {
        if (!parseType(typeName, tif, silent))
            return false;
    }
#if (IDA_VER >= 70)
    return set_member_tinfo(struc, member, offset, tif, 0) == SMT_OK;
#else
    return set_member_tinfo2(struc, member, offset, tif, 0) == SMT_OK;
#endif
}
