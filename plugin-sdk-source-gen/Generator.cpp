#include "..\shared\Utility.h"
#include "Generator.h"
#include "JsonIO.h"
#include "StringEx.h"
#include "CSV.h"
#include "Paths.h"
#include <fstream>
#include "GvsMacroGenerator.h"
#include <iostream>

void Generator::Generate(path const &sdkpath) {
    for (unsigned int i = 0; i < 3; i++) {
        vector<Module> modules;
        cout << "Reading GTA " << Games::GetGameAbbr(Games::ToID(i)) << endl;
        ReadGame(modules, sdkpath, Games::ToID(i));
        ReadHierarchy(sdkpath, Games::ToID(i), modules);
        // GvsMacroGenerator::Generate(sdkpath, Games::ToID(i));
        cout << "Writing modules for GTA " << Games::GetGameAbbr(Games::ToID(i)) << endl;
        WriteModules(sdkpath, Games::ToID(i), modules);
    }
}

void Generator::ReadGame(vector<Module> &modules, path const &sdkpath, Games::IDs game) {

    path gameDbPath = Paths::GetDatabaseDir(sdkpath, game);

    // read enums
    for (const auto& p : recursive_directory_iterator(gameDbPath / "enums")) {
        if (p.path().extension() == ".json") {
            cout << "    Reading enum " << p.path() << endl;
            ifstream enumFile(p.path().string());
            if (enumFile.is_open()) {
                json j = json::parse(enumFile);
                string moduleName = JsonIO::readJsonString(j, "module");
                if (!moduleName.empty()) {
                    Module *m = Module::Find(modules, moduleName);
                    if (!m) {
                        Module tmp;
                        tmp.mName = moduleName;
                        tmp.mGame = game;
                        modules.push_back(tmp);
                        m = &modules.back();
                    }
                    Enum e;
                    e.mName = JsonIO::readJsonString(j, "name");
                    e.mModuleName = moduleName;
                    e.mScope = JsonIO::readJsonString(j, "scope");
                    e.mWidth = JsonIO::readJsonNumber(j, "width");
                    e.mIsClass = JsonIO::readJsonBool(j, "isClass");
                    e.mIsHexademical = JsonIO::readJsonBool(j, "isHexademical");
                    e.mIsSigned = JsonIO::readJsonBool(j, "isSigned");
                    e.mIsBitfield = JsonIO::readJsonBool(j, "isBitfield");
                    e.mComment = JsonIO::readJsonString(j, "comment");
                    auto &members = j.find("members");
                    if (members != j.end()) {
                        for (auto const &jm : *members) {
                            Enum::Member m;
                            m.mName = JsonIO::readJsonString(jm, "name");
                            m.mValue = JsonIO::readJsonNumber(jm, "value");
                            m.mComment = JsonIO::readJsonString(jm, "comment");
                            e.mMembers.push_back(m);
                        }
                    }
                    m->mEnums.push_back(e);
                }
            }
        }
    }

    // read structs
    for (const auto& p : recursive_directory_iterator(gameDbPath / "structs")) {
        if (p.path().extension() == ".json") {
            cout << "    Reading struct " << p.path() << endl;
            ifstream structFile(p.path().string());
            if (structFile.is_open()) {
                json j = json::parse(structFile);
                string moduleName = JsonIO::readJsonString(j, "module");
                if (!moduleName.empty()) {
                    Module *m = Module::Find(modules, moduleName);
                    if (!m) {
                        Module tmp;
                        tmp.mName = moduleName;
                        modules.push_back(tmp);
                        m = &modules.back();
                    }
                    Struct s;
                    s.mName = JsonIO::readJsonString(j, "name");
                    s.mScope = JsonIO::readJsonString(j, "scope");
                    s.mModuleName = moduleName;
                    string kind = JsonIO::readJsonString(j, "kind");
                    if (kind == "struct")
                        s.mKind = Struct::Kind::Struct;
                    else if (kind == "union")
                        s.mKind = Struct::Kind::Union;
                    else
                        s.mKind = Struct::Kind::Class;
                    s.mSize = JsonIO::readJsonNumber(j, "size");
                    s.mAlignment = JsonIO::readJsonNumber(j, "alignment");
                    s.mIsAnonymous = JsonIO::readJsonBool(j, "isAnonymous");
                    s.mComment = JsonIO::readJsonString(j, "comment");
                    auto &members = j.find("members");
                    if (members != j.end()) {
                        for (auto const &jm : *members) {
                            Struct::Member m;
                            m.mName = JsonIO::readJsonString(jm, "name");
                            m.mFullType = JsonIO::readJsonString(jm, "rawType"); // read custom type
                            if (m.mFullType.empty()) // if custom is not defined, read default type
                                m.mFullType = JsonIO::readJsonString(jm, "type");
                            m.mOffset = JsonIO::readJsonNumber(jm, "offset");
                            m.mSize = JsonIO::readJsonNumber(jm, "size");
                            m.mIsAnonymous = JsonIO::readJsonBool(j, "isAnonymous");
                            m.mComment = JsonIO::readJsonString(jm, "comment");
                            if (m.mFullType.empty()) {
                                if (m.mSize == 1)
                                    m.mFullType = "char";
                                else if (m.mSize == 2)
                                    m.mFullType = "short";
                                else if (m.mSize == 4)
                                    m.mFullType = "int";
                                else
                                    m.mFullType = "char[" + to_string(m.mSize) + "]";
                            }
                            m.mType.SetFromString(m.mFullType);
                            s.mMembers.push_back(m);
                        }
                    }
                    m->mStructs.push_back(s);
                }
            }
        }
    }

    if (Games::GetGameVersionsCount(game) > 0) {
        // read variables file for each game version:
        // 1.0 us/english version should be always present, and its index is always '0'.
        for (unsigned int i = 0; i < Games::GetGameVersionsCount(game); i++) {
            cout << "    Reading variables for GTA " << Games::GetGameAbbr(game) << " " << Games::GetGameVersionName(game, i) << endl;
            // example filepath: plugin-sdk.sa.variables.10us.csv
            path varsFilePath = gameDbPath / ("plugin-sdk." + Games::GetGameAbbrLow(game) + ".variables." + Games::GetGameVersionName(game, i) + ".csv");
            std::ifstream varsFile(varsFilePath);
            if (!varsFile.is_open()) {
                // exit if can't open base file
                if (i == 0) {
                    Message("%s: Unable to open base file for variables (%s)", __FUNCTION__, varsFilePath.string().c_str());
                    break;
                }
            }
            else {
                auto csvLines = CSV::ReadLines(varsFile);
                // if base version
                if (i == 0) {
                    for (string const &csvLine : csvLines) {
                        string varAddress, varModuleName, varName, varDemName, varType, varRawType, varSize, varDefaultValues, varComment, varIsReadOnly;
                        CSV::Read(csvLine, varAddress, varModuleName, varName, varDemName, varType, varRawType, varSize, varDefaultValues, varComment, varIsReadOnly);
                        if (!varModuleName.empty()) {
                            // get module for this variable
                            Module *m = Module::Find(modules, varModuleName);
                            if (!m) {
                                Module tmp;
                                tmp.mName = varModuleName;
                                modules.push_back(tmp);
                                m = &modules.back();
                            }
                            // get variable type
                            string finalVarType = varRawType;
                            if (finalVarType.empty())
                                finalVarType = varType;
                            // if var type and var name are not empty
                            if (!finalVarType.empty() && !varDemName.empty()) {
                                string varScope;
                                String::Break(varDemName, "::", varScope, varDemName, true);
                                Variable newVar;
                                newVar.mName = varDemName;
                                newVar.mMangledName = varName;
                                newVar.mModuleName = varModuleName;
                                newVar.mScope = varScope;
                                newVar.mDefaultValues = varDefaultValues;
                                newVar.mComment = varComment;
                                newVar.mType.SetFromString(finalVarType);
                                newVar.mSize = String::ToNumber(varSize);
                                newVar.mVersionInfo[0].mAddress = String::ToNumber(varAddress);
                                newVar.mIsReadOnly = String::ToNumber(varIsReadOnly);
                                if (varScope.empty())
                                    m->mVariables.push_back(newVar);
                                else {
                                    // find class inside module
                                    Struct *s = m->FindStruct(varScope, true);
                                    if (s)
                                        s->mVariables.push_back(newVar);
                                    else {
                                        // variable class not found
                                        string newClassName, newClassScope;
                                        String::Break(varScope, "::", newClassScope, newClassName, true);
                                        Struct *newClass = m->AddEmptyStruct(newClassName, newClassScope);
                                        newClass->mKind = Struct::Kind::Class;
                                        newClass->mVariables.push_back(newVar);
                                    }
                                }
                            }
                            else {
                                m->mWarnings.push_back(String::Format("wrong variable '(%s) %s' (address %s)",
                                    (finalVarType.empty()? "<no-type>" : finalVarType.c_str()),
                                    (varDemName.empty() ? "<no-name>" : varDemName.c_str()), varAddress.c_str()));
                            }
                        }
                    }
                }
                else {
                    for (string const &csvLine : csvLines) {
                        string varBaseAddress, varRefAddress, varRefName;
                        CSV::Read(csvLine, varBaseAddress, varRefAddress, varRefName);
                        if (!varRefAddress.empty()) {
                            unsigned int refAddress = String::ToNumber(varRefAddress);
                            if (refAddress != 0) {
                                unsigned int baseAddress = String::ToNumber(varBaseAddress);
                                if (baseAddress != 0) {
                                    for (auto &m : modules) {
                                        Variable *pv = m.GetVariable(baseAddress);
                                        if (pv) {
                                            pv->mVersionInfo[i].mAddress = refAddress;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                varsFile.close();
            }
        }

        // read functions file for each game version:
        // 1.0 us/english version should be always present, and its index is always '0'.
        for (unsigned int i = 0; i < Games::GetGameVersionsCount(game); i++) {
            cout << "    Reading functions for GTA " << Games::GetGameAbbr(game) << " " << Games::GetGameVersionName(game, i) << endl;
            // example filepath: plugin-sdk.sa.functions.10us.csv
            path funcsFilePath = gameDbPath / ("plugin-sdk." + Games::GetGameAbbrLow(game) + ".functions." + Games::GetGameVersionName(game, i) + ".csv");
            std::ifstream funcsFile(funcsFilePath);
            if (!funcsFile.is_open()) {
                // exit if can't open base file
                if (i == 0) {
                    Message("%s: Unable to open base file for functions (%s)", __FUNCTION__, funcsFilePath.string().c_str());
                    break;
                }
            }
            else {
                auto csvLines = CSV::ReadLines(funcsFile);
                // if base version
                if (i == 0) {
                    for (string const &csvLine : csvLines) {
                        // 10us,Module,Name,DemangledName,Type,CC,RetType,Parameters,IsConst,Comment
                        string fnAddress, fnModuleName, fnName, fnDemName, fnType, fnCC, fnRetType, fnParameters, fnIsConst, fnRefsStr, fnComment, fnPriority;
                        CSV::Read(csvLine, fnAddress, fnModuleName, fnName, fnDemName, fnType, fnCC, fnRetType, fnParameters, fnIsConst, fnRefsStr, fnComment,
                            fnPriority);
                        if (!fnModuleName.empty()) {
                            // get module for this function
                            Module *m = Module::Find(modules, fnModuleName);
                            if (!m) {
                                Module tmp;
                                tmp.mName = fnModuleName;
                                modules.push_back(tmp);
                                m = &modules.back();
                            }
                            Function::CC cc = Function::CC_UNKNOWN;
                            bool isEllipsis = false;
                            if (fnCC == "thiscall")
                                cc = Function::CC_THISCALL;
                            else if(fnCC == "cdecl" || fnCC == "voidarg")
                                cc = Function::CC_CDECL;
                            else if (fnCC == "ellipsis") {
                                cc = Function::CC_CDECL;
                                isEllipsis = true;
                            }
                            // if function name empty
                            if (fnDemName.empty()) {
                                m->mWarnings.push_back(String::Format("function '%s' has no name", fnAddress.c_str()));
                            }
                            else if (fnType.empty()) {
                                m->mWarnings.push_back(String::Format("function '%s' (address %s) has no type",
                                    fnDemName.c_str(), fnAddress.c_str()));
                            }
                            else if (cc == Function::CC_UNKNOWN) {
                                m->mWarnings.push_back(String::Format("function '%s' (address %s) has non-supported calling convention type",
                                    fnDemName.c_str(), fnAddress.c_str()));
                            }
                            else {
                                string fnScope;
                                auto bp = fnDemName.find('(');
                                if (bp != string::npos)
                                    fnDemName = fnDemName.substr(0, bp);
                                String::Break(fnDemName, "::", fnScope, fnDemName, true);
                                Function newFn;
                                newFn.mVersionInfo[0].mAddress = String::ToNumber(fnAddress);
                                newFn.mName = fnDemName;
                                newFn.mMangledName = fnName;
                                newFn.mModuleName = fnModuleName;
                                newFn.mScope = fnScope;
                                newFn.mCC = cc;
                                newFn.mType = fnType;
                                newFn.mVersionInfo[0].mRefsStr = fnRefsStr;
                                newFn.mCC = cc;
                                newFn.mIsEllipsis = isEllipsis;
                                newFn.mIsConst = String::ToNumber(fnIsConst);
                                newFn.mComment = fnComment;
                                newFn.mPriority = String::ToNumber(fnPriority);
                                string retType = fnRetType;
                                if (String::StartsWith(retType, "raw "))
                                    retType = retType.substr(4);
                                newFn.mRetType.SetFromString(retType);
                                // raw CPool<CPed> *:pool int:value
                                // [raw] Type : Name
                                size_t currPos = 0;
                                while (1) {
                                    auto colonPos = fnParameters.find(':', currPos);
                                    if (colonPos == string::npos)
                                        break;
                                    string paramType = fnParameters.substr(currPos, colonPos - currPos);
                                    String::Trim(paramType);
                                    Function::Parameter param;
                                    bool isRawParam = false;
                                    if (String::StartsWith(paramType, "raw ")) {
                                        param.mType.SetFromString(paramType.substr(4));
                                        isRawParam = true;
                                    }
                                    else
                                        param.mType.SetFromString(paramType);
                                    auto spacePos = fnParameters.find(' ', colonPos + 1);
                                    if (spacePos == string::npos) {
                                        param.mName = fnParameters.substr(colonPos + 1);
                                        newFn.mParameters.push_back(param);
                                        break;
                                    }
                                    param.mName = fnParameters.substr(colonPos + 1, spacePos - (colonPos + 1));
                                    currPos = spacePos + 1;
                                    newFn.mParameters.push_back(param);
                                }
                                bool isThiscall = newFn.mCC == Function::CC_THISCALL;
                                unsigned int rvoParamIndex = isThiscall ? 1 : 0;
                                newFn.ForAllParameters([&](Function::Parameter &p, unsigned int index) {
                                    if (index == 0 && isThiscall)
                                        p.mName = "this";
                                    else {
                                        if (p.mName.empty())
                                            p.mName = String::Format("arg%d", index + 1);
                                        else {
                                            if (index == rvoParamIndex && String::StartsWith(p.mName, "ret_")) {
                                                newFn.mRVOParamIndex = index;
                                            }
                                            else if (String::StartsWith(p.mName, "ref_")) {
                                                if (p.mType.mPointers.size() > 0 && p.mType.mPointers.back() == '*') {
                                                    p.mType.mPointers.back() = '&';
                                                    p.mName = p.mName.substr(4);
                                                }
                                            }
                                        }
                                    }
                                });
                                if (newFn.mRVOParamIndex != -1)
                                    newFn.mNumParamsToSkipForWrapper = newFn.mRVOParamIndex + 1;
                                else if (isThiscall)
                                    newFn.mNumParamsToSkipForWrapper = 1;

                                if (fnScope.empty())
                                    m->AddFunction(newFn);
                                else {
                                    // find class inside module
                                    Struct *s = m->FindStruct(fnScope, true);
                                    if (s)
                                        s->AddFunction(newFn);
                                    else {
                                        // function class not found
                                        string newClassName, newClassScope;
                                        String::Break(fnScope, "::", newClassScope, newClassName, true);
                                        Struct *newClass = m->AddEmptyStruct(newClassName, newClassScope);
                                        newClass->mKind = Struct::Kind::Class;
                                        newClass->AddFunction(newFn);
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    for (string const &csvLine : csvLines) {
                        string fnBaseAddress, fnRefAddress, fnRefsList, fnRefName;
                        CSV::Read(csvLine, fnBaseAddress, fnRefAddress, fnRefsList, fnRefName);
                        if (!fnRefAddress.empty()) {
                            unsigned int refAddress = String::ToNumber(fnRefAddress);
                            if (refAddress != 0) {
                                unsigned int baseAddress = String::ToNumber(fnBaseAddress);
                                if (baseAddress != 0) {
                                    for (auto &m : modules) {
                                        Function *pf = m.GetFunction(baseAddress);
                                        if (pf) {
                                            pf->mVersionInfo[i].mAddress = refAddress;
                                            pf->mVersionInfo[i].mRefsStr = fnRefsList;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                funcsFile.close();
            }
        }
    }
}

void Generator::WriteModules(path const &sdkpath, Games::IDs game, vector<Module> &modules) {
    path folder = Paths::GetModulesDir(sdkpath, game);
    for (auto &m : modules) {
        cout << "GTA" << Games::GetGameAbbr(game) << ": Writing module '" << m.mName << "'" << endl;
        m.Write(folder, modules, game);
    }
}

void Generator::ReadHierarchy(path const &sdkpath, Games::IDs game, vector<Module> &modules) {
    if (modules.size() == 0)
        return;
    path filePath = Paths::GetDatabaseDir(sdkpath, game) / "class-hierarchy.txt";
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
        for (Struct &s : m.mStructs) {
            if (!s.mIsAnonymous) {
                for (auto const &i : links) {
                    if (i.first == s.mName)
                        s.mParent = i.second;
                }
            }
        }
    }
}
