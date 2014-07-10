/*****************************************************************
*_TITLE clinom_aepar.h 
*
*_DESC  Contains parameters needed by ae*calc routines; these
*       parameters apply to both photoclinometry and radarclinometry.
*
*_HIST Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
*      Jul 29 2002  RK, Added ioct
**********************************************************************/
#ifndef CLINOM_AEPAR_H
#define CLINOM_AEPAR_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_aepar1_f_common {
  FLOAT4 bnorm;  /* Normalization of photometric or backscatter
                    function: photometric fn at datum
		    incidence/emission angles, or backscatter at
		    center incidence angle i times tan(i) area
		    factor */
  FLOAT4 bmax;   /* Maximum possible value of photometric fn or
                    backscatter * pixel area factor */
  FLOAT4 bnull;  /* Internal flag value to mark null pixels (which
                    contrib. only roughness terms to gradient and
		    Hessian).  Also, for PC, all image DN's mapping
		    to larger values than this are flagged by this
		    value */
  FLOAT4 bclip;  /* All image DN's mapping to larger values than this
                    (but not to nulls) will be scaled to this value */
  FLOAT4 bclip0; /* Level at which to clip brightness in biscopic
                    SSIPSF-PI PC to avoid slopes > those at which the
		    nonlinear brightness becomes infinite (shadow in
		    denominator) */
  FLOAT4 aspect; /* */
  FLOAT4 delaz;  /* */
  FLOAT4 dbrat;  /* */
  FLOAT4 charaz; /* Azimuth of characteristics, Cartesian */
} CLINOM_AEPAR1_F_COMMON;

extern CLINOM_AEPAR1_F_COMMON     FTN_COMMON(aepar1);

static CLINOM_AEPAR1_F_COMMON *C_AEPAR1 = &FTN_COMMON(aepar1);

typedef struct clinom_aepar2_f_common {
  INT1 flop;   /* */
  INT1 logimg; /* */
} CLINOM_AEPAR2_F_COMMON;

extern CLINOM_AEPAR2_F_COMMON     FTN_COMMON(aepar2);

static CLINOM_AEPAR2_F_COMMON *C_AEPAR2 = &FTN_COMMON(aepar2);

typedef struct clinom_aepar3_f_common {
  INT4 ioct;   /* Octant containing azimuth of characteristics:
                  -2=-90 to -45 deg, -1=-45 to 0, 1=0 to 45,
		  2=45 to 90 */
} CLINOM_AEPAR3_F_COMMON;

extern CLINOM_AEPAR3_F_COMMON     FTN_COMMON(aepar3);

static CLINOM_AEPAR3_F_COMMON *C_AEPAR3 = &FTN_COMMON(aepar3);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
