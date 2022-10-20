#ifndef CPU_H
#define CPU_H

#include "cmds.h"
#include "stack.h"

#define COMMA           ,
#define CUR             BUF[IP]
#define ARG(type)       (*((type##_TYPE*) &(CUR)))
#define INC(type)       IP += BYTES_##type
#define REG(name)       REGS[REG_##name]

#define PUSH(val)       StackPush (&STK, val)
#define TOP             StackTop  (&STK)
#define POP             StackPop  (&STK)

#define RFLAGS          REG(RFLAGS)

// temporary cringe
#define CF_IND          0
#define ZF_IND          1

#define CF              ((RFLAGS >> CF_IND) & 1)
#define ZF              ((RFLAGS >> ZF_IND) & 1)

#define GET_VAL(flags, var)     \
    if (flags & FLG_IMM)        \
    {                           \
        var += ARG(VAL);        \
        INC(VAL);               \
    }                           \
                                \
    if (flags & FLG_REG)        \
    {                           \
        var += REGS[ARG(REG)];  \
        INC(REG);               \
    }                           \
                                \
    if (flags & FLG_MEM)        \
    {                           \
        var = MEM[var];         \
    }

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
