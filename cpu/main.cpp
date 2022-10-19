#include <fcntl.h>
#include <stdint.h>
#include "cpu.h"
#include "cmd.h"
#include "stack.h"

int main()
{
    int fd_inp = open("output.bin", O_RDONLY, 0);
    SIZE_TYPE size = 0;
    read(fd_inp, (void*) &size, BYTES_SIZE);

    Proc cpu = {};
    ProcCtor(&cpu);

    ProcLoadFromFile(&cpu, "output.bin");

    // TODO
    close(fd_inp);

    while (cpu.instr_ptr < size)
    {
        CMD_TYPE com = 0;
        read(fd_inp, &com, BYTES_CMD);

#define STK             cpu.stk
#define BUF             cpu.buf
#define IP              cpu.instr_ptr
#define REGS            cpu.regs

#define CMD_DEF(name, arg, code) \
    case CMD_##name:             \
        INC(CMD_CONT);           \
        code                     \
        break;          

#define JMP_DEF(name, cond) CMD_DEF(name, LBL_ARG, \
        {                                          \
            LBL_TYPE lbl = 0;                      \
                                                   \
            INC(CMD_FLAGS);                        \
                                                   \
            lbl = ARG(LBL);                        \
            INC(LBL);                              \
                                                   \
            if (cond)                              \
                IP = lbl;                          \
        })

        switch ((CMD_TYPE) (*((CMD_TYPE*) &com) & COM_MASK))
        {
            #include "cmd_def.h"
#undef CMD_DEF
#undef JMP_DEF
        }
    }

    StackDtor(&stk);

    return 0;
}

int64_t ProcGetValue(Proc *proc)
{
    FLAGS_TYPE flags = 
}
