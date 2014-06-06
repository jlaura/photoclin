#include <Xm/TextF.h>

#include "pc2d.h"

void CBlistDBexitPB (Widget id, XtPointer client_data, 
                             XtPointer call_data)

{
  int dsp = (int) client_data;
  DSPINFO *d;

  d = &GBLdsp.d[dsp];
  XtUnmanageChild (XtParent(XtParent(id)));
  
  return;
}



void CBlistDBnextPB (Widget id, XtPointer client_data, 
                              XtPointer call_data)

/***********************************************************************
*
*_Title	CBlistDBnextPB callback for list ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Contains dsp number but
*						needs to be cast as an 
*						int
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	
*
*_DESC	This routine is called when a user presses the next button on the list
*	dialog box. This routine then obtains the list area for the next core
*       band or backplane band.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Sep 26 1996  Tracie Sucharski,  Added backplane listing.
*       Aug 15 1997  TLS, Added a line of header info to list file and
*                      fix bug listing backplanes.
*
*_END
************************************************************************/

{
  int dsp = (int) client_data;
  DSPINFO *d;

  d = &GBLdsp.d[dsp];

  if (d->list_x == -1) {
    XBell (xinfo.display,0);
    return;
  }

  if (d->list_core && (!(d->band[1] || d->band[2]))) {
    XBell (xinfo.display,0);
    return;
  } 

  if (d->list_core) {
    d->list_band++;
    if (d->list_band > 2) d->list_band = 0;
  }
/*------------------------------------------------------------------------
/  Increment backplane.
/-----------------------------------------------------------------------*/
  else {
    d->list_back++;
    if (d->list_back > d->f.nbck) d->list_back = 1;
  }

  /*
  d->tool_x = d->list_x;
  d->tool_y = d->list_y;
  */
  ListArea (dsp);

  return;
}



void CBlistDBsavePB (Widget id, XtPointer client_data, 
                              XtPointer call_data)

{
  int dsp = (int) client_data;
  DSPINFO *d;
  int band,n;
  int header;
  int sband,eband;
  FILE *fp;

  d = &GBLdsp.d[dsp];

  if (d->list_x == -1) {
    XBell (xinfo.display,0);
    return;
  }

  if (d->list_all) {
    sband = 1;
    if (d->list_core) eband = d->f.nb;
    else eband = d->f.nbck;
  }
  else {
    if (d->list_core) sband = d->band[d->list_band];
    else sband = d->list_back;
    eband = sband;
  }

  if (strcmp(d->list_file,"")) {
/*------------------------------------------------------------------------
/  Determine if file exists.  If not, once it is opened, write a line
/  of header info.
/-----------------------------------------------------------------------*/
    header = 0;
    if (u_file_exist (d->list_file) == TRUE) header = 1;
    fp = fopen (d->list_file,"a");
    if (fp == NULL) {
      sprintf (GBLerr.string,"Unable to open list file %s",d->list_file);
      QviewMessage (dsp);
      return;
    }
    if (header == 0)
      fprintf (fp," Samp    Line Band       Average      Std Dev             "
                  "Minimum        Maximum   Valid Pts  "
                  "Total Pts\n");

    for (band=sband; band<=eband; band++) {
      n = ListGetStats (dsp,band);
      if (n > 0) {
        fprintf (fp,"%5d %5d %3d %14g %14g %14g %14g %10d %10d\n",
                 d->list_samp,d->list_line,band,d->list_avg,d->list_std,
                 d->list_min,d->list_max,d->list_vpts,d->list_npts);
      }
    }

    fclose (fp);
  }

  return;
}
