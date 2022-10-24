#include "cmds.h"

#define REG_DEF(name) #name,
const char * const REGS_NAME[] = 
    {
#include "reg_def.h"
    };
#undef REG_DEF

