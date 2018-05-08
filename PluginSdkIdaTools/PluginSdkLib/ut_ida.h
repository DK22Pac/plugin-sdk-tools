#pragma once
#include "idp.hpp"
#include "typeinf.hpp"
#include "struct.hpp"

bool isPrefixReserved(qstring const &name);
bool isFunctionPrefixReserved(qstring const &name);
bool isDataPrefixReserved(qstring const &name);
bool isDataSegment(qstring const &name);
bool isInDataSegment(ea_t ea);
bool isPureFunctionName(qstring const &name);
bool isNullFunctionName(qstring const &name);

bool getLine(qstring *buf, FILE *fp);

bool parseType(qstring const &typeName, tinfo_t &out, bool silent = true);
bool setType(ea_t ea, qstring const &typeName, bool silent = true);
bool setType(struc_t *struc, size_t offset, qstring const &typeName, bool silent = true);
bool setType(struc_t *struc, member_t *member, size_t offset, qstring const &typeName, bool silent = true);

qstring getVTableClassName(qstring const &vtableVarName);

qstring getAddrName(ea_t ea);
qstring getFunctionName(ea_t ea);

unsigned int getDword(ea_t ea);
bool isOffsetAtAddress(ea_t ea);
bool isCodeAtAddress(ea_t ea);
int guessTInfo(tinfo_t *tif, tid_t id);
int getInstructionSize(ea_t ea);
