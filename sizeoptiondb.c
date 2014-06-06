#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>

#include "pc2d.h"

struct {
  Widget DB;
  Widget nsTF;
  Widget nlTF;
  Widget okPB;
} GBLsize[V_MAXDSP];

void CreateSizeDB (int dsp)

/***********************************************************************
*
*_Title	CreatesizeDB Creates size option dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	display number 

*_DESC	This routine creates the size option dialog box which allows the
*	user to select the size of the display window 

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  Widget DB;
  Widget RC,RC2;
  Widget   LB;
  Widget   nsTF;
  Widget   nlTF;
  Widget CM;
  Widget   okPB;
  Widget   canPB;
  Widget   helpPB;

  XmString xstring;
  char *sptr;
  int i;
  DSPINFO *d;
  Dimension width,height;
  static Boolean firstime = True;

  void CBsizeDBokPB (Widget id, XtPointer client_data, XtPointer call_data);

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLsize[i].DB = NULL;
  }

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (GBLsize[dsp].DB == NULL) {
    DB = XmCreateFormDialog (d->shell,"sizeDB",NULL,0);
    sprintf (sptr,"Display %d Size",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLsize[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,&GBLsize[dsp].DB);

/*------------------------------------------------------------------------------
/ Create the nl and ns text fields 
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"sizeRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmHORIZONTAL,
                      XmNadjustLast, False,
                      NULL);

    LB = XmCreateLabel (RC,"Samples",NULL,0);
    nsTF = XmCreateTextField (RC,"sizeTF",NULL,0);
    XtVaSetValues (nsTF,XmNcolumns, 4, XmNmaxLength, 4, NULL);

    XtManageChild (LB);
    XtManageChild (nsTF);
    XtManageChild (RC);

    RC2 = XmCreateRowColumn (DB,"sizeDB",NULL,0);
    XtVaSetValues (RC2,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC2,"Lines",NULL,0);
    nlTF = XmCreateTextField (RC2,"sizeTF",NULL,0);
    XtVaSetValues (nlTF,XmNcolumns, 4, XmNmaxLength, 4, NULL);

    XtManageChild (LB);
    XtManageChild (nlTF);
    XtManageChild (RC2);

    GBLsize[dsp].nlTF = nlTF;
    GBLsize[dsp].nsTF = nsTF;

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    CM = XmCreateSeparatorGadget (DB,"sizeDB",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC2,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

    RC = XmCreateRowColumn (DB,"sizeDB",NULL,0);
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
                      XmNspacing, 45,
                      NULL);

    okPB = XmCreatePushButton (RC,"Ok",NULL,0);
    XtAddCallback (okPB,XmNactivateCallback,CBsizeDBokPB,(XtPointer)((int)dsp));
    GBLsize[dsp].okPB = okPB;

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,
                   CBqviewCancelExitPB,(XtPointer)DB);
 
    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (okPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC);
  }

  XtVaGetValues (d->DA,XmNwidth,&width,XmNheight,&height,NULL);

  sprintf (sptr,"%d",(int)width);
  XmTextFieldSetString (GBLsize[dsp].nsTF,sptr);
  
  sprintf (sptr,"%d",(int)height);
  XmTextFieldSetString (GBLsize[dsp].nlTF,sptr);
  
  if (XtIsManaged (GBLsize[dsp].DB)) {
    XtUnmanageChild (GBLsize[dsp].DB);
    XtManageChild (GBLsize[dsp].DB);
  }
  else {
    XtManageChild (GBLsize[dsp].DB);
  }
 
  return;
}



void CBsizeDBokPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBsizeDBokPB callback for size ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Contains dsp number but
*						needs to be cast as an 
*						int
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a user presses the ok button on the size 
*	options dialog box. This routine then obtains the number of lines &
*	samples from the text fields and the checks them for validity and
*	reports any error to the user. If everything is ok it changes the 
*	size of the display.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  int dsp = (int) client_data;
  DSPINFO *d;
  Dimension height,width;
  char *sptr;
  int nl,ns;

  d = &GBLdsp.d[dsp];

  sptr = XmTextFieldGetString (GBLsize[dsp].nsTF);
  sscanf (sptr,"%d",&ns); 
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLsize[dsp].nlTF);
  sscanf (sptr,"%d",&nl); 
  XtFree (sptr);

  if (nl < 1) {
    strcpy (GBLerr.string,"Invalid value for lines");
    QviewMessage (dsp);
    return;
  }

  if (ns < 1) {
    strcpy (GBLerr.string,"Invalid value for samples");
    QviewMessage (dsp);
    return;
  }

  if (nl > xinfo.height) nl = xinfo.height;
  if (ns > xinfo.width) ns = xinfo.width;

  width = ns;
  height = nl;

  XtVaSetValues (d->DA,XmNwidth,width,XmNheight,height,NULL);

  if (id == GBLsize[dsp].okPB) XtUnmanageChild (GBLsize[dsp].DB);
  XSync (xinfo.display,False);

  return;
}
