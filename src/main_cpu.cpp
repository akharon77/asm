#include <fcntl.h>
#include <stdint.h>
#include "cpu.h"
#include "cmds.h"
#include "stack.h"
#include "iostr.h"
#include "colors.h"

int main(int argc, const char* argv[])
{
    const char *input_filename   = "output.bin";

    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options, EXEC_OPTIONS, N_EXEC_OPTIONS);
    if (!ok)
    {
        printf(RED "Wrong arguments\n" NORMAL);
        return 1;
    }

    if (options[HELP_OPTION])
    {
        for (int i = 0; i < N_EXEC_OPTIONS; ++i)
            printf("%10s %5s %s\n",
                    EXEC_OPTIONS[i].strFormLong,
                    EXEC_OPTIONS[i].strFormShort,
                    EXEC_OPTIONS[i].description);
        return 0;
    }

    if (options[INPUT_FILE_OPTION])
        input_filename = argv[options[INPUT_FILE_OPTION] + 1];

    Proc cpu = {};

    int mem_size = 2048;  // magic??

    if (options[VMEM_OPTION])
        mem_size = 80*24;  // magic??
    if (options[MEM_OPTION])
        mem_size = atoi(argv[options[MEM_OPTION] + 1]);

    ProcCtor(&cpu, mem_size);

#define STK     cpu.stk
#define BUF     cpu.buf
#define IP      cpu.instr_ptr
#define REGS    cpu.regs
#define MEM     cpu.mem
#define CALLSTK cpu.callstk

    ProcLoadFromFile(&cpu, input_filename);

    while (cpu.instr_ptr < cpu.buf_size)
    {

#define CMD_DEF(name, arg, code) \
    case CMD_##name:             \
        INC(CMD_CONT);           \
        code                     \
        break;          

#define JMP_DEF(name, cond)   \
    CMD_DEF(name, LBL_ARG,    \
        {                     \
            LBL_TYPE lbl = 0; \
                              \
            INC(CMD_FLAGS);   \
                              \
            lbl = ARG(LBL);   \
            INC(LBL);         \
                              \
            if (cond)         \
                IP = lbl;     \
        })

        switch (ARG(CMD_CONT))
        {
            #include "cmd_def.h"
        }

#undef CMD_DEF
#undef JMP_DEF
#undef STK
#undef BUF
#undef IP
#undef REGS
#undef MEM
#undef CALLST

    }

    ProcDtor(&cpu);

    return 0;
}

