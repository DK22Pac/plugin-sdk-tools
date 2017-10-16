#include "plugin.h"
#include "CLEO.h"

using namespace plugin;

class $classname$ {
public:
    static OpcodeResult WINAPI OpcodeFunc(CScriptThread* thread) {
        // Добавьте здесь тело опкода
        
        return OR_CONTINUE;
    }

    $classname$() {
        CLEO_RegisterOpcode(/*Напишите здесь ID опкода*/, OpcodeFunc);
    }
} $classinstancename$;
