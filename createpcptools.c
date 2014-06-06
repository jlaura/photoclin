#include <X11/Intrinsic.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <LiteClue.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/DrawnB.h>
#include <Xm/Separator.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/Scale.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include <Xm/TextF.h>
#include "pc2d.h"
#include "pc2d_bitmaps.h"
#include "pcp.h"
#include "clinom_ops.h"

Widget CreatePcpTools (Widget FM)
/************************************************************************
*
*_Title CreatePcpTools - Create PCP tools at bottom of display window.
*
*_Args  Type   Variable                 I/O     Description
*_Parm  Widget  FM                      I       Main Form id 
*_Parm  Widget  CreatePcpTools          O       Output PCP tool widget id
 
*_Hist  Nov 10 2001 Janet Barrett, USGS Flagstaff, Original Version
*
*_End
***********************************************************************/


{
  Widget liteClue;
  Widget FM2;
  Widget RC,RC1,RC2,RC3,RC4,RC5,RC6,RC7;
  Widget PB;
  Widget RB;
  Widget LB;
  Widget FR1,FR2,FR,FRL,FRR;
  static Widget pcplist[15];
  Pixmap pmap;
  unsigned long fg,bg;
  unsigned long fg2,bg2;
  XmString xstring;
  char *sptr;
  XFontStruct *font1;
  XmFontList fontlist;
  XColor closest;
  XColor exact;

  font1 = XLoadQueryFont (xinfo.display,
          "-adobe-courier-medium-r-normal--12-100-100-100-m-90-iso8859-1");
  fontlist = XmFontListCreate (font1,"charset1");

  sptr = (char *) GBLwork.work[0];

  bg2 = WhitePixelOfScreen (xinfo.screen);
  fg2 = BlackPixelOfScreen (xinfo.screen);

  liteClue = XtCreatePopupShell("popup_shell",xcgLiteClueWidgetClass,
             xinfo.topShell,NULL,0);
  XAllocNamedColor(xinfo.display,xinfo.cmap,"lightyellow",
                   &closest,&exact);
  XtVaSetValues (liteClue,XmNbackground,closest.pixel,NULL);
  XtVaSetValues (liteClue,XmNforeground,fg2,NULL);

/**************************************************************************
* Get the background and foreground colors used by the parent widget and
* then create the row/column for the tool bar
**************************************************************************/

  XtVaGetValues (FM,XmNbackground,&bg,
                    XmNforeground,&fg,
                    NULL);

  FM2 = XmCreateForm (FM,"PcpTools",NULL,0);

  FRL = XmCreateFrame (FM2,"FRL",NULL,0);
  XtVaSetValues (FRL,XmNshadowType,XmSHADOW_ETCHED_IN,
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
/*		    XmNmarginHeight,5,
		    XmNmarginWidth,5,*/
		    NULL);
  XtManageChild (FRL);

  FRR = XmCreateFrame (FM2,"FRR",NULL,0);
  XtVaSetValues (FRR,XmNshadowType,XmSHADOW_ETCHED_IN,
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,FRL,
                    XmNrightAttachment,XmATTACH_FORM,
/*		    XmNmarginHeight,5,
		    XmNmarginWidth,5,*/
		    NULL);
  XtManageChild (FRR);

  RC = XmCreateRowColumn (FRL,"RC",NULL,0);
  XtVaSetValues (RC,XmNorientation,XmHORIZONTAL,
                 NULL);
  XtManageChild (RC);

  RC1 = XmCreateRowColumn (RC,"RC1",NULL,0);
  XtVaSetValues (RC1,XmNorientation,XmVERTICAL,
/*		 XmNspacing,5,*/
		 XmNtopAttachment,XmATTACH_WIDGET,
		 XmNtopWidget,RC,
		 XmNbottomAttachment,XmATTACH_WIDGET,
		 XmNbottomWidget,RC,
		 XmNleftAttachment,XmATTACH_WIDGET,
		 XmNleftWidget,RC,
/*		 XmNmarginHeight,5,
		 XmNmarginWidth,25,*/
                 NULL);
  XtManageChild (RC1);

  RC2 = XmCreateRowColumn (RC1,"RC2",NULL,0);
  XtVaSetValues (RC2,XmNorientation,XmHORIZONTAL,
		 XmNspacing,6,
		 XmNtopAttachment,XmATTACH_WIDGET,
		 XmNtopWidget,RC1,
		 XmNleftAttachment,XmATTACH_WIDGET,
		 XmNleftWidget,RC1,
		 XmNrightAttachment,XmATTACH_WIDGET,
		 XmNrightWidget,RC1,
                 NULL);
  XtManageChild (RC2);

  RC3 = XmCreateRowColumn (RC1,"RC3",NULL,0);
  XtVaSetValues (RC3,XmNorientation,XmHORIZONTAL,
/*		 XmNspacing,120,*/
		 XmNspacing,13,
		 XmNtopAttachment,XmATTACH_WIDGET,
		 XmNtopWidget,RC2,
		 XmNleftAttachment,XmATTACH_WIDGET,
		 XmNleftWidget,RC1,
		 XmNbottomAttachment,XmATTACH_WIDGET,
		 XmNbottomWidget,RC1,
		 XmNrightAttachment,XmATTACH_WIDGET,
		 XmNrightWidget,RC1,
                 NULL);
  XtManageChild (RC3);

  FR = XmCreateFrame (RC1,"FR",NULL,0);
  XtVaSetValues (FR,XmNshadowType,XmSHADOW_ETCHED_IN,
                    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,RC3,
                    XmNbottomAttachment,XmATTACH_WIDGET,
		    XmNbottomWidget,RC1,
                    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC1,
                    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC1,
/*		    XmNmarginHeight,5,
		    XmNmarginWidth,5,*/
		    NULL);
  XtManageChild (FR);

  RC4 = XmCreateRowColumn (FR,"RC4",NULL,0);
  XtVaSetValues (RC4,XmNorientation,XmHORIZONTAL,
		 XmNradioBehavior,True,
		 XmNradioAlwaysOne,True,
                 NULL);
  XtManageChild (RC4);

  RC5 = XmCreateRowColumn (FRR,"RC5",NULL,0);
  XtVaSetValues (RC5,XmNorientation,XmVERTICAL,
/*		 XmNspacing,10,*/
		 XmNtopAttachment,XmATTACH_WIDGET,
		 XmNtopWidget,FRR,
		 XmNleftAttachment,XmATTACH_WIDGET,
		 XmNleftWidget,FRR,
		 XmNrightAttachment,XmATTACH_WIDGET,
		 XmNrightWidget,FRR,
		 XmNbottomAttachment,XmATTACH_WIDGET,
		 XmNbottomWidget,FRR,
/*		 XmNmarginHeight,5,
		 XmNmarginWidth,35,*/
                 NULL);
  XtManageChild (RC5);

  RC7 = XmCreateRowColumn (RC5,"RC7",NULL,0);
  XtVaSetValues (RC7,XmNorientation,XmHORIZONTAL,
                 NULL);
  XtManageChild (RC7);

/*  FR1 = XmCreateFrame (RC7,"FR1",NULL,0);
  XtVaSetValues (FR1,XmNshadowType,XmSHADOW_ETCHED_IN,
                    XmNbottomAttachment,XmATTACH_WIDGET,
		    XmNbottomWidget,RC7,
                    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC7,
                    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,RC7,
		    NULL);
  XtManageChild (FR1);*/

  FR2 = XmCreateFrame (RC7,"FR2",NULL,0);
  XtVaSetValues (FR2,XmNshadowType,XmSHADOW_ETCHED_IN,
                    XmNbottomAttachment,XmATTACH_WIDGET,
		    XmNbottomWidget,RC7,
/*                    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,FR1,*/
                    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,RC7,
                    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC7,
		    NULL);
  XtManageChild (FR2);

/*  RC6 = XmCreateRowColumn (FR1,"RC6",NULL,0);
  XtVaSetValues (RC6,XmNorientation,XmHORIZONTAL,
		 XmNradioBehavior,True,
		 XmNradioAlwaysOne,True,
                 NULL);
  XtManageChild (RC6);*/

  RC7 = XmCreateRowColumn (FR2,"RC7",NULL,0);
  XtVaSetValues (RC7,XmNorientation,XmHORIZONTAL,
		 XmNradioBehavior,True,
		 XmNradioAlwaysOne,True,
                 NULL);
  XtManageChild (RC7);

/**************************************************************************
* Create the go pushbutton
**************************************************************************/

  XAllocNamedColor(xinfo.display,xinfo.cmap,"green",
                   &closest,&exact);

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)go_bits,go_width,go_height,
                              closest.pixel,bg,xinfo.depth);
/*                              fg,bg,xinfo.depth);*/

  PB = XmCreateDrawnButton (RC2,"goPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_GO_BTN),
		    XmNalignment,XmALIGNMENT_CENTER,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);
  
  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  XcgLiteClueAddWidget(liteClue,PB,"Start NR Iteration",0,0);

  pcplist[1] = PB;

/**************************************************************************
* Create the go 1 step pushbutton
**************************************************************************/

  XAllocNamedColor(xinfo.display,xinfo.cmap,"green",
                   &closest,&exact);

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)go1_bits,go1_width,go1_height,
                              closest.pixel,bg,xinfo.depth);
/*                              fg,bg,xinfo.depth);*/

  PB = XmCreateDrawnButton (RC2,"go1PB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_GO1_BTN),
		    XmNalignment,XmALIGNMENT_CENTER,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);
  
  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  XcgLiteClueAddWidget(liteClue,PB,"Do 1 NR Step",0,0);
  pcplist[2] = PB;

/**************************************************************************
* Create the pause pushbutton
**************************************************************************/

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)pause_bits,pause_width,pause_height,
                              fg,bg,xinfo.depth);

  PB = XmCreateDrawnButton (RC2,"pausePB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_PAUSE_BTN),
		    XmNalignment,XmALIGNMENT_CENTER,
 		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  XcgLiteClueAddWidget(liteClue,PB,"Stop NR Iteration",0,0);
  pcplist[3] = PB;

/**************************************************************************
* Create the resolution down pushbutton
**************************************************************************/

  XAllocNamedColor(xinfo.display,xinfo.cmap,"lightgoldenrod1",
                   &closest,&exact);

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)resdn_bits,resdn_width,resdn_height,
                              closest.pixel,bg,xinfo.depth);
/*                              fg,bg,xinfo.depth);*/

  PB = XmCreateDrawnButton (RC2,"resdnPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_RESDN_BTN),
		    XmNalignment,XmALIGNMENT_CENTER,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  XcgLiteClueAddWidget(liteClue,PB,"Decrease Res",0,0);
  pcplist[4] = PB;

/**************************************************************************
* Create the resolution up pushbutton
**************************************************************************/

  XAllocNamedColor(xinfo.display,xinfo.cmap,"lightskyblue",
                   &closest,&exact);

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)resup_bits,resup_width,resup_height,
                              closest.pixel,bg,xinfo.depth);
/*                              fg,bg,xinfo.depth);*/

  PB = XmCreateDrawnButton (RC2,"resupPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_RESUP_BTN),
		    XmNalignment,XmALIGNMENT_CENTER,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  XcgLiteClueAddWidget(liteClue,PB,"Increase Res",0,0);
  XtSetSensitive(PB,False);
  pcplist[5] = PB;

/**************************************************************************
* Create the Done pushbutton
**************************************************************************/

  XAllocNamedColor(xinfo.display,xinfo.cmap,"red",&closest,&exact);

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)stop_bits,stop_width,stop_height,
                              closest.pixel,bg,xinfo.depth);
/*                              fg,bg,xinfo.depth);*/

  PB = XmCreateDrawnButton (RC2,"stopPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_DONE_BTN),
		    XmNalignment,XmALIGNMENT_CENTER,
		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  XcgLiteClueAddWidget(liteClue,PB,"Exit Program",0,0);
  pcplist[6] = PB;

/**************************************************************************
* Create the Back (undo) pushbutton
**************************************************************************/

  pmap = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                              (char *)back_bits,back_width,back_height,
                              fg,bg,xinfo.depth);

  PB = XmCreateDrawnButton (RC2,"backPB",NULL,0);
  XtVaSetValues (PB,XmNlabelPixmap,pmap,
                    XmNlabelType,XmPIXMAP,
                    XmNpushButtonEnabled,True,
                    XmNuserData,((int) V_BACK_BTN),
		    XmNalignment,XmALIGNMENT_CENTER,
 		    XmNwidth,31,
		    XmNheight,31,
                    NULL);
  XtSetSensitive(PB,False);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  XcgLiteClueAddWidget(liteClue,PB,"Undo 1 NR Step",0,0);
  pcplist[7] = PB;

/**************************************************************************
* Create the Azimuth of Characteristics text field
**************************************************************************/
  charazLB = XmCreateLabel (RC5,"Az of Characteristics=",NULL,0);
  sprintf(sptr,"Az of Characteristics=%f",charazgrid);
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (charazLB,XmNfontList,fontlist,
                    XmNlabelString,xstring,
		    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,RC5,
		    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC5,
		    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC5,
		    XmNmarginHeight,0,
		    XmNmarginWidth,0,
                    NULL);
  XtManageChild (charazLB);
  XmStringFree(xstring);

/**************************************************************************
* Create the Resolution factor text field
**************************************************************************/
  fmagLB = XmCreateLabel (RC5,"Resolution factor=1",NULL,0);
  sprintf(sptr,"Resolution factor=1");
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (fmagLB,XmNfontList,fontlist,
                    XmNlabelString,xstring,
		    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,charazLB,
		    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC5,
		    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC5,
		    XmNmarginHeight,0,
		    XmNmarginWidth,0,
                    NULL);
  XtManageChild (fmagLB);
  XmStringFree(xstring);

/**************************************************************************
* Create the RMS right hand side and B error text field
**************************************************************************/
  rmsrhsberLB = XmCreateLabel (RC5,"RMS righthand side,B error=",NULL,0);
  sprintf(sptr,"RMS righthand side,B error=");
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (rmsrhsberLB,XmNfontList,fontlist,
                    XmNlabelString,xstring,
		    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,fmagLB,
		    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC5,
		    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC5,
		    XmNmarginHeight,0,
		    XmNmarginWidth,0,
                    NULL);
  XtManageChild (rmsrhsberLB);
  XmStringFree(xstring);

/**************************************************************************
* Create the Dip,Az of dip (degrees) text field
**************************************************************************/
  dipLB = XmCreateLabel (RC5,"Dip,Az of dip (degrees)=",NULL,0);
  sprintf(sptr,"Dip,Az of dip (degrees)=");
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (dipLB,XmNfontList,fontlist,
                    XmNlabelString,xstring,
		    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,rmsrhsberLB,
		    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC5,
		    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC5,
		    XmNmarginHeight,0,
		    XmNmarginWidth,0,
                    NULL);
  XtManageChild (dipLB);
  XmStringFree(xstring);

/**************************************************************************
* Create the Slope of plane toward Sun in degrees text field
**************************************************************************/
  plnslpLB = XmCreateLabel (RC5,"Slope of plane toward Sun (degrees)=",NULL,0);
  sprintf(sptr,"Slope of plane toward Sun (degrees)=");
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (plnslpLB,XmNfontList,fontlist,
                    XmNlabelString,xstring,
		    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,dipLB,
		    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC5,
		    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC5,
		    XmNmarginHeight,0,
		    XmNmarginWidth,0,
                    NULL);
  XtManageChild (plnslpLB);
  XmStringFree(xstring);

/**************************************************************************
* Create the RMS residual to fit in meters,pixels text field
**************************************************************************/
  rmsresLB = XmCreateLabel (RC5,"RMS residual to fit (m,pixels)=",NULL,0);
  sprintf(sptr,"RMS residual to fit (m,pixels)=");
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (rmsresLB,XmNfontList,fontlist,
                    XmNlabelString,xstring,
		    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,plnslpLB,
		    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC5,
		    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC5,
		    XmNmarginHeight,0,
		    XmNmarginWidth,0,
                    NULL);
  XtManageChild (rmsresLB);
  XmStringFree(xstring);

/**************************************************************************
* Create the Hints text field
**************************************************************************/
  hintsLB = XmCreateLabel (RC5,"Hints/Errors:",NULL,0);
  sprintf(sptr,"Hints/Errors:");
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (hintsLB,XmNfontList,fontlist,
                    XmNlabelString,xstring,
		    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,rmsresLB,
		    XmNbottomAttachment,XmATTACH_WIDGET,
		    XmNbottomWidget,RC5,
		    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC5,
		    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC5,
		    XmNmarginHeight,0,
		    XmNmarginWidth,0,
                    NULL);
  XtManageChild (hintsLB);
  XmStringFree(xstring);

/**************************************************************************
* Create the Errors text field
**************************************************************************/
/*  errorsLB = XmCreateLabel (RC5,"Errors:",NULL,0);
  sprintf(sptr,"Errors:");
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (errorsLB,XmNfontList,fontlist,
                    XmNlabelString,xstring,
		    XmNtopAttachment,XmATTACH_WIDGET,
		    XmNtopWidget,hintsLB,
		    XmNleftAttachment,XmATTACH_WIDGET,
		    XmNleftWidget,RC5,
		    XmNrightAttachment,XmATTACH_WIDGET,
		    XmNrightWidget,RC5,
		    XmNmarginHeight,0,
		    XmNmarginWidth,0,
                    NULL);
  XtManageChild (errorsLB);
  XmStringFree(xstring);
*/
/**************************************************************************
* Create the Smooth (Boxcar) pushbutton
**************************************************************************/

/*  PB = XmCreatePushButton (RC5,"Smooth",NULL,0);
  XtVaSetValues (PB,XmNuserData,((int) V_SMOOTH_BTN),
		    XmNfontList,fontlist,
                    NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  pcplist[7] = PB;*/

/**************************************************************************
* Create the Parameters pushbutton
**************************************************************************/

/*  PB = XmCreatePushButton (RC5,"Parameters",NULL,0);
  XtVaSetValues (PB,XmNuserData,((int) V_PARAM_BTN),
		    XmNfontList,fontlist,
                    NULL);
  XtManageChild (PB);

  XtAddCallback (PB,XmNactivateCallback,CBtoolPcpPB,(XtPointer) pcplist);
  pcplist[8] = PB;*/

/**************************************************************************
* Create the Relaxation slider bar
**************************************************************************/

  WMAXslider = XmCreateScale (RC3,"relaxSB",NULL,0);
  XtVaSetValues (WMAXslider,XmNminimum,0,
                    XmNmaximum,20,
		    XmNdecimalPoints,1,
		    XmNvalue,10,
		    XmNshowValue,True,
		    XmNorientation,XmHORIZONTAL,
		    XtVaTypedArg,XmNtitleString,XmRString,
		    "Relaxation",11,
		    XmNfontList,fontlist,
		    NULL);
  XtManageChild (WMAXslider);

/**************************************************************************
* Create the Maximum SOR step slider bar
**************************************************************************/

  ITMAXslider = XmCreateScale (RC3,"maxsorSB",NULL,0);
  XtVaSetValues (ITMAXslider,XmNminimum,1,
                    XmNmaximum,30,
		    XmNvalue,15,
		    XmNshowValue,True,
		    XmNorientation,XmHORIZONTAL,
		    XtVaTypedArg,XmNtitleString,XmRString,
		    "Max SOR Steps",14,
		    XmNfontList,fontlist,
		    NULL);
  XtManageChild (ITMAXslider);

/**************************************************************************
* Create the Solution Type label
**************************************************************************/

  LB = XmCreateLabel (RC4,"Solution Type:",NULL,0);
  XtVaSetValues (LB,XmNalignment,XmALIGNMENT_BEGINNING,
		    XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);

/**************************************************************************
* Create the SOR toggle button
**************************************************************************/

  SORbtn = XmCreateToggleButton (RC4,"SOR",NULL,0);
  XtVaSetValues (SORbtn,XmNuserData,((int) V_SOR_BTN),
		    XmNfontList,fontlist,
                    NULL);
  XmToggleButtonSetState(SORbtn,True,False);
  XtManageChild (SORbtn);
  pcplist[8] = SORbtn;

/**************************************************************************
* Create the DIR toggle button
**************************************************************************/

  DIRbtn = XmCreateToggleButton (RC4,"Direct",NULL,0);
  XtVaSetValues (DIRbtn,XmNuserData,((int) V_DIRECT_BTN),
		    XmNfontList,fontlist,
                    NULL);
  XtSetSensitive(DIRbtn,True);
  XtManageChild (DIRbtn);
  pcplist[9] = DIRbtn;

/**************************************************************************
* Create the CGM toggle button
**************************************************************************/

  CGMbtn = XmCreateToggleButton (RC4,"CGM",NULL,0);
  XtVaSetValues (CGMbtn,XmNuserData,((int) V_CGM_BTN),
		    XmNfontList,fontlist,
                    NULL);
  XtSetSensitive(CGMbtn,False);
  XtManageChild (CGMbtn);
  pcplist[10] = CGMbtn;

/**************************************************************************
* Create the Display label
**************************************************************************/

/*  LB = XmCreateLabel (RC6,"Display:",NULL,0);
  XtVaSetValues (LB,XmNalignment,XmALIGNMENT_BEGINNING,
		    XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);*/

/**************************************************************************
* Create the Display On toggle button
**************************************************************************/

/*  DSPONbtn = XmCreateToggleButton (RC6,"On",NULL,0);
  XtVaSetValues (DSPONbtn,XmNuserData,((int) V_DSPON_BTN),
		    XmNfontList,fontlist,
                    NULL);
  XmToggleButtonSetState(DSPONbtn,True,False);
  XtManageChild (DSPONbtn);
  pcplist[11] = DSPONbtn;*/

/**************************************************************************
* Create the Display Off toggle button
**************************************************************************/

/*  DSPOFFbtn = XmCreateToggleButton (RC6,"Off",NULL,0);
  XtVaSetValues (DSPOFFbtn,XmNuserData,((int) V_DSPOFF_BTN),
                    XmNfontList,fontlist,
		    NULL);
  XtSetSensitive(DSPOFFbtn,False);
  XtManageChild (DSPOFFbtn);
  pcplist[12] = DSPOFFbtn;*/

/**************************************************************************
* Create the Inject label
**************************************************************************/

  LB = XmCreateLabel (RC7,"Inject:",NULL,0);
  XtVaSetValues (LB,XmNalignment,XmALIGNMENT_BEGINNING,
		    XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);

/**************************************************************************
* Create the Inject Exact toggle button
**************************************************************************/

  EXACTbtn = XmCreateToggleButton (RC7,"Exact",NULL,0);
  XtVaSetValues (EXACTbtn,XmNuserData,((int) V_EXACT_BTN),
		    XmNfontList,fontlist,
                    NULL);
  XmToggleButtonSetState(EXACTbtn,True,False);
  XtManageChild (EXACTbtn);
  pcplist[13] = EXACTbtn;

/**************************************************************************
* Create the Inject Fast toggle button
**************************************************************************/

  FASTbtn = XmCreateToggleButton (RC7,"Fast",NULL,0);
  XtVaSetValues (FASTbtn,XmNuserData,((int) V_FAST_BTN),
                    XmNfontList,fontlist,
		    NULL);
  XtSetSensitive(FASTbtn,True);
  XtManageChild (FASTbtn);
  pcplist[14] = FASTbtn;

  return FM2;
}
