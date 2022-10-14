#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "asm.h"
#include "cmds.h"

void AsmLabelProcess(const char* str_label, LabelsInfo *labels_info, char *buf, int *instr_ptr)
{
    char label_name[MAX_LABEL_LEN] = "";
    sscanf(str_label, "%s", label_name);

    bool in_labels = false;
    for (int i = 0; i < MAX_N_LABELS; ++i)
        if (strcasecmp(labels_info->labels[i].name, label_name) == 0)
        {
            in_labels = true;
            *((int32_t*)(buf + *instr_ptr)) = labels_info->labels[i].pos;
            break;
        }

    if (!in_labels)
    {
        strcpy(labels_info->labels[labels_info->n_labels].name, label_name);
        labels_info->labels[labels_info->n_labels].pos = -1;

        labels_info->fixups[labels_info->n_fixups] = 
            {
                labels_info->n_labels,
                *instr_ptr
            };

        *(int32_t*)(buf + *instr_ptr) = -1;

        ++labels_info->n_labels;
        ++labels_info->n_fixups;
    }

    *instr_ptr += BYTES_LABEL;
}

void AsmLabelAdd(const char *label_name, int pos, LabelsInfo *labels_info)
{

    bool in_labels = false;
    for (int i = 0; i < labels_info->n_labels; ++i)
        if (strcasecmp(labels_info->labels[i].name, label_name) == 0)
        {
            in_labels = true;
            labels_info->labels[i].pos = pos;
            break;
        }

    if (!in_labels)
    {
        strcpy(labels_info->labels[labels_info->n_labels].name, label_name);
        labels_info->labels[labels_info->n_labels].pos = pos;

        ++labels_info->n_labels;
    }
}

void AsmFixLabels(char *buf, LabelsInfo *labels_info)
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

