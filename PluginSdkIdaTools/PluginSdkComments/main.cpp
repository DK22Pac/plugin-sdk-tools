#include "ida.hpp"
#include "idp.hpp"
#include "loader.hpp"
#include "enum.hpp"
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
    Variable,
    Struct,
    Enum
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

qstring GetModuleFromComment(qstring const &comment, CommentEntity entityType) {
    qstring outComment, outModuleName, strDummy; bool boolDummy;
    if (entityType == CommentEntity::Function)
        getFunctionExtraInfo(comment, outComment, outModuleName, strDummy, strDummy, boolDummy, boolDummy);
    else if (entityType == CommentEntity::Variable)
        getVariableExtraInfo(comment, outComment, outModuleName, strDummy);
    else if (entityType == CommentEntity::Struct)
        getStructExtraInfo(comment, outComment, outModuleName, strDummy, boolDummy, boolDummy, boolDummy, boolDummy, boolDummy);
    else if (entityType == CommentEntity::Enum)
        getEnumExtraInfo(comment, outComment, outModuleName, strDummy, boolDummy, strDummy);
    return outModuleName;
}

qstring GetCommentWithNewModule(qstring const &oldComment, qstring const &moduleName, CommentEntity entityType) {
    qstring oldModuleName = GetModuleFromComment(oldComment, entityType);
    qstring newModuleStr = qstring("module:") + moduleName + " ";
    qstring realComment, optionStr;
    qstrvec_t options;
    if (entityType == CommentEntity::Function)
        options = getFunctionOptions();
    else if (entityType == CommentEntity::Variable)
        options = getVariableOptions();
    else if (entityType == CommentEntity::Struct)
        options = getStructOptions();
    else if (entityType == CommentEntity::Enum)
        options = getEnumOptions();
    getOptionsAndComment(oldComment, options, realComment, optionStr);
    optionStr += " ";
    qstring moduleStr = qstring("module:") + oldModuleName + " ";
    auto moduleStrPos = optionStr.find(moduleStr);
    if (moduleStrPos != qstring::npos)
        optionStr.remove(moduleStrPos, moduleStr.length());
    else
        moduleStrPos = 0;
    optionStr.insert(moduleStrPos, newModuleStr);
    optionStr.trim2();
    qstring result = optionStr;
    if (realComment.size() > 0)
        result += qstring("\n") + realComment;
    return result;
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
            auto oldModuleName = GetModuleFromComment(comment, IsVariable? CommentEntity::Variable : CommentEntity::Function);
            if constexpr(std::is_same_v<CommentOptionType, ModuleOnly>) {
                qstring str = oldModuleName;
            #if (IDA_VER >= 70)
                if (ask_str(&str, HIST_IDENT, "Enter module name for %s:", entityName.c_str())) {
            #else
                char *askresult = askstr(HIST_IDENT, oldModuleName.c_str(), "Enter module name for %s:", entityName.c_str());
                if (askresult) {
                    str = askresult;
            #endif
                    LastModuleName = str;
                    changeComment = true;
                }
            }
            else if constexpr(std::is_same_v<CommentOptionType, PreviousModule>)
                changeComment = true;
            if (changeComment)
                comment = GetCommentWithNewModule(comment, LastModuleName, IsVariable ? CommentEntity::Variable : CommentEntity::Function);
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

struct assign_module_action_handler : public action_handler_t {
    virtual int idaapi activate(action_activation_ctx_t *) {
        qstring moduleName;     
    #if (IDA_VER >= 70)
        if (ask_str(&moduleName, HIST_IDENT, "Module name:")) {
    #else
        char *askresult = askstr(HIST_IDENT, NULL, "Module name:");
        if (askresult) {
            moduleName = askresult;
        #endif
            if (!moduleName.empty()) {

                unsigned int numFunctions = 0, numVariables = 0, numEnums = 0, numStructs = 0;
                
                // functions

                auto func = get_next_func(0);
                while (func) {
                    ea_t funcAddr;
                #if (IDA_VER >= 70)
                    funcAddr = func->start_ea;
                #else
                    funcAddr = func->startEA;
                #endif
                    qstring funcName = get_short_name(funcAddr);
                    if (startsWith(funcName, moduleName)) {
                        auto funcName2 = funcName.substr(moduleName.length());
                        if (startsWith(funcName2, "::") || startsWith(funcName2, "__")) {
                        #if (IDA_VER >= 70)
                            qstring oldComment;
                            get_func_cmt(&oldComment, func, false);
                        #else
                            qstring oldComment = get_func_cmt(func, false);
                        #endif
                            qstring newComment = GetCommentWithNewModule(oldComment, moduleName, CommentEntity::Function);
                            if (!set_func_cmt(func, newComment.c_str(), false)) {
                                warning("Unable to set function '%s' comment at address 0x%X\nComment:\n%s",
                                    funcName.c_str(), funcAddr, newComment.c_str());
                            }
                            else
                                numFunctions++;
                        }
                    }
                #if (IDA_VER >= 70)
                    func = get_next_func(func->start_ea);
                #else
                    func = get_next_func(func->startEA);
                #endif
                }

                // variables

                auto seg = get_first_seg();
                while (seg) {
                    qstring segName;
                #if (IDA_VER >= 70)
                    get_segm_name(&segName, seg);
                #else
                    static char segNameBuf[32];
                    if (get_true_segm_name(seg, segNameBuf, 32) != static_cast<ssize_t>(-1))
                        segName = segNameBuf;
                #endif
                    if (isDataSegment(segName)) {
                    #if (IDA_VER >= 70)
                        auto ea = seg->start_ea;
                        while (ea < seg->end_ea) {
                    #else
                        auto ea = seg->startEA;
                        while (ea < seg->endEA) {
                    #endif
                            int size = 1;
                            tinfo_t type;
                        #if (IDA_VER >= 70)
                            if (get_tinfo(&type, ea)) {
                        #else
                            if (get_tinfo2(ea, &type)) {
                        #endif
                                // get type size
                                size = type.get_size();
                                if (size < 1)
                                    size = 1;
                                auto itemSize = get_item_size(ea);
                                if (itemSize > size)
                                    size = itemSize;
                            }
                            auto varName = get_short_name(ea);
                            if (startsWith(varName, moduleName)) {
                                auto varName2 = varName.substr(moduleName.length());
                                if (startsWith(varName2, "::") || startsWith(varName2, "__")) {
                                    qstring oldComment;
                                #if (IDA_VER >= 70)
                                    get_cmt(&oldComment, ea, false);
                                #else
                                    static char cmtLineBuf[2048];
                                    if (get_cmt(ea, false, cmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                                        oldComment = cmtLineBuf;
                                #endif
                                    qstring newComment = GetCommentWithNewModule(oldComment, moduleName, CommentEntity::Variable);
                                    if (!set_cmt(ea, newComment.c_str(), false)) {
                                        warning("Unable to set variable '%s' comment at address 0x%X\nComment:\n%s",
                                            varName.c_str(), ea, newComment.c_str());
                                    }
                                    else
                                        numVariables++;
                                }
                            }
                            ea += size;
                        }
                    }
                #if (IDA_VER >= 70)
                    seg = get_next_seg(seg->start_ea);
                #else
                    seg = get_next_seg(seg->startEA);
                #endif
                }

                // struct

                auto strucid = get_struc_id(moduleName.c_str());
                if (strucid != BADADDR) {
                    qstring oldComment;
                #if (IDA_VER >= 70)
                    get_struc_cmt(&oldComment, strucid, false);
                #else
                    static char cmtLineBuf[2048];
                    if (get_struc_cmt(strucid, false, cmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                        oldComment = cmtLineBuf;
                #endif
                    qstring newComment = GetCommentWithNewModule(oldComment, moduleName, CommentEntity::Struct);
                    set_struc_cmt(strucid, newComment.c_str(), false);
                    numStructs++;
                }

                // enum

                auto enumid = get_enum(moduleName.c_str());
                if (enumid != BADADDR) {
                    qstring oldComment;
                #if (IDA_VER >= 70)
                    get_enum_cmt(&oldComment, enumid, false);
                #else
                    static char cmtLineBuf[2048];
                    if (get_enum_cmt(enumid, false, cmtLineBuf, 2048) != static_cast<ssize_t>(-1))
                        oldComment = cmtLineBuf;
                #endif
                    qstring newComment = GetCommentWithNewModule(oldComment, moduleName, CommentEntity::Enum);
                    set_enum_cmt(enumid, newComment.c_str(), false);
                    numEnums++;
                }

                msg("Assigned module '%s' to: %d function(s), %d variable(s), %d struct(s), %d enum(s)\n",
                    moduleName.c_str(), numFunctions, numVariables, numStructs, numEnums);
            }
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
assign_module_action_handler assign_module_action;
action_desc_t set_comment_action_desc = ACTION_DESC_LITERAL("PluginSdk:set_comment", "Set plugin-sdk comment...", &set_comment_action, NULL, NULL, -1);
action_desc_t set_module_action_desc = ACTION_DESC_LITERAL("PluginSdk:set_module", "Set plugin-sdk module...", &set_module_action, NULL, NULL, -1);
action_desc_t set_previous_module_action_desc =
    ACTION_DESC_LITERAL("PluginSdk:set_previous_module", PreviousModuleLabelName, &set_previous_module_action, NULL, NULL, -1);
action_desc_t assign_module_action_desc = ACTION_DESC_LITERAL("PluginSdk:assign_module", "Assign plugin-sdk module...", 
    &assign_module_action, NULL, NULL, -1);

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
    register_action(assign_module_action_desc);
    attach_action_to_menu("Edit/Other", "PluginSdk:assign_module", SETMENU_APP);
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
        unregister_action("PluginSdk:assign_module");
        detach_action_from_menu("Edit/Other", "PluginSdk:assign_module");
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
