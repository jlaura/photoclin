#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>

#include "pc2d.h"

struct {
  Widget DB;
  Widget TF;
  Widget okPB;
} GBLzoom[V_MAXDSP];


void CreateZoomOptionsDB (int dsp)


/***********************************************************************
*
*_Title	CreateZoomOptionsDB Creates zoom option dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	display number 

*_DESC	This routine creates the zoom option dialog box which allows the
*	user to select the zoom factor when using the zoom tool

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  Widget DB;
  Widget   RC;
  Widget     LB;
  Widget     zoomTF;
  Widget   CM;
  Widget     okPB;
  Widget     applyPB;
  Widget     canPB;
  Widget     helpPB;

  XmString xstring;
  char *sptr;
  DSPINFO *d;
  static Boolean firstime = True;
  int i;

  void CBzoomOptionsDBokPB (Widget id, XtPointer client_data, 
                            XtPointer call_data);
  void CBzoomOptionsDBcancelPB (Widget id, XtPointer client_data, 
                                XtPointer call_data);

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLzoom[i].DB = NULL;
  }

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.

/-----------------------------------------------------------------------------*/
 
  if (GBLzoom[dsp].DB == NULL) {
    DB = XmCreateFormDialog (d->shell,"zoomDB",NULL,0);
    sprintf (sptr,"Display %d Zoom Options",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLzoom[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,
                   (XtPointer)&GBLzoom[dsp].DB);

/*------------------------------------------------------------------------------
/  Create zoom text field
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"zoomRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmHORIZONTAL,
                      XmNadjustLast, False,
                      NULL);

    LB = XmCreateLabel (RC,"Zoom Rate",NULL,0);
    zoomTF = XmCreateTextField (RC,"zoomTF",NULL,0);
    XtVaSetValues (zoomTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);
    GBLzoom[dsp].TF = zoomTF;

    XtManageChild (LB);
    XtManageChild (zoomTF);
    XtManageChild (RC);

    CM = XmCreateSeparatorGadget (DB,"zoomCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"zoomRC",NULL,0);
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
    XtAddCallback (okPB,XmNactivateCallback,CBzoomOptionsDBokPB,
                   (XtPointer)((int)dsp));
    GBLzoom[dsp].okPB = okPB;

    applyPB = XmCreatePushButton (RC,"Apply",NULL,0);
    XtAddCallback (applyPB,XmNactivateCallback,CBzoomOptionsDBokPB,
                   (XtPointer)((int)dsp));

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,CBqviewCancelExitPB,
                   (XtPointer)DB);
 
    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (okPB);
    XtManageChild (applyPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC);
  }

  sprintf (sptr,"%g",d->zoom_factor);
  XmTextFieldSetString (GBLzoom[dsp].TF,sptr);
  
  if (XtIsManaged (GBLzoom[dsp].DB)) {
    XtUnmanageChild (GBLzoom[dsp].DB);
    XtManageChild (GBLzoom[dsp].DB);
  }
  else {
    XtManageChild (GBLzoom[dsp].DB);
  }
 
  return;
}



void CBzoomOptionsDBokPB (Widget id, XtPointer client_data, 
                          XtPointer call_data)

/***********************************************************************
*
*_Title	CBzoomOptionsDBokPB callback for zoom ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Contains dsp number but
*						needs to be cast as an 
*						int
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a user press the ok button on the zoom
*	options dialog box. This routine then obtains the zoom factor from
*	the text field and the checks it for validity and reports any error
*	to the user. If everything is ok it changes the zoom factor when
*	the zoom tool is in use.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/


{
  int dsp = (int) client_data;
  DSPINFO *d;
  char *sptr;
  double factor;

  d = &GBLdsp.d[dsp];

  sptr = XmTextFieldGetString (GBLzoom[dsp].TF);
  sscanf (sptr,"%lf",&factor);
  XtFree (sptr);

  if ((factor <= 1.0) || (factor > 10.0)) {
    strcpy (GBLerr.string,"Zoom factor must be greater than one and ");
    strcat (GBLerr.string,"less than or equal to ten");
    QviewMessage (dsp);
    return;
  }

  d->zoom_factor = factor;

  if (id == GBLzoom[dsp].okPB) XtUnmanageChild (GBLzoom[dsp].DB);
  
  return;
}
