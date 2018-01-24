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

    if (options & OPTION_STRUCTURES) {
        
    }

    if (options & OPTION_ENUMS) {
        // read enums
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
                    if (et == BADNODE)
                        continue;
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
                    bool enIsClass = jsonReadBool(j, "isClass");
                    if (enIsClass)
                        enFullCommentLine += " isClass:true";
                    enFullCommentLine += "\n";
                    enFullCommentLine += jsonReadString(j, "comment");
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
                                qstring enMemberComment = jsonReadString(jm, "comment");
                                if (!enMemberComment.empty()) {
                                    enum_member_comment_info commentInfo;
                                    commentInfo.memberName = enMemberName;
                                    commentInfo.comment = enMemberComment;
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
