#include "pc2d.h"
#include <math.h>
#include <string.h>
#include <Xm/Text.h>

void ListArea (int dsp)

/***********************************************************************
*
*_Title	ListArea - Updates the list area.
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	display number 

*_DESC	This routine updates the list dialog box.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Sep 25 1996  Tracie Sucharski,  Added the option to list 
*                      backplane values.
*       Oct 01 1996  TLS, Fixed so that when changing location in drawing
*                      area, backplane number is not incremented.
*_END
************************************************************************/

{
  DSPINFO *d;
  int band;
  XmString xstring;
  int status;
  FLOAT4 dn;
  char buf[4],*sptr;
  int line,samp;
  int sl,ss,el,es;
  INT4 limit[3][2],ret;

  d = &GBLdsp.d[dsp];
/*--------------------------------------------------------------------------
/  If listing core planes find first displayed band.
/-------------------------------------------------------------------------*/
  if (d->list_core) {
    while (!d->band[d->list_band]) {
      d->list_band++;
      if (d->list_band > 2) d->list_band = 0;
    }
    band = d->band[d->list_band];
  }
/*--------------------------------------------------------------------------
/  If listing backplanes, make sure the last backplane hasn't been reached.
/-------------------------------------------------------------------------*/
  else {
    if (d->list_back > d->f.nbck) d->list_back = 1;
    band = d->list_back;
  }


  status = ListGetStats (dsp,band);
  if (status < 0) {
    XBell (xinfo.display,0);
    return;
  }
  else if (status == 0) {
    strcpy (GBLerr.string,"All points in box are invalid, no statistisics ");
    strcat (GBLerr.string,"could be obtained");
    QviewMessage (dsp);
    return;
  }

  d->list_x = d->tool_x;
  d->list_y = d->tool_y;

/*-------------------------------------------------------------------------------
/  If there is more than 1 band displayed, band 0 is the red band, else if there
/  is only one band displayed, it is a mono band.  If listing the backplane
/  region, print the backplane name.
/------------------------------------------------------------------------------*/
  if (d->list_core) {
    if (d->list_band == 0) {
      if (d->band[1] || d->band[2]) {
	sprintf (GBLerr.string,"Band=Red (%d)",band);
      }
      else {
	sprintf (GBLerr.string,"Band=Mono (%d)",band);
      }
    }
    else if (d->list_band == 1) {
      sprintf (GBLerr.string,"Band=Green (%d)",band);
    }
    else {
      sprintf (GBLerr.string,"Band=Blue (%d)",band);
    }
  }
  else {
    sprintf (GBLerr.string,"Backplane=%s",d->f.bck_names[band-1]);
  }

  xstring = XmStringCreateSimple (GBLerr.string);
  XtVaSetValues (d->list_bandLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLerr.string,"Box Size=%dx%d",d->list_box_ns,d->list_box_nl);
  xstring = XmStringCreateSimple (GBLerr.string);
  XtVaSetValues (d->list_boxLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLerr.string,"Samp:Line=%d:%d",d->list_samp,d->list_line);
  xstring = XmStringCreateSimple (GBLerr.string);
  XtVaSetValues (d->list_lsLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLerr.string,"Average=%g",d->list_avg);
  xstring = XmStringCreateSimple (GBLerr.string);
  XtVaSetValues (d->list_avgLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);
    
  sprintf (GBLerr.string,"Standard Deviation=%g",d->list_std);
  xstring = XmStringCreateSimple (GBLerr.string);
  XtVaSetValues (d->list_stdLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLerr.string,"Minimum=%g",d->list_min);
  xstring = XmStringCreateSimple (GBLerr.string);
  XtVaSetValues (d->list_minLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLerr.string,"Maximum=%g",d->list_max);
  xstring = XmStringCreateSimple (GBLerr.string);
  XtVaSetValues (d->list_maxLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  line = d->linelut[d->list_y];
  samp = d->samplut[d->list_x];

  sl = line - 3;
  el = line + 3;

  ss = samp - 3;
  es = samp + 3;

  sptr = GBLwork.big;
  strcpy (sptr,"");
  for (line=sl; line<=el; line++) {
    if ((line < 1) || (line > d->f.nl)) {
      for (samp=1; samp<=7; samp++) strcat (sptr,"            ");
      if (line != el) strcat (sptr,"\n");
    }
    else {
      for (samp=ss; samp<=es; samp++) {
        if ((samp < 1) || (samp > d->f.ns)) {
          strcat (sptr,"            "); 
        }
        else {
          limit[0][0] = samp;
          limit[0][1] = samp;
          limit[1][0] = line;
          limit[1][1] = line;
          limit[2][0] = band;
          limit[2][1] = band;
	  if (d->list_core) q_lio_cbrick (d->f.fid,1,limit,buf,&ret);
	  if (!d->list_core) q_lio_sbrick (d->f.fid,1,3,limit,buf,&ret);
	  if (ret) {
	    strcpy (GBLerr.string,"Unable to read from file ");
	    strcat (GBLerr.string,d->f.name);
	    QviewExit ();
	  }
  
          if (d->list_core && d->f.type == 1) {
            if (*(UINT1 *) buf < VALID_MIN1) {
              strcpy (GBLerr.string,"         NUL");
            }
            else {
              dn = *(UINT1 *) buf;  
              dn = d->f.scale[0] + d->f.scale[1] * (FLOAT4) dn;
              sprintf (GBLerr.string,"%12g",dn); 
            }
          }
          else if (d->list_core && d->f.type == 2) {
            dn = *(INT2 *) buf;  
            if (dn < VALID_MIN2) {
              strcpy (GBLerr.string,"         ");
              if (dn == NULL2) strcat (GBLerr.string,"NUL");
              if (dn == LOW_INSTR_SAT2) strcat (GBLerr.string,"LIS");
              if (dn == LOW_REPR_SAT2) strcat (GBLerr.string,"LRS");
              if (dn == HIGH_INSTR_SAT2) strcat (GBLerr.string,"HIS");
              if (dn == HIGH_REPR_SAT2) strcat (GBLerr.string,"HRS");
            }
            else {
              dn = d->f.scale[0] + d->f.scale[1] * (FLOAT4) dn;
              sprintf (GBLerr.string,"%12g",dn); 
            }
          }
          else {
            dn = *(FLOAT4 *) buf;
            if (dn < VALID_MIN4) {
              strcpy (GBLerr.string,"         ");
              if (dn == NULL4) strcat (GBLerr.string,"NUL");
              if (dn == LOW_INSTR_SAT4) strcat (GBLerr.string,"LIS");
              if (dn == LOW_REPR_SAT4) strcat (GBLerr.string,"LRS");
              if (dn == HIGH_INSTR_SAT4) strcat (GBLerr.string,"HIS");
              if (dn == HIGH_REPR_SAT4) strcat (GBLerr.string,"HRS");
            }
            else {
              sprintf (GBLerr.string,"%12g",dn); 
            }
          }
          strcat (sptr,GBLerr.string);
        }
      }
      if (line != el) strcat (sptr,"\n");
    }
  }
  XmTextSetString (d->list_dnTF,sptr);
  XmTextSetHighlight (d->list_dnTF,291,303,XmHIGHLIGHT_SELECTED);

  return;
}

int ListGetStats (dsp,band) 

{
  DSPINFO *d;
  int line,samp;
  int sl,ss,el,es;
  INT4 limit[3][2];
  UINT1 *ibuf;
  INT2 *wbuf;
  FLOAT4 *fbuf;
  int j,n;
  double sumx, sumxx;
  double rmin,rmax;
  double avg,stdev;
  double a,b;
  INT4 ret;

  d = &GBLdsp.d[dsp];

  line = d->linelut[d->tool_y];
  samp = d->samplut[d->tool_x];

  sl = line - d->list_box_nl / 2;
  el = line + d->list_box_nl / 2;

  ss = samp - d->list_box_ns / 2;
  es = samp + d->list_box_ns / 2;

  if ((sl > d->f.nl) || (ss > d->f.ns) || (el < 1) || (es < 1)) return -1; 

  if (sl < 1) sl = 1;
  if (ss < 1) ss = 1;
  if (el > d->f.nl) el = d->f.nl;
  if (es > d->f.ns) es = d->f.ns;

  limit[0][0] = ss;
  limit[0][1] = es;
  limit[1][0] = sl;
  limit[1][1] = el;

  limit[2][0] = band;
  limit[2][1] = band;
  if (d->list_core) q_lio_cbrick (d->f.fid,1,limit,GBLwork.big,&ret);
  if (!d->list_core) q_lio_sbrick (d->f.fid,1,3,limit,GBLwork.big,&ret);
  if (ret) {
    strcpy (GBLerr.string,"Unable to read from file ");
    strcat (GBLerr.string,d->f.name);
    QviewExit ();
  }

  sumx = 0.0;
  sumxx = 0.0;
  n = 0;
  rmin = VALID_MAX4;
  rmax = VALID_MIN4;

  if (d->list_core && d->f.type == 1) {
    ibuf = (UINT1 *) GBLwork.big;
    for (j=0; j<d->list_box_nl*d->list_box_ns; j++) {
      if ((ibuf[j] >= VALID_MIN1) && (ibuf[j] <= VALID_MAX1)) {
        sumx += (double) ibuf[j];
        sumxx += (double) ibuf[j] * (double) ibuf[j];
        n++;
        if ((FLOAT4) ibuf[j] < rmin) rmin = ibuf[j];
        if ((FLOAT4) ibuf[j] > rmax) rmax = ibuf[j];
      }
    }
  }
  else if (d->list_core && d->f.type == 2) {
    wbuf = (INT2 *) GBLwork.big;
    for (j=0; j<d->list_box_nl*d->list_box_ns; j++) {
      if ((wbuf[j] >= VALID_MIN2) && (wbuf[j] <= VALID_MAX2)) {
        sumx += (double) wbuf[j];
        sumxx += (double) wbuf[j] * (double) wbuf[j];
        n++;
        if (wbuf[j] < rmin) rmin = wbuf[j];
        if (wbuf[j] > rmax) rmax = wbuf[j];
      }
    }
  }
  else {
    fbuf = (FLOAT4 *) GBLwork.big;
    for (j=0; j<d->list_box_nl*d->list_box_ns; j++) {
      if ((fbuf[j] >= VALID_MIN4) && (fbuf[j] <= VALID_MAX4)) {
        sumx += (double) fbuf[j];
        sumxx += (double) fbuf[j] * (double) fbuf[j];
        n++;
        if (fbuf[j] < rmin) rmin = fbuf[j];
        if (fbuf[j] > rmax) rmax = fbuf[j];
      }
    }
  }

  if (n > 0) {
    avg = sumx / (double) n;
    if (d->list_core && d->f.type != 3) {
      avg = d->f.scale[0] + d->f.scale[1] * (FLOAT4) avg;
    }

    if (d->list_core && d->f.type != 3) {
      a = (double) d->f.scale[0];
      b = (double) d->f.scale[1];
      stdev = (double) n * a * a + 2.0 * a * b * sumx + b * b * sumxx;
      stdev = stdev * (double) n;
      stdev -= (double) (n * n) * a * a + 2.0 * (double) n * a * b * sumx;
      stdev -= b * b * sumx * sumx;
      stdev = stdev / (double) (n * n);
      if (stdev < 0.0) stdev = 0.0; 
      stdev = sqrt (stdev);
    }
    else {
      stdev = (double) n * sumxx - sumx * sumx;
      stdev = stdev / (double) (n * n);
      if (stdev < 0.0) stdev = 0.0;
      stdev = sqrt(stdev);
    }

    if (d->list_core && d->f.type != 3) {
      rmin = d->f.scale[0] + d->f.scale[1] * (FLOAT4) rmin;
      rmax = d->f.scale[0] + d->f.scale[1] * (FLOAT4) rmax;
    }

    d->list_avg = avg;
    d->list_std = stdev;
    d->list_min = rmin;
    d->list_max = rmax;
    d->list_line = line;
    d->list_samp = samp;
  }

  d->list_npts = d->list_box_nl * d->list_box_ns;
  d->list_vpts = n;

  return n;
}
