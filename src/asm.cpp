#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "asm.h"
#include "cmds.h"
#include "iostr.h"
#include "colors.h"

const Option EXEC_OPTIONS[] = 
    {
        {"--file",        "-f",  INPUT_FILE_OPTION,       "from file (default: input.asm)"                          },
        {"--help",        "-h",  HELP_OPTION,             "show help"                                               },
        {"--output",      "-o",  OUTPUT_FILE_OPTION,      "output text to file (default: output.bin)"               },
        {"--listing",     "-l",  LISTING_FILE_OPTION,     "output listing of program to file (default: listing.txt)"}
    };
 
const size_t N_EXEC_OPTIONS = sizeof(EXEC_OPTIONS) / sizeof(Option);

#define CMD_DEF(name, arg, code) arg,
#define JMP_DEF(name, cond) LBL_ARG,
const int32_t ARG_TYPE[] =
    {
#include "cmd_def.h"
    };
#undef CMD_DEF
#undef JMP_DEF

void AsmCtor(Asm *asmbler, const char *filename, int *err)
{
    ASSERT(asmbler  != NULL);
    ASSERT(filename != NULL);
    ASSERT(err      != NULL);

    asmbler->instr_ptr = 0;

    memset(asmbler->buf, 0, MAX_PROG_SIZE);
    TextInfoCtor(&asmbler->text);

    InputText(&asmbler->text, filename, err);
    MarkOutText(&asmbler->text, err);

    LabelsInfoCtor(&asmbler->labels_info);
}

void AsmDtor(Asm *asmbler)
{
    ASSERT(asmbler != NULL);

    memset(asmbler->buf, -1, MAX_PROG_SIZE);

    TextInfoDtor   (&asmbler->text);
    LabelsInfoDtor (&asmbler->labels_info);
}

void AsmRun(Asm *asmbler)
{
    ASSERT(asmbler != NULL);

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

        if (strchr(text->lines[i].ptr, ';') != NULL)
            continue;

#define BUF buf
#define IP  *instr_ptr

#define CMD_DEF(name, arg, code)                                                                                  \
        if (strcasecmp(cmd, #name) == 0)                                                                          \
        {                                                                                                         \
            ARG(CMD) = CMD_##name;                                                                                \
            SIZE_TYPE instr_ptr_cmd = *instr_ptr;                                                                 \
            INC(CMD);                                                                                             \
                                                                                                                  \
            if (arg != ZERO_ARG)                                                                                  \
            {                                                                                                     \
                if (arg != LBL_ARG)                                                                               \
                {                                                                                                 \
                    for (int32_t j = 0; j < arg; ++j)                                                             \
                    {                                                                                             \
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
        }                                                                                                         \
        else

#define JMP_DEF(name, cond) CMD_DEF(name, LBL_ARG,)

        #include "cmd_def.h"

        if (cmd[offset - 1] == ':')
        {
            cmd[offset - 1] = '\0';
            AsmLabelUpd(labels_info, cmd, *instr_ptr);
        }
        else
        {
            printf(RED "Syntax error\n" NORMAL);
        }

#undef CMD_DEF

#undef IP
#undef BUF

    }

    AsmDoFixups(labels_info, buf);
}

LBL_TYPE AsmLabelProcess(LabelsInfo *labels_info, const char* str_label, int32_t instr_ptr)
{
    ASSERT(labels_info != NULL);
    ASSERT(str_label   != NULL);

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
    ASSERT(labels_info != NULL);

    labels_info->fixups[labels_info->n_fixups] = 
        {
            label_id,
            instr_ptr
        };

    ++labels_info->n_fixups;
}

LBL_TYPE AsmLabelFind(LabelsInfo *labels_info, const char *str_label)
{
    ASSERT(labels_info != NULL);

    char label_name[MAX_LABEL_LEN] = "";
    sscanf(str_label, "%s", label_name);

    for (int32_t i = 0; i < labels_info->n_labels; ++i)
        if (strcasecmp(labels_info->labels[i].name, label_name) == 0)
            return i;

    return NO_LABEL;
}

void AsmLabelAdd(LabelsInfo *labels_info, const char *str_label, LBL_TYPE pos)
{
    ASSERT(labels_info != NULL);
    ASSERT(str_label   != NULL);

    strcpy(labels_info->labels[labels_info->n_labels].name, str_label);
    labels_info->labels[labels_info->n_labels].pos = pos;
    ++labels_info->n_labels;
}

LBL_TYPE AsmLabelGet(LabelsInfo *labels_info, const char *label_name)
{
    ASSERT(labels_info != NULL);
    ASSERT(label_name  != NULL);

    LBL_TYPE label_id = AsmLabelFind(labels_info, label_name);

    if (label_id != NO_LABEL)
        return label_id;

    AsmLabelAdd(labels_info, label_name, NOT_INIT_LABEL);
    return labels_info->n_labels - 1;
}

void AsmLabelUpd(LabelsInfo *labels_info, const char *str_label, LBL_TYPE pos)
{
    ASSERT(labels_info != NULL);
    ASSERT(str_label   != NULL);

    LBL_TYPE label_id = AsmLabelGet(labels_info, str_label);
    labels_info->labels[label_id].pos = pos;
}

void AsmDoFixups(LabelsInfo *labels_info, char *buf)
{
    ASSERT(labels_info != NULL);
    ASSERT(buf         != NULL);

    for (int32_t i = 0; i < labels_info->n_fixups; ++i)
    {
        LBL_TYPE label_id = labels_info->fixups[i].label_id;
        *((LBL_TYPE*) (buf + labels_info->fixups[i].pos)) = labels_info->labels[label_id].pos;
    }
}

void LabelsInfoCtor(LabelsInfo *labels_info)
{
    ASSERT(labels_info != NULL);

    labels_info->n_labels = 0;
    labels_info->n_fixups = 0;
}

void LabelsInfoDtor(LabelsInfo *labels_info)
{
    ASSERT(labels_info != NULL);
}

int32_t AsmArgProcess(const char *str, CMD_FLAGS_TYPE *flags, char *buf, int32_t *instr_ptr)
{
    ASSERT(str       != NULL);
    ASSERT(flags     != NULL);
    ASSERT(buf       != NULL);
    ASSERT(instr_ptr != NULL);

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
        sscanf(arg + (is_ram ? 1 : 0), "%ld+%s", &arg_v, arg_s);
        is_reg = true;
        is_val = true;
    }
    else
    {
        int is_num = sscanf(arg + (is_ram ? 1 : 0), "%ld", &arg_v);
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
    ASSERT(reg_name != NULL);

    for (int32_t i = 0; i < N_REG; ++i)
        if (strcasecmp(REGS_NAME[i], reg_name) == 0)
            return i;

    return -1;
}

void AsmOut(Asm *asmbler, const char *filename)
{
    ASSERT(asmbler  != NULL);
    ASSERT(filename != NULL);

    int fd_output = creat(filename, S_IRWXU);

    write(fd_output, (void*) &asmbler->instr_ptr, BYTES_SIZE);
    write(fd_output, (void*)  asmbler->buf,       asmbler->instr_ptr);

    close(fd_output);
}

#define LIST_ARG(type)                                                                                             \
    do {                                                                                                           \
        sprintf(list_line + list_line_offset, "%0*x ", BYTES_##type * 2, *((type##_TYPE*) (buf + *instr_ptr_cmd))); \
        *instr_ptr_cmd    += BYTES_##type;                                                                          \
        list_line[list_line_offset + BYTES_##type * 2 + 1] = ' ';                                                  \
    } while (0)

#define LIST_INC(type) list_line_offset += BYTES_##type * 2 + 1

int32_t AsmLineList(char buf[], const char cmd[], int *instr_ptr_cmd, char list_line[], int arg)
{
    ASSERT(buf       != NULL);
    ASSERT(cmd       != NULL);
    ASSERT(list_line != NULL);

    int32_t list_line_offset = 0;
    memset(list_line, ' ', 2 * MAX_LINE_LEN + 1);

    sprintf(list_line, "%0*x | ", BYTES_SIZE * 2, *instr_ptr_cmd);
    LIST_INC(SIZE);
    list_line_offset += 2; // |\s

    LIST_ARG(CMD_CONT);
    LIST_INC(CMD_CONT);
    CMD_FLAGS_TYPE flags = *((CMD_FLAGS_TYPE*) (buf + *instr_ptr_cmd));

    LIST_ARG(CMD_FLAGS);
    LIST_INC(CMD_FLAGS);

    if (arg == LBL_ARG)
        LIST_ARG(LBL);
    LIST_INC(LBL);

    for (int i = 0; i < TWO_ARG; ++i)
    {
        if ((flags >> (FLAGS_POS_OCCUP * i)) & FLG_IMM)
            LIST_ARG(VAL);
        LIST_INC(VAL);

        if ((flags >> (FLAGS_POS_OCCUP * i)) & FLG_REG)
            LIST_ARG(REG);
        LIST_INC(REG);
    }

    int32_t cmd_offset = 0;
    sprintf(list_line + list_line_offset, "| %s\n%n", cmd, &cmd_offset);

    return list_line_offset + cmd_offset;
}

#define BUF buf
#define IP  instr_ptr_cmd

void AsmListing(Asm *asmbler, const char *filename)
{
    ASSERT(asmbler != NULL);
    ASSERT(filename != NULL);

    TextInfo *text = &asmbler->text;
    char     *buf  =  asmbler->buf;

    int fd_inp = creat(filename, S_IRWXU);
    ASSERT(fd_inp != -1);

    int32_t instr_ptr_cmd = 0;
    for (int i = 0; i < text->nlines; ++i)
    {
        if (text->lines[i].ptr[text->lines[i].len - 1] == ':' || strchr(text->lines[i].ptr, ';') != NULL)
        {
            dprintf(fd_inp, "%s\n", text->lines[i].ptr);
            continue;
        }
        char list_line[2 * MAX_LINE_LEN + 1] = "";

        int32_t len = AsmLineList(buf, text->lines[i].ptr, &instr_ptr_cmd, list_line, ARG_TYPE[ARG(CMD_CONT)]);

        write(fd_inp, list_line, len);
    }

    close(fd_inp);
}

#undef BUF
#undef IP
