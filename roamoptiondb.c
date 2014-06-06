#include <Xm/PushB.h>
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
} GBLroam[V_MAXDSP];


void CreateRoamOptionsDB (int dsp)

/***********************************************************************
*
*_Title	CreateRoamOptionsDB Creates roam option dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	display number 

*_DESC	This routine creates the roam option dialog box which allows the
*	user to select the zoom rate when using the roam tool

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  Widget DB;
  Widget RC;
  Widget   LB;
  Widget   roamTF;
  Widget CM;
  Widget   okPB;
  Widget   applyPB;
  Widget   canPB;
  Widget   helpPB;

  XmString xstring;
  char *sptr;
  static Boolean firstime = True;
  int i;
  DSPINFO *d;

  void CBroamOptionsDBokPB (Widget id, XtPointer client_data, 
                            XtPointer call_data);
  void CBroamOptionsDBcancelPB (Widget id, XtPointer client_data, 
                                XtPointer call_data);

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLroam[V_MAXDSP].DB = NULL;
  }

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (GBLroam[dsp].DB == NULL) {
    DB = XmCreateFormDialog (d->shell,"roamDB",NULL,0);
    sprintf (sptr,"Display %d Roam Options",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLroam[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,&GBLroam[dsp].DB);

/*------------------------------------------------------------------------------
/ Create the text field roam widget 
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"roamRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmHORIZONTAL,
                      XmNadjustLast, False,
                      NULL);

    LB = XmCreateLabel (RC,"Roam Percentage",NULL,0);
    roamTF = XmCreateTextField (RC,"roamTF",NULL,0);
    XtVaSetValues (roamTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);
    GBLroam[dsp].TF = roamTF;

    XtManageChild (LB);
    XtManageChild (roamTF);
    XtManageChild (RC);

    CM = XmCreateSeparatorGadget (DB,"roamCM",NULL,0);
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

    RC = XmCreateRowColumn (DB,"roamRC",NULL,0);
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
                   CBroamOptionsDBokPB,(XtPointer)((int)dsp));
    GBLroam[dsp].okPB = okPB;

    applyPB = XmCreatePushButton (RC,"Apply",NULL,0);
    XtAddCallback (applyPB,XmNactivateCallback,
                   CBroamOptionsDBokPB,(XtPointer)((int)dsp));

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,
                   CBqviewCancelExitPB,(XtPointer)DB);
 
    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (okPB);
    XtManageChild (applyPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC);
  }

  sprintf (sptr,"%g",d->roam_percent*100.0);
  XmTextFieldSetString (GBLroam[dsp].TF,sptr);
  
  if (XtIsManaged (GBLroam[dsp].DB)) {
    XtUnmanageChild (GBLroam[dsp].DB);
    XtManageChild (GBLroam[dsp].DB);
  }
  else {
    XtManageChild (GBLroam[dsp].DB);
  }
 
  return;
}



void CBroamOptionsDBokPB (Widget id, XtPointer client_data, 
                          XtPointer call_data)


/***********************************************************************
*
*_Title	CBroamOptionsDBokPB callback for roam ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Contains dsp number but
*						needs to be cast as an 
*						int
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a user press the ok button on the roam
*	options dialog box. This routine then obtains the roam rate from
*	the text field and the checks it for validity and reports any error
*	to the user. If everything is ok it changes the roam rate when
*	the roam tool is in use.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/
{
  int dsp = (int) client_data;
  DSPINFO *d;
  char *sptr;
  double percent;

  d = &GBLdsp.d[dsp];

  sptr = XmTextFieldGetString (GBLroam[dsp].TF);
  sscanf (sptr,"%lf",&percent);
  XtFree (sptr);

  if ((percent < 25.0) || (percent > 100.0)) {
    strcpy (GBLerr.string,"Roam percentage must be greater than 25 and ");
    strcat (GBLerr.string,"less than 100, inclusive");
    QviewMessage (dsp);
    return;
  }

  d->roam_percent = percent / 100.0; 

  if (id == GBLroam[dsp].okPB) XtUnmanageChild (GBLroam[dsp].DB);
  
  return;
}
