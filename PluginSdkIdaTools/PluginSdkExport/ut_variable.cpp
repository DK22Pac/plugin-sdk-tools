#include "ut_variable.h"
#include "ut_string.h"

unsigned int Variable::Find(qstring const &name, qvector<Variable> const &entries) {
    for (auto const &i : entries) {
        if (i.m_name == name)
            return i.m_address;
    }
    return 0;
}

qvector<Variable> Variable::FromCSV(char const *filepath) {
    qvector<Variable> entries;
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

bool Variable::ToCSV(qvector<Variable> const &entries, char const *filepath, char const *version) {
    auto outFile = qfopen(filepath, "wt");
    if (outFile) {
        qfprintf(outFile, "%s,Module,Name,DemangledName,Type,Attributes,Comment\n", version);
        for (auto const &i : entries) {
            qfprintf(outFile, "0x%X,%s,%s,%s,%s,", i.m_address,
                csvvalue(i.m_module).c_str(),
                csvvalue(i.m_name).c_str(),
                csvvalue(i.m_demangledName).c_str(),
                csvvalue(i.m_type).c_str());
            qstring attributes;
            for (size_t a = 0; a < i.m_attributes.size(); a++) {
                attributes += i.m_attributes[a];
                if (a != (i.m_attributes.size() - 1))
                    attributes += ' ';
            }
            qfprintf(outFile, "%s,%s\n", csvvalue(attributes).c_str(), csvvalue(i.m_comment).c_str());
        }
        qfclose(outFile);
        return true;
    }
    warning("Unable to open '%s'", filepath);
    return false;
}

bool Variable::ToReferenceCSV(qvector<Variable> const &baseEntries, char const *baseVersion,
    qvector<Variable> const &entries, char const *version, char const *filepath)
{
    auto outFile = qfopen(filepath, "wt");
    if (outFile) {
        qfprintf(outFile, "%s,%s,NameComment\n", baseVersion, version);
        for (auto const &i : baseEntries) {
            unsigned int addr = Variable::Find(i.m_name, entries);
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
