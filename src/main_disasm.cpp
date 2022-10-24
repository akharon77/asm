#include <fcntl.h>
#include <stdint.h>
#include "cpu.h"
#include "cmds.h"
#include "stack.h"

int main()
{
    Proc cpu = {};
    ProcCtor(&cpu, 1);

#define BUF     cpu.buf
#define IP      cpu.instr_ptr
#define SIZE    cpu.buf_size

    ProcLoadFromFile(&cpu, "output.bin");

    while (IP < SIZE)
    {
        printf("%*x| ", BYTES_SIZE, cpu.instr_ptr);

#define CMD_DEF(name, arg, code)                                \
    case CMD_##name:                                            \
        {                                                       \
            INC(CMD_CONT);                                      \
            printf("%s ", #name);                               \
            if (arg == LBL_ARG)                                 \
            {                                                   \
                INC(CMD_FLAGS);                                 \
                printf("%*x ", BYTES_LBL, ARG(LBL));            \
                INC(LBL);                                       \
            }                                                   \
            else                                                \
            {                                                   \
                CMD_FLAGS_TYPE flags0 = ARG(CMD_FLAGS);         \
                INC(CMD_FLAGS);                                 \
                for (int i = 0; i < arg; ++i)                   \
                {                                               \
                    CMD_FLAGS_TYPE flags = FLAGS(flags0);       \
                                                                \
                    if (flags & FLG_MEM)                        \
                        putc('[', stdout);                      \
                                                                \
                    if (flags & FLG_IMM)                        \
                    {                                           \
                        printf("%d", ARG(VAL));                 \
                        INC(VAL);                               \
                    }                                           \
                                                                \
                    if ((flags & FLG_IMM) && (flags & FLG_REG)) \
                        putc('+', stdout);                      \
                                                                \
                    if (flags & FLG_REG)                        \
                    {                                           \
                        printf("%s", REGS_NAME[ARG(REG)]);      \
                        INC(REG);                               \
                    }                                           \
                                                                \
                    if (flags & FLG_MEM)                        \
                        putc(']', stdout);                      \
                                                                \
                    flags0 = RET_INC_FLAGS(flags0);             \
                }                                               \
            }                                                   \
            putc('\n', stdout);                                 \
        }                                                       \
        break;          

#define JMP_DEF(name, cond)   \
    CMD_DEF(name, LBL_ARG,)

        switch (ARG(CMD_CONT))
        {
            #include "cmd_def.h"
        }

#undef CMD_DEF
#undef JMP_DEF
#undef BUF
#undef IP

    }

    ProcDtor(&cpu);

    return 0;
}

