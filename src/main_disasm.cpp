#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include "cpu.h"
#include "cmds.h"
#include "stack.h"
#include "colors.h"

int main(int argc, const char *argv[])
{
    const char *input_filename  = "output.bin",
               *output_filename = "output.txt";

    int fd_output = STDOUT_FILENO;

    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options, EXEC_OPTIONS, N_EXEC_OPTIONS);
    if (!ok)
    {
        printf(RED "Wrong arguments\n" NORMAL);
        return 1;
    }

    if (options[INPUT_FILE_OPTION])
        input_filename = argv[options[INPUT_FILE_OPTION] + 1];
    if (options[OUTPUT_FILE_OPTION])
    {
        output_filename = argv[options[OUTPUT_FILE_OPTION] + 1];
        fd_output = open(output_filename, 0, O_WRONLY);
    }

    Proc cpu = {};
    ProcCtor(&cpu, 1);

#define BUF     cpu.buf
#define IP      cpu.instr_ptr
#define SIZE    cpu.buf_size

    ProcLoadFromFile(&cpu, input_filename);

    while (IP < SIZE)
    {
        dprintf(fd_output, "%*x| ", BYTES_SIZE, cpu.instr_ptr);

#define CMD_DEF(name, arg, code)                                            \
    case CMD_##name:                                                        \
        {                                                                   \
            INC(CMD_CONT);                                                  \
            dprintf(fd_output, "%s ", #name);                               \
            if (arg == LBL_ARG)                                             \
            {                                                               \
                INC(CMD_FLAGS);                                             \
                dprintf(fd_output, "%*x ", BYTES_LBL, ARG(LBL));            \
                INC(LBL);                                                   \
            }                                                               \
            else                                                            \
            {                                                               \
                CMD_FLAGS_TYPE flags0 = ARG(CMD_FLAGS);                     \
                INC(CMD_FLAGS);                                             \
                for (int i = 0; i < arg; ++i)                               \
                {                                                           \
                    CMD_FLAGS_TYPE flags = FLAGS(flags0);                   \
                                                                            \
                    if (flags & FLG_MEM)                                    \
                        dprintf(fd_output, "[");                            \
                                                                            \
                    if (flags & FLG_IMM)                                    \
                    {                                                       \
                        dprintf(fd_output, "%d", ARG(VAL));                 \
                        INC(VAL);                                           \
                    }                                                       \
                                                                            \
                    if ((flags & FLG_IMM) && (flags & FLG_REG))             \
                        dprintf(fd_output, "+");                            \
                                                                            \
                    if (flags & FLG_REG)                                    \
                    {                                                       \
                        dprintf(fd_output, "%s", REGS_NAME[ARG(REG)]);      \
                        INC(REG);                                           \
                    }                                                       \
                                                                            \
                    if (flags & FLG_MEM)                                    \
                        dprintf(fd_output, "]");                            \
                                                                            \
                    flags0 = RET_INC_FLAGS(flags0);                         \
                    dprintf(fd_output, " ");                                \
                }                                                           \
            }                                                               \
            dprintf(fd_output, "\n");                                       \
        }                                                                   \
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
    close(fd_output);

    return 0;
}

