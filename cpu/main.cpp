#include <fcntl.h>
#include <stdint.h>
#include "cmds.h"
#include "stack.h"

int main()
{
    int fd_inp = open("output.bin", O_RDONLY, 0);
    SIZE_TYPE size = 0;
    read(fd_inp, (void*) &size, BYTES_SIZE);

    Proc cpu = {};
    ProcCtor(&cpu, size, fd_inp);

    close(fd_inp);

    while (cpu.instr_ptr < size)
    {
        CMD_TYPE com = 0;
        read(fd_inp, &com, BYTES_CMD);

#define COMMA         ,

#define STK           &cpu.stk

#define BUF           cpu.buf
#define IP            cpu.instr_ptr
#define ARG(type)     *((type##_TYPE*) BUF[IP])
#define LOL(flags)    
        VAL_TYPE val = 0;
        if (flags & CMD_IMM)
        {
            val += ARG(VAL);
            IP  += BYTES_VAL;
        }
        if (flags & CMD_REG)
        {
            val += ARG(REG);
            IP  += BYTES_REG;
        }
        if (flags & CMD_MEM)
        {
            
        }

#define CMD_DEF(name, arg, code) \
    case CMD_##name:             \
        IP += BYTES_CMD;         \
        code                     \
        break;          

        switch ((CMD_TYPE) (*((CMD_TYPE*) &com) & COM_MASK))
        {
            #include "cmd_def.h"
#undef CMD_DEF
        }
    }

    StackDtor(&stk);

    return 0;
}

int32_t ProcGetValue(Proc *proc
