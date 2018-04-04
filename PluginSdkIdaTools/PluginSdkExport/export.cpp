#pragma once
#include "ida.hpp"
#include "export.h"
#include "idp.hpp"
#include "typeinf.hpp"
#include "enum.hpp"
#include "struct.hpp"
#include "bytes.hpp"
#include "name.hpp"
#include "shared.h"
#include "ut_string.h"
#include "ut_range.h"
#include "ut_func.h"
#include "ut_variable.h"
#include "ut_enum.h"
#include "ut_ref.h"
#include "ut_ida.h"
#include "ut_options.h"
#include "..\..\shared\translator.h"
#include "..\..\shared\Games.h"

using namespace std;

const bool gTranslateAddresses = true;

void exportdb(int selectedGame, unsigned short selectedVersion, unsigned short options, path const &output) {
    msg("--------------------\nExport started\n--------------------\n");
    if (selectedGame == -1) {
        warning("Can't detect game version");
        return;
    }
    string gameName = Games::GetGameAbbrLow(Games::ToID(selectedGame));
    bool isBaseVersion = selectedVersion == 0;
    string versionName = Games::GetGameVersionName(Games::ToID(selectedGame), selectedVersion);
    string baseVersionName = Games::GetGameVersionName(Games::ToID(selectedGame), 0);

    path dbFolderPath = output / "database" / Games::GetGameFolder(Games::ToID(selectedGame));

    if (!exists(dbFolderPath)) {
        error_code errCode;
        create_directories(dbFolderPath, errCode);
        if (errCode) {
            warning("Unable to create database folder (%s):\n%s", dbFolderPath.string().c_str(), errCode.message().c_str());
            return;
        }
    }

    if (options & OPTION_FUNCTIONS) {
        qvector<Function> functions;

        qvector<AddressRange> skipRanges;
        if (selectedGame == Games::GTASA)
            AddRange(skipRanges, 0x837170, 0x848F10);
        
        string fileName = "plugin-sdk." + gameName + ".functions." + versionName + ".csv";
        path filePath = dbFolderPath / fileName;

        auto func = get_next_func(0);
        while (func) {
        #if (IDA_VER >= 70)
            auto ea = func->start_ea;
        #else
            auto ea = func->startEA;
        #endif
            if (!isBaseVersion || !IsInRange(ea, skipRanges)) {
                Function entry;
                tinfo_t type;
                entry.m_address = ea;
            #if (IDA_VER >= 70)
                get_tinfo(&type, ea);
                get_func_name(&entry.m_name, ea);
            #else
                get_tinfo2(ea, &type);
                get_func_name2(&entry.m_name, ea);
            #endif
                type.print(&entry.m_type);
                if (isFunctionPrefixReserved(entry.m_name))
                    entry.m_name.clear();
                else {
                    get_short_name(&entry.m_demangledName, ea);
                    qstring tmpdem = entry.m_demangledName;
                    tmpdem.replace("__", "::");
                    if (entry.m_name == tmpdem)
                        entry.m_demangledName = entry.m_name;
                }
            #if (IDA_VER >= 70)
                qstring cmtLine;
                get_func_cmt(&cmtLine, func, false);
            #else
                qstring cmtLine = get_func_cmt(func, false);
            #endif
                switch (type.get_cc()) {
                case CM_CC_INVALID:
                    entry.m_cc = "";
                    break;
                case CM_CC_VOIDARG:
                    entry.m_cc = "voidarg";
                    break;
                case CM_CC_CDECL:
                    entry.m_cc = "cdecl";
                    break;
                case CM_CC_ELLIPSIS:
                    entry.m_cc = "ellipsis";
                    break;
                case CM_CC_STDCALL:
                    entry.m_cc = "stdcall";
                    break;
                case CM_CC_PASCAL:
                    entry.m_cc = "pascal";
                    break;
                case CM_CC_FASTCALL:
                    entry.m_cc = "fastcall";
                    break;
                case CM_CC_THISCALL:
                    entry.m_cc = "thiscall";
                    break;
                case CM_CC_MANUAL:
                    entry.m_cc = "manual";
                    break;
                case CM_CC_SPOILED:
                    entry.m_cc = "spoiled";
                    break;
                default:
                    entry.m_cc = "unknown";
                    break;
                }
                type.get_rettype().print(&entry.m_retType);
                func_type_data_t fi;
                if (type.get_func_details(&fi)) {
                    for (auto const &p : fi) {
                        Function::Param funcParam;
                        funcParam.m_name = p.name;
                        p.type.print(&funcParam.m_type);
                        if (!funcParam.m_name.empty() && startsWith(funcParam.m_name, "rt_")) {
                            qstring funcParamRawType;
                            getFunctionArgumentExtraInfo(cmtLine, funcParam.m_name, funcParamRawType);
                            if (!funcParamRawType.empty()) {
                                funcParam.m_type = funcParamRawType;
                                funcParam.m_rawType = true;
                            }
                        }
                        entry.m_params.push_back(funcParam);
                    }
                }
                qstring funcRawRetType;
                getFunctionExtraInfo(cmtLine, entry.m_comment, entry.m_module, funcRawRetType, entry.m_isConst);
                if (!funcRawRetType.empty()) {
                    entry.m_retType = funcRawRetType;
                    entry.m_rawRetType = true;
                }
                entry.m_refsStr = getXrefsToAddressAsString(ea);
                functions.push_back(entry);
            }
        #if (IDA_VER >= 70)
            func = get_next_func(func->start_ea);
        #else
            func = get_next_func(func->startEA);
        #endif
        }
        if (!isBaseVersion) {
            string baseFileName = "plugin-sdk." + gameName + ".functions." + baseVersionName + ".csv";
            path baseFilePath = dbFolderPath / baseFileName;
            auto baseEntries = Function::FromCSV(baseFilePath.string().c_str());
            if (baseEntries.size() > 0) {
                if (gTranslateAddresses) { // NOTE: that's a temporary feature
                    qvector<Function> tmpfuncs;
                    for (size_t i = 0; i < baseEntries.size(); i++) {
                        Function &f = tmpfuncs.push_back();
                        f.m_address = translateAddr(Games::ToID(selectedGame), selectedVersion, baseEntries[i].m_address);
                    }
                    Function::ToReferenceCSV(baseEntries, baseVersionName.c_str(), tmpfuncs, versionName.c_str(),
                        filePath.string().c_str());
                }
                else {
                    Function::ToReferenceCSV(baseEntries, baseVersionName.c_str(), functions, versionName.c_str(),
                        filePath.string().c_str());
                }
            }
        }
        else
            Function::ToCSV(functions, filePath.string().c_str(), versionName.c_str());
    }

    if (options & OPTION_VARIABLES) {
        qvector<Variable> variables;

        string fileName = "plugin-sdk." + gameName + ".variables." + versionName + ".csv";
        path filePath = dbFolderPath / fileName;

        auto seg = get_first_seg();
        while (seg) {
            qstring segName;
        #if (IDA_VER >= 70)
            get_segm_name(&segName, seg);
        #else
            static char segNameBuf[32];
            if (get_true_segm_name(seg, segNameBuf, 32) != static_cast<ssize_t>(-1))
                segName = segNameBuf;
        #endif
            if (isDataSegment(segName)) {
            #if (IDA_VER >= 70)
                msg("Scanning segment %s: (0x%X;0x%X)\n", segName.c_str(), seg->start_ea, seg->end_ea);
                auto ea = seg->start_ea;
                while (ea < seg->end_ea) {
            #else
                msg("Scanning segment %s: (0x%X;0x%X)\n", segName.c_str(), seg->startEA, seg->endEA);
                auto ea = seg->startEA;
                while (ea < seg->endEA) {
            #endif
                    int size = 1;
                    bool hasType = false;
                    tinfo_t type;
                #if (IDA_VER >= 70)
                    if (get_tinfo(&type, ea)) {
                #else
                    if (get_tinfo2(ea, &type)) {
                #endif
                        // get type size
                        size = type.get_size();
                        if (size < 1)
                            size = 1;
                        auto itemSize = get_item_size(ea);
                        if (itemSize > size)
                            size = itemSize;
                        hasType = true;
                    }
                #if (IDA_VER >= 70)
                    qstring addrName = get_name(ea);
                #else
                    qstring addrName = get_true_name(ea);
                #endif
                    if (!addrName.empty() && !isDataPrefixReserved(addrName) && get_str_type(ea) == -1) {
                        Variable entry;
                        entry.m_address = ea;
                        entry.m_size = size;
                        if (hasType)
                            type.print(&entry.m_type);
                        entry.m_name = addrName;
                        entry.m_demangledName = get_short_name(ea);
                        qstring cmtLine;
                    #if (IDA_VER >= 70)
                        get_cmt(&cmtLine, ea, false);
                    #else
                        static char cmtLineBuf[2048];
                        if (get_cmt(ea, false, cmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                            cmtLine = cmtLineBuf;
                    #endif
                        getVariableExtraInfo(cmtLine, entry.m_comment, entry.m_module, entry.m_rawType);
                        // get default value(s) for constant variable
                        if (hasType && type.is_const()) {
                            static char fmtbuf[32];
                            qstring values;
                            unsigned int numValues = 1;
                            tinfo_t compType = type;
                            if (compType.is_array()) {
                                numValues = compType.get_array_nelems();
                                compType = compType.get_array_element();
                            }
                            bool allRead = true;
                            auto arrEa = ea;
                            for (unsigned int val = 0; val < numValues; val++) {
                                if (compType.is_bool()) {
                                    bool bVal = get_byte(arrEa);
                                    addValueCSVLine(values, (bVal ? "true" : "false"));
                                    arrEa += 1;
                                }
                                else if (compType.is_float()) {
                                #if (IDA_VER >= 70)
                                    unsigned int u32 = get_dword(arrEa);
                                #else
                                    unsigned int u32 = get_long(arrEa);
                                #endif
                                    float f32 = *reinterpret_cast<float *>(&u32);
                                    qsnprintf(fmtbuf, 32, "%g", f32);
                                    qstring strval = fmtbuf;
                                    if (strval.find('.') == qstring::npos)
                                        strval += ".0";
                                    strval += "f";
                                    addValueCSVLine(values, strval);
                                    arrEa += 4;
                                }
                                else if (compType.is_double()) {
                                    unsigned int u64 = get_qword(arrEa);
                                    float f64 = *reinterpret_cast<float *>(&u64);
                                    qsnprintf(fmtbuf, 32, "%lg", f64);
                                    qstring strval = fmtbuf;
                                    if (strval.find('.') == qstring::npos)
                                        strval += ".0";
                                    addValueCSVLine(values, strval);
                                    arrEa += 8;
                                }
                                else if (compType.is_int32() || compType.is_uint32()) {
                                #if (IDA_VER >= 70)
                                    unsigned int u32 = get_dword(arrEa);
                                #else
                                    unsigned int u32 = get_long(arrEa);
                                #endif
                                    if (compType.is_int32()) {
                                        int i32 = *reinterpret_cast<float *>(&u32);
                                        qsnprintf(fmtbuf, 32, "%d", i32);
                                    }
                                    else
                                        qsnprintf(fmtbuf, 32, "%u", u32);
                                    addValueCSVLine(values, fmtbuf);
                                    arrEa += 4;
                                }
                                else if (compType.is_int16() || compType.is_uint16()) {
                                    unsigned int u16 = get_word(arrEa);
                                    if (compType.is_int16()) {
                                        int i16 = *reinterpret_cast<float *>(&u16);
                                        qsnprintf(fmtbuf, 32, "%d", i16);
                                    }
                                    else
                                        qsnprintf(fmtbuf, 32, "%u", u16);
                                    addValueCSVLine(values, fmtbuf);
                                    arrEa += 2;
                                }
                                else if (compType.is_char() || compType.is_uchar()) {
                                    unsigned int u8 = get_byte(arrEa);
                                    if (compType.is_char()) {
                                        int i8 = *reinterpret_cast<float *>(&u8);
                                        qsnprintf(fmtbuf, 32, "%d", i8);
                                    }
                                    else
                                        qsnprintf(fmtbuf, 32, "%u", u8);
                                    addValueCSVLine(values, fmtbuf);
                                    arrEa += 1;
                                }
                                else {
                                    allRead = false;
                                    break;
                                }
                            }

                            if (allRead) {
                                if (type.is_array()) {
                                    values.insert(0, "{");
                                    values += "}";
                                }
                                entry.m_defaultValues = values;
                            }
                        }
                        if (segName == ".rdata")
                            entry.m_isReadOnly = true;
                        variables.push_back(entry);
                    }
                    ea += size;
                }
            }
        #if (IDA_VER >= 70)
            seg = get_next_seg(seg->start_ea);
        #else
            seg = get_next_seg(seg->startEA);
        #endif
        }
        if (!isBaseVersion) {
            string baseFileName = "plugin-sdk." + gameName + ".variables." + baseVersionName + ".csv";
            path baseFilePath = dbFolderPath / baseFileName;
            auto baseEntries = Variable::FromCSV(baseFilePath.string().c_str());
            if (baseEntries.size() > 0) {
                if (gTranslateAddresses) {
                    qvector<unsigned int> addresses;
                    for (size_t i = 0; i < baseEntries.size(); i++)
                        addresses.push_back(translateAddr(Games::ToID(selectedGame), selectedVersion, baseEntries[i].m_address));
                    Variable::ToReferenceCSV(baseEntries, baseVersionName.c_str(), addresses, versionName.c_str(),
                        filePath.string().c_str());
                }
                else {
                    Variable::ToReferenceCSV(baseEntries, baseVersionName.c_str(), variables, versionName.c_str(),
                        filePath.string().c_str());
                }
            }
        }
        else
            Variable::ToCSV(variables, filePath.string().c_str(), versionName.c_str());
    }

    if (options & OPTION_STRUCTURES) {
        path structFolderPath = dbFolderPath / "structs";
        bool folderExists = false;
        if (!exists(structFolderPath)) {
            error_code errCode;
            create_directories(structFolderPath, errCode);
            if (errCode)
                warning("Unable to create 'structs' folder (%s):\n%s", structFolderPath.string().c_str(), errCode.message().c_str());
            else
                folderExists = true;
        }
        else {
            folderExists = true;
            for (const auto& p : directory_iterator(structFolderPath)) {
                if (p.path().extension() == ".json") {
                    error_code remErrCode;
                    remove(p.path(), remErrCode);
                    if (remErrCode) {
                        warning("Unable to remove file '%s' in 'structs' folder (%s)",
                            p.path().filename().string().c_str(), structFolderPath.string().c_str());
                    }
                }
            }
        }

        if (folderExists) {
            for (size_t i = 0; i < get_struc_qty(); i++) {
                auto stid = get_struc_by_idx(i);
                auto s = get_struc(stid);
                qstring name = get_struc_name(stid);
                bool isUnion = s->is_union();
                unsigned int size = get_struc_size(s);
                int alignment = s->get_alignment();

                qstring cmtLine;
            #if (IDA_VER >= 70)
                get_struc_cmt(&cmtLine, stid, false);
            #else
                static char cmtLineBuf[2048];
                if (get_struc_cmt(stid, false, cmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                    cmtLine = cmtLineBuf;
            #endif
                qstring comment, moduleName, scope;
                bool isStruct, isAnonymous;
                getStructExtraInfo(cmtLine, comment, moduleName, scope, isStruct, isAnonymous);
                if (!isStruct && startsWith(name, "t"))
                    isStruct = true;
                startWritingToJson();
                json j;
                j[jsonOrderedName("name")] = name.c_str();
                j[jsonOrderedName("module")] = moduleName.c_str();
                j[jsonOrderedName("scope")] = scope.c_str();
                if (isUnion)
                    j[jsonOrderedName("kind")] = "union";
                else if (isStruct)
                    j[jsonOrderedName("kind")] = "struct";
                else
                    j[jsonOrderedName("kind")] = "class";
                if (size >= 10)
                    j[jsonOrderedName("size")] = toHexString(size).c_str();
                else
                    j[jsonOrderedName("size")] = size;
                j[jsonOrderedName("alignment")] = alignment;
                if (isAnonymous)
                    j[jsonOrderedName("isAnonymous")] = isAnonymous;
                j[jsonOrderedName("comment")] = comment.c_str();
                auto &members = j[jsonOrderedName("members")];
                members = json::array();

                unsigned int offset = 0;
                while (offset <= size) {
                    member_t *member = get_member(s, offset);
                    if (member) {
                        auto mid = member->id;
                        unsigned int msize = get_member_size(member);
                        unsigned int moffset = member->get_soff();
                    #if (IDA_VER >= 70)
                        qstring mname = get_member_name(mid);
                    #else
                        qstring mname = get_member_name2(mid);
                    #endif
                        qstring mtype;
                        tinfo_t mtinfo;
                    #if (IDA_VER >= 70)
                        if (get_or_guess_member_tinfo(&mtinfo, member))
                    #else
                        if (get_or_guess_member_tinfo2(member, &mtinfo))
                    #endif
                            mtinfo.print(&mtype);

                        qstring memberCmtLine;
                    #if (IDA_VER >= 70)
                        get_member_cmt(&memberCmtLine, mid, false);
                    #else
                        static char memberCmtLineBuf[2048];
                        if (get_member_cmt(mid, false, memberCmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                            memberCmtLine = memberCmtLineBuf;
                    #endif

                        qstring memberComment, memberRawType;
                        bool isMemberAnonymous;
                        getStructMemberExtraInfo(memberCmtLine, memberComment, memberRawType, isMemberAnonymous);

                        json jmember;
                        jmember[jsonOrderedName("name")] = mname.c_str();
                        jmember[jsonOrderedName("type")] = mtype.c_str();
                        if (!memberRawType.empty())
                            jmember[jsonOrderedName("rawType")] = memberRawType.c_str();
                        if (moffset >= 10)
                            jmember[jsonOrderedName("offset")] = toHexString(moffset).c_str();
                        else
                            jmember[jsonOrderedName("offset")] = moffset;
                        if (msize >= 10)
                            jmember[jsonOrderedName("size")] = toHexString(msize).c_str();
                        else
                            jmember[jsonOrderedName("size")] = msize;
                        if ((member->flag & 0x50000000) == 0x50000000)
                            jmember[jsonOrderedName("isString")] = true;
                        if (isMemberAnonymous)
                            jmember[jsonOrderedName("isAnonymous")] = isMemberAnonymous;
                        if (!memberComment.empty())
                            jmember[jsonOrderedName("comment")] = memberComment.c_str();
                        members.push_back(jmember);
                    }
                    offset = get_struc_next_offset(s, offset);
                }

                string fileName = "gta" + gameName + "." + getValidFileName(name).c_str() + ".json";
                path filePath = structFolderPath / fileName;
                jsonWriteToFile(j, filePath.string().c_str());
                endWritingToJson();
            }
        }
    }

    if (options & OPTION_ENUMS) {
        path enumFolderPath = dbFolderPath / "enums";
        bool folderExists = false;
        if (!exists(enumFolderPath)) {
            error_code errCode;
            create_directories(enumFolderPath, errCode);
            if (errCode)
                warning("Unable to create 'enums' folder (%s):\n%s", enumFolderPath.string().c_str(), errCode.message().c_str());
            else
                folderExists = true;
        }
        else {
            folderExists = true;
            for (const auto& p : directory_iterator(enumFolderPath)) {
                if (p.path().extension() == ".json") {
                    error_code remErrCode;
                    remove(p.path(), remErrCode);
                    if (remErrCode) {
                        warning("Unable to remove file '%s' in 'enums' folder (%s)",
                            p.path().filename().string().c_str(), enumFolderPath.string().c_str());
                    }
                }
            }
        }

        if (folderExists) {
            for (size_t i = 0; i < get_enum_qty(); i++) {
                auto e = getn_enum(i);
                qstring name = get_enum_name(e);
                qstring cmtLine;
            #if (IDA_VER >= 70)
                get_enum_cmt(&cmtLine, e, false);
            #else
                static char cmtLineBuf[2048];
                if (get_enum_cmt(e, false, cmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                    cmtLine = cmtLineBuf;
            #endif
                qstring comment, moduleName, scope;
                bool isClass;
                getEnumExtraInfo(cmtLine, comment, moduleName, scope, isClass);

                auto flags = get_enum_flag(e);
            #if (IDA_VER >= 70)
                bool isHexademical = (flags & hex_flag()) == hex_flag();
                int enumWidth = get_enum_width(e);
            #else
                bool isHexademical = (flags & hexflag()) == hexflag();
                int enumWidth = get_enum_width(e);
                if (enumWidth == 3)
                    enumWidth = 4;
                else if (enumWidth == 4)
                    enumWidth = 8;
                else if (enumWidth != 0)
                    enumWidth = 0;
            #endif
                if (enumWidth > 8)
                    enumWidth = 0;

                startWritingToJson();

                json j;
                j[jsonOrderedName("name")] = name.c_str();
                j[jsonOrderedName("module")] = moduleName.c_str();
                j[jsonOrderedName("scope")] = scope.c_str();
                j[jsonOrderedName("width")] = enumWidth;
                j[jsonOrderedName("isClass")] = isClass;
                j[jsonOrderedName("isHexademical")] = isHexademical;
                j[jsonOrderedName("isSigned")] = (flags & 0x20000) == 0x20000;
                j[jsonOrderedName("isBitfield")] = is_bf(e);
                j[jsonOrderedName("comment")] = comment.c_str();
                auto &members = j[jsonOrderedName("members")];
                members = json::array();

                struct enum_visitor : public enum_member_visitor_t {
                    enum_visitor(json &members, bool hex) : m_members(members), m_hex(hex) {}
                    virtual int idaapi visit_enum_member(const_t cid, uval_t value) {
                        qstring name;
                        get_enum_member_name(&name, cid);
                        json jenumm;
                        jenumm[jsonOrderedName("name")] = name.c_str();
                        if (m_hex) {
                            static char hexValue[32];
                            qsnprintf(hexValue, 32, "0x%X", value);
                            jenumm[jsonOrderedName("value")] = hexValue;
                        }
                        else
                            jenumm[jsonOrderedName("value")] = value;
                        qstring memberCmtLine;
                    #if (IDA_VER >= 70)
                        get_enum_member_cmt(&memberCmtLine, cid, false);
                    #else
                        static char memberCmtLineBuf[2048];
                        if (get_enum_member_cmt(cid, false, memberCmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                            memberCmtLine = memberCmtLineBuf;
                    #endif
                        qstring memberComment;
                        int bitWidth;
                        bool isCounter;
                        getEnumMemberExtraInfo(memberCmtLine, memberComment, bitWidth, isCounter);
                        if (bitWidth != 0)
                            jenumm[jsonOrderedName("bitWidth")] = bitWidth;
                        if (isCounter)
                            jenumm[jsonOrderedName("isCounter")] = isCounter;
                        if (!memberComment.empty())
                            jenumm[jsonOrderedName("comment")] = memberComment.c_str();
                        m_members.push_back(jenumm);
                        return 0;
                    }
                    json &m_members;
                    bool m_hex;
                };

                enum_visitor visitor(members, isHexademical);
                for_all_enum_members(e, visitor);

                string fileName = "gta" + gameName + "." + getValidFileName(name).c_str() + ".json";
                path filePath = enumFolderPath / fileName;
                jsonWriteToFile(j, filePath.string().c_str());
                endWritingToJson();
            }
        }
    }

    warning("Export finished");
}
