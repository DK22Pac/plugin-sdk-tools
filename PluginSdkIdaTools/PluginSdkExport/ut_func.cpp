#include "ut_func.h"
#include "ut_string.h"

unsigned int Function::Find(qstring const &name, qvector<Function> const &entries) {
    for (auto const &i : entries) {
        if (i.m_name == name)
            return i.m_address;
    }
    return 0;
}

qvector<Function> Function::FromCSV(char const *filepath) {
    qvector<Function> entries;
    //auto inFile = qfopen(filepath, "rt");
    //if (inFile) {
    //    static char line[1024];
    //    if (qfgets(line, 1024, inFile)) {
    //        while (qfgets(line, 1024, inFile)) {
    //            Function entry;
    //            qstring addr;
    //            readcsv(line, addr, entry.m_module, entry.m_name, entry.m_demangledName, entry.m_type,
    //                entry.m_attributes, entry.m_comment);
    //            qsscanf(addr.c_str(), "0x%X", &entry.m_address);
    //            entries.push_back(entry);
    //        }
    //    }
    //    else
    //        warning("File '%s' is empty", filepath);
    //    qfclose(inFile);
    //}
    //else
    //    warning("Unable to open '%s'", filepath);
    return entries;
}

bool Function::ToCSV(qvector<Function> const &entries, char const *filepath, char const *version) {
    auto outFile = qfopen(filepath, "wt");
    if (outFile) {
        // header
        qfprintf(outFile, "%s,Module,Name,DemangledName,Type,CC,RetType,Parameters,IsConst,Comment\n", version);
        // entries
        for (auto const &i : entries) {
            qstring retType = i.m_rawRetType.empty() ? i.m_retType : (qstring("raw ") + i.m_rawRetType);
            qfprintf(outFile, "0x%X,%s,%s,%s,%s,%s,%s,", i.m_address,
                csvvalue(i.m_module).c_str(),
                csvvalue(i.m_name).c_str(),
                csvvalue(i.m_demangledName).c_str(),
                csvvalue(i.m_type).c_str(),
                csvvalue(i.m_cc).c_str(),
                csvvalue(retType).c_str());
            qstring parameters;
            for (size_t p = 0; p < i.m_params.size(); p++) {
                if (!i.m_params[p].m_rawType.empty()) {
                    parameters += "raw ";
                    parameters += i.m_params[p].m_rawType;
                }
                else
                    parameters += i.m_params[p].m_type;
                parameters += ':';
                parameters += i.m_params[p].m_name;
                if (p != (i.m_params.size() - 1))
                    parameters += ' ';
            }
            qfprintf(outFile, "%s,%d,%s\n", csvvalue(parameters).c_str(), i.m_isConst, csvvalue(i.m_comment).c_str());
        }
        qfclose(outFile);
        return true;
    }
    warning("Unable to open '%s'", filepath);
    return false;
}

bool Function::ToReferenceCSV(qvector<Function> const &baseEntries, char const *baseVersion,
    qvector<Function> const &entries, char const *version, char const *filepath)
{
    auto outFile = qfopen(filepath, "wt");
    if (outFile) {
        qfprintf(outFile, "%s,%s,NameComment\n", baseVersion, version);
        for (auto const &i : baseEntries) {
            unsigned int addr = Function::Find(i.m_name, entries);
            if (addr != 0)
                qfprintf(outFile, "0x%X,0x%X,%s\n", i.m_address, addr, csvvalue(i.m_demangledName).c_str());
            else
                qfprintf(outFile, "0x%X,,%s\n", i.m_address, csvvalue(i.m_demangledName).c_str());
        }
        qfclose(outFile);
        return true;
    }
    warning("Unable to open '%s'", filepath);
    return false;
}
