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
        startsWith(name, "CLSID_") ||
        startsWith(name, "??_") ||
        startsWith(name, "_eh_");
}

bool isPrefixReserved(qstring const &name) {
    return isFunctionPrefixReserved(name) || isDataPrefixReserved(name);
}

bool isDataSegment(qstring const &name) {
    return name == ".data" || name == ".rdata" || name == ".bss";
}

bool isInDataSegment(ea_t ea) {
    auto seg = get_first_seg();
    while (seg) {
    #if (IDA_VER >= 70)
        if (ea >= seg->start_ea && ea <= seg->end_ea) {
    #else
        if (ea >= seg->startEA && ea <= seg->endEA) {
        #endif
            qstring segName;
        #if (IDA_VER >= 70)
            get_segm_name(&segName, seg);
        #else
            static char segNameBuf[32];
            if (get_true_segm_name(seg, segNameBuf, 32) != static_cast<ssize_t>(-1))
                segName = segNameBuf;
        #endif
            if (isDataSegment(segName))
                return true;
        }
    #if (IDA_VER >= 70)
        seg = get_next_seg(seg->start_ea);
    #else
        seg = get_next_seg(seg->startEA);
    #endif
    }
    return false;
}

bool isPureFunctionName(qstring const &name) {
    return startsWith(name, "__pure");
}

bool isNullFunctionName(qstring const &name) {
    return startsWith(name, "nullsub") || startsWith(name, "j_nullsub");
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

bool getLine(qstring *buf, FILE *fp) {
#if (IDA_VER >= 70)
    return qgetline(buf, fp) != -1;
#else
    buf->clear();
    if (feof(fp))
        return false;
    while (true) {
        int c = qfgetc(fp);
        if (c == EOF || c == '\n')
            break;
        buf->append(c);
    }
    return true;
#endif
}

qstring getVTableClassName(qstring const &vtableVarName) {
    qstring result;
    if (startsWith(vtableVarName, "`vtable for'"))
        result = vtableVarName.substr(12);
    else if (endsWith(vtableVarName, "::`vftable'")) {
        if (startsWith(vtableVarName, "const "))
            result = vtableVarName.substr(6, vtableVarName.length() - 11);
        else
            result = vtableVarName.substr(0, vtableVarName.length() - 11);
    }
    return result;
}

qstring getAddrName(ea_t ea) {
#if (IDA_VER >= 70)
    return get_name(ea);
#else
    return get_true_name(ea);
#endif
}

qstring getFunctionName(ea_t ea) {
    qstring name;
#if (IDA_VER >= 70)
    get_func_name(&name, ea);
#else
    get_func_name2(&name, ea);
#endif
    return name;
}

unsigned int getDword(ea_t ea) {
#if (IDA_VER >= 70)
    return get_dword(ea);
#else
    return get_long(ea);
#endif
}

bool isOffsetAtAddress(ea_t ea) {
#if (IDA_VER >= 70)
    return is_off0(get_flags(ea));
#else
    return isOff0(get_flags_novalue(ea));
#endif
}

bool isCodeAtAddress(ea_t ea) {
#if (IDA_VER >= 70)
    return is_code(get_flags(ea));
#else
    return isCode(get_flags_novalue(ea));
#endif
}

int guessTInfo(tinfo_t *tif, tid_t id) {
#if (IDA_VER >= 70)
    return guess_tinfo(tif, id);
#else
    return guess_tinfo2(id, tif);
#endif
}

int getInstructionSize(ea_t ea) {
#if (IDA_VER >= 70)
    insn_t insn;
    return decode_insn(&insn, ea);
#else
    return decode_insn(ea);
#endif
}
