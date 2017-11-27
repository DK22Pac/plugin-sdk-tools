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
#include "ut_ida.h"

#define IDA_TOOLS

#include "..\..\shared\json\json.hpp"

using namespace std;
using json = nlohmann::json;

void exportdb(unsigned int selectedGame, unsigned short selectedVersion, unsigned short options, path const &output) {
    string gameName;
    bool isBaseVersion = false;
    string versionName;
    string baseVersionName;
    if (selectedGame == GTA_SA) {
        gameName = "sa";
        baseVersionName = "10us";
        switch (selectedVersion) {
        case GAMEVERSION_10US:
            versionName = "10us";
            isBaseVersion = true;
            break;
        case GAMEVERSION_10EU:
            versionName = "10eu";
            break;
        case GAMEVERSION_11US:
            versionName = "11us";
            break;
        case GAMEVERSION_11EU:
            versionName = "11eu";
            break;
        case GAMEVERSION_STEAM:
            versionName = "Steam";
            break;
        }
    }
    else {
        baseVersionName = "10";
        if (selectedGame == GTA_VC)
            gameName = "vc";
        else
            gameName = "3";
        switch (selectedVersion) {
        case GAMEVERSION_10US:
        case GAMEVERSION_10EU:
            versionName = "10";
            isBaseVersion = true;
            break;
        case GAMEVERSION_11US:
        case GAMEVERSION_11EU:
            versionName = "11";
            break;
        case GAMEVERSION_STEAM:
            versionName = "Steam";
            break;
        }
    }
    string gameFolder = "gta" + gameName;
    path fileFolderPath = output / gameFolder;
    remove_all(fileFolderPath);
    if (!exists(fileFolderPath))
        create_directories(fileFolderPath);

    if (options & OPTION_FUNCTIONS) {
        qvector<Function> functions;

        qvector<AddressRange> skipRanges;
        switch (selectedGame) {
        case GTA_SA:
            AddRange(skipRanges, 0x837170, 0x848F10);
            break;
        }
        
        string fileName = "plugin-sdk." + gameName + ".functions." + versionName + ".csv";
        path filePath = fileFolderPath / fileName;

        auto func = get_next_func(0);
        while (func) {
            auto ea = func->start_ea;
            if (!isBaseVersion || !IsInRange(ea, skipRanges)) {
                Function entry;
                tinfo_t type;
                entry.m_address = ea;
                get_tinfo(&type, ea);
                type.print(&entry.m_type);
                get_func_name(&entry.m_name, ea);
                if (isPrefixReserved(entry.m_name))
                    entry.m_name.clear();
                else {
                    get_short_name(&entry.m_demangledName, ea);
                    qstring tmpdem = entry.m_demangledName;
                    tmpdem.replace("__", "::");
                    if (entry.m_name == tmpdem)
                        entry.m_demangledName = entry.m_name;
                }
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
                        entry.m_params.push_back(funcParam);
                    }
                }
                if (type.is_const())
                    entry.m_attributes.push_back("const");
                qstring comment;
                get_func_cmt(&comment, func, false);
                qstring realComment, optionsStr;
                splitComment(comment, realComment, optionsStr);
                if (!realComment.empty())
                    entry.m_comment = realComment;
                if (!optionsStr.empty()) {
                    auto modulename = extractValuesFromComment(optionsStr, "module");
                    if (modulename.size() > 0)
                        entry.m_module = modulename[0];
                    auto attributes = extractValuesFromComment(optionsStr, "attributes");
                    if (attributes.size() > 0) {
                        if (attributes.find("const") && !entry.m_attributes.find("const"))
                            entry.m_attributes.push_back("const");
                    }
                }
                functions.push_back(entry);
            }
            func = get_next_func(func->start_ea);
        }
        if (!isBaseVersion) {
            string baseFileName = "plugin-sdk." + gameName + ".functions." + baseVersionName + ".csv";
            path baseFilePath = output / gameFolder / baseFileName;
            auto baseEntries = Function::FromCSV(baseFilePath.string().c_str());
            if (baseEntries.size() > 0)
                Function::ToReferenceCSV(baseEntries, baseVersionName.c_str(), functions, versionName.c_str(),
                    filePath.string().c_str());
        }
        else
            Function::ToCSV(functions, filePath.string().c_str(), versionName.c_str());
    }

    if (options & OPTION_VARIABLES) {
        qvector<Variable> variables;

        string fileName = "plugin-sdk." + gameName + ".variables." + versionName + ".csv";
        path filePath = fileFolderPath / fileName;

        auto seg = get_first_seg();
        while (seg) {
            qstring segName;
            get_segm_name(&segName, seg);
            if (segName == ".data") {
                msg("Scanning segment %s: (0x%X;0x%X)\n", segName.c_str(), seg->start_ea, seg->end_ea);
                auto ea = seg->start_ea;
                while (ea < seg->end_ea) {
                    int size = 1;
                    tinfo_t type;
                    if (get_tinfo(&type, ea)) {
                        Variable entry;
                        entry.m_address = ea;
                        type.print(&entry.m_type);
                        entry.m_name = get_name(ea);
                        entry.m_demangledName = get_short_name(ea);
                        
                        qstring comment;
                        get_cmt(&comment, ea, false);
                        qstring realComment, optionsStr;
                        splitComment(comment, realComment, optionsStr);
                        if (!realComment.empty())
                            entry.m_comment = realComment;
                        if (!optionsStr.empty()) {
                            auto modulename = extractValuesFromComment(optionsStr, "module");
                            if (modulename.size() > 0)
                                entry.m_module = modulename[0];
                        }
                        if (type.is_funcptr())
                            entry.m_attributes.push_back("function");
                        if (type.is_array())
                            entry.m_attributes.push_back("array");
                        
                        variables.push_back(entry);

                        size = type.get_size();
                        if (size < 1)
                            size = 1;
                        auto itemSize = get_item_size(ea);
                        if (itemSize > size)
                            size = itemSize;
                    }
                    ea += size;
                }
            }
            seg = get_next_seg(seg->start_ea);
        }
        if (!isBaseVersion) {
            string baseFileName = "plugin-sdk." + gameName + ".variables." + baseVersionName + ".csv";
            path baseFilePath = output / gameFolder / baseFileName;
            auto baseEntries = Variable::FromCSV(baseFilePath.string().c_str());
            if (baseEntries.size() > 0)
                Variable::ToReferenceCSV(baseEntries, baseVersionName.c_str(), variables, versionName.c_str(),
                    filePath.string().c_str());
        }
        else
            Variable::ToCSV(variables, filePath.string().c_str(), versionName.c_str());
    }

    if (options & OPTION_STRUCTURES) {
        
    }

    if (options & OPTION_ENUMS) {

        path enumFolderPath = fileFolderPath / "enums";
        if (!exists(enumFolderPath))
            create_directories(enumFolderPath);

        for (size_t i = 0; i < get_enum_qty(); i++) {
            auto e = getn_enum(i);
            qstring name = get_enum_name(e);
            qstring comment;
            get_enum_cmt(&comment, e, false);
            qstring realComment, optionsStr, moduleName;
            bool isClass = false;
            splitComment(comment, realComment, optionsStr);

            if (!optionsStr.empty()) {
                auto modulename = extractValuesFromComment(optionsStr, "module");
                if (modulename.size() > 0)
                    moduleName = modulename[0];
                auto attributes = extractValuesFromComment(optionsStr, "attributes");
                if (attributes.size() > 0) {
                    if (attributes.find("class"))
                        isClass = true;
                }
            }

            auto flags = get_enum_flag(e);
            bool isHexademical = (flags & hex_flag()) == hex_flag();

            startWritingToJson();

            json j;
            j[jsonOrderedName("name")] = name.c_str();
            j[jsonOrderedName("module")] = moduleName.c_str();
            j[jsonOrderedName("width")] = static_cast<int>(get_enum_width(e));
            j[jsonOrderedName("isClass")] = isClass;
            j[jsonOrderedName("isHexademical")] = isHexademical;
            j[jsonOrderedName("isSigned")] = (flags & 0x20000) == 0x20000;
            j[jsonOrderedName("isBitfield")] = is_bf(e);
            j[jsonOrderedName("comment")] = realComment.c_str();
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
                    qstring memberComment;
                    get_enum_member_cmt(&memberComment, cid, false);
                    if (!memberComment.empty())
                        jenumm[jsonOrderedName("comment")] = readMemberComment(memberComment).c_str();
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

            auto outFile = qfopen(filePath.string().c_str(), "wt");
            if (outFile) {
                qstring jsonStr = j.dump(4).c_str();
                jsonRemoveOrderingSigns(jsonStr);
                qfputs(jsonStr.c_str(), outFile);
                qfclose(outFile);
            }
            else
                warning("Unable to open '%s'", filePath.string().c_str());

            endWritingToJson();
        }
    }
}
