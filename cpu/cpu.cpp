#include "cpu.h"

void ProcCtor(Proc *cpu, SIZE_TYPE size, int fd)
{
    ASSERT(cpu != NULL);

    cpu->size      = size;
    cpu->instr_ptr = 0;
    StackCtor(cpu->stk, 64);
    read(fd, cpu->buf, size);
}

void ProcDtor(Proc *cpu)
{
    ASSERT(cpu != NULL);

    cpu->size      = -1;
    cpu->instr_ptr = 0;

    memset(cpu->buf, -1, MAX_PROG_SIZE);
}

