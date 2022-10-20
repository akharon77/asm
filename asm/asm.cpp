#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "asm.h"
#include "cmds.h"
#include "iostr.h"

#define REG_DEF(name) #name,
const char *regs_name[N_REG] = 
    {
#include "reg_def.h"
    };
#undef REG_DEF

void AsmCtor(Asm *asmbler, const char *filename, int *err)
{
    asmbler->instr_ptr = 0;

    memset(asmbler->buf, 0, MAX_PROG_SIZE);
    TextInfoCtor(&asmbler->text);

    InputText(&asmbler->text, filename, err);
    MarkOutText(&asmbler->text, err);

    LabelsInfoCtor(&asmbler->labels_info);
}

void AsmDtor(Asm *asmbler)
{
    memset(asmbler->buf, -1, MAX_PROG_SIZE);

    TextInfoDtor   (&asmbler->text);
    LabelsInfoDtor (&asmbler->labels_info);
}

void AsmRun(Asm *asmbler)
{
    TextInfo   *text        = &asmbler->text;
    LabelsInfo *labels_info = &asmbler->labels_info;
    char       *buf         =  asmbler->buf;
    int32_t    *instr_ptr   = &asmbler->instr_ptr;

    for (int i = 0; i < text->nlines; ++i)
    {
        char cmd[MAX_LINE_LEN]           = "";
        int  offset                      = 0;
        char list_line[2 * MAX_LINE_LEN] = "";

        sscanf(text->lines[i].ptr, "%s%n", cmd, &offset);

#define BUF buf
#define IP  *instr_ptr

#define CMD_DEF(name, arg, code)                                                                                  \
        if (strcasecmp(cmd, #name) == 0)                                                                          \
        {                                                                                                         \
            ARG(CMD) = CMD_##name;                                                                                \
            /**((CMD_TYPE*) (buf + *instr_ptr)) = CMD_##name;*/                                                   \
            dprintf(2, "%s found\n", #name);                                                                      \
                                                                                                                  \
            SIZE_TYPE instr_ptr_cmd = *instr_ptr;                                                                 \
                                                                                                                  \
            /**instr_ptr += BYTES_CMD;*/                                                                          \
                                                                                                                  \
            INC(CMD);                                                                                             \
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
                        offset += AsmArgProcess(text->lines[i].ptr + offset, &flags, buf, instr_ptr);             \
                                                                                                                  \
                        *((CMD_TYPE*) (buf + instr_ptr_cmd)) |= (flags << (BITS_CMD_CONT + FLAGS_POS_OCCUP * j)); \
                    }                                                                                             \
                }                                                                                                 \
                else                                                                                              \
                {                                                                                                 \
                    LBL_TYPE label_pos = AsmLabelProcess(labels_info, text->lines[i].ptr + offset, *instr_ptr);   \
                    *((LBL_TYPE*) (buf + *instr_ptr)) = label_pos;                                                \
                    *instr_ptr += BYTES_LBL;                                                                      \
                }                                                                                                 \
            }                                                                                                     \
                                                                                                                  \
            int32_t list_line_offset = 0;                                                                         \
                                                                                                                  \
            sprintf(list_line, "%8x ", *((CMD_TYPE*) (buf + instr_ptr_cmd)));                                     \
            instr_ptr_cmd += BYTES_CMD;                                                                           \
            list_line_offset += 9;                                                                                \
                                                                                                                  \
            CMD_FLAGS_TYPE flags = *((CMD_FLAGS_TYPE*) (buf + instr_ptr_cmd));                                    \
            sprintf(list_line + list_line_offset, "%8x ", flags);                                                 \
            list_line_offset += 9;                                                                                \
            instr_ptr_cmd += BYTES_CMD_FLAGS;                                                                     \
                                                                                                                  \
            if (flags & FLG_IMM)                                                                                  \
            {                                                                                                     \
                sprintf(list_line + list_line_offset, "%8x ", *((VAL_TYPE*) (buf + instr_ptr_cmd)));              \
                instr_ptr_cmd += BYTES_VAL;                                                                       \
                list_line_offset += 9;                                                                            \
            }                                                                                                     \
                                                                                                                  \
            if (flags & FLG_REG)                                                                                  \
            {                                                                                                     \
                sprintf(list_line + list_line_offset, "%8x ", *((REG_TYPE*) (buf + instr_ptr_cmd)));              \
                instr_ptr_cmd += BYTES_REG;                                                                       \
                list_line_offset += 9;                                                                            \
            }                                                                                                     \
                                                                                                                  \
            sprintf(list_line + list_line_offset, "| %s\n", cmd);                                                 \
                                                                                                                  \
            printf("%s", list_line);                                                                              \
        }

#define JMP_DEF(name, cond) CMD_DEF(name, LBL_ARG,)

        #include "cmd_def.h"

        if (cmd[offset - 1] == ':')
        {
            cmd[offset - 1] = '\0';
            AsmLabelUpd(labels_info, cmd, *instr_ptr);
        }

#undef CMD_DEF
#undef BUF
#undef IP

    }

    AsmDoFixups(labels_info, buf);
}

LBL_TYPE AsmLabelProcess(LabelsInfo *labels_info, const char* str_label, int32_t instr_ptr)
{
    char label_name[MAX_LABEL_LEN] = "";
    sscanf(str_label, "%s", label_name);

    LBL_TYPE label_id = AsmLabelGet(labels_info, label_name);

    LBL_TYPE ret_pos = labels_info->labels[label_id].pos;

    if (ret_pos == NOT_INIT_LABEL)
        AsmFixupAdd(labels_info, label_id, instr_ptr);

    return ret_pos;
}

void AsmFixupAdd(LabelsInfo *labels_info, LBL_TYPE label_id, int32_t instr_ptr)
{
    labels_info->fixups[labels_info->n_fixups] = 
        {
            label_id,
            instr_ptr
        };

    ++labels_info->n_fixups;
}

LBL_TYPE AsmLabelFind(LabelsInfo *labels_info, const char *str_label)
{
    char label_name[MAX_LABEL_LEN] = "";
    sscanf(str_label, "%s", label_name);

    for (int32_t i = 0; i < labels_info->n_labels; ++i)
        if (strcasecmp(labels_info->labels[i].name, label_name) == 0)
            return i;

    return NO_LABEL;
}

void AsmLabelAdd(LabelsInfo *labels_info, const char *str_label, LBL_TYPE pos)
{
    strcpy(labels_info->labels[labels_info->n_labels].name, str_label);
    labels_info->labels[labels_info->n_labels].pos = pos;
    ++labels_info->n_labels;
}

LBL_TYPE AsmLabelGet(LabelsInfo *labels_info, const char *label_name)
{
    LBL_TYPE label_id = AsmLabelFind(labels_info, label_name);

    if (label_id != NO_LABEL)
        return label_id;

    AsmLabelAdd(labels_info, label_name, NOT_INIT_LABEL);
    return labels_info->n_labels - 1;
}

void AsmLabelUpd(LabelsInfo *labels_info, const char *str_label, LBL_TYPE pos)
{
    LBL_TYPE label_id = AsmLabelGet(labels_info, str_label);
    labels_info->labels[label_id].pos = pos;
}

void AsmDoFixups(LabelsInfo *labels_info, char *buf)
{
    for (int32_t i = 0; i < labels_info->n_fixups; ++i)
    {
        LBL_TYPE label_id = labels_info->fixups[i].label_id;
        *((LBL_TYPE*) (buf + labels_info->fixups[i].pos)) = labels_info->labels[label_id].pos;
    }
}

void LabelsInfoCtor(LabelsInfo *labels_info)
{
    labels_info->n_labels = 0;
    labels_info->n_fixups = 0;
}

void LabelsInfoDtor(LabelsInfo *labels_info)
{

}

int32_t AsmArgProcess(const char *str, CMD_FLAGS_TYPE *flags, char *buf, int32_t *instr_ptr)
{
    *flags = 0;
    char arg[MAX_LINE_LEN] = "";
    int32_t offset = 0;

    sscanf(str, "%s%n", arg, &offset);

    bool is_ram = false;
    if (arg[0] == '[')
    {
        is_ram = true;
        *flags |= FLG_MEM;
        *strchr(arg, ']') = '\0';
    }

    bool     is_val = false,
             is_reg = false;
    char     arg_s[MAX_LINE_LEN] = "";
    VAL_TYPE arg_v = 0;

    char *plus_ptr = strchr(arg, '+');
    if (plus_ptr != NULL)
    {
        sscanf(arg + (is_ram ? 1 : 0), "%d+%s", &arg_v, arg_s);
        is_reg = true;
        is_val = true;
    }
    else
    {
        int is_num = sscanf(arg + (is_ram ? 1 : 0), "%d", &arg_v);
        if (is_num == 0)
        {
            sscanf(arg + (is_ram ? 1 : 0), "%s", arg_s);
            is_reg = true;
        }
        else
        {
            is_val = true;
        }
    }

    if (is_val)
    {
        *flags |= FLG_IMM;

        *((VAL_TYPE*) (buf + *instr_ptr)) = arg_v;
        *instr_ptr += BYTES_VAL;
    }

    if (is_reg)
    {
        *flags |= FLG_REG;

        REG_TYPE reg_id = AsmRegFind(arg_s);

        *((REG_TYPE*) (buf + *instr_ptr)) = reg_id;
        *instr_ptr += BYTES_REG;
    }

    return offset;
}

REG_TYPE AsmRegFind(const char *reg_name)
{
    for (int32_t i = 0; i < N_REG; ++i)
        if (strcasecmp(regs_name[i], reg_name) == 0)
            return i;

    return -1;
}

void AsmOut(Asm *asmbler, const char *filename)
{
    int fd_output = creat(filename, S_IRWXU);

    write(fd_output, (void*) &asmbler->instr_ptr, BYTES_SIZE);
    write(fd_output, (void*)  asmbler->buf,       asmbler->instr_ptr);

    close(fd_output);
}
