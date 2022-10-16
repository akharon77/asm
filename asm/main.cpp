#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "iostr.h"
#include "asm.h"
#include "cmds.h"

int main(int argc, const char* argv[])
{
    int err = 0;
    const char *input_filename = "input.asm", *output_filename = "output.bin";

    TextInfo text = {};
    TextInfoCtor(&text);

    char buf[MAX_PROG_SIZE] = {};

    LabelsInfo labels_info = {};
    LabelsInfoCtor(&labels_info);

    inputText(&text, input_filename, &err);
    markOutText(&text, &err);

    int32_t instr_ptr = 0;

    for (int i = 0; i < text.nlines; ++i)
    {
        char cmd[MAX_LINE_LEN] = "";
        int  offset            = 0;

        sscanf(text.lines[i].ptr, "%s%n", cmd, &offset);

#define CMD_DEF(name, arg)                                                                                        \
        if (strcasecmp(cmd, #name) == 0)                                                                          \
        {                                                                                                         \
            *((CMD_TYPE*) (buf + instr_ptr)) = CMD_##name;                                                        \
            dprintf(2, "%s found\n", #name);                                                                      \
                                                                                                                  \
            int32_t instr_ptr_cmd = instr_ptr;                                                                    \
                                                                                                                  \
            instr_ptr += BYTES_CMD;                                                                               \
                                                                                                                  \
            if (arg != ZERO_ARG)                                                                                  \
            {                                                                                                     \
                dprintf(2, "no zero arg\n");                                                                      \
                if (arg != LBL_ARG)                                                                               \
                {                                                                                                 \
                    dprintf(2, "no lbl arg\n");                                                                   \
                    for (int32_t j = 0; j < arg; ++j)                                                             \
                    {                                                                                             \
                        dprintf(2, "j = %d\n");                                                                   \
                        CMD_FLAGS_TYPE flags = 0;                                                                 \
                        offset += AsmArgProcess(text.lines[i].ptr + offset, &flags, buf, &instr_ptr);             \
                                                                                                                  \
                        *((CMD_TYPE*) (buf + instr_ptr_cmd)) |= (flags << (BITS_CMD_CONT + FLAGS_POS_OCCUP * j)); \
                    }                                                                                             \
                }                                                                                                 \
                else                                                                                              \
                {                                                                                                 \
                    LBL_TYPE label_pos = AsmLabelProcess(&labels_info, text.lines[i].ptr + offset, instr_ptr);    \
                    *((LBL_TYPE*) (buf + instr_ptr)) = label_pos;                                                 \
                    instr_ptr += BYTES_LBL;                                                                       \
                }                                                                                                 \
            }                                                                                                     \
        }

#include "cmd_def.h"

#undef CMD_DEF

        // if (strcasecmp(cmd, "push") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_PUSH;
        //     instr_ptr += BYTES_CMD;

        //     int32_t val = 0;
        //     sscanf(text.lines[i].ptr + offset, "%d", &val);

        //     *((int32_t*)(buf + instr_ptr)) = val;

        //     instr_ptr += 4;
        // }
        // else if (strcasecmp(cmd, "pop") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_POP;
        //     instr_ptr += BYTES_CMD;
        // }
        // else if (strcasecmp(cmd, "add") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_ADD;
        //     instr_ptr += BYTES_CMD;
        // }
        // else if (strcasecmp(cmd, "sub") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_SUB;
        //     instr_ptr += BYTES_CMD;
        // }
        // else if (strcasecmp(cmd, "mul") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_MUL;
        //     instr_ptr += BYTES_CMD;
        // }
        // else if (strcasecmp(cmd, "div") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_DIV;
        //     instr_ptr += BYTES_CMD;
        // }
        // else if (strcasecmp(cmd, "out") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_OUT;
        //     instr_ptr += BYTES_CMD;
        // }
        // else if (strcasecmp(cmd, "hlt") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_HLT;
        //     instr_ptr += BYTES_CMD;
        // }
        // else if (strcasecmp(cmd, "jb") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_JB;
        //     instr_ptr += BYTES_CMD;
        //     AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        // }
        // else if (strcasecmp(cmd, "jbe") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_JBE;
        //     instr_ptr += BYTES_CMD;
        //     AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        // }
        // else if (strcasecmp(cmd, "ja") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_JA;
        //     instr_ptr += BYTES_CMD;
        //     AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        // }
        // else if (strcasecmp(cmd, "jae") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_JAE;
        //     instr_ptr += BYTES_CMD;
        //     AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        // }
        // else if (strcasecmp(cmd, "je") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_JE;
        //     instr_ptr += BYTES_CMD;
        //     AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        // }
        // else if (strcasecmp(cmd, "jne") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_JNE;
        //     instr_ptr += BYTES_CMD;
        //     AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        // }
        // else if (strcasecmp(cmd, "jmp") == 0)
        // {
        //     *((int16_t*) (buf + instr_ptr)) = CMD_JMP;
        //     instr_ptr += BYTES_CMD;
        //     AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        // }
        // else if (cmd[offset - 1] == ':')
        // {
        //     cmd[offset - 1] = '\0';
        //     AsmLabelAdd(cmd, instr_ptr, &labels_info);
        // }
    }

    AsmDoFixups(&labels_info, buf);

    int fd_output = creat(output_filename, S_IRWXU);

    write(fd_output, (void*) &instr_ptr, BYTES_SIZE);
    write(fd_output, buf,                instr_ptr);

    close(fd_output);

    TextInfoDtor   (&text);
    LabelsInfoDtor (&labels_info);

    return 0;
}

