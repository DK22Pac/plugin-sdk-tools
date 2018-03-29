#include "ida.hpp"
#include "idp.hpp"
#include "loader.hpp"

#pragma message("This plugin is unfinished")

struct set_function_comment_handler_t : public action_handler_t {
    virtual int idaapi activate(action_activation_ctx_t *) {
        warning("Action enabled!");
        return 0;
    }

    virtual action_state_t idaapi update(action_update_ctx_t *) {
        return AST_ENABLE_ALWAYS;
    }
};
static set_function_comment_handler_t set_function_comment_action;
action_desc_t set_function_comment_action_desc =
    ACTION_DESC_LITERAL("PluginSdk:set_function_comment", "Set Plugin-SDK comment", &set_function_comment_action, NULL, NULL, -1);

ssize_t idaapi hook_cb(void *user_data, int notification_code, va_list va) {
#if (IDA_VER >= 70)
    if (notification_code == ui_populating_widget_popup) {
#else
    if (notification_code == ui_populating_tform_popup) {
#endif
    }
#if (IDA_VER >= 70)
    else if (notification_code == ui_finish_populating_widget_popup) {
        TWidget *widget = va_arg(va, TWidget *);
        auto widgetType = get_widget_type(widget);
#else
    else if (notification_code == ui_finish_populating_tform_popup) {
        TForm *widget = va_arg(va, TForm *);
        auto widgetType = get_tform_type(widget);
#endif
        if (widgetType == BWN_FUNCS) {
            TPopupMenu *popupMenu = va_arg(va, TPopupMenu *);
            attach_action_to_popup(widget, popupMenu, "PluginSdk:set_function_comment");
        }
    }
    return 0;
}

static bool inited = false;

int idaapi init(void) {
#if (IDA_VER >= 70)
    hook_to_notification_point(HT_UI, hook_cb);
#else
    hook_to_notification_point(HT_UI, hook_cb, nullptr);
#endif
    register_action(set_function_comment_action_desc);
    inited = true;
    return PLUGIN_KEEP;
}

#if (IDA_VER >= 70)
bool idaapi run(size_t) {
    return false;
#else
void idaapi run(int) {
#endif
}

void idaapi term(void) {
    if (inited) {
        unregister_action("PluginSdk:set_function_comment");
        unhook_from_notification_point(HT_UI, hook_cb);
    }
}

plugin_t __declspec(dllexport) PLUGIN = {
    IDP_INTERFACE_VERSION,
    PLUGIN_HIDE,               // plugin flags
    init,                      // initialize
    term,                      // terminate. this pointer may be NULL.
    run,                       // invoke plugin
    NULL,                      // long comment about the plugin
    NULL,                      // multiline help about the plugin
    "Plugin-SDK Comments",     // the preferred short name of the plugin
    NULL                       // the preferred hotkey to run the plugin
};
