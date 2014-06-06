#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binding.h"
#include "isistypes.h"

void CC_NAME(xcmain_gui) (INT4 idum[],INT4 n1,INT4 nx,INT4 ny,INT4 iaction,
                          INT4 sordir,BOOLEAN *aeknown,INT4 *n1lmag,
			  INT4 *n2lmag,INT4 *n4lmag,INT4 *nxlmag,INT4 *nylmag,
			  BOOLEAN *wrtimg,INT4 *mag,INT4 *minmag,
			  FLOAT8 res,FLOAT8 azinc,BOOLEAN fastinj,INT4 *ret)
{
  extern void FTN_NAME(xcmain_gui) (INT4 idum[],INT4 *n1,INT4 *nx,
                                    INT4 *ny,INT4 *iaction,
			            INT4 *sordir,BOOLEAN *aeknown,
				    INT4 *n1lmag,INT4 *n2lmag,INT4 *n4lmag,
				    INT4 *nxlmag,INT4 *nylmag,BOOLEAN *wrtimg,
				    INT4 *mag,INT4 *minmag,
				    FLOAT8 *res,FLOAT8 *azinc,BOOLEAN *fastinj,
				    INT4 *ret);

  (void) FTN_NAME(xcmain_gui) (idum,&n1,&nx,&ny,&iaction,&sordir,
                               aeknown,n1lmag,n2lmag,n4lmag,nxlmag,nylmag,
			       wrtimg,mag,minmag,&res,&azinc,&fastinj,ret);

  return;
}

void CC_NAME(synthdiff) (CHAR difnam[],CHAR synnam[],FLOAT4 img[],FLOAT4 work[],
                         INT4 nx,INT4 ny,FLOAT4 fmag,INT4 fid)
{
  extern void FTN_NAME(synthdiff) (CHAR difnam[],CHAR synnam[],FLOAT4 img[],
                                   FLOAT4 work[],INT4 *nx,INT4 *ny,FLOAT4 *fmag,
                                   INT4 *fid,INT4 difnam_len,
				   INT4 synnam_len);

  CHAR *difnam_mem;
  CHAR *synnam_mem;
  INT4 difnam_len = (INT4) strlen(difnam);
  INT4 synnam_len = (INT4) strlen(synnam);

  difnam_mem = b_c2fstr (1,difnam,difnam_len,NULL,(difnam_len),B_DISCARD_PAD);
  synnam_mem = b_c2fstr (1,synnam,synnam_len,NULL,(synnam_len),B_DISCARD_PAD);

  (void) FTN_NAME(synthdiff) (difnam_mem,synnam_mem,img,work,&nx,&ny,&fmag,&fid,difnam_len,
                              synnam_len);

  (void) b_free(difnam_mem);
  (void) b_free(synnam_mem);

  return;
}

void CC_NAME(synthetic) (CHAR imgnam[],FLOAT4 img[],FLOAT4 z[],INT4 nx,
                     INT4 ny,FLOAT4 dnd,FLOAT4 dna,FLOAT4 fmag,INT4 fid)
{
  extern void FTN_NAME(synthetic) (CHAR imgnam[],FLOAT4 img[],FLOAT4 z[],
                               INT4 *nx,INT4 *ny,FLOAT4 *dnd,FLOAT4 *dna,
                               FLOAT4 *fmag,INT4 *fid,INT4 imgnam_len);

  CHAR *imgnam_mem;
  INT4 imgnam_len = (INT4) strlen(imgnam);

  imgnam_mem = b_c2fstr (1,imgnam,imgnam_len,NULL,(imgnam_len),B_DISCARD_PAD);

  (void) FTN_NAME(synthetic) (imgnam_mem,img,z,&nx,&ny,&dnd,&dna,&fmag,&fid,imgnam_len);

  (void) b_free(imgnam_mem);

  return;
}

void CC_NAME(xcsep) (INT4 cbuf[],INT4 nmax,INT4 n1,INT4 n2,INT4 n3,INT4 n4,
                     INT4 n5,INT4 nx,INT4 ny,INT4 fn)
{
  extern void FTN_NAME(xcsep) (INT4 cbuf[],INT4 *nmax,INT4 *n1,INT4 *n2,
                     INT4 *n3,INT4 *n4,INT4 *n5,INT4 *nx,INT4 *ny,INT4 *fn);

  (void) FTN_NAME(xcsep) (cbuf,&nmax,&n1,&n2,&n3,&n4,&n5,&nx,&ny,&fn);

  return;
}
