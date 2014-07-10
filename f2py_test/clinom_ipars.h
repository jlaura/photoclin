/*********************************************************************
*_TITLE clinom_ipars.h 
*
*_DESC  Common block with parameters controlling iteration
*
*_HIST Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
*      Jul 31 2002  RK, Added ALASYM,ALRAT,EXPERT
**********************************************************************/
#ifndef CLINOM_IPARS_H
#define CLINOM_IPARS_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_ipars1_f_common {
  FLOAT4 al;         /* Inverse of penalty # */
  FLOAT4 contrast;   /* Decrease brightness error by this much */
  FLOAT4 wmax;       /* Asymtotic weight for SOR */
  FLOAT4 divtol;     /* (Max Z change)/(RMS Z change) value that
                        signals divergence at a point (Note that
			a global maximum is tested so we know IF
			divergence occurred, but not WHERE) */
  FLOAT4 ddztol;     /* Relinearize when each SOR step is less
                        than this fraction of the total Z change */
  FLOAT4 etol;       /* RMS gradient (righthand side) value that
                        indicates solution has converged, so stop */
  FLOAT4 bigtol;
  FLOAT4 oldtol;
  FLOAT4 std[2];
  FLOAT4 cpulimit;
  FLOAT4 weight0;
  FLOAT4 taufac;     /* Fraction of truncation error to achieve for
                        convergence */
  FLOAT4 alasym;     /* Asymptotic value of AL as iteration proceeds */
  FLOAT4 alrat;      /* Controls rate of approach of AL to ALASYM */
} CLINOM_IPARS1_F_COMMON;

extern CLINOM_IPARS1_F_COMMON     FTN_COMMON(ipars1);

static CLINOM_IPARS1_F_COMMON *C_IPARS1 = &FTN_COMMON(ipars1);

typedef struct clinom_ipars2_f_common {
  INT4 itmax;
  INT4 depthlim;
  INT4 nds[2];
  INT4 maxpass;
  INT4 maxnr;
} CLINOM_IPARS2_F_COMMON;

extern CLINOM_IPARS2_F_COMMON     FTN_COMMON(ipars2);

static CLINOM_IPARS2_F_COMMON *C_IPARS2 = &FTN_COMMON(ipars2);

typedef struct clinom_ipars3_f_common {
  CHAR apofile[80];
} CLINOM_IPARS3_F_COMMON;

extern CLINOM_IPARS3_F_COMMON     FTN_COMMON(ipars3);

static CLINOM_IPARS3_F_COMMON *C_IPARS3 = &FTN_COMMON(ipars3);

typedef struct clinom_ipars4_f_common {
  INT1 expert;
} CLINOM_IPARS4_F_COMMON;

extern CLINOM_IPARS4_F_COMMON     FTN_COMMON(ipars4);

static CLINOM_IPARS4_F_COMMON *C_IPARS4 = &FTN_COMMON(ipars4);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
