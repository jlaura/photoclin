/* Prototypes for the Spice routines */

#ifndef PCP_P
#define PCP_P

#include "isistypes.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

extern void xctae_files (CHAR zin[],INT4 zin_len,INT4 *lzin,BOOLEAN *usezin,
			 BOOLEAN *usedatum,CHAR logfile[],INT4 logfile_len,
			 INT4 *llog,CHAR note[],INT4 note_len,INT4 *lnote);

extern void geomset (FLOAT4 fmag);

extern void phoxfm (FLOAT4 clinc[],FLOAT8 azinc[],FLOAT8 clemi[],
                    FLOAT8 azemi[],FLOAT8 aznor[],INT4 mapped);

extern void pbder (INT4 i,INT4 j,FLOAT4 dz1,FLOAT4 dz2,FLOAT4 *be,
                   FLOAT4 *db1,FLOAT4 *db2,INT4 mode);

extern void findbmax (void);

extern void zbrent_int (INT4 *fn,FLOAT4 x1);

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

extern void fit2dip(FLOAT4 buf[],INT4 nx1,INT4 ny1,FLOAT4 dnscl,FLOAT4 pixht,
                    FLOAT4 aspect,FLOAT4 sunaz,FLOAT4 charaz,FLOAT4 *dip,
		    FLOAT4 *dipaz,FLOAT4 *sundip,FLOAT4 *rmsdn,
		    FLOAT4 *rmspix,FLOAT4 *rmsm);

extern void xcsto(CHAR zin[],FLOAT4 z[],INT4 nx,INT4 ny,FLOAT4 zbuf[],
                  INT4 ins,INT4 inl,INT4 ioct,INT4 *ret);

extern void chk4inv(FLOAT4 datbuf[],INT4 nx,INT4 ny,INT4 *invcnt);

extern void xcsep(INT4 cbuf[],INT4 nmax,INT4 n1,INT4 n2,INT4 n3,INT4 n4,INT4 n5,
                  INT4 nx,INT4 ny,INT4 fn);

extern void CBtoolPcpPB (Widget id,XtPointer client_data,XtPointer call_data);

extern void pcpmessage (CHAR msg[]);

extern void xcmain_gui(INT4 idum[],INT4 n1,INT4 nx,INT4 ny,INT4 iaction,
                      INT4 sordir,BOOLEAN *aeknown,INT4 *n1lmag,
		      INT4 *n2lmag,INT4 *n4lmag,INT4 *nxlmag,INT4 *nylmag,
		      BOOLEAN *wrtimg,INT4 *mag,INT4 *minmag,
		      FLOAT8 res,FLOAT8 azinc,BOOLEAN fastinj,INT4 *ret); 

extern void CreateParamOptionsDB();

extern void CreateDoneOptionsDB();

extern void pcplog (CHAR msg[],INT4 dest);

extern void hinterror (CHAR msg1[],CHAR msg2[],INT4 dest);

extern void zlineout(FLOAT4 ibuf[],INT4 nx,INT4 ny,FLOAT4 obuf[],INT4 ins,
                      INT4 il,FLOAT4 scale,INT4 ioct,FLOAT4 cosemi);

extern void synthetic(CHAR imgnam[],FLOAT4 img[],FLOAT4 z[],INT4 nx,INT4 ny,
                  FLOAT4 dnd,FLOAT4 dna,FLOAT4 fmag,INT4 fid);

extern void synthdiff(CHAR difnam[],CHAR synnam[],FLOAT4 img[],FLOAT4 work[],
                      INT4 nx,INT4 ny,FLOAT4 fmag,INT4 fid);

extern void datum (INT4 i,INT4 j,INT4 npts,FLOAT4 *z01,FLOAT4 *z02,
                   FLOAT4 *z03,FLOAT4 *z04);

extern void zinterp(FLOAT4 zin[],FLOAT4 zout[],INT4 nx,INT4 ny,FLOAT4 scale,
                    FLOAT4 cosemi,INT4 *ret);

#ifdef __cplusplus
}
#endif  /*  __cplusplus  */
#endif
