#include <fcntl.h>
#include <stdint.h>
#include "cpu.h"
#include "cmds.h"
#include "stack.h"

int main()
{
    Proc cpu = {};
    ProcCtor(&cpu, 1024);

#define STK     cpu.stk
#define BUF     cpu.buf
#define IP      cpu.instr_ptr
#define REGS    cpu.regs
#define MEM     cpu.mem
#define CALLSTK cpu.callstk

    ProcLoadFromFile(&cpu, "output.bin");

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

