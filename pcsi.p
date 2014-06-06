/* Prototypes for the pcsi routines */

#ifndef PCSI_P
#define PCSI_P

#include "isistypes.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

extern void xctae_files (CHAR zin[],INT4 zin_len,INT4 *lzin,BOOLEAN *usezin,
			 BOOLEAN *usedatum,CHAR logfile[],INT4 logfile_len,
			 INT4 *llog,CHAR note[],INT4 note_len,INT4 *lnote);

extern void geomset (FLOAT4 fmag);

extern void pbder (INT4 i,INT4 j,FLOAT4 dz1,FLOAT4 dz2,FLOAT4 *be,
                   FLOAT4 *db1,FLOAT4 *db2,INT4 mode);

extern void findbmax (void);

extern FLOAT4 zbrent(INT4 *fn,FLOAT4 x1,FLOAT4 x2,FLOAT4 tol);

extern FLOAT4 ddberr(FLOAT4 *delazin);

extern void xctae_ipars(INT4 *nsmooth);

extern void write2log(INT4 flg,INT4 ins,INT4 inl,CHAR note[],INT4 lnote,
                      CHAR from[],FLOAT4 clinc,FLOAT8 azinc,FLOAT8 clemi,
		      FLOAT8 azemi,FLOAT8 res,FLOAT4 dip,
		      FLOAT4 az,FLOAT4 charazgrid,FLOAT4 radius,FLOAT4 dndatum,
		      FLOAT4 aspect);

extern void pblinein1(FLOAT4 ibuf[],INT4 ins,FLOAT4 cdum[],INT4 nx,INT4 ny,
                      INT4 il,FLOAT4 dnatm,INT4 ioct);

extern void pbscale(FLOAT4 b[],FLOAT4 work[],INT4 nx,INT4 ny,FLOAT4 dndatum);

extern void chk4inv(FLOAT4 datbuf[],INT4 nx,INT4 ny,INT4 *invcnt);

extern void xcsep(INT4 cbuf[],INT4 nmax,INT4 n1,INT4 n2,INT4 n3,INT4 n4,INT4 n5,
                  INT4 nx,INT4 ny,INT4 fn);

extern void xcmain_pcsi(INT4 idum[],INT4 n1,INT4 nx,INT4 ny,INT1 dozout,
                        INT4 *nsmooth,INT4 sordir,INT4 *ret);

extern void zlineout(FLOAT4 ibuf[],INT4 nx,INT4 ny,FLOAT4 obuf[],INT4 ins,
                      INT4 il,FLOAT4 scale,INT4 ioct,FLOAT4 cosemi);

extern void datum (INT4 i,INT4 j,INT4 npts,FLOAT4 *z01,FLOAT4 *z02,
                   FLOAT4 *z03,FLOAT4 *z04);

extern void zinterp(FLOAT4 zin[],FLOAT4 zout[],INT4 nx,INT4 ny,FLOAT4 scale,
                    FLOAT4 cosemi,INT4 *ret);

extern void writezout (void);

extern void pcsi_ipars(INT4 *nsmooth);

#ifdef __cplusplus
}
#endif  /*  __cplusplus  */
#endif
