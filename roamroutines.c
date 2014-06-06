#include "pc2d.h"

void Roam (int dsp)

/***********************************************************************
*
*_Title Roam - Determines direction of roam
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  This routine determines the direction of the roam operation.
*
*_HIST  Jan 1 1996 Jeff Anderson, Original version
*       Dec 31 1997  Tracie Sucharski,  Added the option to roam
*                      diagonally.
*_END
************************************************************************/
{
  DSPINFO *d;
  Dimension width,height;
  double ns;
  Boolean left,right,up,down;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  right = False;
  left = False;
  up = False;
  down = False;

  if ((double)d->tool_x < (double)(width)*0.25) left = True;
  if ((double)d->tool_x > (double)(width)*0.75) right = True;
  if ((double)d->tool_y < (double)(height)*0.25) up = True;
  if ((double)d->tool_y > (double)(height)*0.75) down = True;

  if (right && !(up||down)) RoamRight (dsp);
  if (left && !(up||down)) RoamLeft (dsp);
  if (up && !(right||left)) RoamUp (dsp);
  if (down && !(right||left)) RoamDown (dsp);
  if (left && down) RoamSW (dsp);
  if (right && down) RoamSE (dsp);
  if (left && up) RoamNW (dsp);
  if (right && up) RoamNE (dsp);

  return;
}

void RoamRight (int dsp)
 
{
  DSPINFO *d;
  Dimension width,height;
  double ns;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if ((d->samplut[width-1] > d->f.ns) && (!GBLdsp.tvreg)) {
    XBell (xinfo.display,0);
    return;
  }

  ns = (double) width * d->roam_percent * d->inc;
  if (GBLdsp.tvreg) ns = d->inc;

  d->ss += ns;
  if (!GBLdsp.linked[dsp]) {
    if (d->ss + (double) width * d->inc - 1.0 > (double) d->f.ns + 50.0) {
      d->ss = d->f.ns + 50.0 - width * d->inc + 1.0;
    }
  }

  LoadCube (dsp);

  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      ns = (double) width * d->roam_percent * d->inc;
      d->ss += ns;

      LoadCube (dsp);
    }
  }
 
  return;
}




void RoamLeft (int dsp)
 
{
  DSPINFO *d;
  Dimension width,height;
  double ns;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if ((d->samplut[0] < 1) && (!GBLdsp.tvreg)) {
    XBell (xinfo.display,0);
    return;
  }

  ns = (double) width * d->roam_percent * d->inc;
  if (GBLdsp.tvreg) ns = d->inc;

  d->ss -= ns;
  if (!GBLdsp.linked[dsp]) {
    if (d->ss < -50.0) d->ss = -50.0;
  }

  LoadCube (dsp);
 
  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      ns = (double) width * d->roam_percent * d->inc;
      d->ss -= ns;

      LoadCube (dsp);
    }
  }
 
  return;
}


void RoamDown (int dsp)

{
  DSPINFO *d;
  Dimension width,height;
  double nl;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if ((d->linelut[height-1] > d->f.nl) && (!GBLdsp.tvreg)) {
    XBell (xinfo.display,0);
    return;
  }

  nl = (double) height * d->roam_percent * d->inc;
  if (GBLdsp.tvreg) nl = d->inc;

  d->sl += nl;
  if (!GBLdsp.linked[dsp]) {
    if (d->sl + (double) height * d->inc - 1.0 > (double) d->f.nl + 50.0) {
      d->sl = d->f.nl + 50.0 - height * d->inc + 1.0;
    }
  }

  LoadCube (dsp);
 
  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      nl = (double) height * d->roam_percent * d->inc;
      d->sl += nl;

      LoadCube (dsp);
    }
  }

  return;
}




void RoamUp (int dsp)

{
  DSPINFO *d;
  Dimension width,height;
  double nl;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if ((d->linelut[0] < 1) && (!GBLdsp.tvreg)) {
    XBell (xinfo.display,0);
    return;
  }

  nl = (double) height * d->roam_percent * d->inc;
  if (GBLdsp.tvreg) nl = d->inc;

  d->sl -= nl;
  if (!GBLdsp.linked[dsp]) {
    if (d->sl < -50.0) d->sl = -50.0;
  }

  LoadCube (dsp);
 
  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      nl = (double) height * d->roam_percent * d->inc;
      d->sl -= nl;

      LoadCube (dsp);
    }
  }

  return;
}



void RoamNE (int dsp)
 
/***********************************************************************
*
*_Title RoamNE - Roam in the direction of the upper-right corner
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  This routine will reload the cube moving in the direction of
*       the upper-right corner.
*
*_HIST  Dec 31 1997 Tracie Sucharski, Original version
*
*_END
************************************************************************/
{
  DSPINFO *d;
  Dimension width,height;
  double ns,nl;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if ((d->samplut[width-1] > d->f.ns || d->linelut[0] < 1) && 
         (!GBLdsp.tvreg)) {
    XBell (xinfo.display,0);
    return;
  }

  ns = (double) width * d->roam_percent * d->inc;
  nl = (double) height * d->roam_percent * d->inc;
  if (GBLdsp.tvreg) {
    ns = d->inc;
    nl = d->inc;
  }

  d->ss += ns;
  d->sl -= nl;
  if (!GBLdsp.linked[dsp]) {
    if (d->ss + (double) width * d->inc - 1.0 > (double) d->f.ns + 50.0) {
      d->ss = d->f.ns + 50.0 - width * d->inc + 1.0;
    }
    if (d->sl < -50.0) d->sl = -50.0;
  }

  LoadCube (dsp);

  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      ns = (double) width * d->roam_percent * d->inc;
      d->ss += ns;
      nl = (double) height * d->roam_percent * d->inc;
      d->sl -= nl;

      LoadCube (dsp);
    }
  }
 
  return;
}


void RoamNW (int dsp)
 
/***********************************************************************
*
*_Title RoamNW - Roam in the direction of the upper-left corner
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  This routine will reload the cube moving in the direction of
*       the upper-left corner.
*
*_HIST  Jan 05 1998 Tracie Sucharski, Original version
*
*_END
************************************************************************/
{
  DSPINFO *d;
  Dimension width,height;
  double ns,nl;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if ((d->samplut[0] < 1 || d->linelut[0] < 1) && 
         (!GBLdsp.tvreg)) {
    XBell (xinfo.display,0);
    return;
  }

  ns = (double) width * d->roam_percent * d->inc;
  nl = (double) height * d->roam_percent * d->inc;
  if (GBLdsp.tvreg) {
    ns = d->inc;
    nl = d->inc;
  }

  d->ss -= ns;
  d->sl -= nl;
  if (!GBLdsp.linked[dsp]) {
    if (d->ss < -50.0) d->ss = -50.0;
    if (d->sl < -50.0) d->sl = -50.0;
  }

  LoadCube (dsp);

  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      ns = (double) width * d->roam_percent * d->inc;
      d->ss -= ns;
      nl = (double) height * d->roam_percent * d->inc;
      d->sl -= nl;

      LoadCube (dsp);
    }
  }
 
  return;
}




void RoamSE (int dsp)
 
/***********************************************************************
*
*_Title RoamSE - Roam in the direction of the lower-right corner
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  This routine will reload the cube moving in the direction of
*       the lower-right corner.
*
*_HIST  Jan 05 1998 Tracie Sucharski, Original version
*
*_END
************************************************************************/
{
  DSPINFO *d;
  Dimension width,height;
  double ns,nl;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if ((d->samplut[width-1] > d->f.ns || d->linelut[height-1] > d->f.nl) && 
         (!GBLdsp.tvreg)) {
    XBell (xinfo.display,0);
    return;
  }

  ns = (double) width * d->roam_percent * d->inc;
  nl = (double) height * d->roam_percent * d->inc;
  if (GBLdsp.tvreg) {
    ns = d->inc;
    nl = d->inc;
  }

  d->ss += ns;
  d->sl += nl;
  if (!GBLdsp.linked[dsp]) {
    if (d->ss + (double) width * d->inc - 1.0 > (double) d->f.ns + 50.0) {
      d->ss = d->f.ns + 50.0 - width * d->inc + 1.0;
    }
    if (d->sl + (double) height * d->inc - 1.0 > (double) d->f.nl + 50.0) {
      d->sl = d->f.nl + 50.0 - height * d->inc + 1.0;
    }
  }

  LoadCube (dsp);

  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      ns = (double) width * d->roam_percent * d->inc;
      d->ss += ns;
      nl = (double) height * d->roam_percent * d->inc;
      d->sl += nl;

      LoadCube (dsp);
    }
  }
 
  return;
}



void RoamSW (int dsp)
 
/***********************************************************************
*
*_Title RoamSW - Roam in the direction of the lower-left corner
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  This routine will reload the cube moving in the direction of
*       the lower-left corner.
*
*_HIST  Jan 05 1998 Tracie Sucharski, Original version
*
*_END
************************************************************************/
{
  DSPINFO *d;
  Dimension width,height;
  double ns,nl;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if ((d->samplut[0] < 1 || d->linelut[height-1] > d->f.nl) && 
         (!GBLdsp.tvreg)) {
    XBell (xinfo.display,0);
    return;
  }

  ns = (double) width * d->roam_percent * d->inc;
  nl = (double) height * d->roam_percent * d->inc;
  if (GBLdsp.tvreg) {
    ns = d->inc;
    nl = d->inc;
  }

  d->ss -= ns;
  d->sl += nl;
  if (!GBLdsp.linked[dsp]) {
    if (d->ss < -50.0) d->ss = -50.0;
    if (d->sl + (double) height * d->inc - 1.0 > (double) d->f.nl + 50.0) {
      d->sl = d->f.nl + 50.0 - height * d->inc + 1.0;
    }
  }

  LoadCube (dsp);

  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      ns = (double) width * d->roam_percent * d->inc;
      d->ss -= ns;
      nl = (double) height * d->roam_percent * d->inc;
      d->sl += nl;

      LoadCube (dsp);
    }
  }
 
  return;
}




