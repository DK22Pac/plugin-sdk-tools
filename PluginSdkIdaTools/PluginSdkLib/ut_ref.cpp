#include "ut_ref.h"
#include "idp.hpp"
#include "name.hpp"
#include "xref.hpp"
#include "ut_string.h"

qvector<XRef> getXrefToAddress(ea_t ea, bool isFunc) {
    qvector<XRef> xrefs;
    unsigned int lastobjid = 0;
    unsigned int xrefindex = 0;
    xrefblk_t xb;
    for (bool ok = xb.first_to(ea, XREF_ALL); ok; ok = xb.next_to()) {
        XRef xref;
        xref.m_address = xb.from;
        if (xb.type == fl_CF || xb.type == fl_CN)
            xref.m_type = XRef::Call;
        else if (xb.type == fl_JF || xb.type == fl_JN)
            xref.m_type = XRef::Jump;
        else if (xb.type == fl_USobsolete || xb.type == dr_O)
            xref.m_type = XRef::Callback;
        else
            continue;
        if (isFunc && xb.type == dr_O) { // vtable
            lastobjid = 0;
            xrefindex = 0;
            xref.m_index = 1;
            xref.m_objectid = 0;
            const int max_vmethods_search = 200;
            for (int i = 0; i < max_vmethods_search; i++) {
                ea_t searchaddr = xref.m_address - i * 4;
                qstring searchaddrname = get_name(searchaddr);
                if (!searchaddrname.empty()) {
                    if (startsWith(searchaddrname, "_ZTV"))
                        xref.m_objectid = searchaddr;
                    break;
                }
            }
        }
        else {
            func_t *xreffunc = get_func(xb.from);
            if (xreffunc) {
                xref.m_objectid = xreffunc->start_ea;
                if (xref.m_objectid != lastobjid) {
                    xrefindex = 0;
                    lastobjid = xref.m_objectid;
                }
                else
                    xrefindex++;
                xref.m_index = xrefindex + 1;
            }
            else {
                xref.m_objectid = 0;
                xref.m_index = 0;
                lastobjid = 0;
                xrefindex = 0;
            }
        }
        xrefs.push_back(xref);
    }
    return xrefs;
}

qstring getXrefsToAddressAsString(ea_t ea) {
    qvector<XRef> xrefs = getXrefToAddress(ea);
    qstring refsStr;
    bool firstRef = true;
    for (int i = 0; i < xrefs.size(); i++) {
        XRef &x = xrefs[i];
        if (!firstRef)
            refsStr += "  ";
        else
            firstRef = false;
        refsStr += format("0x%X %d 0x%X %d", x.m_address, x.m_type, x.m_objectid, x.m_index);
    }
    return refsStr;
}
