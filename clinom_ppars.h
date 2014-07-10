/*****************************************************************
*_TITLE clinom_ppars.h 
*
*_DESC  Common block defining the photometric behavior of the
*       image(s)
*
*_HIST Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_PPARS_H
#define CLINOM_PPARS_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_ppars1_f_common {
  INT4 nps;        /* Number of input files to use in
                      photoclinometry (set to 1 because
                      from2 is ignored */
  INT4 mulps;      /* # of operations to evaluate photom funs */
  INT4 mulpsp;     /* # of operations to evaluate pho fun derivs */
  INT4 piopt;      /* If 0, then use Henyey-Greenstein function
                      If 1, then use Legendre function */
} CLINOM_PPARS1_F_COMMON;

extern CLINOM_PPARS1_F_COMMON     FTN_COMMON(ppars1);

static CLINOM_PPARS1_F_COMMON *C_PPARS1 = &FTN_COMMON(ppars1);

typedef struct clinom_ppars2_f_common {
  FLOAT4 pex;         /* Minnaert exponents for image(s) */
  FLOAT4 pex1;        /* Minnaert exponents minus 1 */
  FLOAT4 pl2;         /* Amount of lunar photometric behavior */
  FLOAT4 pl1;         /* Amount of Minnaert photometric behavior */
  FLOAT4 falpha[2];   /* Henyey-Greenstein f(alpha) */
  FLOAT4 twogam;      /* 2*SQRT(1-W) coeff in Hapke approx.
                         to H-function */
} CLINOM_PPARS2_F_COMMON;

extern CLINOM_PPARS2_F_COMMON     FTN_COMMON(ppars2);

static CLINOM_PPARS2_F_COMMON *C_PPARS2 = &FTN_COMMON(ppars2);

typedef struct clinom_ppars3_f_common {
  INT1 hapke;      /* Use Hapke function instead? */
} CLINOM_PPARS3_F_COMMON;

extern CLINOM_PPARS3_F_COMMON     FTN_COMMON(ppars3);

static CLINOM_PPARS3_F_COMMON *C_PPARS3 = &FTN_COMMON(ppars3);

typedef struct clinom_ppars4_f_common {
  CHAR phofunc[7];      /* Photometric function name */
} CLINOM_PPARS4_F_COMMON;

extern CLINOM_PPARS4_F_COMMON     FTN_COMMON(ppars4);

static CLINOM_PPARS4_F_COMMON *C_PPARS4 = &FTN_COMMON(ppars4);

typedef struct clinom_ppars5_f_common {
  FLOAT4 pwh;       /* Single scattering albedo of Hapke */
  FLOAT4 phg1;      /* Asymmetry parameter for Henyey-Greenstein
                       alternate phase function */
  FLOAT4 phg2;      /* Second asymmetry parameter for 2-lobed
                       Henyey-Greenstein alternate phase 
		       function */
  FLOAT4 phh;       /* Opposition surge width for Hapke */
  FLOAT4 pb0;       /* Opposition surge strength for Hapke */
  FLOAT4 ptheta;    /* Small scale surface roughness in degrees
                       for Hapke */
  FLOAT4 pbh;       /* First Legendre coefficient of phase 
                       function of a single particle */
  FLOAT4 pch;       /* Second Legendre coefficient of phase
                       function of a single particle */
} CLINOM_PPARS5_F_COMMON;

extern CLINOM_PPARS5_F_COMMON     FTN_COMMON(ppars5);

static CLINOM_PPARS5_F_COMMON *C_PPARS5 = &FTN_COMMON(ppars5);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
