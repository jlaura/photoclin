#include "pc2d.h"

#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/MessageB.h>

struct {
  Widget DB;
  Widget TB[V_MAXDSP];
  Widget regTB;
  Widget regTF;
  Widget okPB;
  int x,y;
  Widget ctlTF;
  double line1,line2;
  double samp1,samp2;
} GBLlink;

void CreateLinkDB (void)

/***********************************************************************
*
*_Title	CreateLinkDB - Creates the link display dialog box
*

*_DESC	This routine creates the dialog box which allows the linking
*	of display for blinking/zooming/roaming/etc
*

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*	Sep 19 1996 JAA, Add registration option
*_END
************************************************************************/
{
  static Widget DB = NULL;
  Widget RC;
  Widget   TB[V_MAXDSP];
  Widget   regTB;
  Widget   LB;
  Widget   regTF;
  Widget CM;
  Widget   okPB;
  Widget   applyPB;
  Widget   canPB;
  Widget   helpPB;

  XmString xstring;
  int i;

  void CBlinkDBokPB (Widget id, XtPointer client_data, XtPointer call_data);

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (DB == NULL) {
    DB = XmCreateFormDialog (xinfo.topShell,"linkDB",NULL,0);
    xstring = XmStringCreateSimple ("Qview Link Options");
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLlink.DB = DB;

/*------------------------------------------------------------------------------
/  Create the display toggle buttons
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"linkRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmVERTICAL,
                      XmNadjustLast, False,
                      NULL);

    for (i=0; i<V_MAXDSP; i++) {
      sprintf (GBLwork.big,"Display %d",i+1);
      TB[i] = XmCreateToggleButton (RC,GBLwork.big,NULL,0);
      XtManageChild (TB[i]);
      GBLlink.TB[i] = TB[i];
    }
    XtManageChild (RC);

    CM = XmCreateSeparatorGadget (DB,"linkCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

    regTB = XmCreateToggleButton (DB,"Register",NULL,0);
    XtVaSetValues (regTB,XmNtopAttachment, XmATTACH_WIDGET,
                         XmNtopWidget, CM,
                         XmNtopOffset, 5,
                         XmNleftAttachment, XmATTACH_FORM,
                         XmNleftOffset, 5,
                         NULL);
    XtManageChild (regTB);
    GBLlink.regTB = regTB;

    LB = XmCreateLabel (DB,"Control Point File:",NULL,0);
    XtVaSetValues (LB,XmNtopAttachment, XmATTACH_WIDGET,
                         XmNtopWidget, regTB,
                         XmNtopOffset, 5,
                         XmNleftAttachment, XmATTACH_FORM,
                         XmNleftOffset, 5,
                         XmNrightAttachment, XmATTACH_FORM,
                         XmNrightOffset, 5,
                         NULL);
    XtManageChild (LB);

    regTF = XmCreateTextField (DB,"regTF",NULL,0);
    XtVaSetValues (regTF,XmNtopAttachment, XmATTACH_WIDGET,
                         XmNtopWidget, LB,
                         XmNtopOffset, 5,
                         XmNleftAttachment, XmATTACH_FORM,
                         XmNleftOffset, 5,
                         XmNrightAttachment, XmATTACH_FORM,
                         XmNrightOffset, 5,
                         NULL);
    XtManageChild (regTF);
    GBLlink.regTF = regTF;

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    CM = XmCreateSeparatorGadget (DB,"linkCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, regTF,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

    RC = XmCreateRowColumn (DB,"linkRC",NULL,0);
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
    XtAddCallback (okPB,XmNactivateCallback,CBlinkDBokPB,NULL);
    GBLlink.okPB = okPB;

    applyPB = XmCreatePushButton (RC,"Apply",NULL,0);
    XtAddCallback (applyPB,XmNactivateCallback,CBlinkDBokPB,NULL);

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,CBqviewCancelExitPB,(XtPointer)DB);
 
    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (okPB);
    XtManageChild (applyPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC);
  }

  for (i=0; i<V_MAXDSP; i++) {
    XtSetSensitive (GBLlink.TB[i],GBLdsp.used[i]);
    XmToggleButtonSetState (GBLlink.TB[i],GBLdsp.linked[i],False);
  }

  if (XtIsManaged (DB)) {
    XtUnmanageChild (DB);
    XtManageChild (DB);
  }
  else {
    XtManageChild (DB);
  }
 
  return;
}




void CBlinkDBokPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBlinkDBokPB - Callback for ok/apply pushbuttons
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I 	Not used	
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the ok or apply pushbutton is pressed
*	on the link dialog box. It checks the toggle buttons and records
*	which displays are linked.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  int i;
  char *ptr,*cptr;

  if (XmToggleButtonGetState(GBLlink.regTB)) {
    if ((!XmToggleButtonGetState (GBLlink.TB[0])) ||
        (!XmToggleButtonGetState (GBLlink.TB[1]))) {
      strcpy (GBLerr.string,"Displays 1 and 2 must be linked in order to ");
      strcat (GBLerr.string,"select control points.");
      QviewMessage (-1);
      return;
    }

    ptr = XmTextFieldGetString (GBLlink.regTF);
    cptr = ptr;
    while ((*cptr == ' ') || (*cptr == '\t')) cptr++;
    if (*cptr == 0) {
      strcpy (GBLerr.string,"A filename must be entered to ");
      strcat (GBLerr.string,"select control points.");
      QviewMessage (-1);
      XtFree (ptr);
      return;
    }

    GBLdsp.tvreg = True;
    XtFree (ptr);
  }
  else {
    GBLdsp.tvreg = False;
  }

  for (i=0; i<V_MAXDSP; i++) {
    GBLdsp.linked[i] = XmToggleButtonGetState (GBLlink.TB[i]);
  } 

  if (id == GBLlink.okPB) {
    XtUnmanageChild (GBLlink.DB);
  }

  return;
}




void SaveRegPoint (void)

{
  Window root_ret,child_ret;
  int root_x,root_y;
  int win_x,win_y;
  unsigned int mask;
  DSPINFO *d1,*d2;
  Dimension height1,height2;
  Dimension width1,width2;

  void CreateCpointDB (void);

  d1 = &GBLdsp.d[0];
  d2 = &GBLdsp.d[1];

  if (GBLdsp.tvreg == True) {
    XtVaGetValues (d1->DA,XmNwidth,&width1,XmNheight,&height1,NULL);
    XtVaGetValues (d2->DA,XmNwidth,&width2,XmNheight,&height2,NULL);

    XQueryPointer (xinfo.display,XtWindow(d1->DA),&root_ret,&child_ret,
                   &root_x,&root_y,&win_x,&win_y,&mask); 

    if (win_x < 0) return;
    if (win_y < 0) return;
    if (win_x >= (int) width1) return;
    if (win_y >= (int) height1) return;
    if (win_x >= (int) width2) return;
    if (win_y >= (int) height2) return;

    GBLlink.x = win_x;
    GBLlink.y = win_y;       

    CreateCpointDB ();
  }

  return;
}



void CreateCpointDB (void)

/***********************************************************************
*
*_Title	CreateCpointDB - Creates the control point dialog for TVREG
*

*_DESC	This routine creates the dialog box which prints the control
*       point that is currently being saved.
*
*_HIST	Sep 19 1996 Jeff Anderson, Original version
*	Jun 02 1997 Tracie Sucharski, Prints out the control point which
*           is being saved.  Automatically increments, control point
*           number and formats the control point file so that random
*           and other programs will be able to read.
*       Jul 10 1997 TLS, Swapped line and sample in control point file.
*       
*_END
************************************************************************/
{

  Widget DB = NULL;

  DSPINFO *d1,*d2;
  char tmpstring[200];
  char tmpstring2[200];
  XmString xstring;
  XmString msg;

  void CBcpointDBokPB (Widget id, XtPointer client_data, XtPointer call_data);
  void CBcpointDBcancelPB (Widget id, XtPointer client_data, XtPointer call_data);

  d1 = &GBLdsp.d[0];
  d2 = &GBLdsp.d[1];

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
  xinfo.cpt++;
  GBLlink.line1 = d1->linelut[GBLlink.y];
  GBLlink.line2 = d2->linelut[GBLlink.y];
  GBLlink.samp1 = d1->samplut[GBLlink.x];
  GBLlink.samp2 = d2->samplut[GBLlink.x];

 
  if (DB == NULL) {
    DB = XmCreateMessageDialog (xinfo.topShell,"cptDB",NULL,0);
    sprintf (tmpstring,"Save Control Point #%d",xinfo.cpt);
    xstring = XmStringCreateSimple (tmpstring);
    sprintf (tmpstring,"The following control point will be saved:\n");
    sprintf (tmpstring2,"%d,%12f,%12f,%12f,%12f\n",xinfo.cpt,GBLlink.line1,GBLlink.samp1,
                      GBLlink.line2,GBLlink.samp2);
    strcat (tmpstring,tmpstring2);
    msg = XmStringCreateLocalized (tmpstring);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
		      XmNmessageString, msg,
                      NULL); 
    XmStringFree (xstring);

    XtAddCallback (DB,XmNokCallback,CBcpointDBokPB,NULL);
    XtAddCallback (DB,XmNcancelCallback,CBcpointDBcancelPB,(XtPointer)DB);
    XtAddCallback (DB,XmNhelpCallback,CBqviewHelp,"");
 
  }

  if (XtIsManaged (DB)) {
    XtUnmanageChild (DB);
    XtManageChild (DB);
  }
  else {
    XtManageChild (DB);
  }
 
  return;
}




void CBcpointDBokPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBcpointDBokPB - Callback for control point ok pushbutton
*
*_DESC	This routine is called when a user pressed the OK button
*       to save a control point.
*
*_HIST	Sep 19 1996 Jeff Anderson, Original version
*	Jun 02 1997 Tracie Sucharski, Saves control point to file.
*           Automatically increments, control point
*           number and formats the control point file so that random
*           and other programs will be able to read.
*       Jul 10 1997 TLS, Swapped line and sample in control point file.
*       
*_END
************************************************************************/
{
  char *filename;
  FILE *fp;

  filename = XmTextFieldGetString (GBLlink.regTF);

  fp = fopen (filename,"a");
  if (fp == NULL) {
    strcpy (GBLerr.string,"Unable to open control point file ");
    strcat (GBLerr.string,filename);
    QviewMessage (-1);
    XtFree (filename);
    return;
  }


  XtFree (filename);

  fprintf (fp,"%d,%.2f,%.2f,%.2f,%.2f\n",xinfo.cpt,GBLlink.line1,GBLlink.samp1,
                                    GBLlink.line2,GBLlink.samp2);

  fclose (fp);

  XtUnmanageChild (id);
  return;
}


void CBcpointDBcancelPB (Widget id, XtPointer client_data, 
                          XtPointer call_data)
/***********************************************************************
*
*_Title CBcpointDBcancelPB - Unmanages dialog box due to cancel/exit
*
*_Args  Type            Variable        I/O     Description
*_Parm  Widget          id              I       See Xt toolkit reference 
*                                               manual
*_Parm  XtPointer       client_data     I       Contains dialog box widget
*                                               to unmanage
*_Parm  XtPointer       call_data       I       See Xt toolkit reference
*                                               manual
 
*_DESC  This routine is called when a user presses the cancel
*       button on the control point dialog box. It decrements the control
*       point number and unmanages the dialog box which  is passed in as
*       client data.
*
*_HIST  Jun 02 1997 Tracie Sucharski, Original version
*_END
************************************************************************/
 
 
{
  Widget wid = (Widget) client_data;
 
  xinfo.cpt--;
  XtUnmanageChild (wid);
  return;

}
