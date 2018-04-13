#include "ut_variable.h"
#include "ut_string.h"
#include "ut_ida.h"

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
        qstring line;
        if (getLine(&line, inFile)) {
            while (getLine(&line, inFile)) {
                Variable entry;
                qstring addr, size, isReadOnly;
                readcsv(line, addr, entry.m_module, entry.m_name, entry.m_demangledName, entry.m_type,
                    entry.m_rawType, size, entry.m_defaultValues, entry.m_comment, isReadOnly);
                entry.m_address = toNumber(addr);
                entry.m_size = toNumber(size);
                if (!isReadOnly.empty())
                    entry.m_isReadOnly = toNumber(isReadOnly);
                entries.push_back(entry);
            }
        }
        else
            warning("File '%s' is empty", filepath);
        qfclose(inFile);
    }
    else
        warning("Unable to open '%s' for reading", filepath);
    return entries;
}

qvector<Variable> Variable::FromReferenceCSV(char const *filepath, qvector<Variable> const &baseVars) {
    qvector<Variable> entries;
    size_t baseVarsCount = baseVars.size();
    bool wrongAddresses = false;
    if (baseVarsCount > 0) {
        auto inFile = qfopen(filepath, "rt");
        if (inFile) {
            qstring line;
            if (getLine(&line, inFile)) {
                unsigned int counter = 0;
                while (getLine(&line, inFile)) {
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
            warning("Unable to open '%s' file for reading", filepath);
    }
    return entries;
}

bool Variable::ToCSV(qvector<Variable> const &entries, char const *filepath, char const *version) {
    auto outFile = qfopen(filepath, "wt");
    if (outFile) {
        qfprintf(outFile, "%s,Module,Name,DemangledName,Type,RawType,Size,DefaultValues,Comment,IsReadOnly\n", version);
        for (auto const &i : entries) {
            qfprintf(outFile, "0x%X,%s,%s,%s,%s,%s,%d,%s,%s,%d\n", i.m_address,
                csvvalue(i.m_module).c_str(),
                csvvalue(i.m_name).c_str(),
                csvvalue(i.m_demangledName).c_str(),
                csvvalue(i.m_type).c_str(),
                csvvalue(i.m_rawType).c_str(),
                i.m_size,
                csvvalue(i.m_defaultValues).c_str(),
                csvvalue(i.m_comment).c_str(),
                i.m_isReadOnly);
        }
        qfclose(outFile);
        return true;
    }
    warning("Unable to open '%s' file for writing", filepath);
    return false;
}

bool Variable::ToReferenceCSV(qvector<Variable> const &baseEntries, char const *baseVersion,
    qvector<unsigned int> const &addresses, char const *version, char const *filepath)
{
    auto outFile = qfopen(filepath, "wt");
    if (outFile) {
        qfprintf(outFile, "%s,%s,NameComment\n", baseVersion, version);
        for (size_t i = 0; i < baseEntries.size(); i++)
            qfprintf(outFile, "0x%X,0x%X,%s\n", baseEntries[i].m_address, addresses[i], csvvalue(baseEntries[i].m_demangledName).c_str());
        qfclose(outFile);
        return true;
    }
    warning("Unable to open '%s' file for writing", filepath);
    return false;
}

bool Variable::ToReferenceCSV(qvector<Variable> const &baseEntries, char const *baseVersion,
    qvector<Variable> const &entries, char const *version, char const *filepath)
{
    qvector<unsigned int> addresses;
    for (size_t i = 0; i < baseEntries.size(); i++) {
        if (!baseEntries[i].m_name.empty())
            addresses.push_back(Variable::Find(baseEntries[i].m_name, entries));
        else
            addresses.push_back(0);
    }
    return ToReferenceCSV(baseEntries, baseVersion, addresses, version, filepath);
}
