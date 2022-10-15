#ifndef CMDS_H
#define CMDS_H

#include <stdint.h>

#define CMD_DEF(name) CMD_##name
enum CMDS
{
#include "cmd_def.h"
};
#undef CMD_DEF

#define BITS_CMD       16  // 2 * 8
#define BITS_CMD_FLAGS 8   // 1 * 8
#define BITS_CMD_CONT  8   // 1 * 8
#define BITS_VAL       32  // 4 * 8
#define BITS_LBL       32  // 4 * 8
#define BITS_SIZE      32  // 4 * 8

#define CMD_TYPE         int##BITS_CMD##_t
#define BYTES_CMD        sizeof(CMD_TYPE)

#define CMD_CONT_TYPE    int##BITS_CMD_CONT##_t
#define BYTES_CMD_CONT   sizeof(CMD_CONT_TYPE)
#define CMD_MASK         ((1ull << 8 * BYTES_CMD_CONT) - 1)

#define CMD_FLAGS_TYPE   int##BITS_CMD_FLAGS##_t
#define BYTES_CMD_FLAGS  sizeof(CMD_FLAGS_TYPE)

#define VAL_TYPE         int##BITS_VAL##_t
#define BYTES_VAL        sizeof(VAL_TYPE)

#define LBL_TYPE         int##BITS_LBL##_t
#define BYTES_LBL        sizeof(LBL_TYPE)

#define IMM_IND          0
#define CMD_IMM          (1ull << (8 * BYTES_CMD_CONT + IMM_IND))

#define REG_IND          1
#define CMD_REG          (1ull << (8 * BYTES_CMD_CONT + REG_IND))

#define MEM_IND          2
#define CMD_MEM          (1ull << (8 * BYTES_CMD_CONT + MEM_IND))


#endif  // CMDS_H
 
