#include "ida.hpp"
#include "idp.hpp"
#include "loader.hpp"
#include "ut_ida.h"
#include "ut_options.h"

// save last module name here
qstring LastModuleName;

// buffer to hold label name for set_module_optional
char PreviousModuleLabelName[256];

// What are we looking for?
enum class CommentEntity {
    Unknown,
    Function,
    Variable
};

// Get entity type at this address
CommentEntity GetEntityAt(ea_t address) {
    // if function at this address
    if (get_func(address))
        return CommentEntity::Function;
    // if variable at this address
    if (isInDataSegment(address) && !getAddrName(address).empty())
        return CommentEntity::Variable;
    // not a function, not a variable
    return CommentEntity::Unknown;
}

// Get entity type at current address
CommentEntity GetCurrentEntity() {
    return GetEntityAt(get_screen_ea());
}

struct Default {};        // Set plugin-sdk comment...
struct ModuleOnly {};     // Set plugin-sdk module...
struct PreviousModule {}; // Set plugin-sdk module to %s (optional)

template<typename CommentOptionType>
struct CommentHandler : public action_handler_t {
    virtual int idaapi activate(action_activation_ctx_t *) {
        auto address = get_screen_ea();
        bool IsVariable = GetEntityAt(address) == CommentEntity::Variable;
        qstring entityName, comment;
        bool changeComment = false;
        if (IsVariable) {
            entityName.sprnt("variable '%s'", getAddrName(address).c_str());
        #if (IDA_VER >= 70)
            get_cmt(&comment, address, false);
        #else
            static char cmtLineBuf[2048];
            if (get_cmt(address, false, cmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                comment = cmtLineBuf;
        #endif
        }
        else {
            qstring funcName = getFunctionName(address);
            entityName.sprnt("function '%s'", funcName.c_str());
            auto func = get_func(address);
            if (func) {
            #if (IDA_VER >= 70)
                get_func_cmt(&comment, func, false);
            #else
                comment = get_func_cmt(func, false);
            #endif
            }
            else
                warning("Unable to get function data for %s at address 0x%X", entityName.c_str(), address);
        }
        if constexpr(std::is_same_v<CommentOptionType, Default>) {
        #if (IDA_VER >= 70)
            changeComment = ask_text(&comment, 2047, comment.c_str(), "Enter comment for %s:", entityName.c_str());
        #else
            static char commentbuf[2048];
            char *askresult = asktext(2047, commentbuf, comment.c_str(), "Enter comment for %s:", entityName.c_str());
            if (askresult) {
                comment = commentbuf;
                changeComment = true;
            }
        #endif
        }
        else {
            qstring outComment, outModuleName, outRetType, outPriority;
            bool outIsConst;
            getFunctionExtraInfo(comment, outComment, outModuleName, outRetType, outPriority, outIsConst);
            qstring moduleStr = qstring("module:") + outModuleName + " ";
            if constexpr(std::is_same_v<CommentOptionType, ModuleOnly>) {
                qstring str = outModuleName;
                
            #if (IDA_VER >= 70)
                if (ask_str(&str, HIST_IDENT, "Enter module name for %s:", entityName.c_str())) {
            #else
                char *askresult = askstr(HIST_IDENT, outModuleName.c_str(), "Enter module name for %s:", entityName.c_str());
                if (askresult) {
                    str = askresult;
            #endif
                    LastModuleName = str;
                    changeComment = true;
                }
            }
            else if constexpr(std::is_same_v<CommentOptionType, PreviousModule>)
                changeComment = true;
            if (changeComment) {
                qstring newModuleStr = qstring("module:") + LastModuleName + " ";
                qstring realComment, optionStr;
                getOptionsAndComment(comment, IsVariable? getVariableOptions() : getFunctionOptions(), realComment, optionStr);
                optionStr += " ";
                auto moduleStrPos = optionStr.find(moduleStr);
                if (moduleStrPos != qstring::npos)
                    optionStr.remove(moduleStrPos, moduleStr.length());
                else
                    moduleStrPos = 0;
                optionStr.insert(moduleStrPos, newModuleStr);
                optionStr.trim2();
                comment = optionStr;
                if (realComment.size() > 0)
                    comment += qstring("\n") + realComment;
            }
        }
        if (changeComment) {
            bool err = false;
            if (IsVariable)
                err = !set_cmt(address, comment.c_str(), false);
            else {
                auto func = get_func(address);
                if (func)
                    err = !set_func_cmt(func, comment.c_str(), false);
                else
                    warning("Unable to get function data for %s at address 0x%X", entityName.c_str(), address);
            }
            if (err)
                warning("Unable to set %s comment at address 0x%X", entityName.c_str(), address);
        }
        return 0;
    }

    virtual action_state_t idaapi update(action_update_ctx_t *) {
        return AST_ENABLE_ALWAYS;
    }
};

static CommentHandler<Default> set_comment_action;
static CommentHandler<ModuleOnly> set_module_action;
static CommentHandler<PreviousModule> set_previous_module_action;
action_desc_t set_comment_action_desc = ACTION_DESC_LITERAL("PluginSdk:set_comment", "Set plugin-sdk comment...", &set_comment_action, NULL, NULL, -1);
action_desc_t set_module_action_desc = ACTION_DESC_LITERAL("PluginSdk:set_module", "Set plugin-sdk module...", &set_module_action, NULL, NULL, -1);
action_desc_t set_previous_module_action_desc =
    ACTION_DESC_LITERAL("PluginSdk:set_previous_module", PreviousModuleLabelName, &set_previous_module_action, NULL, NULL, -1);

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
        if (widgetType == BWN_DISASM && GetCurrentEntity() != CommentEntity::Unknown) {
            TPopupMenu *popupMenu = va_arg(va, TPopupMenu *);
            attach_action_to_popup(widget, popupMenu, "PluginSdk:set_comment");
            attach_action_to_popup(widget, popupMenu, "PluginSdk:set_module");
            if (!LastModuleName.empty()) {
                unregister_action("PluginSdk:set_previous_module");
                qsnprintf(PreviousModuleLabelName, 256, "Set plugin-sdk module to '%s'", LastModuleName.c_str());
                register_action(set_previous_module_action_desc);
                attach_action_to_popup(widget, popupMenu, "PluginSdk:set_previous_module");
            }
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
    register_action(set_comment_action_desc);
    register_action(set_module_action_desc);
    register_action(set_previous_module_action_desc);
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
        unregister_action("PluginSdk:set_comment");
        unregister_action("PluginSdk:set_module");
        unregister_action("PluginSdk:set_previous_module");
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
