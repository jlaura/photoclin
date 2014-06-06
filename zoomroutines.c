#include "pc2d.h"

void ZoomIn (int dsp)
/***********************************************************************
*
*_Title ZoomIn - Zoom in using the user defined zoom factor.
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  
*
*_HIST  Jan 1 1996 Jeff Anderson, Original version
*       Nov 17 1998  Tracie Sucharski,  Make sure the first pixel
*                      in the display is a whole pixel rather than a
*                      fractional pixel.
*
*_END
************************************************************************/
 
{
  DSPINFO *d;
  double line,samp;
  Dimension width,height;
  int cdsp;
  int x,y;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  x = d->tool_x;
  y = d->tool_y;
  line = d->linelut[y];
  samp = d->samplut[x];
  d->zoom_savex = samp;
  d->zoom_savey = line;
      
  d->inc /= d->zoom_factor;
  d->sl = (int) (line - d->inc * (double) height / 2.0 + .5);
  d->ss = (int) (samp - d->inc * (double) width / 2.0 + .5);

  LoadCube (dsp);

  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (cdsp == dsp) continue;
    
      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      line = d->linelut[y];
      samp = d->samplut[x];
      
      d->inc /= d->zoom_factor;
      d->sl = (int) (line - d->inc * (double) height / 2.0 + .5);
      d->ss = (int) (samp - d->inc * (double) width / 2.0 + .5);

      d->world = False;

      LoadCube (dsp);
    }
  }
 
  return;
}





void ZoomOut (int dsp)
/***********************************************************************
*
*_Title ZoomOut - Zoom out using the user defined zoom factor.
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  
*
*_HIST  Jan 1 1996 Jeff Anderson, Original version
*       Nov 17 1998  Tracie Sucharski,  Make sure the first pixel
*                      in the display is a whole pixel rather than a
*                      fractional pixel.
*
*_END
************************************************************************/
 
{
  DSPINFO *d;
  double line,samp;
  Dimension width,height;
  int cdsp;
  int x,y;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  x = d->tool_x;
  y = d->tool_y;
  line = d->linelut[y];
  samp = d->samplut[x];
  d->zoom_savex = samp;
  d->zoom_savey = line;
      
  d->inc *= d->zoom_factor;
  d->sl = (int) (line - d->inc * (double) height / 2.0 + .5);
  d->ss = (int) (samp - d->inc * (double) width / 2.0 + .5);

  LoadCube (dsp);

  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (cdsp == dsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      line = d->linelut[y];
      samp = d->samplut[x];
      
      d->inc *= d->zoom_factor;
      d->sl = (int) (line - d->inc * (double) height / 2.0 + .5);
      d->ss = (int) (samp - d->inc * (double) width / 2.0 + .5);

      d->world = False;

      LoadCube (dsp);
    }
  }
 
  return;
}




void ZoomCenter (int dsp)
/***********************************************************************
*
*_Title ZoomCenter - Zoom in using the user defined zoom factor and 
*                      centering the pixel under the cursor in the
*                      center of the display window.
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  
*
*_HIST  Jan 1 1996 Jeff Anderson, Original version
*       Nov 17 1998  Tracie Sucharski,  Make sure the first pixel
*                      in the display is a whole pixel rather than a
*                      fractional pixel.
*
*_END
************************************************************************/
 
{
  DSPINFO *d;
  double line,samp;
  Dimension width,height;
  int cdsp;
  int x,y;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  x = d->tool_x;
  y = d->tool_y;
  line = d->linelut[y];
  samp = d->samplut[x];
  d->zoom_savex = samp;
  d->zoom_savey = line;
      
  d->sl = (int) (line - d->inc * (double) height / 2.0 + .5);
  d->ss = (int) (samp - d->inc * (double) width / 2.0 + .5);

  LoadCube (dsp);
 
  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (dsp == cdsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      line = d->linelut[y];
      samp = d->samplut[x];
      
      d->sl = (int) (line - d->inc * (double) height / 2.0 + .5);
      d->ss = (int) (samp - d->inc * (double) width / 2.0 + .5);

      d->world = False;

      LoadCube (dsp);
    }
  }
 
  return;
}




void ZoomFill (int dsp)
/***********************************************************************
*
*       -----------------
*       |   ZoomFill    |
*       -----------------
*
*_Title ZoomFill - Fill display
*
*_Args  Type   Variable        I/O      Description
*_Parm  int    dsp              I       display number
*
*_DESC  This routine will fit the image to fill the display.
*
*_HIST  Jan 01 1996  Jeff Anderson, USGS, Flagstaff, Original version
*       Jan 29 1998  Tracie Sucharski, Save filled display to temporary
*                      file.
*       Mar 27 1998  TLS, When images are linked set world to True.
*
*_END
************************************************************************/
 
{
  DSPINFO *d;
  Dimension width,height;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  if (!d->read_tmp) d->save_tmp = True;

  d->sl = d->scs_sl;
  d->ss = d->scs_ss;
  d->inc = (double) (d->scs_el - d->scs_sl + 1) / (double) height;
  if (d->inc < (double) (d->scs_es - d->scs_ss + 1) / (double) width) {
    d->inc = (double) (d->scs_es - d->scs_ss + 1) / (double) width;
  }

/*
  d->sl = 1.0;
  d->ss = 1.0;
  d->inc = (double) d->f.nl / (double) height;
  if (((double) d->f.ns / (double) width) > d->inc) {
    d->inc = (double) d->f.ns / (double) width;
  }
*/

  LoadCube (dsp);

  if (GBLdsp.linked[dsp]) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (cdsp == dsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);
      
      d->sl = d->scs_sl;
      d->ss = d->scs_ss;
      d->inc = (double) (d->scs_el - d->scs_sl + 1) / (double) height;
      if (d->inc < (double) (d->scs_es - d->scs_ss + 1) / (double) width) {
        d->inc = (double) (d->scs_es - d->scs_ss + 1) / (double) width;
      }

      if (!d->read_tmp) d->save_tmp = True;
      d->world = True;
      LoadCube (dsp);
    }
  }
 
  return;
}




void Zoom1X (int dsp)
/***********************************************************************
*
*_Title Zoom1X - Zoom at a scale of 1.0.
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       display number 
 
*_DESC  
*
*_HIST  Jan 1 1996 Jeff Anderson, Original version
*       Nov 17 1998  Tracie Sucharski,  Make sure the first pixel
*                      in the display is a whole pixel rather than a
*                      fractional pixel.
*
*_END
************************************************************************/
 
{
  DSPINFO *d;
  double line,samp;
  Dimension width,height;
  int x,y;
  int cdsp;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  x = d->tool_x;
  y = d->tool_y;
  line = d->linelut[y];
  samp = d->samplut[x];
  d->zoom_savex = samp;
  d->zoom_savey = line;
      
  d->inc = 1.0;
  d->sl = (int) (line - d->inc * (double) height / 2.0 + .5);
  d->ss = (int) (samp - d->inc * (double) width / 2.0 + .5);

  LoadCube (dsp);

  if ((GBLdsp.linked[dsp]) && (!GBLdsp.tvreg)) {
    cdsp = dsp;
    for (dsp=0; dsp<V_MAXDSP; dsp++) {
      if (!GBLdsp.used[dsp]) continue;
      if (!GBLdsp.linked[dsp]) continue;
      if (cdsp == dsp) continue;

      d = &GBLdsp.d[dsp];
      XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

      line = d->linelut[y];
      samp = d->samplut[x];
      
      d->inc = 1.0;
      d->sl = (int) (line - d->inc * (double) height / 2.0 + .5);
      d->ss = (int) (samp - d->inc * (double) width / 2.0 + .5);

      d->world = False;
      LoadCube (dsp);
    }
  }
 
  return;
}
