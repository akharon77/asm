#ifndef ASM_H
#define ASM_H

#include "cmds.h"

struct Label
{
    char      name[MAX_LABEL_LEN];
    LBL_TYPE  pos;
};

struct Fixup
{
    int32_t  label_id;
    LBL_TYPE pos;
};

struct LabelsInfo
{
    int32_t n_labels;
    int32_t n_fixups;

    Label   labels[MAX_N_LABELS];
    Fixup   fixups[MAX_N_FIXUPS];
};

LBL_TYPE AsmLabelProcess (LabelsInfo *labels_info, const char* str_label, int32_t instr_ptr);
int32_t     AsmArgProcess   (const char *str, CMD_FLAGS_TYPE *flags, char *buf, int32_t *instr_ptr);

void     AsmDoFixups     (LabelsInfo *labels_info, char *buf);
void     AsmFixupAdd     (LabelsInfo *labels_info, LBL_TYPE label_id, int32_t instr_ptr);

LBL_TYPE AsmLabelGet     (LabelsInfo *labels_info, const char *label_name);
void     AsmLabelUpd     (LabelsInfo *labels_info, const char *str_label, LBL_TYPE pos);
LBL_TYPE AsmLabelFind    (LabelsInfo *labels_info, const char *str_label);
void     AsmLabelAdd     (LabelsInfo *labels_info, const char *str_label, LBL_TYPE pos);

REG_TYPE AsmRegFind(const char *reg_name);

void     LabelsInfoCtor  (LabelsInfo *labels_info);
void     LabelsInfoDtor  (LabelsInfo *labels_info);

#endif  // ASM_H
