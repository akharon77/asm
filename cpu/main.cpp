#include "cmds.h"

int main()
{
    int fd_inp = open("output.bin", S_IRWXU);

    Stack stk = {};
    StackCtor(&stk);

    int32_t size = 0;
    read(fd_inp, &size, BYTE_SIZE);

    int instr_ptr = 0;
    while (instr_ptr < size)
    {
        int16_t com = 0;
        read(fd_inp, &com, BYTES_COMMAND);

        switch (com & COM_MASK)
        {
            case CMD_PUSH:
                int32_t val = 0;
                read(fd_inp, &val, BYTES_VAL);
                instr_ptr += BYTES_VAL;
                StackPush(&stk, val);
                break;
            case CMD_POP:
                StackPop(&stk);
                break;
            case CMD_ADD:
                int32_t val1 = 0, val2 = 0;
                val1 = StackPop(&stk);
                val2 = StackPop(&stk);
                StackPush(&stk, val1 + val2);
                break;
            case CMD_SUB:
                int32_t val1 = 0, val2 = 0;
                val1 = StackPop(&stk);
                val2 = StackPop(&stk);
                StackPush(&stk, val2 - val1);
                break;
            case CMD_MUL:
                int32_t val1 = 0, val2 = 0;
                val1 = StackPop(&stk);
                val2 = StackPop(&stk);
                StackPush(&stk, val2 * val1);
                break;
            case CMD_DIV:
                int32_t val1 = 0, val2 = 0;
                val1 = StackPop(&stk);
                val2 = StackPop(&stk);
                StackPush(&stk, val2 / val1);
                break;
            case CMD_OUT:
                int32_t val = StackPop(&stk);
                printf("%d\n", val);
                break;
            case CMD_HLT:
                exit(0);
            case CMD_JB:
                
        }
        instr_ptr += BYTES_COM;
    }

    StackDtor(&stk);

    return 0;
}
