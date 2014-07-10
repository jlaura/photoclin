/*****************************************************************
*_TITLE clinom_ops.h 
*
*_DESC  Common block to keep track of computational work
*
*_HIST Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_OPS_H
#define CLINOM_OPS_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_ops1_f_common {
  FLOAT4 nops;        /* Number of floating point operations */
} CLINOM_OPS1_F_COMMON;

extern CLINOM_OPS1_F_COMMON     FTN_COMMON(ops1);

static CLINOM_OPS1_F_COMMON *C_OPS1 = &FTN_COMMON(ops1);

typedef struct clinom_ops2_f_common {
  INT4 iguess;      /* Initial guess type */
} CLINOM_OPS2_F_COMMON;

extern CLINOM_OPS2_F_COMMON     FTN_COMMON(ops2);

static CLINOM_OPS2_F_COMMON *C_OPS2 = &FTN_COMMON(ops2);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
