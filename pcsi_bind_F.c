#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binding.h"
#include "isistypes.h"

void CC_NAME(xcmain_pcsi) (INT4 idum[],INT4 n1,INT4 nx,INT4 ny,
                           INT1 dozout,INT4 *nsmooth,
                           INT4 sordir,INT4 *ret)
{
  extern void FTN_NAME(xcmain_pcsi) (INT4 idum[],INT4 *n1,INT4 *nx,
                                     INT4 *ny,INT1 *dozout,
                                     INT4 *nsmooth,INT4 *sordir,INT4 *ret);

  (void) FTN_NAME(xcmain_pcsi) (idum,&n1,&nx,&ny,&dozout,nsmooth,&sordir,ret);

  return;
}

void CC_NAME(pcsi_xcsep) (INT4 cbuf[],INT4 nmax,INT4 n1,INT4 n2,INT4 n3,INT4 n4,
                     INT4 n5,INT4 nx,INT4 ny,INT4 fn)
{
  extern void FTN_NAME(pcsi_xcsep) (INT4 cbuf[],INT4 *nmax,INT4 *n1,INT4 *n2,
                     INT4 *n3,INT4 *n4,INT4 *n5,INT4 *nx,INT4 *ny,INT4 *fn);

  (void) FTN_NAME(pcsi_xcsep) (cbuf,&nmax,&n1,&n2,&n3,&n4,&n5,&nx,&ny,&fn);

  return;
}

void CC_NAME(pcsi_ipars) (INT4 *nsmooth)
{
  extern void FTN_NAME(pcsi_ipars) (INT4 *nsmooth);

  (void) FTN_NAME(pcsi_ipars) (nsmooth);

  return;
}
