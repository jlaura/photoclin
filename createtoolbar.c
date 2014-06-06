#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/DrawnB.h>
#include "pc2d.h"
#include "pc2d_bitmaps.h"

Widget CreateToolBar (Widget FM)
/************************************************************************
*
*_Title CreateToolBar - Create toolbar at top of display window.
*
*_Args  Type   Variable                 I/O     Description
*_Parm  Widget  FM                      I       Main Form id 
*_Parm  Widget  CreateToolBar           O       Output toolbar widget id
 
*_Hist  Jan 01 1996 Jeff Anderson  U.S.G.S. Flagstaff, Original Version
*       Mar 06 1998 Tracie Sucharski, Add measure tool.
*       Dec 09 2001 Janet Barrett, Modified to work with pc2d program.
*
*_End
***********************************************************************/


{
  Widget FM2;
    Widget PB;
    Widget lastPB;
  Pixmap pmap;
  unsigned long fg,bg;
  unsigned long fg2,bg2;

  static Widget list[V_MAXDSP][15];
  int dsp;

/**************************************************************************
* Get the background and foreground colors used by the parent widget and
* then create the row/column for the tool bar
**************************************************************************/

  XtVaGetValues (FM,XmNbackground,&bg,
                    XmNforeground,&fg,
                    NULL);

  FM2 = XmCreateForm (FM,"ToolBar",NULL,0);

  bg2 = WhitePixelOfScreen (xinfo.screen);
  fg2 = BlackPixelOfScreen (xinfo.screen);

  dsp = GBLdsp.free;
  list[dsp][0] = (Widget) dsp;

/**************************************************************************
* Create the zoom tool pushbutton
**************************************************************************/

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)zoom_bits,zoom_width,zoom_height,
                              fg,bg,xinfo.depth);

  PB = XmCreateDrawnButton (FM2,"zoomPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_ZOOM_TOOL),
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);
  
  XtAddCallback (PB,XmNactivateCallback,CBtoolBarPB,(XtPointer) list[dsp]);
  lastPB = PB;
  list[dsp][1] = PB;

/**************************************************************************
* Create the roam tool pushbutton
**************************************************************************/

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)roam_bits,roam_width,roam_height,
                              fg,bg,xinfo.depth);

  PB = XmCreateDrawnButton (FM2,"roamPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,False,
                    XmNuserData,((int) V_ROAM_TOOL),
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_WIDGET,
                    XmNleftWidget,lastPB,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);
  
  XtAddCallback (PB,XmNactivateCallback,CBtoolBarPB,(XtPointer) list[dsp]);
  lastPB = PB;
  list[dsp][2] = PB;

/**************************************************************************
* Create the stretch tool pushbutton
**************************************************************************/

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)stretch_bits,stretch_width,stretch_height,
                              fg,bg,xinfo.depth);

  PB = XmCreateDrawnButton (FM2,"stretchPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,False,
                    XmNuserData,((int) V_STRETCH_TOOL),
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_WIDGET,
                    XmNleftWidget,lastPB,
 		    XmNwidth,31,
		    XmNheight,31,
                   NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolBarPB, (XtPointer) list[dsp]);
  lastPB = PB;
  list[dsp][3] = PB;

/**************************************************************************
* Create the list tool pushbutton
**************************************************************************/

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)text_bits,text_width,text_height,
                              fg,bg,xinfo.depth);

  PB = XmCreateDrawnButton (FM2,"listPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,False,
                    XmNuserData,((int) V_LIST_TOOL),
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_WIDGET,
                    XmNleftWidget,lastPB,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolBarPB, (XtPointer) list[dsp]);
  lastPB = PB;
  list[dsp][4] = PB;

/**************************************************************************
* Create the track pushbutton
**************************************************************************/

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)track_bits,track_width,track_height,
                              fg,bg,xinfo.depth);

  PB = XmCreateDrawnButton (FM2,"trackPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_TRACK_TOOL),
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolBarPB, (XtPointer) list[dsp]);
  lastPB = PB;

/**************************************************************************
* Create the world/fill/full pushbutton
**************************************************************************/

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)world_bits,world_width,world_height,
                              fg,bg,xinfo.depth);

  PB = XmCreateDrawnButton (FM2,"worldPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_WORLD_TOOL),
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_WIDGET,
                    XmNrightWidget,lastPB,
 		    XmNwidth,31,
		    XmNheight,31,
                   NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolBarPB, (XtPointer) list[dsp]);

  return FM2;
}
