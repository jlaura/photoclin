#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <X11/cursorfont.h>
#include "pc2d.h"

void InitDisplay (void)

/***********************************************************************
*
*_Title	InitDisplay - Initializes the display window
*
*_DESC	This routine creates the main display window.
*
*_HIST	Apr 16 2002 Janet Barrett, USGS, Original version created by
*                   extracting code from the Qview routine createdisplay
*                   and modifying it to work with the pc2d program.
*_END
************************************************************************/

{
  Widget TB;
  Widget FML,FMR;
  Widget FML1,FML2,FML3,FML4;
  Widget FMR1,FMR2,FMR3,FMR4;
  Widget SP;
  Widget PT;
  Widget LB;
  Widget RCL2,RCR2;
  Widget RCL3L,RCR3L,RCL3R,RCR3R;
  Widget RCL4L,RCR4L,RCL4R,RCR4R;
  int gblfree;
  XFontStruct *font1;
  XmFontList fontlist;
  Widget CreatePcpTools(Widget FM);

  font1 = XLoadQueryFont (xinfo.display,
          "-adobe-courier-medium-r-normal--12-100-100-100-m-90-iso8859-1");
  fontlist = XmFontListCreate (font1,"charset1");

/******************************************************************************
* Create the display window 
******************************************************************************/
  TB = CreateToolBar (qviewFM);
  XtVaSetValues (TB,XmNtopAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);

  PT = CreatePcpTools (qviewFM);
  XtVaSetValues (PT,XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNbottomOffset,5,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
/*		    XmNborderWidth,2,*/
                    NULL);

  SP = XmCreateSeparator(qviewFM,"SP",NULL,0);
  XtVaSetValues (SP,XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget,PT,
		    XmNbottomOffset,5,
		    XmNleftAttachment,XmATTACH_FORM,
		    XmNrightAttachment,XmATTACH_FORM,
		    NULL);

  FML = XmCreateForm (qviewFM,"FML",NULL,0);
  XtVaSetValues (FML,XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget, SP,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNtopAttachment,XmATTACH_WIDGET,
                    XmNtopWidget, TB,
                    XmNleftOffset,5,
                    XmNtopOffset,5,
		    NULL);

  FMR = XmCreateForm (qviewFM,"FMR",NULL,0);
  XtVaSetValues (FMR,XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget, SP,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNtopAttachment,XmATTACH_WIDGET,
                    XmNtopWidget, TB,
                    XmNleftAttachment,XmATTACH_WIDGET,
                    XmNleftWidget, FML,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    XmNtopOffset,5,
		    NULL);

/*  FML1 = XmCreateForm (FML,"FML1",NULL,0);
  XtVaSetValues (FML1,XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);

  FMR1 = XmCreateForm (FMR,"FMR1",NULL,0);
  XtVaSetValues (FMR1,XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);
*/
/*  GBLdsp.d[0].bandLB = XmCreateLabel (FML1,"Band=",NULL,0);
  XtVaSetValues (GBLdsp.d[0].bandLB,XmNbottomAttachment,XmATTACH_FORM,
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNalignment,XmALIGNMENT_BEGINNING,
                    XmNrecomputeSize,False,
		    XmNfontList,fontlist,
                    NULL);

  GBLdsp.d[1].bandLB = XmCreateLabel (FMR1,"Band=",NULL,0);
  XtVaSetValues (GBLdsp.d[1].bandLB,XmNbottomAttachment,XmATTACH_FORM,
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNalignment,XmALIGNMENT_BEGINNING,
                    XmNrecomputeSize,False,
		    XmNfontList,fontlist,
                    NULL);
*/
  FML2 = XmCreateForm (FML,"FML2",NULL,0);
  XtVaSetValues (FML2,XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);

  FMR2 = XmCreateForm (FMR,"FMR2",NULL,0);
  XtVaSetValues (FMR2,XmNbottomAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);

  RCL2 = XmCreateRowColumn (FML2,"RCL2",NULL,0);
  XtVaSetValues (RCL2,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNleftAttachment,XmATTACH_FORM,
                 XmNrightAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCL2);

  LB = XmCreateLabel (RCL2,"Lat:Lon=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[0].latLB = XmCreateLabel (RCL2,"Off",NULL,0);
  XtVaSetValues (GBLdsp.d[0].latLB,XmNfontList,fontlist,
                    NULL);

  RCR2 = XmCreateRowColumn (FMR2,"RCR2",NULL,0);
  XtVaSetValues (RCR2,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNleftAttachment,XmATTACH_FORM,
                 XmNrightAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCR2);

  LB = XmCreateLabel (RCR2,"Lat:Lon=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[1].latLB = XmCreateLabel (RCR2,"Off",NULL,0);
  XtVaSetValues (GBLdsp.d[1].latLB,XmNfontList,fontlist,
                    NULL);

  FML3 = XmCreateForm (FML,"FML3",NULL,0);
  XtVaSetValues (FML3,XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget, FML2,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);

  FMR3 = XmCreateForm (FMR,"FMR3",NULL,0);
  XtVaSetValues (FMR3,XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget, FMR2,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);

  RCL3L = XmCreateRowColumn (FML3,"RCL3L",NULL,0);
  XtVaSetValues (RCL3L,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNleftAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCL3L);

  RCL3R = XmCreateRowColumn (FML3,"RCL3R",NULL,0);
  XtVaSetValues (RCL3R,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNrightAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCL3R);

  RCR3L = XmCreateRowColumn (FMR3,"RCR3L",NULL,0);
  XtVaSetValues (RCR3L,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNleftAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCR3L);

  RCR3R = XmCreateRowColumn (FMR3,"RCR3R",NULL,0);
  XtVaSetValues (RCR3R,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNrightAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCR3R);

  LB = XmCreateLabel (RCL3L,"Pixel=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[0].dnLB = XmCreateLabel (RCL3L,"Off",NULL,0);
  XtVaSetValues (GBLdsp.d[0].dnLB,XmNfontList,fontlist,
                    NULL);

  LB = XmCreateLabel (RCR3L,"Pixel=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[1].dnLB = XmCreateLabel (RCR3L,"Off",NULL,0);
  XtVaSetValues (GBLdsp.d[1].dnLB,XmNfontList,fontlist,
                    NULL);

  LB = XmCreateLabel (RCL3R,"Scale=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[0].scaleLB = XmCreateLabel (RCL3R," ",NULL,0);
  XtVaSetValues (GBLdsp.d[0].scaleLB,XmNfontList,fontlist,
                    NULL);

  LB = XmCreateLabel (RCR3R,"Scale=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[1].scaleLB = XmCreateLabel (RCR3R," ",NULL,0);
  XtVaSetValues (GBLdsp.d[1].scaleLB,XmNfontList,fontlist,
                    NULL);

  FML4 = XmCreateForm (FML,"FML4",NULL,0);
  XtVaSetValues (FML4,XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget,FML3,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);

  FMR4 = XmCreateForm (FMR,"FMR4",NULL,0);
  XtVaSetValues (FMR4,XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget,FMR3,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNleftOffset,5,
                    XmNrightOffset,5,
                    NULL);

  RCL4L = XmCreateRowColumn (FML4,"RCL4L",NULL,0);
  XtVaSetValues (RCL4L,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNleftAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCL4L);

  RCL4R = XmCreateRowColumn (FML4,"RCL4R",NULL,0);
  XtVaSetValues (RCL4R,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNrightAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCL4R);

  RCR4L = XmCreateRowColumn (FMR4,"RCR4L",NULL,0);
  XtVaSetValues (RCR4L,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNleftAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCR4L);

  RCR4R = XmCreateRowColumn (FMR4,"RCR4R",NULL,0);
  XtVaSetValues (RCR4R,XmNorientation,XmHORIZONTAL,
                 XmNtopAttachment,XmATTACH_FORM,
                 XmNbottomAttachment,XmATTACH_FORM,
                 XmNrightAttachment,XmATTACH_FORM,
		 NULL);
  XtManageChild (RCR4R);

  LB = XmCreateLabel (RCL4L,"Samp:Line=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[0].posLB = XmCreateLabel (RCL4L,"Off",NULL,0);
  XtVaSetValues (GBLdsp.d[0].posLB,XmNfontList,fontlist,
                    NULL);

  LB = XmCreateLabel (RCR4L,"Samp:Line=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[1].posLB = XmCreateLabel (RCR4L,"Off",NULL,0);
  XtVaSetValues (GBLdsp.d[1].posLB,XmNfontList,fontlist,
                    NULL);

  LB = XmCreateLabel (RCL4R,"Area=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[0].infoLB = XmCreateLabel (RCL4R," ",NULL,0);
  XtVaSetValues (GBLdsp.d[0].infoLB,XmNfontList,fontlist,
                    NULL);

  LB = XmCreateLabel (RCR4R,"Area=",NULL,0);
  XtVaSetValues (LB,XmNfontList,fontlist,
                    NULL);
  XtManageChild (LB);
  GBLdsp.d[1].infoLB = XmCreateLabel (RCR4R," ",NULL,0);
  XtVaSetValues (GBLdsp.d[1].infoLB,XmNfontList,fontlist,
                    NULL);

  GBLdsp.d[0].pmap = XCreatePixmap(xinfo.display,xinfo.root,xinfo.width,xinfo.height,xinfo.depth);
  XSetForeground(xinfo.display,xinfo.imgGC,BlackPixelOfScreen(xinfo.screen));
  XFillRectangle(xinfo.display,GBLdsp.d[0].pmap,xinfo.imgGC,0,0,xinfo.width,xinfo.height);

  GBLdsp.d[0].DA = XmCreateDrawingArea (FML,"DA",NULL,0);
  XtVaSetValues (GBLdsp.d[0].DA,XmNwidth,GBLdefault.display_ns,
                    XmNheight,GBLdefault.display_nl,
                    XmNbackground,BlackPixelOfScreen(xinfo.screen),
                    XmNforeground,BlackPixelOfScreen(xinfo.screen),
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
                    XmNrightAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget,FML4,
                    XmNbackgroundPixmap,GBLdsp.d[0].pmap,
                    NULL);
  gblfree = 0;
  XtAddCallback (GBLdsp.d[0].DA,XmNinputCallback,CBinputDA,(XtPointer)gblfree);
  XtAddCallback (GBLdsp.d[0].DA,XmNresizeCallback,CBresizeDA,(XtPointer)gblfree);
  XtAddCallback (GBLdsp.d[0].DA,XmNexposeCallback,CBexposeDA,(XtPointer)gblfree);

  GBLdsp.d[1].pmap = XCreatePixmap(xinfo.display,xinfo.root,xinfo.width,xinfo.height,xinfo.depth);
  XSetForeground(xinfo.display,xinfo.imgGC,BlackPixelOfScreen(xinfo.screen));
  XFillRectangle(xinfo.display,GBLdsp.d[1].pmap,xinfo.imgGC,0,0,xinfo.width,xinfo.height);

  GBLdsp.d[1].DA = XmCreateDrawingArea (FMR,"DA",NULL,0);
  XtVaSetValues (GBLdsp.d[1].DA,XmNwidth,GBLdefault.display_ns,
                    XmNheight,GBLdefault.display_nl,
                    XmNbackground,BlackPixelOfScreen(xinfo.screen),
                    XmNforeground,BlackPixelOfScreen(xinfo.screen),
                    XmNtopAttachment,XmATTACH_FORM,
                    XmNleftAttachment,XmATTACH_FORM,
		    XmNrightAttachment,XmATTACH_FORM,
                    XmNbottomAttachment,XmATTACH_WIDGET,
                    XmNbottomWidget,FMR4,
                    XmNbackgroundPixmap,GBLdsp.d[1].pmap,
                    NULL);
  gblfree = 1;
  XtAddCallback (GBLdsp.d[1].DA,XmNinputCallback,CBinputDA,(XtPointer)gblfree);
  XtAddCallback (GBLdsp.d[1].DA,XmNresizeCallback,CBresizeDA,(XtPointer)gblfree);
  XtAddCallback (GBLdsp.d[1].DA,XmNexposeCallback,CBexposeDA,(XtPointer)gblfree);

  XtManageChild (GBLdsp.d[0].DA);
  XtManageChild (GBLdsp.d[1].DA);
  XtManageChild (GBLdsp.d[0].posLB);
  XtManageChild (GBLdsp.d[1].posLB);
/*  XtManageChild (GBLdsp.d[0].bandLB);
  XtManageChild (GBLdsp.d[1].bandLB);*/
  XtManageChild (GBLdsp.d[0].dnLB);
  XtManageChild (GBLdsp.d[1].dnLB);
  XtManageChild (GBLdsp.d[0].infoLB);
  XtManageChild (GBLdsp.d[1].infoLB);
  XtManageChild (GBLdsp.d[0].scaleLB);
  XtManageChild (GBLdsp.d[1].scaleLB);
  XtManageChild (GBLdsp.d[0].latLB);
  XtManageChild (GBLdsp.d[1].latLB);
  XtManageChild (FML4);
  XtManageChild (FML3);
  XtManageChild (FML2);
/*  XtManageChild (FML1);
  XtManageChild (FMR1);*/
  XtManageChild (FMR2);
  XtManageChild (FMR3);
  XtManageChild (FMR4);
  XtManageChild (TB);
  XtManageChild (PT);
  XtManageChild (FML);
  XtManageChild (FMR);
  XtManageChild (SP);

  return;
}
