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
  Widget coreTB;
  Widget backTB;
  Widget nlTF;
  Widget nsTF;
  Widget fileTF;
  Widget oneTB;
  Widget allTB;
  Widget okPB;
} GBLlist[V_MAXDSP];

void CreateListOptionsDB (dsp)

/***********************************************************************
*
*_Title	CreateListOptionsDB Creates list option dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	display number 

*_DESC	This routine creates the list option dialog box which allows the
*	user to select the list features (box size, output file, etc)
*	when using the list tool

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Sep 25 1996  Tracie Sucharski,  Added toggle buttons for listing
*                      core vs backplane values.
*       Jan 19 2000  TLS, Get rid of ROI and RECT radio buttons, not
*                      used for now.
*_END
************************************************************************/

{
  Widget DB;
  Widget RB;
  Widget RB2;
  Widget   coreTB;
  Widget   backTB;
  Widget RC,RC2;
  Widget   LB;
  Widget   nlTF;
  Widget   nsTF;
  Widget RC3;
  Widget   fileTF; 
  Widget   oneTB;
  Widget   allTB;
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

  void CBlistOptionsDBokPB (Widget id, XtPointer client_data, 
                            XtPointer call_data);

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLlist[i].DB = NULL;
  }

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (GBLlist[dsp].DB == NULL) {
    DB = XmCreateFormDialog (d->shell,"listDB",NULL,0);
    sprintf (sptr,"Display %d List Options",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLlist[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,&GBLlist[dsp].DB);

/*------------------------------------------------------------------------------
/ Create the list text fields and buttons 
/-----------------------------------------------------------------------------*/
    RB2 = XmCreateRadioBox (DB,"ListRegRB",NULL,0);
    XtVaSetValues (RB2,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, DB,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmHORIZONTAL,
                      XmNadjustLast, False,
                      NULL);

    coreTB = XmCreateToggleButton (RB2,"List Core",NULL,0);
    backTB = XmCreateToggleButton (RB2,"List Backplanes",NULL,0);

    XtManageChild (coreTB);
    XtManageChild (backTB);
    XtManageChild (RB2);

    RC = XmCreateRowColumn (DB,"listRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RB2,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmHORIZONTAL,
                      XmNadjustLast, False,
                      NULL);

    LB = XmCreateLabel (RC,"Number of lines",NULL,0);
    nlTF = XmCreateTextField (RC,"listTF",NULL,0);
    XtVaSetValues (nlTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (nlTF);
    XtManageChild (RC);

    RC2 = XmCreateRowColumn (DB,"listRC",NULL,0);
    XtVaSetValues (RC2,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC2,"Number of samples",NULL,0);
    nsTF = XmCreateTextField (RC2,"listTF",NULL,0);
    XtVaSetValues (nsTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (nsTF);
    XtManageChild (RC2);

    CM = XmCreateSeparatorGadget (DB,"listCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC2,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

    RC3 = XmCreateRowColumn (DB,"listRC",NULL,0);
    XtVaSetValues (RC3,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,CM,
                       XmNtopOffset,8,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC3,"Save file",NULL,0);
    fileTF = XmCreateTextField (RC3,"listTF",NULL,0);
    XtVaSetValues (fileTF,XmNcolumns,20,XmNmaxLength,256,NULL);

    XtManageChild (LB);
    XtManageChild (fileTF);
    XtManageChild (RC3);

    RB = XmCreateRadioBox (DB,"listRB",NULL,0);
    XtVaSetValues (RB,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC3,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmVERTICAL,
                      XmNadjustLast, False,
                      NULL);

    oneTB = XmCreateToggleButton (RB,"Save viewed band",NULL,0);
    allTB = XmCreateToggleButton (RB,"Save all bands",NULL,0);

    XtManageChild (oneTB);
    XtManageChild (allTB);
    XtManageChild (RB);

    GBLlist[dsp].coreTB = coreTB;
    GBLlist[dsp].backTB = backTB;
    GBLlist[dsp].nlTF = nlTF;
    GBLlist[dsp].nsTF = nsTF;
    GBLlist[dsp].fileTF = fileTF;
    GBLlist[dsp].oneTB = oneTB;
    GBLlist[dsp].allTB = allTB;

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    CM = XmCreateSeparatorGadget (DB,"listCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RB,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

    RC = XmCreateRowColumn (DB,"listRC",NULL,0);
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
                   CBlistOptionsDBokPB,(XtPointer)((int)dsp));
    GBLlist[dsp].okPB = okPB;

    applyPB = XmCreatePushButton (RC,"Apply",NULL,0);
    XtAddCallback (applyPB,XmNactivateCallback,
                   CBlistOptionsDBokPB,(XtPointer)((int)dsp));

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

  XmToggleButtonSetState (GBLlist[dsp].coreTB,d->list_core,False);
  XmToggleButtonSetState (GBLlist[dsp].backTB,!d->list_core,False);

  XmTextFieldSetString (GBLlist[dsp].fileTF,d->list_file);

  XmToggleButtonSetState (GBLlist[dsp].oneTB,!d->list_all,False);
  XmToggleButtonSetState (GBLlist[dsp].allTB,d->list_all,False);

  sprintf (sptr,"%d",d->list_box_nl);
  XmTextFieldSetString (GBLlist[dsp].nlTF,sptr);
  
  sprintf (sptr,"%d",d->list_box_ns);
  XmTextFieldSetString (GBLlist[dsp].nsTF,sptr);

  if (XtIsManaged (GBLlist[dsp].DB)) {
    XtUnmanageChild (GBLlist[dsp].DB);
    XtManageChild (GBLlist[dsp].DB);
  }
  else {
    XtManageChild (GBLlist[dsp].DB);
  }
 
  return;
}



void CBlistOptionsDBokPB (Widget id, XtPointer client_data, 
                          XtPointer call_data)

/***********************************************************************
*
*_Title	CBlistOptionsDBokPB callback for list ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Contains dsp number but
*						needs to be cast as an 
*						int
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a user press the ok button on the list
*	options dialog box. This routine then obtains the list options from
*	the text fields and toggle buttons and the checks them for validity 
*	and reports any error to the user. If everything is ok it changes 
*	it updates the options so the list tool will act appropriately.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Sep 25 1996  Tracie Sucharski,  Added toggle buttons for listing
*                      core vs backplane values.
*       Jan 19 2000  TLS, Get rid of ROI/RECT radio buttons
*_END
************************************************************************/

{
  int dsp = (int) client_data;
  DSPINFO *d;
  char *sptr;
  int nl,ns;

  d = &GBLdsp.d[dsp];
  

  sptr = XmTextFieldGetString (GBLlist[dsp].nlTF);
  sscanf (sptr,"%d",&nl);
  XtFree (sptr);
  if ((nl < 1) || (nl > 101) || (nl % 2 == 0)) {
    strcpy (GBLerr.string,"Number of lines must be between 1 and 101, ");
    strcat (GBLerr.string,"inclusive, and odd");
    QviewMessage (dsp);
    return;
  }
    
  sptr = XmTextFieldGetString (GBLlist[dsp].nsTF);
  sscanf (sptr,"%d",&ns);
  XtFree (sptr);
  if ((ns < 1) || (ns > 101) || (ns % 2 == 0)) {
    strcpy (GBLerr.string,"Number of samples must be between 1 and 101, ");
    strcat (GBLerr.string,"inclusive, and odd");
    QviewMessage (dsp);
    return;
  }

  sptr = XmTextFieldGetString (GBLlist[dsp].fileTF);
  strcpy (d->list_file,sptr);
  XtFree (sptr);

  d->list_all = XmToggleButtonGetState (GBLlist[dsp].allTB);

  d->list_box_nl = nl;
  d->list_box_ns = ns;

  if (XmToggleButtonGetState (GBLlist[dsp].coreTB)) {
    d->list_core = True;
  }
  else {
    d->list_core = False;
    if (d->f.nbck == 0) {
      strcpy (GBLerr.string,"This image has no backplanes");
      QviewMessage (dsp);
      return;
    }
  }

  if (id == GBLlist[dsp].okPB) XtUnmanageChild (GBLlist[dsp].DB);
 
  return;
}
