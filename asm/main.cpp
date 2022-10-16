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
        if (cmd[offset - 1] == ':')
        {
            cmd[offset - 1] = '\0';
            AsmLabelUpd(&labels_info, cmd, instr_ptr);
        }
#undef CMD_DEF
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

