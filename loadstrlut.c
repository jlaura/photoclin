#include "pc2d.h"

void LoadStrLut (int dsp)

{
  DSPINFO *d;
  UINT1 *lut;
  int i,j;
  double fmin,fmax,dn;
  double slope;

  d = &GBLdsp.d[dsp];

  for (i=0; i<3; i++) {
    if (i == 0) {
      fmin = d->rmin;
      fmax = d->rmax;
      lut = d->rlut;
    }
    else if (i == 1) {
      fmin = d->gmin;
      fmax = d->gmax;
      lut = d->glut;
    }
    else {
      fmin = d->bmin;
      fmax = d->bmax;
      lut = d->blut;
    }

    if (d->f.type != 3) {
      fmin = (fmin - d->f.scale[0]) / d->f.scale[1];
      fmax = (fmax - d->f.scale[0]) / d->f.scale[1];
    }

    if (d->f.type  == 1) {
      if (fmax == fmin) {
        slope = VALID_MAX4;
      }
      else {
        slope = (254 - 1) / (fmax - fmin);
      }
      for (j=0; j<=255; j++) {
        dn = slope * (j - fmin) + 1.0;
        if (dn < 0) dn = 0;
        if (dn > 255) dn = 255;
        lut[j] = dn;
      }
    }
    else if (d->f.type == 2) {
      if (fmax == fmin) {
        slope = VALID_MAX4;
      }
      else {
        slope = (254 - 1) / (fmax - fmin);
      }
      for (j=-32768; j<=32767; j++) {
        dn = slope * (j - fmin) + 1;
        if (dn < 0) dn = 0;
        if (dn > 255) dn = 255;
        lut[j+32768] = dn;
      }
    }
  }

  return;
}
