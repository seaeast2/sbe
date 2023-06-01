#ifndef _TARGET_H_
#define _TARGET_H_

#include "Common.h"
#include "Function.h"

class Target
{
public:
    std::string name;
    char apple;
    int gpr0;   // first general purpose reg
    int ngpr;   // reg 개수
    int fpr0;   // first floating point reg
    int nfpr;   // fp reg 개수
    bits rglob; // globally live regs (e.g., sp, fp)
    int nrglob;
    int *rsave; // caller-save register
    int nrsave[2];
    bits (*retregs)(Ref, int[2]);
    bits (*argregs)(Ref, int[2]);
    int (*memargs)(int);
    void (*abi0)(Fn *);
    void (*abi1)(Fn *);
    void (*isel)(Fn *);
    void (*emitfn)(Fn *, FILE *);
    void (*emitfin)(FILE *);
    char asloc[4];
    char assym[4];

public:
    Target();
    ~Target();
};

#endif