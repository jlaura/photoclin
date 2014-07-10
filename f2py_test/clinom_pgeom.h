/*****************************************************************
*_TITLE clinom_pgeom.h 
*
*_DESC  Common block defining the imaging geometry for the image(s)
*
*_HIST  Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_PGEOM_H
#define CLINOM_PGEOM_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_pgeom_f_common {
  FLOAT4 si1[2];    /* Coeff of 1st diag difference dz1 in
                       cos(incidence) for 1st, 2nd images */
  FLOAT4 si2[2];    /* Coeff of 2nd diag diff dz2 in cos(inc) */
  FLOAT4 ci[2];     /* Constant coeff in cos(incidence) */
  FLOAT4 se1[2];    /* As si1 but for cos(emission) */
  FLOAT4 se2[2];    /* As si2 but for cos(emission) */
  FLOAT4 ce[2];     /* As ci but for cos(emission) */
} CLINOM_PGEOM_F_COMMON;

extern CLINOM_PGEOM_F_COMMON     FTN_COMMON(pgeom);

static CLINOM_PGEOM_F_COMMON *C_PGEOM = &FTN_COMMON(pgeom);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
