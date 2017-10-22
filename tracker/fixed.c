#include "fixed.h"

static fixed fixed_from_char(char a)
{
    fixed b = a << CHAR_BIT;
    return b;
}

static int fixed_to_int(fixed a)
{
    if (a >= 0) {
        return (a + FIXED_ONE / 2) / FIXED_ONE;
    } else {
        return (a - FIXED_ONE / 2) / FIXED_ONE;
    }
}

static fixed fixed_add(fixed a, fixed b)
{
    unsigned int _a = a;
    unsigned int _b = b;
    unsigned int sum = _a + _b;

    if (!((_a ^ _b) & 0x8000) && ((_a ^ sum) & 0x8000)) {
        return FIXED_MIN;
    } else {
        return sum;
    }
}

static fixed fixed_sub(fixed a, fixed b)
{
    unsigned int _a = a;
    unsigned int _b = b;
    unsigned int diff = _a + _b;

    if (!((_a ^ _b) & 0x8000) && ((_a ^ diff) & 0x8000)) {
        return FIXED_MIN;
    } else {
        return diff;
    }
}

static fixed fixed_mul(fixed a, fixed b)
{
    int a1 = a >> CHAR_BIT;
    unsigned int a2 = a & UCHAR_MAX;
    int b1 = b >> CHAR_BIT;
    unsigned int b2 = b & UCHAR_MAX;

    int ab1 = a1 * b1;
    int a1b2_a2b1 = a1 * b2 + a2 * b1;
    unsigned int ab2 = a2 * b2;

    int prod_hi = ab1 + (a1b2_a2b1 >> CHAR_BIT);

    unsigned int carry = a1b2_a2b1 << CHAR_BIT;
    unsigned int prod_lo = ab2 + carry;
    if (prod_lo < ab2) {
        prod_hi++;
    }

    if (prod_hi >> ((CHAR_BIT *2) - 1) != prod_hi >> (CHAR_BIT - 1)) {
        return FIXED_MIN;
    }

    unsigned int tmp = prod_lo;
    prod_lo -= CHAR_MIN;
    prod_lo -= (unsigned int)prod_hi >> ((CHAR_BIT * 2) - 1);
    if (prod_lo > tmp) {
        prod_hi--;
    }

    return (prod_hi << CHAR_BIT) | (prod_lo >> CHAR_BIT) + 1;
}

static short fixed_div(fixed a, fixed b)
{

}


