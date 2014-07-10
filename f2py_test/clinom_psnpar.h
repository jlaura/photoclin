/*****************************************************************
*_TITLE clinom_psnpar.h 
*
*_DESC  Common block hiding phase angle for psneg, dpsneg
*
*_HIST Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_PSNPAR_H
#define CLINOM_PSNPAR_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_psnpar_f_common {
  FLOAT4 phase[2]; /* Phase angles for image(s) */
} CLINOM_PSNPAR_F_COMMON;

extern CLINOM_PSNPAR_F_COMMON     FTN_COMMON(psnpar);

static CLINOM_PSNPAR_F_COMMON *C_PSNPAR = &FTN_COMMON(psnpar);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
