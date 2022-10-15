#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "asm.h"
#include "cmds.h"

int32_t AsmFindLabel(const char *str_label, LabelsInfo *labels_info)
{
    char label_name[MAX_LABEL_LEN] = "";
    sscanf(str_label, "%s", label_name);

    for (int i = 0; i < MAX_N_LABELS; ++i)
        if (strcasecmp(labels_info->labels[i].name, label_name) == 0)
            return labels_info->labels[i].pos;
    return -1;
}

int32_t AsmGetLabel(LabelInfo *labels_info, const char *str_label)

int32_t AsmLabelProcess(const char* str_label, LabelsInfo *labels_info, char *buf)
{
    int32_t label_id = AsmFindLabel(str_label, labels_info);
    
    if (label_id != -1)
        return labels_info->labels[label_id].pos;

    AsmLabelUpd(str_label, -1);

    labels_info->fixups[labels_info->n_fixups] = 
        {
            labels_info->n_labels,
            *instr_ptr
        };

    *(int32_t*)(buf + *instr_ptr) = -1;

    ++labels_info->n_labels;
    ++labels_info->n_fixups;

    *instr_ptr += BYTES_LABEL;
}

void AsmLabelAdd(const char *label_name, LabelsInfo *labels_info)
{
    strcpy(labels_info->labels[labels_info->n_labels].name, label_name);
    labels_info->labels[labels_info->n_labels].pos = pos;
    ++labels_info->n_labels;
}

void AsmLabelUpd(LabelsInfo *labels_info, const char *label_name, int pos)
{
    int32_t label_id = AsmFindLabel(label_name, labels_info);

    if (label_id != -1)
        AsmLabelAdd(label_name, labels_info);
    else
        labels_info->labels[label_id].pos = pos
}

void AsmFixLabels(LabelsInfo *labels_info, char *buf)
{
    for (int i = 0; i < labels_info->n_fixups; ++i)
    {
        int label_id = labels_info->fixups[i].label_id;
        *((int32_t*) (buf + labels_info->fixups[i].pos)) = labels_info->labels[label_id].pos;
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

