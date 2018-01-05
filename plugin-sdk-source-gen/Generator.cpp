#pragma once
#include "Generator.h"
#include "JsonIO.h"
#include "String.h"
#include "..\shared\Utility.h"

void Generator::Generate(path const &sdkpath) {
    for (unsigned int i = 0; i < 3; i++) {
        vector<Module> modules = ReadGame(sdkpath, Games::ToID(i));
        ReadHierarchy(sdkpath, Games::ToID(i), modules);
        WriteModules(sdkpath, Games::ToID(i), modules);
    }
}

vector<Module> Generator::ReadGame(path const &sdkpath, Games::IDs game) {

    vector<Module> modules;

    // read enums
    for (const auto& p : recursive_directory_iterator(sdkpath / Games::GetGameFolder(game) / "database" / "enums")) {
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
    for (const auto& p : recursive_directory_iterator(sdkpath / Games::GetGameFolder(game) / "database" / "structs")) {
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
                            m.m_type.SetFromString(m.m_fullType);
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

void Generator::WriteModules(path const &sdkpath, Games::IDs game, vector<Module> &modules) {
    path folder = sdkpath / "generated" / ("plugin_" + Games::GetGameAbbrLow(game)) / ("game_" + Games::GetGameAbbrLow(game));
    if (!exists(folder))
        create_directories(folder);
    for (auto &module : modules) {
        module.Write(folder);
    }
}

void Generator::ReadHierarchy(path const &sdkpath, Games::IDs game, vector<Module> &modules) {
    if (modules.size() == 0)
        return;
    path filePath = sdkpath / Games::GetGameFolder(game) / "class-hierarchy.txt";
    ifstream file(filePath);
    if (!file.is_open()) {
        Message("Unable to open class-hierarchy.txt (%s)", filePath.string().c_str());
        return;
    }

    vector<pair<string, string>> links;

    vector<string> parents;
    int currParent = -1;
    for (string line; getline(file, line); ) {
        if (line.empty() || String::Compare(line, 0, '#'))
            continue;
    
        size_t numSpaces = line.find_first_not_of(' ');
        if (numSpaces == string::npos)
            continue;
    
        String::Trim(line);
    
        if (numSpaces == 0) {
            parents.clear();
            currParent = 0;
            parents.push_back(line);
        }
        else {
            int mod = numSpaces % 4;
            if (mod != 0) {
                Message("Incorrect file format (at \"%s\")", line.c_str());
                return;
            }
            int offset = numSpaces / 4;
            int diff = offset - currParent;
            if (diff > 1) {
                Message("Incorrect file format (at \"%s\")", line.c_str());
                return;
            }
            if (diff == 1) {
                links.emplace_back(line, parents[currParent]);
                parents.push_back(line);
                currParent++;
            }
            else if (diff == 0) {
                links.emplace_back(line, parents[currParent - 1]);
                parents[currParent] = line;
            }
            else {
                currParent += diff;
                links.emplace_back(line, parents[currParent - 1]);
                parents.resize(currParent);
                parents.push_back(line);
            }
        }
    }
    
    ofstream testf("links.txt");
    if (!testf.is_open())
        return;
    for (auto const &i : links)
        testf << i.first << " : " << i.second << endl;

    for (Module &m : modules) {
        for (Struct &s : m.m_structs) {
            if (!s.m_isAnonymous) {
                for (auto const &i : links)
                    if (i.first == s.m_name)
                        s.m_parent = i.second;
            }
        }
    }
}
