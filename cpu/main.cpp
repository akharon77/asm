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

#define PUSH(val)     StackPush (&cpu.stk, val)
#define POP           StackPop  (&stk)

#define BUF           cpu.buf
#define IP            cpu.instr_ptr
#define ARG(i)        BUF[IP+(i)]
#define ARG_VAL_IP(i) (BYTES_CMD + ((i) - 1) * BYTES_VAL)
#define ARG_LBL_IP(i) (BYTES_CMD + ((i) - 1) * BYTES_ARG)

#define REG(name)     REG_##name

#define RAX           REG(RAX)
#define RBX           REG(RBX)
#define RCX           REG(RCX)
#define RDX           REG(RDX)

#define CF            REG(CF)
#define ZF            REG(ZF)

#define CMD_DEF(name, arg, code) \
    case CMD_##name:             \
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
