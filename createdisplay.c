#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/Label.h>
#include <X11/cursorfont.h>
#include "pc2d.h"

void CreateDisplay (void)

/***********************************************************************
*
*_Title	CreateDisplay - Creates the display window
*
*_DESC	This routine creates the main display window.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Sep 25 1996  Tracie Sucharski,  Added global variable to indicate
*                     whether the core or backplane region is listed.
*       Nov 13 1996  Tracie Sucharski,  Added capability of reading a
*                     resource file for qview defaults.
*       Dec 02 1996  TLS, Changed the right attachment on the lat/lon
*                     label, so that larger fonts don't cut off info.
*       Jan 29 1998  TLS, Open temporary file for the saving of the
*                     ZoomFill region.
*       Mar 18 1998  TLS, Add measure tool option.
*       Mar 23 1998  TLS, Write temporary files to /tmp.
*       Mar 25 1998  TLS, Changed names of variables pertaining
*                      to world button from temp to fill.
*       May 05 1998  TLS, Added default for measure file.
*       Nov 14 1998  TLS, Add defaults for reseau options.
*       Jan 19 2000  TLS, Get rid of roi.
*       Jul 19 2001  Janet Barrett, modified to work with the pc2d
*                    program.
*       Jun 18 2008 Kris Becker Updated Mac conditional to work for 
*                       (Intel/Leopard) Macs; added test for NULL pointer from
*                       XCreateImage
*_END
************************************************************************/

{
  char string[256];
  DSPINFO *d;
  Widget widlist[2];

  XtWorkProcId popup_resource_errors;

  Cursor myCursor;
  Window window;

  char prefix[80];
  char *ptr;
  char *ptr2;

  int ndsp;

/******************************************************************************
* Initializations
******************************************************************************/

  ndsp = GBLdsp.free+1;
  d = &GBLdsp.d[GBLdsp.free];
  sprintf (string,"Display %d:  ",GBLdsp.free+1);
  strcat (string,d->f.name);


  ptr = (char *) GBLwork.work[0];
  ptr2 = (char *) GBLwork.work[1];
/******************************************************************************
*  Indicates that the image written out is to be saved to a temp file.
******************************************************************************/

  strcpy (prefix,"/tmp/qview");
  ptr2 = tmpnam (ptr); 
  if (ptr == NULL) ptr = ptr2;
  ptr = strrchr (ptr,'/') + 1;
  strcat (prefix,ptr);
  strcpy (d->fillfile,prefix);
  d->fill_fp = fopen (d->fillfile,"w+");
  if (d->fill_fp == NULL) {
    strcpy (GBLerr.string,"Cannot open world-fill file ");
    strcpy (GBLerr.string,d->fillfile);
    QviewExit ();
  }
  d->world = True;
  d->save_tmp = True;
  d->read_tmp = False;

/******************************************************************************
* Create and initialize the private color map 
******************************************************************************/

  d->cmap = XCreateColormap (xinfo.display,xinfo.root,
                             xinfo.vinfo.visual,AllocNone);
  LoadColorMap (d);

/******************************************************************************
* Create ximage for display
******************************************************************************/

  d->ximgptr = (char *) malloc(xinfo.height*xinfo.width*BYTEDEPTH);
  d->ximg = XCreateImage (xinfo.display,xinfo.vinfo.visual,xinfo.depth,
                          ZPixmap,0,d->ximgptr,xinfo.width,
                          xinfo.height,BITMAP_PAD,xinfo.width*BYTEDEPTH);
  if (d->ximg == NULL) {
    strcpy (GBLerr.string,"XCreateImage returning Null pointer for createdisplay"); 
    u_error ("PC2D",GBLerr.string,-1,1);
    QviewExit ();
  }

  d->ximg->bits_per_pixel = xinfo.depth;

/******************************************************************************
* Create and initialize the pixmap 
******************************************************************************/

  d->pmap = XCreatePixmap (xinfo.display,xinfo.root,xinfo.width,xinfo.height,
                           xinfo.depth);
  XSetForeground (xinfo.display,xinfo.imgGC,BlackPixelOfScreen(xinfo.screen));
  XFillRectangle (xinfo.display,d->pmap,xinfo.imgGC,
                  0,0,xinfo.width,xinfo.height);

/******************************************************************************
* Create the display window 
******************************************************************************/
  d->shell = xinfo.topShell;

  XtVaSetValues (d->DA,XmNcolormap,d->cmap,
                       XmNbackgroundPixmap,d->pmap,NULL);

  popup_resource_errors = XtAppAddWorkProc (xinfo.appContext, 
                               QviewResourceErrors,xinfo.topShell);
  XSync (xinfo.display,False);

  window = XtWindow (d->DA);
  if (GBLdefault.cursor != -1) {
    myCursor = XCreateFontCursor (xinfo.display, GBLdefault.cursor);
    XDefineCursor (xinfo.display, window, myCursor);
  }

  XmProcessTraversal (d->DA,XmTRAVERSE_CURRENT);


  d->sl = d->scs_sl;
  d->ss = d->scs_ss;
  d->inc = (double) (d->scs_el - d->scs_sl + 1) / GBLdefault.display_nl;
  if (d->inc < (double) (d->scs_es - d->scs_ss + 1) / GBLdefault.display_ns) {
    d->inc = (double) (d->scs_es - d->scs_ss + 1) / GBLdefault.display_ns;
  }

  d->linelut = (double *) malloc (xinfo.height * sizeof (double));
  d->samplut = (double *) malloc (xinfo.width * sizeof (double));

/* Set default values for toolbar functions */

  d->zoom_factor = GBLdefault.zoom_factor;
  d->roam_percent = GBLdefault.roam_percent;
  d->stretch_lowper = GBLdefault.stretch_loper;
  d->stretch_hiper = GBLdefault.stretch_hiper;

  d->list_firstime = True;

  if (strcmp(GBLdefault.list,"List Core") == 0) {
    d->list_core = True;
  }
  else {
    d->list_core = False;
  }
  d->list_box_nl = GBLdefault.list_box_nl;
  d->list_box_ns = GBLdefault.list_box_ns;
  if (strcmp(GBLdefault.list_file,"") == 0) {
    sprintf (d->list_file,"qlist%d.dat",GBLdsp.free+1);
  }
  else {
    strcpy ( d->list_file, GBLdefault.list_file );
  }
  d->list_all = False;
  d->list_band = 0;
  d->list_back = 1;
  d->list_x = -1;
  d->list_y = -1;

  d->rx_point = False;
  d->rx_full_line = False;
  d->rx_se_line = False;
  d->rx_rect = False;
  if ( strstr(GBLdefault.rx_mode, "Point") != NULL ) d->rx_point = True;
  if ( strstr(GBLdefault.rx_mode, "Horiz") != NULL ) d->rx_full_line = True;
  if ( strstr(GBLdefault.rx_mode, "Start") != NULL ) d->rx_se_line = True;
  if ( strstr(GBLdefault.rx_mode, "Rect") != NULL ) d->rx_rect = True;

  d->rx_null = False;
  d->rx_lsat = False;
  d->rx_hsat = False;
  d->rx_dn = False;
  if ( strstr(GBLdefault.rx_value, "Null") != NULL) d->rx_null = True;
  if ( strstr(GBLdefault.rx_value, "Low") != NULL) d->rx_lsat = True;
  if ( strstr(GBLdefault.rx_value, "High") != NULL) d->rx_hsat = True;
  if ( strstr(GBLdefault.rx_value, "Dn") != NULL) d->rx_dn = True;

  d->rx_dnval = GBLdefault.rx_dn;
  d->rx_all = False;
  d->rx_undodn = NULL;
  d->rx_line = NULL;
  d->rx_samp = NULL;
  d->rx_undopix = 0;
  d->rx_readwrite = False;

  d->curtool = V_ZOOM_TOOL;
  d->showpos = GBLdefault.showpos;
  if (d->showpos)  
     XtAddEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
		         EHpositionDA,(XtPointer) GBLdsp.free);
  d->measure_isis = False;
  d->measure_cart = False;
  if ( strstr(GBLdefault.measure_mode, "ISIS") != NULL ) 
         d->measure_isis = True;
  if ( strstr(GBLdefault.measure_mode, "Cartographic") != NULL ) 
         d->measure_cart = True;
  strcpy (d->measure_file,"measure.dat");

  d->reseau_box = 5;
  d->reseau_line_width = 2;
  d->listDB = NULL;
  XSync (xinfo.display,False);
  LoadCube (GBLdsp.free);

  widlist[0] = d->DA;
  widlist[1] = d->shell;
/*    widlist[1] = xinfo.topShell;*/

  XtSetWMColormapWindows (d->shell,widlist,2);
/*    XtSetWMColormapWindows (xinfo.topShell,widlist,2);*/
  XSync (xinfo.display,False);

  return;
}
