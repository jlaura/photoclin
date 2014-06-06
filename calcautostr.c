#include "pc2d.h"
#include <math.h>

void CalcAutoStr (int dsp)

/***********************************************************************
*
*_Title CalcAutoStr - Calculates stretch when first loading image
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	Display number	

*_DESC	Calculates the stretch when first loading an image with the
*       automatic stretch option chosen.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Nov 07 1996 Tracie Sucharski,  Added resource defaults.
*       Mar 24 1998 TLS, Copy original min/max values to actual
*                       min/max values.
*       Nov 09 1998 TLS, For floating point images use a floating
*                       point index in case of overflow.
*       Feb 04 1999 TLS, Check for negative standard deviation 
*                       calculation.
*_END
************************************************************************/

{
  DSPINFO *d;
  unsigned char *buf;
  INT2 *wbuf;
  FLOAT4 *fbuf;
  INT4 *hist,dn;
  int i,j;
  int iline,isamp;
  double sumx,sumxx,fmin,fmax;
  double tmin,tmax,slope;
  double stdev,avg;
  int index,inc;
  float findex;
  int sinc, linc;
  int npts;
  int band;
  INT4 ret,limit[3][2];

  d = &GBLdsp.d[dsp];

  buf = (unsigned char *) GBLwork.work[0];
  wbuf = (INT2 *) GBLwork.work[0];
  fbuf = (FLOAT4 *) GBLwork.work[0];
  hist = (INT4 *) GBLwork.big;

  d->rmin = d->gmin = d->bmin = 0;
  d->rmax = d->gmax = d->bmax = 255;

  q_use_cache (d->f.fid,0,&ret); 

  for (i=0; i<3; i++) {
    band = d->band[i];
    if (band == 0) continue;
    if (d->avalid[band]) {
      if (i == 0) {
        d->rmin = d->amin[band];
        d->rmax = d->amax[band];
      }
      else if (i == 1) {
        d->gmin = d->amin[band];
        d->gmax = d->amax[band];
      }
      else {
        d->bmin = d->amin[band];
        d->bmax = d->amax[band];
      }
      continue;
    }

    limit[0][0] = 1;
    limit[0][1] = d->f.ns;
    limit[2][0] = band;
    limit[2][1] = band;

    for (j=0; j<65536; j++) hist[j] = 0;
    sumx = 0.0;
    sumxx = 0.0;
    npts =  0;
    fmin = VALID_MAX4;
    fmax = VALID_MIN4;
    linc = GBLdefault.prestr_linc;
    sinc = GBLdefault.prestr_sinc;
    if (sinc < 1) sinc = 1;
    if (linc < 1) linc = 1;

    if (d->f.type == 3) {
      inc = d->f.nl / 20;
      if (inc < 1) inc = 1;
      for (iline=1; iline<=d->f.nl; iline+=inc) {
        limit[1][0] = iline;
        limit[1][1] = iline;
     
        q_lio_cbrick (d->f.fid,1,limit,buf,&ret);
        if (ret) {
          strcpy (GBLerr.string,"Unable to read from input file ");
          strcat (GBLerr.string,d->f.name);
          u_error ("READFILE",GBLerr.string,-1,1);
          QviewExit ();
        }

        for (isamp=0; isamp<d->f.ns; isamp+=sinc) {
          if (fbuf[isamp] >= VALID_MIN4) {
            sumx += (double) fbuf[isamp];
            sumxx += (double) fbuf[isamp] * (double) fbuf[isamp];
            npts++;
            if (fbuf[isamp] < fmin) fmin = fbuf[isamp];
            if (fbuf[isamp] > fmax) fmax = fbuf[isamp];
          }
        }
      }
      if (npts > 0) {
        stdev = (double) npts * sumxx - sumx * sumx;
        stdev = stdev / ((double) npts * (double) npts); 
	if (stdev < 0.0) stdev = 0.0;
        stdev = sqrt (stdev);
        avg = sumx / (double) npts;
        tmin = avg - 14.5 * stdev;
        tmax = avg + 14.5 * stdev;
        if (tmin > fmin) fmin = tmin;
        if (tmax < fmax) fmax = tmax;
        if (fmin == fmax) fmax = fmax + 1;
      }
      else {
        fmin = 1;
        fmax = 65534;
      }
    }

    for (iline=1; iline<=d->f.nl; iline+=linc) {
      limit[1][0] = iline;
      limit[1][1] = iline;

      q_lio_cbrick (d->f.fid,1,limit,buf,&ret);
      if (ret) {
        strcpy (GBLerr.string,"Unable to read from input file ");
        strcat (GBLerr.string,d->f.name);
        u_error ("READFILE",GBLerr.string,-1,1);
        QviewExit ();
      }

      if (d->f.type == 1) {
        for (isamp=0; isamp<d->f.ns; isamp+=sinc) {
          hist[buf[isamp]]++;
        }
      }
      else if (d->f.type == 2) {
        for (isamp=0; isamp<d->f.ns; isamp+=sinc) {
          hist[wbuf[isamp]+32768]++;
        }
      }
      else {
        slope = (65534 - 1) / (fmax - fmin);
        for (isamp=0; isamp<d->f.ns; isamp+=sinc) {
          if (fbuf[isamp] >= VALID_MIN4) {
            findex = slope * ((double)fbuf[isamp] - fmin) + 1.0; 
            if (findex < 0) findex = 0;
            if (findex > 65535) findex = 65535;
	    index = (int) findex;
            hist[index]++;
          }
        }
      }
    } 

    if (d->f.type == 1) {
      if (percent8 (hist,d->prestr_minper,(INT4)1,(INT4)254,&dn)) dn = 1;
      fmin = dn;

      if (percent8 (hist,d->prestr_maxper,(INT4)1,(INT4)254,&dn)) dn=254;
      fmax = dn;
    }
    else if (d->f.type == 2) {
      if (percent16 (hist,d->prestr_minper,(INT4)VALID_MIN2,(INT4)VALID_MAX2,&dn)) {
        dn = VALID_MIN2;
      }
      fmin = dn;

      if (percent16 (hist,d->prestr_maxper,(INT4)VALID_MIN2,(INT4)VALID_MAX2,&dn)) {
        dn = VALID_MAX2;
      }
      fmax = dn;
    }
    else if (d->f.type == 3) {
      if (percent16 (hist,d->prestr_minper,(INT4)VALID_MIN2,(INT4)VALID_MAX2,&dn)) {
        tmin = VALID_MIN4;
      }
      else {
        dn += 32768;
        slope = (fmax - fmin) / (double) (65534 - 1);
        tmin = slope * (dn - 1) + fmin;
      }

      if (percent16 (hist,d->prestr_maxper,(INT4)VALID_MIN2,(INT4)VALID_MAX2,&dn)) {
        tmax = VALID_MIN4;
      }
      else {
        dn += 32768;
        slope = (fmax - fmin) / (double) (65534 - 1);
        tmax = slope * (dn - 1) + fmin;
      }
      fmin = tmin;
      fmax = tmax; 
    }

    if (d->f.type != 3) {
      fmin = d->f.scale[0] + d->f.scale[1] * fmin;
      fmax = d->f.scale[0] + d->f.scale[1] * fmax;
    }

    d->amin[band] = fmin;
    d->amax[band] = fmax;
    d->avalid[band] = True;
      
    if (i == 0) {
      d->rmin = fmin;
      d->rmax = fmax;
      d->rmin_real = fmin;
      d->rmax_real = fmax;
    }
    else if (i == 1) {
      d->gmin = fmin;
      d->gmax = fmax;
      d->gmin_real = fmin;
      d->gmax_real = fmax;
    }
    else if (i == 2) {
      d->bmin = fmin;
      d->bmax = fmax;
      d->bmin_real = fmin;
      d->bmax_real = fmax;
    }
  }

  return;
}
