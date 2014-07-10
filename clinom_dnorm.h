/*****************************************************************
*_TITLE clinom_dnorm.h 
*
*_DESC  Common block containing image normalization and pedestal 
*       parameters
*
*_HIST  Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_DNORM_H
#define CLINOM_DNORM_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_dnorm_f_common {
  FLOAT4 dnnorm;   /* DN value on datum, once DNATM is subtracted
                      ('FLAT FIELD' - 'HAZE') */
  FLOAT4 dnatm;    /* DN value contributed by atmosphere or
                      instrument pedestal ('HAZE') */
} CLINOM_DNORM_F_COMMON;

extern CLINOM_DNORM_F_COMMON     FTN_COMMON(dnorm);

static CLINOM_DNORM_F_COMMON *C_DNORM = &FTN_COMMON(dnorm);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
