#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>

#define MAKESURE(what, x) typedef char make_sure_##what[(x) ? 1 : -1]
#define die(...) die_(__FILE__, __VA_ARGS__)

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long bits;

enum
{
    NString = 72,
    NIns = 1 << 20,
    NAlign = 3,
    NField = 32,
    NBit = CHAR_BIT * sizeof(bits),
};

struct BSet
{
    uint nt;
    bits *t;
};

struct Ref
{
    uint type : 3;
    uint val : 29;
};

#endif