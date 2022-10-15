#include <fcntl.h>
#include "cmds.h"
#include "stack.h"

int main()
{
    int fd_inp = open("output.bin", O_RDONLY, 0);

    Stack stk = {};
    StackCtor(&stk, 16);

    int32_t size = 0;
    read(fd_inp, &size, BYTES_SIZE);

    int instr_ptr = 0;
    while (instr_ptr < size)
    {
        int16_t com = 0;
        read(fd_inp, &com, BYTES_CMD);

        switch ((int16_t) ((*((uint16_t*) &com)) & COM_MASK))
        {
            case CMD_PUSH:
            {
                int32_t val = 0;
                read(fd_inp, &val, BYTES_VAL);
                instr_ptr += BYTES_VAL;
                StackPush(&stk, val);
                instr_ptr += BYTES_CMD;
                break;
            }
            case CMD_POP:
            {
                StackPop(&stk);
                instr_ptr += BYTES_CMD;
                break;
            }
            case CMD_ADD:
            {
                int32_t val1 = StackPop(&stk),
                        val2 = StackPop(&stk);
                StackPush(&stk, val1 + val2);
                instr_ptr += BYTES_CMD;
                break;
            }
            case CMD_SUB:
            {
                int32_t val1 = 0, val2 = 0;
                val1 = StackPop(&stk);
                val2 = StackPop(&stk);
                StackPush(&stk, val2 - val1);
                instr_ptr += BYTES_CMD;
                break;
            }
            case CMD_MUL:
            {
                int32_t val1 = 0, val2 = 0;
                val1 = StackPop(&stk);
                val2 = StackPop(&stk);
                StackPush(&stk, val1 * val2);
                instr_ptr += BYTES_CMD;
                break;
            }
            case CMD_DIV:
            {
                int32_t val1 = 0, val2 = 0;
                val1 = StackPop(&stk);
                val2 = StackPop(&stk);
                StackPush(&stk, val2 / val1);
                instr_ptr += BYTES_CMD;
                break;
            }
            case CMD_OUT:
            {
                int32_t val = StackPop(&stk);
                printf("%d\n", val);
                instr_ptr += BYTES_CMD;
                break;
            }
            case CMD_JMP:
            {
                int32_t label = 0;
                read(fd_inp, &label, BYTES_LABEL);
                instr_ptr = BYTES_LABEL;
                break;
            }
            case CMD_HLT:
            {
                exit(0);
            }
        }
    }

    StackDtor(&stk);

    return 0;
}
