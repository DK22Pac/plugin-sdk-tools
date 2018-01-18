#include "Function.h"

string Function::GetFullName() {
    if (mScope.empty())
        return mName;
    return mScope + "::" + mName;
}
