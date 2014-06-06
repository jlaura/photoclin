#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ScrollBar.h>

#include "qview.h"

struct {
  Widget DB;
  Widget minmaxPB;
  Widget linearPB;
  Widget binaryPB;
  Widget minSB;
  Widget maxSB;
  Widget nsTF;
  Widget fileTF;
  Widget oneTB;
  Widget allTB;
  Widget okPB;
} GBLstretch[V_MAXDSP];

#define MINMAX 0
#define LINEAR 1
#define BINARY 2

void CreateStretchDB (dsp)

/***********************************************************************
*
*_Title	CreateStretchDB Creates stretch application dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	display number 
*
*_DESC	This routine creates the stretch application dialog box contains
*       a graphical histogram of the image, a dialog box which allows the
*       user to change the stretch, and a display of the stretch pairs.
*
*_HIST	May 14 1997 Tracie Sucharski, USGS, Original version
*
*_END
************************************************************************/
{
  
  Widget DB;
  Widget FM;
  Widget   RC;
  Widget     PD;
  Widget       PB;
  Widget     OM;
  Widget   LB;
  Widget   SB;
  Widget CM;
  Widget   exitPB;
  Widget   helpPB;
  
  XmString xstring;
  char *sptr;
  int i;
  DSPINFO *d;
  Dimension width,height;
  
  
/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/
  
  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];
  
  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLstretch[i].DB = NULL;
  }

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
  
  if (GBLstretch[dsp].DB == NULL) {
    DB = XmCreateFormDialog (d->shell,"stretchDB",NULL,0);
    sprintf (sptr,"Display %d Stretch ",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL,
		   XmNnoResize,True,
		   XmNdialogTitle,xstring,
		   NULL); 
    XmStringFree (xstring);
    GBLstretch[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,&GBLstretch[dsp].DB);
    
/*------------------------------------------------------------------------------
/  Create form containing stretch options.
/-----------------------------------------------------------------------------*/
    
    FM = XmCreateForm (DB,"stretchFM",NULL,0); 
    XtVaSetValues (FM,XmNtopAttachment,XmATTACH_FORM,
		   XmNtopOffset,5,
		   XmNleftAttachment,XmATTACH_FORM,
		   XmNleftOffset,5,
		   XmNrightAttachment,XmATTACH_FORM,
		   XmNrightOffset,5,
		   NULL);
    XtManageChild (FM);
    
    
    
    RC = XmCreateRowColumn (FM,"StretchRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment,XmATTACH_FORM,
		   XmNleftAttachment,XmATTACH_FORM,
		   XmNrightAttachment,XmATTACH_FORM,
		   XmNorientation, XmHORIZONTAL,
		   NULL);
    XtManageChild (RC);
    
    PD = XmCreatePulldownMenu (RC,"StretchPD",NULL,0);
    
    xstring = XmStringCreateSimple ("Method");
    OM = XmCreateOptionMenu (RC,"StretchOM",NULL,0);
    XtVaSetValues (OM,XmNsubMenuId,PD,
		   XmNlabelString,xstring,
		   NULL);
    XmStringFree (xstring);
    XtManageChild (OM);
    
    minmaxPB = XmCreatePushButton (PD,"Min/Max",NULL,0);
    XtAddCallback (PB,XmNactivateCallback,CBstretchtypePB,
                   (XtPointer)((int) MINMAX));
    XtManageChild (PB);
    
    linearPB = XmCreatePushButton (PD,"Linear",NULL,0);
    XtAddCallback (PB,XmNactivateCallback,CBstretchtypePB,
                   (XtPointer)((int) LINEAR));
    XtManageChild (PB);
    
    binaryPB = XmCreatePushButton (PD,"Binary",NULL,0);
    XtAddCallback (PB,XmNactivateCallback,CBstretchtypePB,
                   (XtPointer)((int) BINARY));
    XtManageChild (PB);
    

/*-------------------------------------------------------------------------
/  Create min/max scroll bars.
/------------------------------------------------------------------------*/

    LB = XmCreateLabel (FM,"Minimum DN",NULL,0);
    XtVaSetValues (LB, XmNtopAttachment, XmATTACH_WIDGET,
		   XmNtopWidget, RC,
		   XmNtopOffset, 5,
		   XmNleftAttachment, XmATTACH_FORM,
		   XmNleftOffset, 25,
		   NULL);
		 
    minSB = XmCreateScrollBar (FM,"strminSB",NULL,0);
    XtVaSetValues (minSB, XmNtopAttachment, XmATTACH_WIDGET,
		   XmNtopWidget,LB,
		   XmNtopOffset,5,
		   XmNleftAttachment, XmATTACH_FORM,
		   XmNleftOffset,25,
		   XmNorientation, XmHORIZONTAL, 
		   XmNheight,20,
		   XmNwidth,200,
		   XmNminimum,0,
		   XmNmaximum,255,
		   XmNvalue,127,
		   XmNsliderSize,10,
		   XmNtraversalOn, True,
		   XmNhighlightOnEnter, True,
		   XmNhighlightThickness, 2,
		   NULL);
    
    XtManageChild (minSB);
    XtManageChild (LB);
    
    XtAddCallback (minSB,XmNvalueChangedCallback,CBstrmin,NULL);
    XtAddCallback (minSB,XmNdragCallback,CBstrmin,NULL);

    LB = XmCreateLabel (FM,"Maximum DN",NULL,0);
    XtVaSetValues (LB, XmNtopAttachment, XmATTACH_WIDGET,
		   XmNtopWidget, minSB,
		   XmNtopOffset, 5,

		   XmNleftAttachment, XmATTACH_FORM,
		   XmNleftOffset, 25,
		   NULL);
		 
    maxSB = XmCreateScrollBar (FM,"strmaxSB",NULL,0);
    XtVaSetValues (maxSB, XmNtopAttachment, XmATTACH_WIDGET,
		   XmNtopWidget,LB,
		   XmNtopOffset,5,
		   XmNleftAttachment, XmATTACH_FORM,
		   XmNleftOffset,25,
		   XmNorientation, XmHORIZONTAL, 
		   XmNheight,20,
		   XmNwidth,200,
		   XmNminimum,0,
		   XmNmaximum,255,
		   XmNvalue,127,
		   XmNsliderSize,10,
		   XmNtraversalOn, True,
		   XmNhighlightOnEnter, True,
		   XmNhighlightThickness, 2,
		   NULL);
    
    XtManageChild (maxSB);
    XtManageChild (LB);
    
    XtAddCallback (maxSB,XmNvalueChangedCallback,CBstrmax,NULL);
    XtAddCallback (maxSB,XmNdragCallback,CBstrmax,NULL);




    CM = XmCreateSeparatorGadget (DB,"stretchCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, maxSB,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"stretchRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, CM,
                      XmNtopOffset, 5,
                      XmNbottomAttachment, XmATTACH_FORM,
                      XmNbottomOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNrightAttachment, XmATTACH_FORM,
                      XmNrightOffset, 5,
                      XmNorientation, XmHORIZONTAL,
                      XmNpacking, XmPACK_COLUMN,
                      XmNentryAlignment, XmALIGNMENT_CENTER,
                      XmNspacing, 35,
                      NULL);

    savePB = XmCreatePushButton (RC,"Save",NULL,0);
    XtAddCallback (exitPB,XmNactivateCallback,CBstretchDBsavePB,(XtPointer)dsp);

    exitPB = XmCreatePushButton (RC,"Exit",NULL,0);
    XtAddCallback (exitPB,XmNactivateCallback,CBstretchDBexitPB,(XtPointer)dsp);

    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (savePB);
    XtManageChild (exitPB);
    XtManageChild (helpPB);
    XtManageChild (RC);
  }

  if (!XtIsManaged (d->stretchDB)) {
    XtManageChild (d->stretchDB);
  }
 
  return;
}
