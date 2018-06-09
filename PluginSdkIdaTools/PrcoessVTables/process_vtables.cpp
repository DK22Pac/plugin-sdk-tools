#include "process_vtables.h"
#include "ida.hpp"
#include "idp.hpp"
#include "loader.hpp"
#include "name.hpp"
#include "ut_string.h"
#include "ut_ida.h"
#include "ut_ref.h"
#include "ut_struct.h"

struct MyVtableMethod {
    qstring name;
    qstring decl;
};

struct MyVtableMethods {
    qstring name;
    MyVtableMethods *parent = nullptr;
    qvector<MyVtableMethod> ary;
};

qvector<MyVtableMethods *> myMethods;

struct MyVtableInfo {
    qstring name;
    qstring mangledName;
    MyVtableInfo *parent = nullptr;
    unsigned int vtAddress;
    unsigned int size;
    unsigned int id;
    unsigned int vtSize;
    qstring ctorName;
};

qvector<MyVtableInfo *> myInfos;

MyVtableMethods *FindMethodsForClass(MyVtableInfo *c) {
    for (MyVtableMethods *m : myMethods) {
        if (m->name == c->name)
            return m;
    }
    if (c->parent)
        return FindMethodsForClass(c->parent);
    return nullptr;
}

bool IsMethodOfParentClass(qstring const &prefix, qstring const &methodName, MyVtableInfo *c) {
    if (startsWith(methodName, prefix + c->mangledName))
        return true;
    if (c->parent)
        return IsMethodOfParentClass(prefix, methodName, c->parent);
    return false;
}

qstring getMangledName(qstring const &in) {
    qstring result;
    auto tbPos = in.find('<');
    if (tbPos != qstring::npos) {
        auto tePos = in.find('>', tbPos + 1);
        if (tePos != qstring::npos) {
            qstring classNameWithoutTemplate = in.substr(0, tbPos);
            qstring templateParam = in.substr(tbPos + 1, tePos);
            result = toString(classNameWithoutTemplate.length()) + classNameWithoutTemplate +
                "I" + toString(templateParam.length()) + templateParam + "E";
        }
    }
    if (result.empty())
        result = toString(in.length()) + in;
    return result;
}

void getClassNamesForDecl(qstring const &className, qstring &typeName, qstring &declName) {
    typeName = className;
    typeName.replace("<", "_");
    typeName.replace(">", "_");
    declName = qstring("class ") + typeName;
}

void setupVirtualFunction(ea_t addr, char const *name, char const *typeName) {
    if (name)
        set_name(addr, name);
    if (typeName)
        setType(addr, typeName, false);
    func_t *func = get_func(addr);
    set_cmt(addr, "", true);
    if (func) {
        func->color = 0xcfffbc;
        set_func_cmt(func, "", true);
    }
}

void process_vtables(char const *tablesFilePath, char const *methodsFilePath,
    bool createVTableStructs, bool VTableStructsOnlyUnique, bool VTableUnionForBaseClass)
{
    msg("--------------------\nNew import started.\n--------------------\n");

    if (methodsFilePath) {
        auto methodsFile = qfopen(methodsFilePath, "rt");
        if (methodsFile) {
            static char line[1024];
            if (qfgets(line, 1024, methodsFile)) {
                while (qfgets(line, 1024, methodsFile)) {
                    qstring vtClass, vtParent;
                    readcsv(line, vtClass, vtParent);
                    MyVtableMethods *methods = nullptr;
                    for (MyVtableMethods *m : myMethods) {
                        if (m->name == vtClass) {
                            methods = m;
                            break;
                        }
                    }
                    if (methods == nullptr) {
                        MyVtableMethods *newMethods = new MyVtableMethods;
                        newMethods->name = vtClass;
                        for (MyVtableMethods *myMethod : myMethods) {
                            if (myMethod->name == vtParent) {
                                newMethods->parent = myMethod;
                                break;
                            }
                        }
                        myMethods.push_back(newMethods);
                        methods = myMethods.back();
                    }
                    MyVtableMethod entry;
                    readcsv(line, vtClass, vtParent, entry.name, entry.decl);

                    methods->ary.push_back(entry);
                }
            }
            qfclose(methodsFile);
        }
        else {
            warning("Unable to open methods file (%s)", methodsFilePath);
            return;
        }
    }

    auto tablesFile = qfopen(tablesFilePath, "rt");
    if (tablesFile) {
        static char line[1024];
        if (qfgets(line, 1024, tablesFile)) {
            while (qfgets(line, 1024, tablesFile)) {
                MyVtableInfo *entry = new MyVtableInfo;
                qstring number, name, parent, vtAddr, size, id, vtSize;
                readcsv(line, number, name, entry->name, parent, vtAddr, size, id, vtSize, entry->ctorName);
                entry->vtAddress = toNumber(vtAddr);
                entry->size = toNumber(size);
                entry->id = toNumber(id);
                entry->vtSize = toNumber(vtSize);
                for (MyVtableInfo *myInfo : myInfos) {
                    if (myInfo->name == parent) {
                        entry->parent = myInfo;
                        break;
                    }
                }
                entry->mangledName = getMangledName(entry->name);
                myInfos.push_back(entry);
            }
        }
        qfclose(tablesFile);
    }
    else {
        warning("Unable to open tables file (%s)", methodsFilePath);
        return;
    }

    unsigned int updatedFunctions = 0;
    unsigned int updatedVTables = 0;
    unsigned int updatedCtors = 0;
    unsigned int updatedDtors = 0;
    unsigned int createdVTableStructs = 0;

    for (MyVtableInfo *myInfo : myInfos) {

        if (myInfo->vtAddress == 0) {
            msg("Warning: vtable for %s has invalid address\n", myInfo->name.c_str());
            continue;
        }
        qstring newName = qstring("_ZTV") + myInfo->mangledName;
        qstring oldName = getAddrName(myInfo->vtAddress);
        if (oldName != newName) {
            if (startsWith(oldName, "_ZTV")) {
                msg("Warning: vtable for %s has invalid name:\n    %s\n    Possible name:\n%s\nPlease check it and fix.\n",
                    myInfo->name.c_str(), oldName.c_str(), newName.c_str());
                continue;
            }
            set_name(myInfo->vtAddress, newName.c_str());
            updatedVTables++;
        }

        qstring classDeclName, classType;
        getClassNamesForDecl(myInfo->name, classType, classDeclName);

        // check for ctor/dtor
        qvector<unsigned int> dtorList;
        qvector<unsigned int> ctorList;
        // vtable references
        qvector<XRef> refs = getXrefToAddress(myInfo->vtAddress, false);
        for (auto &ref : refs) {
            qstring refName = getAddrName(ref.m_objectid);
            if (contains(refName, "_ctor") || contains(refName, "constructor")) {
                if (!contains(refName, toHexString(myInfo->vtAddress, false)) && !contains(refName, classType))
                    msg("Warning: ctor for class %s has strange name:\n    %s\n", myInfo->name.c_str(), refName.c_str());
                else
                    ctorList.push_back(ref.m_objectid);
            }
            else if (contains(refName, "_dtor") || contains(refName, "destructor")) {
                if (!contains(refName, toHexString(myInfo->vtAddress, false)) && !contains(refName, classType))
                    msg("Warning: dtor for class %s has strange name:\n    %s\n", myInfo->name.c_str(), refName.c_str());
                else
                    dtorList.push_back(ref.m_objectid);
            }
        }

        if (ctorList.size() == 1 && !myInfo->ctorName.empty()) {
            set_name(ctorList[0], myInfo->ctorName.c_str());
            set_cmt(ctorList[0], "", true);
            func_t *func = get_func(ctorList[0]);
            if (func)
                set_func_cmt(func, "", true);
            updatedCtors++;
        }
        if (dtorList.size() == 1) {
            qstring dtorName = qstring("_ZN") + myInfo->mangledName + qstring("D2Ev");
            set_name(dtorList[0], dtorName.c_str());
            setType(dtorList[0], qstring("void __thiscall f(") + classDeclName + qstring(" *this)"));
            set_cmt(dtorList[0], "", true);
            func_t *func = get_func(dtorList[0]);
            if (func)
                set_func_cmt(func, "", true);
            updatedDtors++;
        }
        //

        // work with methods
        MyVtableMethods *methodsTable = FindMethodsForClass(myInfo);
        if (methodsTable == nullptr) {
            msg("Warning: unable to find methods for %s class\n", myInfo->name.c_str());
            continue;
        }
        qvector<MyVtableMethods *> vtable_table;
        vtable_table.insert(vtable_table.begin(), methodsTable);
        while (methodsTable->parent) {
            vtable_table.insert(vtable_table.begin(), methodsTable->parent);
            methodsTable = methodsTable->parent;
        }

        unsigned int start_addr = myInfo->vtAddress;
        // for all method tables
        for (MyVtableMethods *methods : vtable_table) {
            // for all methods
            for (auto &method : methods->ary) {

                qstring newMethodName = method.name;
                qstring newMethodDecl = method.decl;
                newMethodName.replace("$CN$", myInfo->mangledName.c_str());
                newMethodDecl.replace("$CN$", classDeclName.c_str());

                // validate method name
                unsigned int methodAddr = getDword(start_addr);

                start_addr += 4;

                qstring oldMethodName = getAddrName(methodAddr);

                // skip if same
                if (oldMethodName == newMethodName) {
                    setupVirtualFunction(methodAddr, nullptr, newMethodDecl.c_str());
                    updatedFunctions++;
                    continue;
                }

                // don't touch pure functions
                if (contains(oldMethodName, "__pure") || contains(oldMethodName, "purecall"))
                    continue;

                // skip if member of parent class
                if (startsWith(oldMethodName, "_ZN")) {
                    bool inParent = false;
                    if (myInfo->parent) {
                        qstring prefix;
                        if (startsWith(newMethodName, "_ZNK"))
                            prefix = "_ZNK";
                        else
                            prefix = "_ZN";
                        inParent = IsMethodOfParentClass(prefix, oldMethodName, myInfo->parent);
                    }

                    if (inParent)
                        continue;
                }

                setupVirtualFunction(methodAddr, newMethodName.c_str(), newMethodDecl.c_str());
                updatedFunctions++;
            }
        }
    }

    if (myInfos.size() > 0 && createVTableStructs) {

        MyVtableInfo *rootClass = myInfos[0];
        MyVtableInfo *rootParent = rootClass->parent;
        while (rootParent) {
            rootClass = rootParent;
            rootParent = rootParent->parent;
        }

        qstring rootClassName, rootClassDeclName;
        getClassNamesForDecl(rootClass->name, rootClassName, rootClassDeclName);

        qvector<qstring> childClassesNames;
        qvector<std::pair<qstring, MyVtableMethods *>> vtStructs;

        if (VTableStructsOnlyUnique) {
            for (MyVtableMethods *i : myMethods)
                vtStructs.push_back(std::make_pair(i->name, i));
        }
        else {
            for (MyVtableInfo *i : myInfos) {
                MyVtableMethods *methods = FindMethodsForClass(i);
                if (methods)
                    vtStructs.push_back(std::make_pair(i->name, methods));
            }
        }

        begin_type_updating(UTP_STRUCT);

        for (auto const &vtStruct : vtStructs) {
            qstring className, classDeclName;
            getClassNamesForDecl(vtStruct.first, className, classDeclName);
            qstring structName = qstring("_vtable_") + className;
            auto methodsTable = vtStruct.second;
            auto stid = get_struc_id(structName.c_str());
            struc_t *s = nullptr;
            if (stid != BADNODE) {
                s = get_struc(stid);
                auto strucsize = get_struc_size(stid);
                if (del_struc_members(s, 0, strucsize + 1) != -1)
                    setflag(s->props, SF_UNION, false);
                else {
                    warning("Error: Unable to delete struct '%s' data", structName.c_str());
                    continue;
                }
            }
            else {
                stid = add_struc(-1, structName.c_str());
                if (stid == BADNODE) {
                    warning("Error: Unable to create struct '%s'", structName.c_str());
                    continue;
                }
                s = get_struc(stid);
            }

            createdVTableStructs++;

            childClassesNames.push_back(className);

            qlist<MyVtableMethods *> tables;

            tables.push_front(methodsTable);
            auto tableParent = methodsTable->parent;
            while (tableParent) {
                tables.push_front(tableParent);
                tableParent = tableParent->parent;
            }

            unsigned int offset = 0;

            std::map<qstring, size_t> tableNames;

            for (auto table : tables) {
                for (auto const &method : table->ary) {

                    qstring memberName;

                    auto classPos = method.name.find("$CN$");
                    if (classPos != qstring::npos) {
                        qstring methodNameMangled = method.name.substr(classPos + 4);
                        if (methodNameMangled == "D0Ev")
                            memberName = "delete";
                        else if (methodNameMangled == "D2Ev")
                            memberName = "destructor";
                        else {
                            char *nameStart;
                            auto nameLength = strtol(methodNameMangled.c_str(), &nameStart, 10);
                            if (nameLength != 0) {
                                size_t startPos = nameStart - methodNameMangled.c_str();
                                memberName = methodNameMangled.substr(startPos, startPos + nameLength);
                            }
                            else
                                memberName = methodNameMangled;
                        }
                    }
                    else
                        memberName = method.name;

                    std::map<qstring, size_t>::iterator nameInTable(tableNames.lower_bound(memberName));
                    if (nameInTable == tableNames.end() || memberName < nameInTable->first)
                        tableNames.insert(nameInTable, std::make_pair(memberName, 1));
                    else {
                        nameInTable->second++;
                        memberName.append(toString(nameInTable->second));
                    }

                    qstring memberType = method.decl;

                    memberType.replace("$CN$", classDeclName.c_str());
                    memberType.replace("__thiscall f", "(__thiscall *f)");

                    struc_error_t err = add_struc_member(s, memberName.c_str(), offset, 0, nullptr, 4);
                    if (err != STRUC_ERROR_MEMBER_OK) {
                        warning("Error: Unable to create struct member '%s' in struct '%s':\n\"%s\"",
                            memberName.c_str(), structName.c_str(), structMemberErrorMessage(err).c_str());
                    }
                    else {
                        auto smem = get_member(s, offset);
                        if (smem) {
                            if (!setType(s, smem, offset, memberType)) {
                                warning("Unable to set type for member '%s' ('%s') in struct '%s'",
                                    memberName.c_str(), memberType.c_str(), structName.c_str());
                            }
                        }
                        else {
                            warning("Can't find member '%s' in struct '%s' (at offset %d)",
                                memberName.c_str(), structName.c_str(), offset);
                        }
                    }
                    offset += 4;
                }
            }
        }

        end_type_updating(UTP_STRUCT);

        begin_type_updating(UTP_STRUCT);

        while (VTableUnionForBaseClass) {
            qstring structName = qstring("_vtable_union_") + rootClassName;
            auto stid = get_struc_id(structName.c_str());
            struc_t *s = nullptr;
            if (stid != BADNODE) {
                s = get_struc(stid);
                auto strucsize = get_struc_size(stid);
                if (del_struc_members(s, 0, strucsize + 1) != -1)
                    setflag(s->props, SF_UNION, true);
                else {
                    warning("Error: Unable to delete struct '%s' data", structName.c_str());
                    break;
                }
            }
            else {
                stid = add_struc(-1, structName.c_str(), true);
                if (stid == BADNODE) {
                    warning("Error: Unable to create struct '%s'", structName.c_str());
                    break;
                }
                s = get_struc(stid);
            }

            createdVTableStructs++;

            for (auto const &childClassName : childClassesNames) {
                qstring memberName = childClassName.substr(1);
                if (memberName.length() > 0)
                    memberName[0] = tolower(memberName[0]);

                qstring memberType = qstring("_vtable_") + childClassName;

                struc_error_t err = add_struc_member(s, memberName.c_str(), 0, 0, nullptr, 4);
                if (err != STRUC_ERROR_MEMBER_OK) {
                    warning("Error: Unable to create struct member '%s' in struct '%s':\n\"%s\"",
                        memberName.c_str(), structName.c_str(), structMemberErrorMessage(err).c_str());
                }
                else {
                    auto smem = get_member_by_name(s, memberName.c_str());
                    if (smem) {
                        if (!setType(s, smem, 0, memberType)) {
                            warning("Unable to set type for member '%s' ('%s') in struct '%s'",
                                memberName.c_str(), memberType.c_str(), structName.c_str());
                        }
                    }
                    else
                        warning("Can't find member '%s' in struct '%s'", memberName.c_str(), structName.c_str());
                }
            }
            break;
        }

        end_type_updating(UTP_STRUCT);
    }

    for (MyVtableInfo *i : myInfos)
        delete i;

    for (MyVtableMethods *i : myMethods)
        delete i;

    myMethods.clear();
    myInfos.clear();

    msg("Done! Stats: VTables (%d), functions (%d), constructors (%d), destructors (%d), VTable structs (%d)\n",
        updatedVTables, updatedFunctions, updatedCtors, updatedDtors, createdVTableStructs);

    warning(
        "Done! Stats:\n    Updated VTables: %d\n    Updated functions: %d\n    Updated constructors: %d\n    "
        "Updated destructors: %d\n    Created VTable structs: %d\n",
        updatedVTables, updatedFunctions, updatedCtors, updatedDtors, createdVTableStructs);
}
