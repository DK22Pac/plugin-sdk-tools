#pragma once
#include "ida.hpp"
#include "import.h"
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
#include "ut_struct.h"
#include "ut_ida.h"
#include "ut_options.h"
#include "Games.h"

using namespace std;

int jsonReadNumber(json const &node, qstring const &key) {
    std::string strKey = key.c_str();
    auto it = node.find(strKey);
    if (it == node.end())
        return 0;
    if ((*it).is_string()) {
        unsigned int value = 0;
        sscanf_s((*it).get<std::string>().c_str(), "0x%X", &value);
        return value;
    }
    return (*it).get<int>();
}

qstring jsonReadString(json const &node, qstring const &key) {
    std::string strKey = key.c_str();
    return qstring(node.value(strKey, "").c_str());
}

bool jsonReadBool(json const &node, qstring const &key) {
    return node.value(key.c_str(), false);
}

json jsonReadFromFile(qstring const &path) {
    json result;
    FILE *f = qfopen(path.c_str(), "rb");
    if (f) {
        qfseek(f, 0, SEEK_END);
        auto length = qftell(f);
        qfseek(f, 0, SEEK_SET);
        auto buffer = new char[length + 1];
        qfread(f, buffer, length);
        buffer[length] = '\0';
        qfclose(f);
        result = json::parse(buffer);
        delete[] buffer;
    }
    return result;
}

void importdb(int selectedGame, unsigned short selectedVersion, unsigned short options, path const &input) {
    msg("--------------------\nImport started\n--------------------\n");
    if (selectedGame == -1) {
        warning("Can't detect game version");
        return;
    }
    string gameName = Games::GetGameAbbrLow(Games::ToID(selectedGame));
    bool isBaseVersion = selectedVersion == 0;
    string versionName = Games::GetGameVersionName(Games::ToID(selectedGame), selectedVersion);
    string baseVersionName = Games::GetGameVersionName(Games::ToID(selectedGame), 0);

    path dbFolderPath = input / "database" / Games::GetGameFolder(Games::ToID(selectedGame));

    if (!exists(dbFolderPath)) {
        warning("Database folder does not exist (%s)", dbFolderPath.string().c_str());
        return;
    }

    // read & create enums
    if (options & OPTION_ENUMS) {
        begin_type_updating(UTP_ENUM);
        for (const auto& p : recursive_directory_iterator(dbFolderPath / "enums")) {
            if (p.path().extension() == ".json") {
                json j = jsonReadFromFile(p.path().string().c_str());
                qstring enumName = jsonReadString(j, "name");
                if (!enumName.empty()) {
                    auto et = get_enum(enumName.c_str());
                    int ord = -1;
                    // delete old enum
                    if (et != BADNODE) {
                        ord = get_enum_type_ordinal(et);
                        del_enum(et);
                    }
                    // create enum
                    et = add_enum(-1, enumName.c_str(), 0);
                    if (et == BADNODE) {
                        // error: can't create enum!
                        warning("Error: Unable to create enum '%s'", enumName.c_str());
                        continue;
                    }
                    // set type id
                    if (ord != -1)
                        set_enum_type_ordinal(et, ord);
                    // width
                    int enWidth = jsonReadNumber(j, "width");
                    if (enWidth != 0)
                        set_enum_width(et, enWidth);
                    flags_t enFlags = get_enum_flag(et);
                    // hexademical
                    bool enIsHexademical = jsonReadBool(j, "isHexademical");
                    if (enIsHexademical)
                        enFlags |= hex_flag();
                    // signed
                    bool enIsSigned = jsonReadBool(j, "isSigned");
                    if (enIsSigned)
                        enFlags |= 0x20000;
                    set_enum_flag(et, enFlags);
                    // comment
                    qstring enFullCommentLine = "module:";
                    enFullCommentLine += jsonReadString(j, "module");
                    qstring enScope = jsonReadString(j, "scope");
                    if (!enScope.empty()) {
                        enFullCommentLine += " scope:";
                        enFullCommentLine += enScope;
                    }
                    bool enIsClass = jsonReadBool(j, "isClass");
                    if (enIsClass)
                        enFullCommentLine += " isclass:true";
                    qstring enCommentLine = jsonReadString(j, "comment");
                    if (!enCommentLine.empty()) {
                        enCommentLine.replace(";;", "\n");
                        enFullCommentLine += "\n"; // we added 'module:X' signature, so we can add a newline here
                        enFullCommentLine += enCommentLine;
                    }
                    set_enum_cmt(et, enFullCommentLine.c_str(), false);
                    // members
                    auto &members = j.find("members");
                    if (members != j.end()) {
                        struct enum_member_comment_info {
                            qstring memberName;
                            qstring comment;
                            bool used;
                        };
                        qvector<enum_member_comment_info> enumMemberComments;
                        for (auto const &jm : *members) {
                            // name & value
                            qstring enMemberName = jsonReadString(jm, "name");
                            auto addEnMemberResult = add_enum_member(et, enMemberName.c_str(), jsonReadNumber(jm, "value"));
                            if (addEnMemberResult == 0) {
                                // comment
                                qstring enFullMemberComment;
                                bool enIsCounter = jsonReadBool(jm, "isCounter");
                                if (enIsCounter)
                                    enFullMemberComment += " iscounter:true";
                                int enBitWidth = jsonReadNumber(jm, "bitWidth");
                                if (enBitWidth != 0)
                                    enFullMemberComment += format(" bitwidth:%d", enBitWidth);
                                qstring enMemberComment = jsonReadString(jm, "comment");
                                if (!enMemberComment.empty()) {
                                    enMemberComment.replace(";;", "\n");
                                    if (!enFullMemberComment.empty())
                                        enFullMemberComment += "\n";
                                    enFullMemberComment += enMemberComment;
                                }
                                if (!enFullMemberComment.empty()) {
                                    const_t enumMember = get_enum_member_by_name(enMemberName.c_str());
                                    if (enumMember != static_cast<const_t>(-1)) {
                                        if (!set_enum_member_cmt(enumMember, enFullMemberComment.c_str(), false)) {
                                            msg("Comment for enum member '%s' in enum '%s' was not set\n",
                                                enMemberName.c_str(), enumName.c_str());
                                        }
                                    }
                                    else {
                                        msg("Unable to retrive reference for enum member '%s' (in enum '%s')\n",
                                            enMemberName.c_str(), enumName.c_str());
                                    }
                                }
                            }
                            else {
                                warning("Error: Unable to create enum member '%s' in enum '%s'",
                                    enMemberName.c_str(), enumName.c_str());
                            }
                        }
                        // set comments for enum members
                        struct enum_visitor : public enum_member_visitor_t {
                            enum_visitor(qvector<enum_member_comment_info> &_comments, qstring const &_enumName) : 
                                m_comments(_comments), m_enumName(_enumName) {}
                            virtual int idaapi visit_enum_member(const_t cid, uval_t value) {
                                qstring name;
                                get_enum_member_name(&name, cid);
                                for (auto &cm : m_comments) {
                                    if (!cm.used && cm.memberName == name) {
                                        set_enum_member_cmt(cid, cm.comment.c_str(), false);
                                        cm.used = true;
                                    }
                                }
                                for (auto &cm : m_comments) {
                                    if (!cm.used) {
                                        
                                    }
                                }
                                return 0;
                            }
                            qvector<enum_member_comment_info> &m_comments;
                            qstring m_enumName;
                        };

                        enum_visitor visitor(enumMemberComments, enumName);
                        for_all_enum_members(et, visitor);
                    }
                    // bitfield
                    set_enum_bf(et, jsonReadBool(j, "isBitfield"));
                }
                else {
                    warning("Empty enum name in file '%s'", p.path().string().c_str());
                }
            }
        }
        end_type_updating(UTP_ENUM);
    }

    // read & create structs
    if (options & OPTION_STRUCTURES) {
        begin_type_updating(UTP_STRUCT);
        // read structs
        qvector<Struct> structs;
        for (const auto& p : recursive_directory_iterator(dbFolderPath / "structs")) {
            if (p.path().extension() == ".json") {
                json j = jsonReadFromFile(p.path().string().c_str());
                qstring structName = jsonReadString(j, "name");
                if (!structName.empty()) {
                    auto stid = get_struc_id(structName.c_str());
                    if (stid != BADNODE && isSystemStruct(structName)) {
                        msg("Note: system struct '%s' was ignored\n", structName.c_str());
                        continue;
                    }
                    Struct entry;
                    entry.m_name = structName;
                    // find struct kind
                    qstring strKind = jsonReadString(j, "kind");
                    if (strKind == "struct")
                        entry.m_kind = Struct::STRT_STRUCT;
                    else if (strKind == "union")
                        entry.m_kind = Struct::STRT_UNION;
                    else
                        entry.m_kind = Struct::STRT_CLASS;
                    //
                    entry.m_module = jsonReadString(j, "module");
                    entry.m_scope = jsonReadString(j, "scope");
                    entry.m_size = jsonReadNumber(j, "size");
                    entry.m_alignment = jsonReadNumber(j, "alignment");
                    entry.m_isAnonymous = jsonReadBool(j, "isAnonymous");
                    entry.m_comment = jsonReadString(j, "comment");
                    auto &members = j.find("members");
                    if (members != j.end()) {
                        for (auto const &jm : *members) {
                            Struct::Member m;
                            m.m_name = jsonReadString(jm, "name");
                            m.m_type = jsonReadString(jm, "type");
                            m.m_rawType = jsonReadString(jm, "rawType");
                            m.m_offset = jsonReadNumber(jm, "offset");
                            m.m_size = jsonReadNumber(jm, "size");
                            m.m_isString = jsonReadBool(jm, "isString");
                            m.m_isAnonymous = jsonReadBool(jm, "isAnonymous");
                            m.m_comment = jsonReadString(jm, "comment");
                            if (m.m_type.empty()) {
                                if (m.m_size == 1)
                                    m.m_type = "char";
                                else if (m.m_size == 2)
                                    m.m_type = "short";
                                else if (m.m_size == 4)
                                    m.m_type = "int";
                                else {
                                    m.m_type = "char[";
                                    m.m_type += to_string(m.m_size).c_str();
                                    m.m_type += "]";
                                }
                            }
                            entry.m_members.push_back(m);
                        }
                    }
                    struc_t *s = nullptr;
                    if (stid != BADNODE) {
                        s = get_struc(stid);
                        auto strucsize = get_struc_size(stid);
                        if (del_struc_members(s, 0, strucsize + 1) != -1) {
                            // switch to/from union
                            if (s->is_union()) {
                                if (entry.m_kind != Struct::STRT_UNION)
                                    setflag(s->props, SF_UNION, false);
                            }
                            else if (entry.m_kind == Struct::STRT_UNION)
                                setflag(s->props, SF_UNION, true);
                        }
                        else {
                            warning("Error: Unable to delete struct '%s' data", entry.m_name.c_str());
                            continue;
                        } 
                    }
                    else {
                        stid = add_struc(-1, entry.m_name.c_str(), entry.m_kind == Struct::STRT_UNION);
                        if (stid == BADNODE) {
                            // error : can't create struct!
                            warning("Error: Unable to create struct '%s'", entry.m_name.c_str());
                            continue;
                        }
                        s = get_struc(stid);
                    }
                    entry.m_idaStruct = s;
                    entry.m_idaStructId = stid;

                    // set alignment
                    set_struc_align(s, entry.m_alignment);
                    // set comment
                    qstring stFullCommentLine = "module:";
                    stFullCommentLine += entry.m_module;
                    if (!entry.m_scope.empty()) {
                        stFullCommentLine += " scope:";
                        stFullCommentLine += entry.m_scope;
                    }
                    if (entry.m_kind == Struct::STRT_STRUCT)
                        stFullCommentLine += " isstruct:true";
                    if (entry.m_isAnonymous)
                        stFullCommentLine += " isanonymous:true";
                    if (!entry.m_comment.empty()) {
                        qstring stCommentLine = entry.m_comment;
                        stCommentLine.replace(";;", "\n");
                        stFullCommentLine += "\n"; // we added 'module:X' signature, so we can add a newline here
                        stFullCommentLine += stCommentLine;
                    }
                    set_struc_cmt(stid, stFullCommentLine.c_str(), false);
                    // create struct members

                    for (auto const &m : entry.m_members) {
                        flags_t mflags = 0;
                        opinfo_t mtinfo;
                        mtinfo.strtype = STRTYPE_C;
                        opinfo_t *pmtinfo = nullptr;
                        if (m.m_isString) {
                            mflags = 0x50000400;
                            pmtinfo = &mtinfo;
                        }
                        struc_error_t err = add_struc_member(s, m.m_name.c_str(), m.m_offset, mflags, pmtinfo, m.m_size);
                        if (err != STRUC_ERROR_MEMBER_OK) {
                            warning("Error: Unable to create struct member '%s' in struct '%s' (error %d)",
                                m.m_name.c_str(), entry.m_name.c_str(), err);
                        }
                    }
                    // validate struct size
                    auto newSize = get_struc_size(s);
                    if (newSize < entry.m_size) {
                        if (add_struc_member(s, format("_pad%X", newSize).c_str(), newSize, 0, nullptr,
                            entry.m_size - newSize) != STRUC_ERROR_MEMBER_OK)
                        {
                            warning("Error: Unable to pad struct '%s' (at offset %d with %d bytes)", entry.m_name.c_str(),
                                newSize, entry.m_size - newSize);
                        }
                    }
                    structs.push_back(entry);
                }
                else {
                    warning("Empty struct name in file '%s'", p.path().string().c_str());
                }
            }
        }
        end_type_updating(UTP_STRUCT);
        // update types & comments for ida structs members
        begin_type_updating(UTP_STRUCT);
        auto til = get_idati();
        for (size_t i = 0; i < structs.size(); i++) {
            Struct &entry = structs[i];
            auto stid = entry.m_idaStructId;
            auto s = entry.m_idaStruct;
            bool isUnion = entry.m_kind == Struct::Kind::STRT_UNION;
            for (auto const &m : entry.m_members) {
                tinfo_t tif; qstring typeOut;
                member_t *smem;
                if (isUnion)
                    smem = get_member_by_name(s, m.m_name.c_str());
                else
                    smem = get_member(s, m.m_offset);
                if (smem) {
                    if ((smem->flag & 0x50000000) != 0x50000000) {
                        if(!setType(s, smem, m.m_offset, m.m_type)) {
                            warning("Unable to set type for member '%s' ('%s') in struct '%s'",
                                m.m_name.c_str(), m.m_type.c_str(), entry.m_name.c_str());
                        }
                    }
                    // member comment
                    qstring stFullMemberComment;
                    if (!m.m_rawType.empty()) {
                        stFullMemberComment += " rawtype:";
                        stFullMemberComment += m.m_rawType;
                    }
                    if (m.m_isAnonymous)
                        stFullMemberComment += " isanonymous:true";
                    if (!m.m_comment.empty()) {
                        qstring stMemberComment = m.m_comment;
                        stMemberComment.replace(";;", "\n");
                        if (!stFullMemberComment.empty())
                            stFullMemberComment += "\n";
                        stFullMemberComment += stMemberComment;
                    }
                    if (!stFullMemberComment.empty())
                        set_member_cmt(smem, stFullMemberComment.c_str(), false);
                }
                else {
                    warning("Can't find member '%s' in struct '%s' (at offset %d)",
                        m.m_name.c_str(), entry.m_name.c_str(), m.m_offset);
                }
            }
        }
        end_type_updating(UTP_STRUCT);
        // validate struct sizes
        for (size_t i = 0; i < structs.size(); i++) {
            auto idaStructSize = get_struc_size(structs[i].m_idaStructId);
            if (structs[i].m_size != idaStructSize) {
                warning("Size of struct '%s' is incorrect (%d bytes, should be %d bytes)",
                    structs[i].m_name.c_str(), structs[i].m_size, idaStructSize);
            }
        }
    }

    // read and update variables
    if (options & OPTION_VARIABLES) {
        string fileName = "plugin-sdk." + gameName + ".variables." + versionName + ".csv";
        path filePath = dbFolderPath / fileName;

        // find data segments
        qvector<AddressRange> dataSegments;
        auto seg = get_first_seg();
        while (seg) {
            qstring segName;
            get_segm_name(&segName, seg);
            if (isDataSegment(segName))
                AddRange(dataSegments, seg->start_ea, seg->end_ea);
            seg = get_next_seg(seg->start_ea);
        }

        // read variables file
        qvector<Variable> variables;
        if (!isBaseVersion) {
            string baseFileName = "plugin-sdk." + gameName + ".variables." + baseVersionName + ".csv";
            path baseFilePath = dbFolderPath / baseFileName;
            auto baseEntries = Variable::FromCSV(baseFilePath.string().c_str());
            if (baseEntries.size() > 0)
                variables = Variable::FromReferenceCSV(filePath.string().c_str(), baseEntries);
        }
        else
            variables = Variable::FromCSV(filePath.string().c_str());

        // update ida variables
        for (size_t i = 0; i < variables.size(); i++) {
            Variable const &v = variables[i];
            if (v.m_address != 0) {
                if (IsInRange(v.m_address, dataSegments)) {
                    if (!v.m_type.empty()) {
                        if (!del_items(v.m_address, DELIT_DELNAMES, v.m_size)) {
                            msg("Unable to clear space for '%s' variable at address 0x%X (%d bytes)\n",
                                v.m_demangledName.c_str(), v.m_address, v.m_size);
                        }
                        for (unsigned int i = 0; i < v.m_size; i++)
                            setType(v.m_address + i, "");
                    }
                    if (!set_name(v.m_address, v.m_name.c_str())) {
                        warning("Unable to set variable '%s' name at address 0x%X",
                            v.m_demangledName.c_str(), v.m_address);
                    }
                    if (!v.m_type.empty()) {
                        qstring fixedType = v.m_type;
                        fixedType.replace("[]", "[1]");
                        if (!setType(v.m_address, fixedType)) {
                            msg("Errors while setting variable '%s' type ('%s') at address 0x%X\n",
                                v.m_demangledName.c_str(), v.m_type.c_str(), v.m_address);
                        }
                    }
                    qstring varFullComment = "module:";
                    varFullComment += v.m_module;
                    if (!v.m_rawType.empty()) {
                        varFullComment += " rawtype:";
                        varFullComment += v.m_rawType;
                    }
                    if (!v.m_comment.empty()) {
                        qstring varComment = v.m_comment;
                        varComment.replace(";;", "\n");
                        varFullComment += "\n";
                        varFullComment += varComment;
                    }
                    if (!set_cmt(v.m_address, varFullComment.c_str(), false)) {
                        warning("Unable to set variable '%s' comment at address 0x%X\nComment:\n%s",
                            v.m_demangledName.c_str(), v.m_address, varFullComment.c_str());
                    }
                }
                else {
                    msg("Variable '%s' is placed outside the data segment range (0x%X)\n",
                        v.m_demangledName.c_str(), v.m_address);
                }
            }
        }
    }

    // read and update functions
    if (options & OPTION_FUNCTIONS) {
        string fileName = "plugin-sdk." + gameName + ".functions." + versionName + ".csv";
        path filePath = dbFolderPath / fileName;

        // read functions file
        qvector<Function> functions;
        if (!isBaseVersion) {
            string baseFileName = "plugin-sdk." + gameName + ".functions." + baseVersionName + ".csv";
            path baseFilePath = dbFolderPath / baseFileName;
            auto baseEntries = Function::FromCSV(baseFilePath.string().c_str());
            if (baseEntries.size() > 0)
                functions = Function::FromReferenceCSV(filePath.string().c_str(), baseEntries);
        }
        else
            functions = Function::FromCSV(filePath.string().c_str());
        warning("%d", functions.size());
        // update ida functions
        for (size_t i = 0; i < functions.size(); i++) {
            Function const &f = functions[i];
            if (f.m_address != 0 && !f.m_name.empty()) {
                auto func = get_func(f.m_address);
                if (!func) {
                    msg("Creating function '%s' at address 0x%X\n", f.m_demangledName.c_str(), f.m_address);
                    if (!add_func(f.m_address)) {
                        warning("Unable to create function '%s' at address 0x%X", f.m_demangledName.c_str(), f.m_address);
                        continue;
                    }
                    func = get_func(f.m_address);
                    if (!func) {
                        warning("Unable to get info for just created function '%s' (at address 0x%X)",
                            f.m_demangledName.c_str(), f.m_address);
                        continue;
                    }
                }
                // function name
                if (!set_name(f.m_address, f.m_name.c_str())) {
                    warning("Unable to set function '%s' name at address 0x%X", f.m_demangledName.c_str(), f.m_address);
                }
                // function type (includes function parameters names)
                if (!f.m_type.empty()){
                    qstring fnType = f.m_type;
                    auto fnNamePos = fnType.find('(', 0);
                    if (fnNamePos != qstring::npos)
                        fnType.insert(fnNamePos, " f");
                    if (!setType(f.m_address, fnType)) {
                        msg("Unable to set function '%s' type ('%s') at address 0x%X\n",
                            f.m_demangledName.c_str(), f.m_type.c_str(), f.m_address);
                    }
                }
                // function comment
                qstring fnFullComment;
                // module
                fnFullComment = "module:";
                fnFullComment += f.m_module;
                // rettype
                if (f.m_rawRetType) {
                    fnFullComment += " rettype:";
                    fnFullComment += f.m_retType;
                }
                // isconst
                if (f.m_isConst)
                    fnFullComment += " isconst:true";
                // raw parameters types
                for (auto const &fp : f.m_params) {
                    if (!fp.m_name.empty() && startsWith(fp.m_name, "rt_") && fp.m_rawType) {
                        fnFullComment += " ";
                        fnFullComment += fp.m_name;
                        fnFullComment += ":\"";
                        fnFullComment += fp.m_type;
                        fnFullComment += "\"";
                    }
                }
                // default comment
                if (!f.m_comment.empty()) {
                    qstring fnComment = f.m_comment;
                    fnComment.replace(";;", "\n");
                    fnFullComment += "\n";
                    fnFullComment += fnComment;
                }
                if (!set_func_cmt(func, fnFullComment.c_str(), false)) {
                    warning("Unable to set function '%s' comment at address 0x%X\nComment:\n%s",
                        f.m_demangledName.c_str(), f.m_address, fnFullComment.c_str());
                }
            }
        }
    }
}
