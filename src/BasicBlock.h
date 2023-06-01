#ifndef _BASICBLOCK_H_
#define _BASICBLOCK_H_

#include "Common.h"

class Phi;
class Ins;

class BasicBlock
{
public:
    Phi *phi;
    Ins *ins;
    uint nins;
    struct
    {
        short type;
        Ref arg;
    } jmp;
    BasicBlock *s1;
    BasicBlock *s2;
    BasicBlock *link;

    uint id;
    uint visit;

    BasicBlock *idom;
    BasicBlock *dom, *dlink;
    BasicBlock **fron;
    uint nfron;

    BasicBlock **pred;
    uint npred;
    BSet in[1], out[1], gen[1];
    int nlive[2];
    int loop;
    char name[NString];

public:
    BasicBlock();
    ~BasicBlock();
};

typedef BasicBlock Blk;

#endif