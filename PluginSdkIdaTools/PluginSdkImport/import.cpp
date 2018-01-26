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
    if (selectedGame == -1) {
        warning("Can't detect game version");
        return;
    }
    string gameName = Games::GetGameAbbrLow(Games::ToID(selectedGame));
    bool isBaseVersion = selectedVersion == 0;
    string versionName = Games::GetGameVersionName(Games::ToID(selectedGame), selectedVersion);
    string baseVersionName = Games::GetGameVersionName(Games::ToID(selectedGame), 0);
    string gameFolder = Games::GetGameFolder(Games::ToID(selectedGame));
    path gameFolderPath = input / gameFolder;

    if(!exists(gameFolderPath)) {
        warning("Folder '%s' (%s) does not exist", gameFolder.c_str(), gameFolderPath.string().c_str());
        return;
    }

    path dbFolderPath = gameFolderPath / "database";

    if (!exists(dbFolderPath)) {
        warning("Database folder does not exist (%s)", dbFolderPath.string().c_str());
        return;
    }

    if (options & OPTION_FUNCTIONS) {
        
    }

    if (options & OPTION_VARIABLES) {
        
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
                    Struct entry;
                    entry.m_name = structName;
                    // find struct kind
                    Struct::Kind kind;
                    qstring strKind = jsonReadString(j, "kind");
                    if (strKind == "struct")
                        entry.m_kind = Struct::STRT_STRUCT;
                    else if (strKind == "union")
                        entry.m_kind = Struct::STRT_UNION;
                    else
                        entry.m_kind = Struct::STRT_CLASS;
                    //
                    entry.m_module = jsonReadString(j, "module");
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
                            m.m_isAnonymous = jsonReadBool(j, "isAnonymous");
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
                    structs.push_back(entry);
                }
            }
        }
        // sort...

        // update ida structs
        for (size_t i = 0; i < structs.size(); i++) {
            Struct &entry = structs[i];
            auto stid = get_struc_id(entry.m_name.c_str());
            struc_t *s = nullptr;
            if (stid != BADNODE) {
                s = get_struc(stid);
                auto strucsize = get_struc_size(stid);
                del_struc_members(s, 0, strucsize);
                // switch to/from union
                if (s->is_union()) {
                    if (entry.m_kind != Struct::STRT_UNION)
                        setflag(s->props, SF_UNION, false);
                }
                else if (entry.m_kind == Struct::STRT_UNION)
                    setflag(s->props, SF_UNION, true);
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
            // alignment
            if (entry.m_alignment != 0)
                set_struc_align(s, entry.m_alignment);
            // comment
        }
        end_type_updating(UTP_STRUCT);
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
                                bool enIsCounter = jsonReadBool(j, "isCounter");
                                if (enIsCounter)
                                    enFullMemberComment += " iscounter:true";
                                int enBitWidth = jsonReadNumber(j, "bitWidth");
                                if (enBitWidth != 0)
                                    enFullMemberComment += format(" bitwidth:%d", enBitWidth);
                                qstring enMemberComment = jsonReadString(jm, "comment");
                                if (!enMemberComment.empty()) {
                                    if (!enFullMemberComment.empty())
                                        enFullMemberComment += "\n";
                                    enFullMemberComment += enMemberComment;
                                }
                                if (!enFullMemberComment.empty()) {
                                    enum_member_comment_info commentInfo;
                                    commentInfo.memberName = enMemberName;
                                    commentInfo.comment = enFullMemberComment;
                                    commentInfo.used = true;
                                    enumMemberComments.push_back(commentInfo);
                                }
                            }
                        }
                        // set comments for enum members
                        struct enum_visitor : public enum_member_visitor_t {
                            enum_visitor(qvector<enum_member_comment_info> &comments) : m_comments(comments) {}
                            virtual int idaapi visit_enum_member(const_t cid, uval_t value) {
                                qstring name;
                                get_enum_member_name(&name, cid);
                                for (auto &cm : m_comments) {
                                    if (!cm.used && cm.memberName == name) {
                                        set_enum_member_cmt(cid, cm.comment.c_str(), false);
                                        cm.used = true;
                                    }
                                }
                                return 0;
                            }
                            qvector<enum_member_comment_info> &m_comments;
                        };

                        enum_visitor visitor(enumMemberComments);
                        for_all_enum_members(et, visitor);
                    }
                    // bitfield
                    set_enum_bf(et, jsonReadBool(j, "isBitfield"));
                }
            }
        }
        end_type_updating(UTP_ENUM);
    }
}
