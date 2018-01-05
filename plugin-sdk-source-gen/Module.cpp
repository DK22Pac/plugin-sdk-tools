#include "Module.h"

Module *Module::Find(vector<Module> &modules, string const &name) {
    for (unsigned int i = 0; i < modules.size(); i++) {
        if (modules[i].m_name == name)
            return &modules[i];
        ;
    }
    return nullptr;
}

void Module::Write(path const &folder) {
    WriteHeader(folder);
}

bool Module::WriteHeader(path const &folder) {
    ofstream stream(folder / (m_name + ".h"));
    if (!stream.is_open()) {
        return false;
    }
    tabs t(0);
    // file header
    stream << "/*" << endl;
    stream << "    Plugin-SDK (Grand Theft Auto) header file" << endl;
    stream << "    Authors: GTA Community. See more here" << endl;
    stream << "    https://github.com/GTAmodding/plugin-sdk" << endl;
    stream << "    Do not delete this comment block. Respect others' work!" << endl;
    stream << "*/" << endl;
    stream << "#pragma once" << endl << endl;
    // include files
    if (m_structs.size() > 0)
        stream << "#include \"plbase/PluginBase_" << Games::GetGameAbbr(m_game) << ".h" << endl << endl;
    // enums
    if (m_enums.size() > 0) {
        for (unsigned int i = 0; i < m_enums.size(); i++) {
            stream << endl;
            m_enums[i].Write(stream, t);
            stream << endl;
        }
    }
    // structs
    if (m_structs.size() > 0) {
        for (unsigned int i = 0; i < m_structs.size(); i++) {
            if (!m_structs[i].m_isAnonymous) {
                stream << endl;
                m_structs[i].Write(stream, t);
                stream << endl;
            }
        }
    }
    return true;
}

bool Module::WriteSource(path const &folder) {
    return false;
}
