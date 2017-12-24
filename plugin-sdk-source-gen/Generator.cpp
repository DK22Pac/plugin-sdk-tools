#pragma once
#include "Generator.h"
#include "JsonIO.h"

void Generator::Generate(path const &sdkpath) {
    for (unsigned int i = 0; i < 3; i++) {
        vector<Module> modules = ReadGame(sdkpath, Games::ToID(i));
        WriteModules(sdkpath, Games::ToID(i), modules);
    }
}

vector<Module> Generator::ReadGame(path const &sdkpath, Games::IDs game) {

    vector<Module> modules;

    // read enums
    for (const auto& p : recursive_directory_iterator(sdkpath / Games::GetGameFolder(game) / "enums")) {
        if (p.path().extension() == ".json") {
            ifstream enumFile(p.path().string());
            if (enumFile.is_open()) {
                json j;
                j << enumFile;
                string moduleName = JsonIO::readJsonString(j, "module");
                if (!moduleName.empty()) {
                    Module *m = Module::Find(modules, moduleName);
                    if (!m) {
                        Module tmp;
                        tmp.m_name = moduleName;
                        tmp.m_game = game;
                        modules.push_back(tmp);
                        m = &modules.back();
                    }
                    Enum e;
                    e.m_name = JsonIO::readJsonString(j, "name");
                    e.m_moduleName = moduleName;
                    e.m_width = JsonIO::readJsonNumber(j, "width");
                    e.m_isClass = JsonIO::readJsonBool(j, "isClass");
                    e.m_isHexademical = JsonIO::readJsonBool(j, "isHexademical");
                    e.m_isSigned = JsonIO::readJsonBool(j, "isSigned");
                    e.m_isBitfield = JsonIO::readJsonBool(j, "isBitfield");
                    e.m_comment = JsonIO::readJsonString(j, "comment");
                    auto &members = j.find("members");
                    if (members != j.end()) {
                        for (auto const &jm : *members) {
                            Enum::Member m;
                            m.m_name = JsonIO::readJsonString(jm, "name");
                            m.m_value = JsonIO::readJsonNumber(jm, "value");
                            m.m_comment = JsonIO::readJsonString(jm, "comment");
                            e.m_members.push_back(m);
                        }
                    }
                    m->m_enums.push_back(e);
                }
            }
        }
    }

    // read structs
    for (const auto& p : recursive_directory_iterator(sdkpath / Games::GetGameFolder(game) / "structs")) {
        if (p.path().extension() == ".json") {
            ifstream structFile(p.path().string());
            if (structFile.is_open()) {
                json j;
                j << structFile;
                string moduleName = JsonIO::readJsonString(j, "module");
                if (!moduleName.empty()) {
                    Module *m = Module::Find(modules, moduleName);
                    if (!m) {
                        Module tmp;
                        tmp.m_name = moduleName;
                        modules.push_back(tmp);
                        m = &modules.back();
                    }
                    Struct s;
                    s.m_name = JsonIO::readJsonString(j, "name");
                    s.m_moduleName = moduleName;
                    string kind = JsonIO::readJsonString(j, "kind");
                    if (kind == "struct")
                        s.m_kind = Struct::Kind::Struct;
                    else if (kind == "union")
                        s.m_kind = Struct::Kind::Union;
                    else
                        s.m_kind = Struct::Kind::Class;
                    s.m_size = JsonIO::readJsonNumber(j, "size");
                    s.m_alignment = JsonIO::readJsonNumber(j, "alignment");
                    s.m_isAnonymous = JsonIO::readJsonBool(j, "isAnonymous");
                    s.m_isPacked = JsonIO::readJsonBool(j, "isPacked");
                    s.m_comment = JsonIO::readJsonString(j, "comment");
                    auto &members = j.find("members");
                    if (members != j.end()) {
                        for (auto const &jm : *members) {
                            Struct::Member m;
                            m.m_name = JsonIO::readJsonString(jm, "name");
                            m.m_fullType = JsonIO::readJsonString(jm, "type");
                            m.m_offset = JsonIO::readJsonNumber(jm, "offset");
                            m.m_size = JsonIO::readJsonNumber(jm, "size");
                            m.m_isParent = JsonIO::readJsonBool(j, "isParent");
                            m.m_isAnonymous = JsonIO::readJsonBool(j, "isAnonymous");
                            m.m_comment = JsonIO::readJsonString(jm, "comment");
                            if (m.m_fullType.empty()) {
                                if (m.m_size == 1)
                                    m.m_fullType = "char";
                                else if (m.m_size == 2)
                                    m.m_fullType = "short";
                                else if (m.m_size == 4)
                                    m.m_fullType = "int";
                                else
                                    m.m_fullType = "char[" + to_string(m.m_size) + "]";
                            }
                            Type::FixDefaultTypeNames(m.m_fullType);
                            m.m_type.ParseFromFullType(m.m_fullType);
                            s.m_members.push_back(m);
                        }
                    }
                    m->m_structs.push_back(s);
                }
            }
        }
    }
    return modules;
}

void Generator::WriteModules(path const &sdkpath, Games::IDs game, vector<Module> modules) {
    path folder = sdkpath / "generated" / ("plugin_" + Games::GetGameAbbrLow(game)) / ("game_" + Games::GetGameAbbrLow(game));
    if (!exists(folder))
        create_directories(folder);
    for (auto &module : modules) {
        module.Write(folder);
    }
}
