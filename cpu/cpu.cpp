#include <string.h>
#include "cpu.h"

void ProcCtor(Proc *cpu, SIZE_TYPE mem_size)
{
    ASSERT(cpu      != NULL);
    ASSERT(mem_size > 0);

    StackCtor(cpu->stk, 16);

    memset(cpu->buf, 0, MAX_PROG_SIZE);

    cpu->buf_size = 0;

    cpu->mem_size = mem_size;
    cpu->mem      = (VAL_TYPE*) calloc(mem_size, BYTES_VAL);

    memset(cpu->regs, 0, N_REG);
}

void ProcDtor(Proc *cpu)
{
    ASSERT(cpu != NULL);

    StackDtor(cpu->stk);

    cpu->mem_size = 0;
    free(cpu->mem);

    cpu->buf_size = 0;
    memset(cpu->buf, 0, MAX_PROG_SIZE);

    memset(cpu->regs, 0, N_REG);
}

void ProcLoadFromFile(Proc *cpu, const char *filename)
{
    ASSERT(cpu != NULL);

    int fd_inp = open(filename, O_RDONLY, 0);
    read(fd_inp, (void*) &cpu->buf_size, BYTES_SIZE);

    read(fd_inp, (void*) cpu->buf, cpu->buf_size);
    
    close(fd_inp);
}
