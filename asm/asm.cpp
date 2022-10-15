#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "asm.h"
#include "cmds.h"

LBL_TYPE AsmLabelProcess(LabelsInfo *labels_info, const char* str_label, int32_t instr_ptr)
{
    LBL_TYPE label_id = AsmLabelGet(str_label, labels_info);

    LBL_TYPE ret_pos = labels_info->labels[label_id].pos;

    if (ret_pos == -1)
        AsmFixupAdd(labels_info, str_label, instr_ptr);

    return ret_pos;
}

LBL_TYPE AsmFixupAdd(LabelsInfo *labels_info, const char *str_label, int32_t instr_ptr)
{
    labels_info->fixups[labels_info->n_fixups] = 
        {
            labels_info->n_labels,
            *instr_ptr
        };

    ++labels_info->n_fixups;
}

LBL_TYPE AsmLabelFind(LabelsInfo *labels_info, const char *str_label)
{
    char label_name[MAX_LABEL_LEN] = "";
    sscanf(str_label, "%s", label_name);
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

LBL_TYPE AsmLabelGet(LabelInfo *labels_info, const char *str_label)
{
    LBL_TYPE label_id = AsmLabelFind(labels_info, str_label);

    if (label_id != -1)
        return label_id;

    AsmLabelAdd(labels_info, label_name, -1);
    return labels_info->n_labels - 1;
}

void AsmLabelUpd(LabelsInfo *labels_info, const char *str_label, LBL_TYPE pos)
{
    LBL_TYPE label_id = AsmGetLabel(str_label, labels_info);
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

VAL_TYPE AsmParseArg(const char *str, CMD_FLAGS_TYPE *flags)
{
    bool isRam = false;

    char arg[MAX_
    sscanf
}

