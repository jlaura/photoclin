#include "pc2d.h"

void StretchLine (UINT1 *buf, int dsp, int chan) 

{
  DSPINFO *d;
  double vmin,vmax,slope,dn;
  int i;
  UINT1 *lut;
  INT2 *wbuf;
  FLOAT4 *fbuf;

  d = &GBLdsp.d[dsp];
  if (chan == 0) {
    vmin = d->rmin;
    vmax = d->rmax;
    lut = d->rlut;
  }
  else if (chan == 1) {
    vmin = d->gmin;
    vmax = d->gmax;
    lut = d->glut;
  }
  else {
    vmin = d->bmin;
    vmax = d->bmax;
    lut = d->blut;
  }
  

  if (d->f.type == 1) {
    if (!(d->autostr || d->manstr)) return;
    for (i=0; i<d->f.ns; i++) {
      buf[i] = lut[buf[i]];
    }
  }
  else if (d->f.type == 2) {
    wbuf = (INT2 *) buf;
    for (i=0; i<d->f.ns; i++) {
      if (wbuf[i] < VALID_MIN2) {
        if ((wbuf[i] == HIGH_INSTR_SAT2) || (wbuf[i] == HIGH_REPR_SAT2)) {
          buf[i] = lut[65535];
        }
        else {
          buf[i] = lut[0];
        } 
      }
      else {
        buf[i] = lut[wbuf[i]+32768];
      }
    }
  }
  else {
    fbuf = (FLOAT4 *) buf;
    if (vmin == vmax) {
      slope = VALID_MIN4;
    }
    else {
      slope = (254 - 1) / (vmax - vmin);
    }
    for (i=0; i<d->f.ns; i++) {
      if (fbuf[i] < VALID_MIN4) {
        if ((fbuf[i] == HIGH_INSTR_SAT4) || (fbuf[i] == HIGH_REPR_SAT4)) {
          buf[i] = 255;
        }
        else {
          buf[i] = 0;
        } 
      }
      else {
        dn = slope * (fbuf[i] - vmin) + 1.0;
        if (dn < 0) dn = 0;
        if (dn > 255) dn = 255;
        buf[i] = dn;
      }
    }
  }

  return;
}

