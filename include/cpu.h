#ifndef CPU_H
#define CPU_H

#include "cmds.h"
#include "iostr.h"
#include "stack.h"

const int WIDTH  = 80;
const int HEIGHT = 24;

const int DEFAULT_MEM_SIZE = 2048;

enum OPTIONS
{
    INPUT_FILE_OPTION,
    HELP_OPTION,
    MEM_OPTION,
    VMEM_OPTION,
    N_OPTIONS
};

extern const Option EXEC_OPTIONS[];
extern const size_t N_EXEC_OPTIONS;

struct Proc
{
    Stack      stk;
    Stack      callstk;

    SIZE_TYPE  instr_ptr;

    SIZE_TYPE  buf_size;
    char       buf[MAX_PROG_SIZE];

    VAL_TYPE   regs[N_REG];

    SIZE_TYPE  mem_size;
    VAL_TYPE  *mem;
};

void ProcCtor         (Proc *cpu, int32_t size);
void ProcDtor         (Proc *cpu);
void ProcLoadFromFile (Proc *cpu, const char *filename);

#endif  // CPU_H
