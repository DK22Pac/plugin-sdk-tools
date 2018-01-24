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
    auto inFile = qfopen(filepath, "rt");
    if (inFile) {
        static char line[1024];
        if (qfgets(line, 1024, inFile)) {
            while (qfgets(line, 1024, inFile)) {
                Variable entry;
                qstring addr;
                readcsv(line, addr, entry.m_module, entry.m_name, entry.m_demangledName, entry.m_type,
                    entry.m_rawType, entry.m_defaultValues, entry.m_comment);
                entry.m_address = toNumber(addr);
                entries.push_back(entry);
            }
        }
        else
            warning("File '%s' is empty", filepath);
        qfclose(inFile);
    }
    else
        warning("Unable to open '%s'", filepath);
    return entries;
}

qvector<Variable> Variable::FromReferenceCSV(char const *filepath, qvector<Variable> const &baseVars) {
    qvector<Variable> entries;
    size_t baseVarsCount = baseVars.size();
    bool wrongAddresses = false;
    if (baseVarsCount > 0) {
        auto inFile = qfopen(filepath, "rt");
        if (inFile) {
            static char line[1024];
            if (qfgets(line, 1024, inFile)) {
                unsigned int counter = 0;
                while (qfgets(line, 1024, inFile)) {
                    counter++;
                    if (baseVarsCount < counter) {
                        entries.clear();
                        warning("Address count in reference file doesn't match with base file ('%s')", filepath);
                        break;
                    }
                    qstring addrBase, addrRef, refName;
                    readcsv(line, addrBase, addrRef, refName);
                    int intAddrBase = toNumber(addrBase);
                    int intAddrRef = toNumber(addrRef);
                    if (intAddrBase != baseVars[counter - 1].m_address) {
                        warning("Address '0x%X' in reference file doesn't match with address ('0x%X') in base file ('%s')", 
                            intAddrBase, baseVars[counter - 1].m_address, filepath);
                        wrongAddresses = true;
                        break;
                    }
                    Variable entry = baseVars[counter - 1];
                    entry.m_address = intAddrRef;
                    entries.push_back(entry);
                }
                if (!wrongAddresses && baseVarsCount != counter) {
                    entries.clear();
                    warning("Address count in reference file doesn't match with base file ('%s')", filepath);
                }
            }
            else
                warning("File '%s' is empty", filepath);
            qfclose(inFile);
        }
        else
            warning("Unable to open '%s'", filepath);
    }
    return entries;
}

bool Variable::ToCSV(qvector<Variable> const &entries, char const *filepath, char const *version) {
    auto outFile = qfopen(filepath, "wt");
    if (outFile) {
        qfprintf(outFile, "%s,Module,Name,DemangledName,Type,RawType,DefaultValues,Comment\n", version);
        for (auto const &i : entries) {
            qfprintf(outFile, "0x%X,%s,%s,%s,%s,%s,%s,%s\n", i.m_address,
                csvvalue(i.m_module).c_str(),
                csvvalue(i.m_name).c_str(),
                csvvalue(i.m_demangledName).c_str(),
                csvvalue(i.m_type).c_str(),
                csvvalue(i.m_rawType).c_str(),
                csvvalue(i.m_defaultValues).c_str(),
                csvvalue(i.m_comment).c_str());
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
            if (!i.m_name.empty()) {
                unsigned int addr = Variable::Find(i.m_name, entries);
                if (addr != 0)
                    qfprintf(outFile, "0x%X,0x%X,%s\n", i.m_address, addr, csvvalue(i.m_demangledName).c_str());
                else
                    qfprintf(outFile, "0x%X,0,%s\n", i.m_address, csvvalue(i.m_demangledName).c_str());
            }
            else
                qfprintf(outFile, "0x%X,0,%s\n", i.m_address, csvvalue(i.m_demangledName).c_str());
        }
        qfclose(outFile);
        return true;
    }
    warning("Unable to open '%s'", filepath);
    return false;
}
