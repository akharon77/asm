#ifndef ASM_H
#define ASM_H

const int MAX_PROG_SIZE = 1024,
          MAX_N_LABELS  = 64,
          MAX_N_FIXUPS  = 128,
          MAX_LABEL_LEN = 16;

struct Label
{
    char name[MAX_LABEL_LEN];
    int  pos;
};

struct Fixup
{
    int label_id;
    int pos;
};

struct LabelsInfo
{
    int   n_labels,
          n_fixups;

    Label labels[MAX_N_LABELS];
    Fixup fixups[MAX_N_FIXUPS];
};

void AsmLabelProcess (const char* str_label, LabelsInfo *labels_info, char *buf, int *instr_ptr);
void AsmLabelAdd     (const char *label_name, int pos, LabelsInfo *labels_info);
void AsmFixLabels(char *buf, LabelsInfo *labels_info);

void LabelsInfoCtor (LabelsInfo *labels_info);
void LabelsInfoDtor (LabelsInfo *labels_info);

#endif  // ASM_H
