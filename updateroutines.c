#include <string.h>
#include "pc2d.h"

void UpdateInfoLB (dsp)

{
  char string[256],string2[256];
  char wstr[256],wstr2[256];
  XmString xstring;
  DSPINFO *d;
  Dimension width,height;
  int line,samp;
  
  INT4    nval,ret;
  FLOAT4  wave;

  d = &GBLdsp.d[dsp];
  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

/*  strcpy (string,"Area=");*/
/*  strcpy (string2,"Band=");*/

  samp = d->samplut[0];
  if (samp < 1) samp = 1;
  sprintf (wstr,"%d",samp);
/*  strcat (string,wstr);*/
  strcpy (string,wstr);
  strcat (string,"-");

  samp = d->samplut[width-1];
  if (samp > d->f.ns) samp = d->f.ns;
  sprintf (wstr,"%d",samp);
  strcat (string,wstr);
  strcat (string,":");

  line = d->linelut[0];
  if (line < 1) line = 1;
  sprintf (wstr,"%d",line);
  strcat (string,wstr);
  strcat (string,"-");

  line = d->linelut[height-1];
  if (line > d->f.nl) line = d->f.nl;
  sprintf (wstr,"%d",line);
  strcat (string,wstr);
  strcat (string,":");

  if (d->band[0]) {
    sprintf (wstr,"%d",d->band[0]);
    strcat (string,wstr);

    /*  Display band number and wavelength  */
/*    strcat (string2,wstr);*/
    nval = d->band[0];
    ret = 0;
    p_get_real_key (d->f.fid,"QUBE","BAND_BIN","BAND_BIN_CENTER",
                    2,&nval,&wave,&ret);
    if (ret == 0) {
      sprintf (wstr2," (%.3f)",wave);
/*      strcat (string2,wstr2);*/
    }
  }
  
  if (d->band[1]) {
    if (d->band[0]) {
      strcat (string,",");
/*      strcat (string2,",");*/
    }
    sprintf (wstr,"%d",d->band[1]);
    strcat (string,wstr);

    /*  Display band number and wavelength  */
/*    strcat (string2,wstr);*/
    nval = d->band[1];
    p_get_real_key (d->f.fid,"QUBE","BAND_BIN","BAND_BIN_CENTER",
                    2,&nval,&wave,&ret);
    if (ret == 0) {
      sprintf (wstr2," (%.3f)",wave);
/*      strcat (string2,wstr2);*/
    }


  }

  if (d->band[2]) {
    if ((d->band[0]) || (d->band[1])) {
      strcat (string,","); 
/*      strcat (string2,",");*/
    }
    sprintf (wstr,"%d",d->band[2]);
    strcat (string,wstr);

    /*  Display band number and wavelength  */
/*    strcat (string2,wstr);*/
    nval = d->band[2];
    p_get_real_key (d->f.fid,"QUBE","BAND_BIN","BAND_BIN_CENTER",
                    2,&nval,&wave,&ret);
    if (ret == 0) {
      sprintf (wstr2," (%.3f)",wave);
/*      strcat (string2,wstr2);*/
    }

  }

  xstring = XmStringCreateSimple (string);
  XtVaSetValues (d->infoLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

/*  xstring = XmStringCreateSimple (string2);
  XtVaSetValues (d->bandLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);*/

/*  strcpy (string,"Scale=");*/
  sprintf (wstr,"%7f",d->inc);
/*  strcat (string,wstr);*/
  strcpy (string,wstr);
  
  xstring = XmStringCreateSimple (string);
  XtVaSetValues (d->scaleLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  return;
}
