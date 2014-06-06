#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>

#include "pc2d.h"

struct {
  Widget DB;
  Widget lowTF;
  Widget hiTF;
  Widget okPB;
} GBLstr[V_MAXDSP];


void CreateStretchOptionsDB (dsp)


/***********************************************************************
*
*_Title	CreateStretchOptionsDB Creates stretch option dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	display number 

*_DESC	This routine creates the stretch option dialog box which allows the
*	user to select the stretch low and high percentages for the 
*	stretch tool

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  Widget DB;
  Widget RC,RC2;
  Widget   LB;
  Widget   lowTF;
  Widget   hiTF;
  Widget CM;
  Widget   okPB;
  Widget   applyPB;
  Widget   canPB;
  Widget   helpPB;

  XmString xstring;
  char *sptr;
  int i;
  DSPINFO *d;
  static Boolean firstime = True;

  void CBstretchOptionsDBokPB (Widget id, XtPointer client_data, 
                               XtPointer call_data);

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLstr[i].DB = NULL;
  } 

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (GBLstr[dsp].DB == NULL) {
    DB = XmCreateFormDialog (d->shell,"strDB",NULL,0);
    sprintf (sptr,"Display %d Stretch Options",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLstr[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,&GBLstr[dsp].DB);

/*------------------------------------------------------------------------------
/  Create the low/high percentage text fields
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"stretchRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmHORIZONTAL,
                      XmNadjustLast, False,
                      NULL);

    LB = XmCreateLabel (RC,"Low percentage",NULL,0);
    lowTF = XmCreateTextField (RC,"stretchTF",NULL,0);
    XtVaSetValues (lowTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (lowTF);
    XtManageChild (RC);

    RC2 = XmCreateRowColumn (DB,"stretchRC",NULL,0);
    XtVaSetValues (RC2,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC2,"High percentage",NULL,0);
    hiTF = XmCreateTextField (RC2,"stretchTF",NULL,0);
    XtVaSetValues (hiTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (hiTF);
    XtManageChild (RC2);

    GBLstr[dsp].lowTF = lowTF;
    GBLstr[dsp].hiTF = hiTF;

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    CM = XmCreateSeparatorGadget (DB,"stretchCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC2,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

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

    okPB = XmCreatePushButton (RC,"Ok",NULL,0);
    XtAddCallback (okPB,XmNactivateCallback,
                   CBstretchOptionsDBokPB,(XtPointer)((int)dsp));
    GBLstr[dsp].okPB = okPB;

    applyPB = XmCreatePushButton (RC,"Apply",NULL,0);
    XtAddCallback (applyPB,XmNactivateCallback,
                   CBstretchOptionsDBokPB,(XtPointer)((int)dsp));

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,
                   CBqviewCancelExitPB,DB);
 
    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (okPB);
    XtManageChild (applyPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC);
  }

  sprintf (sptr,"%g",GBLdsp.d[dsp].stretch_lowper);
  XmTextFieldSetString (GBLstr[dsp].lowTF,sptr);
  
  sprintf (sptr,"%g",GBLdsp.d[dsp].stretch_hiper);
  XmTextFieldSetString (GBLstr[dsp].hiTF,sptr);
  
  if (XtIsManaged (GBLstr[dsp].DB)) {
    XtUnmanageChild (GBLstr[dsp].DB);
    XtManageChild (GBLstr[dsp].DB);
  }
  else {
    XtManageChild (GBLstr[dsp].DB);
  }
 
  return;
}



void CBstretchOptionsDBokPB (Widget id, XtPointer client_data, 
                             XtPointer call_data)

/***********************************************************************
*
*_Title	CBstretchOptionsDBokPB callback for stretch ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Contains dsp number but
*						needs to be cast as an 
*						int
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a user press the ok button on the stretch 
*	options dialog box. This routine then obtains the stretch percentages
* 	from the text fields and the checks for validity and reports any 
*	errorsto the user. If everything is ok it changes the percentages
*	when the stretch tool is in use.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  int dsp = (int) client_data;
  DSPINFO *d;
  char *sptr;
  double lowper,hiper;

  d = &GBLdsp.d[dsp];

  sptr = XmTextFieldGetString (GBLstr[dsp].lowTF);
  sscanf (sptr,"%lf",&lowper);
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLstr[dsp].hiTF);
  sscanf (sptr,"%lf",&hiper);
  XtFree (sptr);

  if ((lowper <= 0.0) || (lowper >= 100.0)) {
    strcpy (GBLerr.string,"Invalid value for low percentage");
    QviewMessage (dsp);
    return;
  }

  if ((hiper <= 0.0) || (hiper >= 100.0) || (lowper >= hiper)) {
    strcpy (GBLerr.string,"Invalid value for high percentage");
    QviewMessage (dsp);
    return;
  }

  d->stretch_lowper = lowper;
  d->stretch_hiper = hiper;

  if (id == GBLstr[dsp].okPB) XtUnmanageChild (GBLstr[dsp].DB);
  
  return;
}
