#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "asm.h"
#include "cmds.h"
#include "iostr.h"

#define REG_DEF(name) #name,
const char *regs[N_REG] = 
    {
#include "reg_def.h"
    };
#undef REG_DEF

LBL_TYPE AsmLabelProcess(LabelsInfo *labels_info, const char* str_label, int32_t instr_ptr)
{
    LBL_TYPE label_id = AsmLabelGet(labels_info, str_label);

    LBL_TYPE ret_pos = labels_info->labels[label_id].pos;

    if (ret_pos == -1)
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
            return labels_info->labels[i].pos;

    return -1;
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

    if (label_id != -1)
        return label_id;

    AsmLabelAdd(labels_info, label_name, -1);
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
        *flags |= CMD_MEM;
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
        *flags |= CMD_IMM;

        *((VAL_TYPE*) (buf + *instr_ptr)) = arg_v;
        *instr_ptr += BYTES_VAL;
    }

    if (is_reg)
    {
        *flags |= CMD_REG;

        REG_TYPE reg_id = AsmRegFind(arg_s);

        *((REG_TYPE*) (buf + *instr_ptr)) = reg_id;
        *instr_ptr += BYTES_REG;
    }

    return offset;
}

REG_TYPE AsmRegFind(const char *reg_name)
{
    for (int32_t i = 0; i < N_REG; ++i)
        if (strcasecmp(regs[i], reg_name) == 0)
            return i;

    return -1;
}
