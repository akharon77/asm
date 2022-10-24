#include <string.h>
#include "cpu.h"
#include "stack.h"
#include "iostr.h"

const Option EXEC_OPTIONS[] = 
    {
        {"--file", "-f",  INPUT_FILE_OPTION, "from file (default: output.bin)"},
        {"--help", "-h",  HELP_OPTION,       "show help"},
        {"--vmem", "-v",  VMEM_OPTION,       "use memory as video-memory"},
        {"--mem",  "-m",  MEM_OPTION,        "size of memory"}
    };
 
const size_t N_EXEC_OPTIONS = sizeof(EXEC_OPTIONS) / sizeof(Option);

void ProcCtor(Proc *cpu, SIZE_TYPE mem_size)
{
    ASSERT(cpu      != NULL);
    ASSERT(mem_size > 0);

    cpu->instr_ptr = 0;

    StackCtor(&cpu->stk,     16);
    StackCtor(&cpu->callstk, 16);

    memset(cpu->buf, 0, MAX_PROG_SIZE);

    cpu->buf_size = 0;

    cpu->mem_size = mem_size;
    cpu->mem      = (VAL_TYPE*) calloc(mem_size, BYTES_VAL);

    memset(cpu->regs, 0, N_REG);
}

void ProcDtor(Proc *cpu)
{
    ASSERT(cpu != NULL);

    cpu->instr_ptr = 0;

    StackDtor(&cpu->stk);
    StackDtor(&cpu->callstk);

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
