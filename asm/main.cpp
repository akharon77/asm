#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include "iostr.h"
#include "asm.h"
#include "prog_mark.h"

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

        if (strcasecmp(cmd, "push") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_PUSH;
            instr_ptr += BYTES_COMMAND;

            int32_t val = 0;
            sscanf(text.lines[i].ptr + offset, "%d", &val);

            *((int32_t*)(buf + instr_ptr)) = val;

            instr_ptr += 4;
        }
        else if (strcasecmp(cmd, "pop") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_POP;
            instr_ptr += BYTES_COMMAND;
        }
        else if (strcasecmp(cmd, "add") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_ADD;
            instr_ptr += BYTES_COMMAND;
        }
        else if (strcasecmp(cmd, "sub") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_SUB;
            instr_ptr += BYTES_COMMAND;
        }
        else if (strcasecmp(cmd, "mul") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_MUL;
            instr_ptr += BYTES_COMMAND;
        }
        else if (strcasecmp(cmd, "div") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_DIV;
            instr_ptr += BYTES_COMMAND;
        }
        else if (strcasecmp(cmd, "out") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_OUT;
            instr_ptr += BYTES_COMMAND;
        }
        else if (strcasecmp(cmd, "hlt") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_HLT;
            instr_ptr += BYTES_COMMAND;
        }
        else if (strcasecmp(cmd, "jb") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_JB;
            instr_ptr += BYTES_COMMAND;
            AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        }
        else if (strcasecmp(cmd, "jbe") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_JBE;
            instr_ptr += BYTES_COMMAND;
            AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        }
        else if (strcasecmp(cmd, "ja") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_JA;
            instr_ptr += BYTES_COMMAND;
            AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        }
        else if (strcasecmp(cmd, "jae") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_JAE;
            instr_ptr += BYTES_COMMAND;
            AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        }
        else if (strcasecmp(cmd, "je") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_JE;
            instr_ptr += BYTES_COMMAND;
            AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        }
        else if (strcasecmp(cmd, "jne") == 0)
        {
            *((int16_t*) (buf + instr_ptr)) = CMD_JNE;
            instr_ptr += BYTES_COMMAND;
            AsmLabelProcess(text.lines[i].ptr + offset, &labels_info, buf, &instr_ptr);
        }
        else if (cmd[offset - 1] == ':')
        {
            cmd[offset - 1] = '\0';
            AsmLabelAdd(cmd, instr_ptr, &labels_info);
        }
    }

    AsmFixLabels(buf, &labels_info);

    int fd_output = creat(output_filename, S_IRWXU);

    write(fd_output, (void*) &instr_ptr, BYTES_SIZE);
    write(fd_output, buf,                instr_ptr);

    close(fd_output);

    TextInfoDtor   (&text);
    LabelsInfoDtor (&labels_info);

    return 0;
}

