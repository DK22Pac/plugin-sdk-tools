#include "ida.hpp"
#include "idp.hpp"
#include "loader.hpp"
#include "interface.h"

int idaapi init(void) {
    return PLUGIN_OK;
}

#if (IDA_VER >= 70)
bool idaapi run(size_t) {
#else
void idaapi run(int) {
#endif
    showform();
#if (IDA_VER >= 70)
    return true;
#endif
}

plugin_t __declspec(dllexport) PLUGIN = {
    IDP_INTERFACE_VERSION,
    0,                         // plugin flags
    init,                      // initialize
    NULL,                      // terminate. this pointer may be NULL.
    run,                       // invoke plugin
    NULL,                      // long comment about the plugin
    NULL,                      // multiline help about the plugin
    "Import from Plugin-SDK...", // the preferred short name of the plugin
    NULL                       // the preferred hotkey to run the plugin
};
