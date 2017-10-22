#include "debug.h"

int break_with_value(int v1, int v2) {
#ifdef __clemency__
    dbrk;
#endif
    return v1;
}

