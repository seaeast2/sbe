#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "Common.h"

class Blk;

struct Op
{
    char *name;
    short argcls[2][4];
    int canfold;
};

struct Ins
{
    uint op : 30;
    uint cls : 2;
    Ref to;
    Ref arg[2];
};

struct Phi
{
    Ref to;
    Ref *arg;
    Blk **blk;
    uint narg;
    int cls;
    Phi *link;
};

#endif