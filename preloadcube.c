#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <stdio.h>
#include <string.h>

#include "pc2d.h"
#include "spi.h"

Boolean PreLoadCube (int dsp, char *ptr, float min, float max, 
      Boolean str, Boolean setworld)

/***********************************************************************
*
*_Title PreLoadCube - Calculates auto stretch and creates display
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       Display number
*_Parm  char    *ptr            I       File name of image
*_Parm  float   min             I       Minimum value for stretch
*_Parm  float   max             I       Maximum value for stretch
*_Parm  Boolean str             I       If True, calculate stretch
*                                       If False, don't calculate stretch
*_Parm  Boolean setworld
*
*_DESC  Pre stretch image and create display. If successful it
*	will load the "f" structure associated with the display and
*	return False. If an error occurs it will return True.
*
*
*_HIST  Jan 1 1996 Jeff Anderson, Original version
*       Apr 25 1997 Tracie Sucharski, Allow display of more than
*                     one image on the TAE command line and allow
*                     min/max values to be entered on command line.
*       Jan 14 1998 TLS, Print error message if file can't be opened.
*       Mar 24 1998 TLS, Copy original min/max values to actual
*                       min/max values.
*       Dec 15 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{

  int i;
  static int firstime=2;
  DSPINFO *d;

  GBLdsp.free = dsp;

  if (OpenCube (dsp,ptr)) {
    sprintf (GBLerr.string,"%s can not be opened",ptr);
    u_error ("OPENERR",GBLerr.string,-1,1);
    return True;
  }

  GBLdsp.used[dsp] = True;
  GBLdsp.linked[dsp] = True;

  d = &GBLdsp.d[dsp];

  d->band[0] = 1;
  d->band[1] = 0;
  d->band[2] = 0;
  d->scs_sl = 1;
  d->scs_ss = 1;
  d->scs_el = d->f.nl;
  d->scs_es = d->f.ns;

  for (i=0; i<MAX_BANDS; i++) d->avalid[i] = False;

  if (str) {
    d->autostr = True;
    d->manstr = False;
    d->prestr_minper = GBLdefault.prestr_lowper;
    d->prestr_maxper = GBLdefault.prestr_hiper;
    QviewWatch (xinfo.topShell,True);
    CalcAutoStr (dsp);
    QviewWatch (xinfo.topShell,False);
/*    else {
      d->autostr = False;
      d->manstr = True;
      d->rmin = min;
      d->rmax = max;
      d->gmin = 0;
      d->gmax = 255;
      d->bmin = 0;
      d->bmax = 255;
      d->rmin_real = d->rmin;
      d->rmax_real = d->rmax;
      d->gmin_real = d->gmin;
      d->gmax_real = d->gmax;
      d->bmin_real = d->bmin;
      d->bmax_real = d->bmax;
    }*/
  }

  if (!str) {
    for (i=0;i<3;i++) d->avalid[i] = False;
    XSync(xinfo.display,False);
    CalcAutoStr(dsp);
    StretchReset (dsp);
  }
  LoadStrLut (dsp);
  if (firstime) {
    CreateDisplay ();
    firstime = firstime - 1;
  } else {
    if (setworld) {
      d->world = True;
      d->ss = d->scs_ss;
      d->sl = d->scs_sl;
      d->inc = ((double) (d->scs_el-d->scs_sl+1))/340.0;
      if ((d->scs_es-d->scs_ss+1)>(d->scs_el-d->scs_sl+1))
        d->inc = ((double) (d->scs_es-d->scs_ss+1))/340.0;
    }
    d->read_tmp = False;
    LoadCube (dsp);
  }

  return False;
}
