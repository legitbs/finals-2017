#ifndef FIXED_H
#define FIXED_H

#ifdef __clemency__
#include "limits.h"
#else
#include <limits.h>
#endif

typedef short fixed;

#define FIXED_ONE (1 << CHAR_BIT)
#define FIXED_MAX ((1 << ((CHAR_BIT * 2) - 1)) - 1)
#define FIXED_MIN (1 << ((CHAR_BIT * 2) - 1))

static fixed fixed_from_char(char);
static int fixed_to_int(fixed);
static fixed fixed_add(fixed, fixed);
static fixed fixed_sub(fixed, fixed);
static fixed fixed_mul(fixed, fixed);
static fixed fixed_div(fixed, fixed);

#endif
