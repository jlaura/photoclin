/* Prototypes for the Spice routines */

#ifndef CHARDIP_P
#define CHARDIP_P

#include "isistypes.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

extern void geomset (FLOAT4 fmag);

extern void pbder (INT4 i,INT4 j,FLOAT4 dz1,FLOAT4 dz2,FLOAT4 *be,
                   FLOAT4 *db1,FLOAT4 *db2,INT4 mode);

extern void findbmax (void);

extern FLOAT4 zbrent(INT4 *fn,FLOAT4 x1,FLOAT4 x2,FLOAT4 tol);

extern FLOAT4 ddberr(FLOAT4 *delazin);

extern void zlineout(FLOAT4 ibuf[],INT4 nx,INT4 ny,FLOAT4 obuf[],INT4 ins,
                      INT4 il,FLOAT4 scale,INT4 ioct,FLOAT4 cosemi);

extern void datum (INT4 i,INT4 j,INT4 npts,FLOAT4 *z01,FLOAT4 *z02,
                   FLOAT4 *z03,FLOAT4 *z04);

#ifdef __cplusplus
}
#endif  /*  __cplusplus  */
#endif
