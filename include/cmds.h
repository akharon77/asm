#ifndef CMDS_H
#define CMDS_H

enum CMDS
{
    CMD_PUSH = 1,
    CMD_POP,
    CMD_ADD,
    CMD_SUB,
    CMD_MUL,
    CMD_DIV,
    CMD_OUT,
    CMD_HLT,
    CMD_JB,
    CMD_JBE,
    CMD_JA,
    CMD_JAE,
    CMD_JE,
    CMD_JNE,
    CMD_N
};

const uint32_t BYTES_SIZE      = 4;
const uint32_t BYTES_COMMAND   = 2;
const uint32_t BYTES_VAL       = 4;
const uint32_t BYTES_LABEL     = 4;
const uint32_t BYTES_COM_FLAGS = 1;
const uint32_t BYTES_COM_CONT  = 1;

const uint32_t IMM_IND         = 0;
const uint32_t REG_IND         = 1;
const uint32_t MEM_IND         = 2;

const uint32_t COM_MASK        = 1 << (8 * BYTES_COM_CONT) - 1;

const uint32_t COM_IMM         = 1 << (8 * BYTES_COM_CONT + IMM_IND);
const uint32_t COM_REG         = 1 << (8 * BYTES_COM_CONT + REG_IND);
const uint32_t COM_MEM         = 1 << (8 * BYTES_COM_CONT + MEM_IND);

#endif  // CMDS_H
 
