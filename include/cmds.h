#ifndef CMDS_H
#define CMDS_H

#include <stdint.h>

extern const char* const REGS_NAME[];

#define CMD_DEF(name, arg, code) CMD_##name,
#define JMP_DEF(name, cond)      CMD_DEF(name,,)
enum CMDS
{
#include "cmd_def.h"
    N_CMD
};
#undef CMD_DEF
#undef JMP_DEF

#define REG_DEF(name) REG_##name,
enum REGS
{
#include "reg_def.h"
    N_REG
};
#undef REG_DEF

#define COMMA                 ,
#define CUR                   BUF[IP]
#define ARG(type)             (*((type##_TYPE*) &(CUR)))
#define INC(type)             IP += BYTES_##type
#define REG(name)             REGS[REG_##name]

#define FLAGS(flags)          ((flags) & FLAGS_MASK)
#define RET_INC_FLAGS(flags)  (flags) >> FLAGS_POS_OCCUP

#define PUSH_CALLSTK(val)     StackPush (&CALLSTK, val)
#define PUSH(val)             StackPush (&STK, val)
#define TOP                   StackTop  (&STK)
#define POP                   StackPop  (&STK)
#define POP_CALLSTK           StackPop  (&CALLSTK)

#define RFLAGS                REG(RFLAGS)

#define CF_IND                0
#define ZF_IND                1

#define CF                    ((RFLAGS >> CF_IND) & 1)
#define ZF                    ((RFLAGS >> ZF_IND) & 1)

#define GET_VAL(flags, var)     \
    if (flags & FLG_IMM)        \
    {                           \
        var += ARG(VAL);        \
        INC(VAL);               \
    }                           \
                                \
    if (flags & FLG_REG)        \
    {                           \
        var += REGS[ARG(REG)];  \
        INC(REG);               \
    }                           \
                                \
    if (flags & FLG_MEM)        \
    {                           \
        var = MEM[var];         \
    }

#define MAX_LABEL_LEN       64
#define MAX_N_LABELS        512
#define MAX_N_FIXUPS        512

#define MAX_PROG_SIZE       10000

#define BITS_SIZE           32
#define SIZE_TYPE           int32_t
#define BYTES_SIZE          (BITS_SIZE      / 8)

#define BITS_CMD            16
#define CMD_TYPE            int16_t
#define BYTES_CMD           (BITS_CMD       / 8) 

#define BITS_CMD_CONT       8 
#define CMD_CONT_TYPE       int8_t
#define BYTES_CMD_CONT      (BITS_CMD_CONT  / 8)

#define BITS_CMD_FLAGS      8 
#define CMD_FLAGS_TYPE      uint8_t
#define BYTES_CMD_FLAGS     (BITS_CMD_FLAGS / 8)

#define CMD_MASK            ((1ull << 8 * BYTES_CMD_CONT) - 1)
#define FLAGS_MASK          ((1ull << FLAGS_POS_OCCUP) - 1)

#define BITS_VAL            64
#define VAL_TYPE            int64_t
#define BYTES_VAL           (BITS_VAL       / 8)

#define BITS_LBL            32
#define LBL_TYPE            int32_t
#define BYTES_LBL           (BITS_LBL       / 8)

#define BITS_REG            8
#define REG_TYPE            int8_t
#define BYTES_REG           (BITS_REG       / 8)

#define FLAGS_POS_OCCUP     3

#define IMM_IND             0
#define FLG_IMM             (1ull << IMM_IND)

#define REG_IND             1
#define FLG_REG             (1ull << REG_IND)

#define MEM_IND             2
#define FLG_MEM             (1ull << MEM_IND)

#define ZERO_ARG            0
#define  ONE_ARG            1
#define  TWO_ARG            2
#define  LBL_ARG           -1

#endif  // CMDS_H
 
