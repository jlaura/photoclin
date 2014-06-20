#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binding.h"
#include "isistypes.h"

void CC_NAME(xctae_files) (CHAR zin[],INT4 zin_len,INT4 *lzin,BOOLEAN *usezin,
			   BOOLEAN *usedatum,CHAR logfile[],INT4 logfile_len,
			   INT4 *llog,CHAR note[],INT4 note_len,INT4 *lnote)
{
  CHAR *zin_mem;
  CHAR *logfile_mem;
  CHAR *note_mem;
  extern void FTN_NAME(xctae_files) (CHAR zin[],INT4 *lzin,BOOLEAN *usezin,
				     BOOLEAN *usedatum,CHAR logfile[],INT4 *llog,
				     CHAR note[],INT4 *lnote,INT4 zin_len,
				     INT4 logfile_len,INT4 note_len);

  zin_mem = b_c2fstr (1,NULL,zin_len,NULL,(zin_len-1),B_RETAIN_PAD);
  logfile_mem = b_c2fstr (1,NULL,logfile_len,NULL,(logfile_len-1),B_RETAIN_PAD);
  note_mem = b_c2fstr (1,NULL,note_len,NULL,(note_len-1),B_RETAIN_PAD);
  (void) FTN_NAME(xctae_files) (zin_mem,lzin,usezin,usedatum,
			       logfile_mem,llog,note_mem,lnote,
                               B_STRING_MAX(zin_len-1),
                               B_STRING_MAX(logfile_len-1),
                               B_STRING_MAX(note_len-1));
  (void) b_f2cstr (1, zin_mem,(zin_len-1),B_RETAIN_PAD,zin,zin_len);
  (void) b_f2cstr (1, logfile_mem,(logfile_len-1),B_RETAIN_PAD,logfile,logfile_len);
  (void) b_f2cstr (1, note_mem,(note_len-1),B_RETAIN_PAD,note,note_len);
  (void) b_free(zin_mem);
  (void) b_free(logfile_mem);
  (void) b_free(note_mem);
  return;
}

void CC_NAME(geomset) (FLOAT4 fmag)
{
  extern void FTN_NAME(geomset) (FLOAT4 *fmag);

  (void) FTN_NAME(geomset) (&fmag);

  return;
}

void CC_NAME(phoxfm) (FLOAT4 clinc[],FLOAT8 azinc[],FLOAT8 clemi[],
                      FLOAT8 azemi[],FLOAT8 aznor[],INT4 mapped)
{
  extern void FTN_NAME(phoxfm) (FLOAT4 clinc[],FLOAT8 azinc[],
                      FLOAT8 clemi[],FLOAT8 azemi[],FLOAT8 aznor[],
		      INT4 *mapped);

  (void) FTN_NAME(phoxfm) (clinc,azinc,clemi,azemi,aznor,&mapped);

  return;
}

void CC_NAME(pbder) (INT4 i,INT4 j,FLOAT4 dz1,FLOAT4 dz2,FLOAT4 *be,
                     FLOAT4 *db1,FLOAT4 *db2,INT4 mode)
{
  extern void FTN_NAME(pbder) (INT4 *i,INT4 *j,FLOAT4 *dz1,FLOAT4 *dz2,
                     FLOAT4 *be,FLOAT4 *db1,FLOAT4 *db2,INT4 *mode);

  (void) FTN_NAME(pbder) (&i,&j,&dz1,&dz2,be,db1,db2,&mode);

  return;
}

void CC_NAME(findbmax) ()
{
  extern void FTN_NAME(findbmax) (void);

  (void) FTN_NAME(findbmax) ();

  return;
}

void CC_NAME(zbrent_int) (INT4 *fn,FLOAT4 x1)
{
  extern void FTN_NAME(zbrent_int) (INT4 *fn,FLOAT4 *x1);

  (void) FTN_NAME(zbrent_int) (fn,&x1);

  return;
}

FLOAT4 CC_NAME(ddberr) (FLOAT4 *delazin)
{
  extern FLOAT4 FTN_NAME(ddberr) (FLOAT4 *delazin);

  return (FTN_NAME(ddberr) (delazin));
}

void CC_NAME(xctae_ipars) (INT4 *nsmooth)
{
  extern void FTN_NAME(xctae_ipars) (INT4 *nsmooth);

  (void) FTN_NAME(xctae_ipars) (nsmooth);

  return;
}

void CC_NAME(write2log) (INT4 flg,INT4 ins,INT4 inl,CHAR note[],INT4 lnote,
                         CHAR from[],FLOAT4 clinc,FLOAT8 azinc,FLOAT8 clemi,
			 FLOAT8 azemi,FLOAT8 res,FLOAT4 dip,
			 FLOAT4 az,FLOAT4 charazgrid,FLOAT4 radius,
			 FLOAT4 dndatum,FLOAT4 aspect)
{
  extern void FTN_NAME(write2log) (INT4 *flg,INT4 *ins,INT4 *inl,CHAR note[],
                                   INT4 *lnote,CHAR from[],FLOAT4 *clinc,
				   FLOAT8 *azinc,FLOAT8 *clemi,FLOAT8 *azemi,
				   FLOAT8 *res,FLOAT4 *dip,
				   FLOAT4 *az,FLOAT4 *charazgrid,FLOAT4 *radius,
				   FLOAT4 *dndatum,FLOAT4 *aspect,
				   INT4 note_len,INT4 from_len);

  CHAR *note_mem;
  CHAR *from_mem;
  INT4 note_len = (INT4) strlen(note);
  INT4 from_len = (INT4) strlen(from);

  note_mem = b_c2fstr (1,note,note_len,NULL,(note_len),B_DISCARD_PAD);
  from_mem = b_c2fstr (1,from,from_len,NULL,(from_len),B_DISCARD_PAD);

  (void) FTN_NAME(write2log) (&flg,&ins,&inl,note_mem,&lnote,from_mem,
                              &clinc,&azinc,&clemi,&azemi,&res,&dip,
			      &az,&charazgrid,&radius,&dndatum,
			      &aspect,note_len,from_len);
  (void) b_free(note_mem);
  (void) b_free(from_mem);

  return;
}

void CC_NAME(pblinein1) (FLOAT4 ibuf[],INT4 ins,FLOAT4 cdum[],INT4 nx,
                         INT4 ny,INT4 il,FLOAT4 dnatm,INT4 ioct)
{
  extern void FTN_NAME(pblinein1) (FLOAT4 ibuf[],INT4 *ins,FLOAT4 cdum[],
                                   INT4 *nx,INT4 *ny,INT4 *il,FLOAT4 *dnatm,
				   INT4 *ioct);

  (void) FTN_NAME(pblinein1) (ibuf,&ins,cdum,&nx,&ny,&il,&dnatm,&ioct);

  return;
}

void CC_NAME(pbscale) (FLOAT4 b[],FLOAT4 work[],INT4 nx,INT4 ny,FLOAT4 dndatum)
{
  extern void FTN_NAME(pbscale) (FLOAT4 b[],FLOAT4 work[],INT4 *nx,INT4 *ny,
                                   FLOAT4 *dndatum);

  (void) FTN_NAME(pbscale) (b,work,&nx,&ny,&dndatum);

  return;
}

void CC_NAME(fit2dip) (FLOAT4 buf[],INT4 nx1,INT4 ny1,FLOAT4 dnscl,
                       FLOAT4 pixht,FLOAT4 aspect,FLOAT4 sunaz,
		       FLOAT4 charaz,FLOAT4 *dip,FLOAT4 *dipaz,
		       FLOAT4 *sundip,FLOAT4 *rmsdn,FLOAT4 *rmspix,
		       FLOAT4 *rmsm)
{
  extern void FTN_NAME(fit2dip) (FLOAT4 buf[],INT4 *nx1,INT4 *ny1,
                       FLOAT4 *dnscl,FLOAT4 *pixht,FLOAT4 *aspect,
		       FLOAT4 *sunaz,FLOAT4 *charaz,FLOAT4 *dip,
		       FLOAT4 *dipaz,FLOAT4 *sundip,FLOAT4 *rmsdn,
		       FLOAT4 *rmspix,FLOAT4 *rmsm);

  (void) FTN_NAME(fit2dip) (buf,&nx1,&ny1,&dnscl,&pixht,&aspect,
                       &sunaz,&charaz,dip,dipaz,sundip,rmsdn,rmspix,rmsm);

  return;
}

void CC_NAME(xcsto) (CHAR zin[],FLOAT4 z[],INT4 nx,INT4 ny,FLOAT4 zbuf[],
                     INT4 ins,INT4 inl,INT4 ioct,INT4 *ret)
{
  extern void FTN_NAME(xcsto) (CHAR zin[],FLOAT4 z[],INT4 *nx,INT4 *ny,
                               FLOAT4 zbuf[],INT4 *ins,INT4 *inl,INT4 *ioct,
			       INT4 *ret,INT4 zin_len);

  CHAR *zin_mem;
  INT4 zin_len = (INT4) strlen(zin);

  zin_mem = b_c2fstr (1,zin,zin_len,NULL,(zin_len),B_DISCARD_PAD);

  (void) FTN_NAME(xcsto) (zin_mem,z,&nx,&ny,zbuf,&ins,&inl,&ioct,ret,zin_len);

  (void) b_free(zin_mem);

  return;
}

void CC_NAME(chk4inv) (FLOAT4 datbuf[],INT4 nx,INT4 ny,INT4 *invcnt)
{
  extern void FTN_NAME(chk4inv) (FLOAT4 datbuf[],INT4 *nx,INT4 *ny,INT4 *invcnt);

  (void) FTN_NAME(chk4inv) (datbuf,&nx,&ny,invcnt);

  return;
}

void CC_NAME(zlineout) (FLOAT4 ibuf[],INT4 nx,INT4 ny,FLOAT4 obuf[],INT4 ins,
                         INT4 il,FLOAT4 scale,INT4 ioct,FLOAT4 cosemi)
{
  extern void FTN_NAME(zlineout) (FLOAT4 ibuf[],INT4 *nx,INT4 *ny,FLOAT4 obuf[],
                                   INT4 *ins,INT4 *il,FLOAT4 *scale,INT4 *ioct,
				   FLOAT4 *cosemi);

  (void) FTN_NAME(zlineout) (ibuf,&nx,&ny,obuf,&ins,&il,&scale,&ioct,&cosemi);

  return;
}

void CC_NAME(datum) (INT4 i,INT4 j,INT4 npts,FLOAT4 *z01,FLOAT4 *z02,
                     FLOAT4 *z03,FLOAT4 *z04)
{
  extern void FTN_NAME(datum) (INT4 *i,INT4 *j,INT4 *npts,FLOAT4 *z01,
                     FLOAT4 *z02,FLOAT4 *z03,FLOAT4 *z04);

  (void) FTN_NAME(datum) (&i,&j,&npts,z01,z02,z03,z04);

  return;
}

void CC_NAME(zinterp) (FLOAT4 zin[],FLOAT4 zout[],INT4 nx,INT4 ny,FLOAT4 scale,
                       FLOAT4 cosemi,INT4 *ret)
{
  extern void FTN_NAME(zinterp) (FLOAT4 zin[],FLOAT4 zout[],INT4 *nx,INT4 *ny,
                                 FLOAT4 *scale,FLOAT4 *cosemi,INT4 *ret);

  (void) FTN_NAME(zinterp) (zin,zout,&nx,&ny,&scale,&cosemi,ret);

  return;
}
