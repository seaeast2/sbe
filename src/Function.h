#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "Common.h"

class Blk;
struct Tmp;
struct Con;
struct Mem;

struct Link
{
    char export;
    char thread;
    char align;
    char *sec;
    char *secf;
};
typedef Link Lnk;

class Function
{
public:
    Blk *start;
    Tmp *tmp;
    Con *con;
    Mem *mem;
    int ntmp;
    int ncon;
    int nmem;
    uint nblk;
    int retty; // index in type[], -1 if no aggregate return
    Ref retr;
    Blk **rpo;
    bits reg;
    int slot;
    char vararg;
    char dynalloc;
    char name[NString];
    Lnk lnk;

public:
    Function(void);
    ~Function(void);
};

typedef Function Fn;

#endif