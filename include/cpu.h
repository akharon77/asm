#ifndef CPU_H
#define CPU_H

struct Proc
{
    Stack     stk;
    SIZE_TYPE size;
    char      buf[MAX_PROG_SIZE];
};

void ProcCtor(Proc *cpu, SIZE_TYPE size, int fd);
void ProcDtor(Proc *cpu);

#endif  // CPU_H
