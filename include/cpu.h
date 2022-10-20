#ifndef CPU_H
#define CPU_H

#include "cmds.h"
#include "stack.h"

struct Proc
{
    Stack     stk;

    SIZE_TYPE instr_ptr;

    SIZE_TYPE buf_size;
    char      buf[MAX_PROG_SIZE];

    VAL_TYPE  regs[N_REG];

    SIZE_TYPE mem_size;
    VAL_TYPE  *mem;
};

void ProcCtor         (Proc *cpu, int32_t size);
void ProcDtor         (Proc *cpu);
void ProcLoadFromFile (Proc *cpu, const char *filename);

#endif  // CPU_H
