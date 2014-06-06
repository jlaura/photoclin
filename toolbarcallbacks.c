#include "pc2d.h"
#include "spi.h"

void CBtoolBarPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title CBtoolBarPB - callback for tool bar push buttons
*
*_Args  Type            Variable        I/O     Description
*_Parm  Widget          id              I       See Xt toolkit reference 
*                                               manual
*_Parm  XtPointer       client_data     I       Display number
*_Parm  XtPointer       call_data       I       See Xt toolkit reference
*                                               manual
 
*_DESC  This routine is called when the user presses a tool bar push button.
*
*_HIST  Jan 01 1996 Jeff Anderson, Original version
*       Jan 13 1998  TLS, Added flag to indicate if the full image is 
*                     displayed.
*       Mar 07 1998  TLS,  Added measure tool.
*       Mar 30 1999 TLS, Updated check for spi_checklev return value.
*       Mar 16 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*    
*_END
************************************************************************/
 
{
  Widget *list = (Widget *) client_data;
  int button;
  int dsp;
  DSPINFO *d;
  Window win;
  char *wptr;
  XmString xstring;
  int i;

  dsp = (int) list[0];
  XtVaGetValues (id,XmNuserData,&button,NULL);

  d = &GBLdsp.d[dsp];
  win = XtWindow (d->DA);

  XmProcessTraversal (d->DA,XmTRAVERSE_CURRENT);

  switch (button) {
    case V_TRACK_TOOL :
      wptr = (char *) GBLwork.work[0];

      if (d->showpos) {
        XtRemoveEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
                              EHpositionDA,(XtPointer)dsp);
/*        strcpy (wptr,"Line:Samp=Off");*/
        strcpy (wptr,"Off");
        xstring = XmStringCreateSimple (wptr);
        XtVaSetValues (d->posLB,XmNlabelString,xstring,NULL);
        XmStringFree (xstring);
/*        strcpy (wptr,"Pixel=Off");*/
        strcpy (wptr,"Off");
        xstring = XmStringCreateSimple (wptr);
        XtVaSetValues (d->dnLB,XmNlabelString,xstring,NULL);
        XmStringFree (xstring);
/*        strcpy (wptr,"Lat:Lon=Off");*/
        strcpy (wptr,"Off");
        xstring = XmStringCreateSimple (wptr);
        XtVaSetValues (d->latLB,XmNlabelString,xstring,NULL);
        XmStringFree (xstring);
      }
      else {
        XtAddEventHandler (d->DA,PointerMotionMask|LeaveWindowMask,False,
                           EHpositionDA,(XtPointer)dsp);
        strcpy (wptr,"Position=N/A");
        xstring = XmStringCreateSimple (wptr);
        XtVaSetValues (d->posLB,XmNlabelString,xstring,NULL);
        XmStringFree (xstring);
/*        strcpy (wptr,"Pixel=N/A");*/
        strcpy (wptr,"N/A");
        xstring = XmStringCreateSimple (wptr);
        XtVaSetValues (d->dnLB,XmNlabelString,xstring,NULL);
        XmStringFree (xstring);
/*        strcpy (wptr,"Lat:Lon=N/A");*/
        strcpy (wptr,"N/A");
        xstring = XmStringCreateSimple (wptr);
        XtVaSetValues (d->latLB,XmNlabelString,xstring,NULL);
        XmStringFree (xstring);
      }

      d->showpos = !d->showpos;
    break;

    case V_ZOOM_TOOL :
      XtVaSetValues (list[1],XmNpushButtonEnabled,True,NULL);
      XtVaSetValues (list[2],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[3],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[4],XmNpushButtonEnabled,False,NULL);

      for (i=1; i<5; i++) {
        XtUnmapWidget (list[i]);
        XtMapWidget (list[i]);
      }
      d->curtool = V_ZOOM_TOOL;
    break;

    case V_ROAM_TOOL :
      XtVaSetValues (list[1],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[2],XmNpushButtonEnabled,True,NULL);
      XtVaSetValues (list[3],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[4],XmNpushButtonEnabled,False,NULL);

      for (i=1; i<5; i++) {
        XtUnmapWidget (list[i]);
        XtMapWidget (list[i]);
      }
      d->curtool = V_ROAM_TOOL;
    break;

    case V_STRETCH_TOOL :
      if (xinfo.vinfo.class == TrueColor) {
        strcpy (GBLerr.string,"Post stretch capabilities are not ");
        strcat (GBLerr.string,"supported on TrueColor devices");
        QviewMessage (dsp);
        return;
      }

      if (xinfo.vinfo.class == PseudoColor) {
        if (d->band[1] || d->band[2]) {
          strcpy (GBLerr.string,"Post stretch capabilities are not ");
          strcat (GBLerr.string,"available when displaying color ");
          strcat (GBLerr.string,"composites on PseudoColor devices");
          QviewMessage (dsp);
          return;
        }
      }

      XtVaSetValues (list[1],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[2],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[3],XmNpushButtonEnabled,True,NULL);
      XtVaSetValues (list[4],XmNpushButtonEnabled,False,NULL);

      for (i=1; i<5; i++) {
        XtUnmapWidget (list[i]);
        XtMapWidget (list[i]);
      }
      d->curtool = V_STRETCH_TOOL;
    break;

    case V_WORLD_TOOL :
      dsp = 0;
      d = &GBLdsp.d[0];
      d->world = True;
      ZoomFill (dsp);
      dsp = 1;
      d = &GBLdsp.d[1];
      d->world = True;
      ZoomFill (dsp);
    break;

    case V_LIST_TOOL :
      XtVaSetValues (list[1],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[2],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[3],XmNpushButtonEnabled,False,NULL);
      XtVaSetValues (list[4],XmNpushButtonEnabled,True,NULL);

      for (i=1; i<5; i++) {
        XtUnmapWidget (list[i]);
        XtMapWidget (list[i]);
      }

      d->curtool = V_LIST_TOOL;

      CreateListDB (dsp);
    break;
  }

  return;
}
