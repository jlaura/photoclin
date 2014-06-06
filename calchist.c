#include <math.h>

#include "pc2d.h"

typedef struct {
  INT4 bandindx;
  INT4 band;
  INT4 nbins;
  INT4 npix;
  FLOAT4 min;
  FLOAT4 max;
  FLOAT8 mean;
  FLOAT8 stdev;
  FLOAT8 skew;
  INT4 mode;
  FLOAT4 median;
  FLOAT4 bin[256];
  INT4   hist[256];
  INT4   tpix[256];
  FLOAT4 pct[256];
  FLOAT4 cumpct[256];
  INT4   sppix[6];
} HISTSTATS;

void CalcHist (INT4 dsp, HISTSTATS *s) 
/***********************************************************************
*
*	------------------
*	|    CalcHist    |
*	------------------
*
*_Title	CalcHist - Calculate histogram and statistical data
*
*_Args	Type   Variable        I/O	Description
*_Parm	
*
*_HIST	Sep 11 1996 Tracie Sucharski, USGS, Flagstaff - Original version
*       Sep 17 1996 TLS, Changed calculation of stats to double precision.
*       Dec 17 1996 TLS, Added special pixel counts.
*       Apr 25 2000 TLS, Check for binning index less than 0 or greater
*                       than nbins-1.
*
*_END
************************************************************************/

{
  DSPINFO *d;

  INT4     is, il;
  INT4     band;
  INT4     i,j;
  INT4     mode;
  INT4     ilim[3][2];
  INT4     ret;
  INT4     sat[2];
  INT4     nvpix, vpix;
  INT4     index;
  INT4     count;

  FLOAT4   diff;
  FLOAT4   mc;
  FLOAT4   *buf;
  FLOAT8   rad;
  FLOAT4   binwid;
  FLOAT4   halfpix;
  FLOAT4   cum;

  FLOAT8   sum, ssum;
  FLOAT8   min, max;

  d = &GBLdsp.d[dsp];
  QviewWatch (d->shell, True);

  min = (FLOAT4) VALID_MAX4;
  max = (FLOAT4) VALID_MIN4;


  band = s->band;
  s->nbins = 256;
  sum = 0;
  ssum = 0;

  buf = (FLOAT4 *) GBLwork.work[0];

  ilim[0][0] = 1;
  ilim[0][1] = d->f.ns;
  ilim[2][0] = band;
  ilim[2][1] = band;
  s->min = VALID_MAX4;
  s->max = VALID_MIN4;

  s->npix = d->f.ns * d->f.nl;

  for (j=0; j<s->nbins; j++) {
    s->hist[j] = 0;
    s->tpix[j] = 0;
    s->pct[j] = 0.;
    s->cumpct[j] = 0.;
  }
  for (j=0; j<6; j++) s->sppix[j] = 0;

  /*----------------------------------------------------------
    /  Cycle through image once to determine min/max.
    /-------------------------------------------------------*/

  for (il=1; il<=d->f.nl; il++) {

    ilim[1][0] = il;
    ilim[1][1] = il;

    q_lio_cbrick (d->f.fid, 1, ilim, buf, &ret);
    if (ret) {
      strcpy (GBLerr.string, "Unable to read input cube ");
      strcat (GBLerr.string, d->f.name);
      u_error ("READFILE", GBLerr.string, -1, 1);
      QviewExit ();
    }
    if (d->f.type != 3) u_convert (d->f.type, 3, buf, buf, d->f.ns, 
                d->f.scale, d->f.scale, 1, sat, &ret);
    if (ret) {
      strcpy (GBLerr.string, "Unable to convert input buffer to real");
      u_error ("CONVBUF", GBLerr.string, -2, 1);
      QviewExit ();
    }

    for (is=0; is<d->f.ns; is++) {
      if (buf[is] >= VALID_MIN4) {
	if (buf[is] < min) min = buf[is];
	if (buf[is] > max) max = buf[is];
      }
    }
  }
/*----------------------------------------------------------------
/  Calculate multiplicative constants for binning
/---------------------------------------------------------------*/
  diff = max - min;
  if (diff != 0.) {
    mc = (s->nbins - 1.) / diff;
  }   /*  This band contains a constant.  */
  else {
    s->nbins = 1;
    mc = 1;
  }

  for (il=1; il<=d->f.nl; il++) {

    ilim[1][0] = il;
    ilim[1][1] = il;

    q_lio_cbrick (d->f.fid, 1, ilim, buf, &ret);
    if (ret) {
      strcpy (GBLerr.string, "Unable to read input cube ");
      strcat (GBLerr.string, d->f.name);
      u_error ("READFILE", GBLerr.string, -1, 1);
      QviewExit ();
    }
    if (d->f.type != 3) u_convert (d->f.type, 3, buf, buf, d->f.ns, 
                d->f.scale, d->f.scale, 1, sat, &ret);
    if (ret) {
      strcpy (GBLerr.string, "Unable to convert input buffer to real");
      u_error ("CONVBUF", GBLerr.string, -2, 1);
      QviewExit ();
    }

    for (is=0; is<d->f.ns; is++) {
      if (buf[is] == NULL4) {
	s->sppix[1]++;
      }
      else if (buf[is] == LOW_REPR_SAT4) {
	s->sppix[2]++;
      }
      else if (buf[is] == LOW_INSTR_SAT4) {
	s->sppix[3]++;
      }
      else if (buf[is] == HIGH_REPR_SAT4) {
	s->sppix[4]++;
      }
      else if (buf[is] == HIGH_INSTR_SAT4) {
	s->sppix[5]++;
      }
      else {
	if (buf[is] < s->min) s->min = buf[is];
	if (buf[is] > s->max) s->max = buf[is];
	index = (INT4) ((buf[is] - min) * mc + 0.5);
	if (index < 0) index = 0;
	if (index > s->nbins-1) 
                index = s->nbins - 1;
	s->hist[index]++;
	sum = sum + (FLOAT8) buf[is];
	ssum = ssum + (FLOAT8) buf[is] * (FLOAT8) buf[is];
      }
    }
  }

/*----------------------------------------------------------------------
/  Add up the number of special pixel values to determine the number
/  of valid points.
/---------------------------------------------------------------------*/
  nvpix = 0;
  for (j=1; j<6; j++) nvpix += s->sppix[j];
  vpix = s->npix - nvpix;
  s->sppix[0] = nvpix;

  s->mean = sum / (FLOAT8) vpix;
  rad = (FLOAT8) vpix * ssum - sum*sum;
  if (rad > 0) {
    s->stdev = 1. / (FLOAT8) vpix * sqrt(rad);
  }
  else {
    s->stdev = 0;
  }

/*---------------------------------------------------------------------
/  Calculate the center point of each hist bin.
/--------------------------------------------------------------------*/
  binwid = (max - min) / (s->nbins-1);
  for (j=0; j<s->nbins; j++) s->bin[j] = min + j*binwid;

/*---------------------------------------------------------------------
/  Calculate the median and skew.
/--------------------------------------------------------------------*/
  halfpix = (FLOAT4) vpix / 2.;
  j = -1;
  count = 0;
  while (count < halfpix) {
    j++;
    count += s->hist[j];
  }

  s->median = s->bin[j];


  if (s->stdev > 0) {
    s->skew = 3. * (s->mean - (FLOAT8) s->median) / s->stdev;
  }
  else {
    s->skew = 0.;
  }

  /*-------------------------------------------------------------------
    /  Calculate mode.
    /----------------------------------------------------------------*/
  mode = 0;
  for (i=0; i<= s->nbins; i++) {
    if (s->hist[i] > s->hist[mode]) mode = i;
  }
  s->mode = mode;

  cum = 0.;
  
/*----------------------------------------------------------------------
/  Tabulate histogram data.
/---------------------------------------------------------------------*/
  for (j=0; j<s->nbins; j++) {

    if (j == 0) {
      s->tpix[j] = s->hist[j];
    }
    else {
      s->tpix[j] = s->tpix[j-1] + s->hist[j];
    }
    s->pct[j] = (FLOAT4) s->hist[j] / vpix;
    cum += s->pct[j];
    s->pct[j] = s->pct[j]*100.;
    s->cumpct[j] = cum*100.;

  }
  QviewWatch (d->shell, False);

}
