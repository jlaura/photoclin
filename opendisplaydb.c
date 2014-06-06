#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>

#include "pc2d.h"
#include "pcp.h"
#include "clinom_msg.h"

struct {
  Widget DB;
  Widget imgRB0,imgRB1;
  Widget zRB0,zRB1;
  Widget dzRB0,dzRB1;
  Widget synRB0,synRB1;
  Widget difRB0,difRB1;
  Widget okPB;
} GBLopendsp;

void CreateOpenDisplayDB (void)

/***********************************************************************
*
*_Title	CreateOpenDisplayDB Creates display options dialog box
*
*_DESC	This routine creates the dialog box which allows the user to
*       determine the images that will be displayed in the left and
*       right displays.
*
*_HIST	Jun 17 2002 Janet Barrett, Original version
*_END
************************************************************************/

{
  static Widget DB = NULL;
  Widget RC,RC0,RC1;
  Widget LB0,LB1;
  Widget RB0,RB1;
  Widget imgRB0,imgRB1;
  Widget zRB0,zRB1;
  Widget dzRB0,dzRB1;
  Widget synRB0,synRB1;
  Widget difRB0,difRB1;
  Widget CMh,CMv;
  Widget okPB;
  Widget applyPB;
  Widget canPB;

  XmString xstring;
  char *sptr;

  void CBOpenDisplayDBokPB (Widget id, XtPointer client_data, 
                            XtPointer call_data);

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  sptr = (char *) GBLwork.work[0];

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (DB == NULL) {
    DB = XmCreateFormDialog (xinfo.topShell,"opendspDB",NULL,0);
    sprintf (sptr,"Open Display Options");
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLopendsp.DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,DB);

/*------------------------------------------------------------------------------
/ Create the left and right radio boxes and buttons
/-----------------------------------------------------------------------------*/

    RC0 = XmCreateRowColumn (DB,"opendspRC0",NULL,0);
    XtVaSetValues (RC0,XmNtopAttachment, XmATTACH_FORM,
                       XmNtopOffset, 5,
		       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5,
		       XmNorientation, XmVERTICAL,
		       XmNadjustLast, False,
		       NULL);
    XtManageChild(RC0);

    CMh = XmCreateSeparatorGadget (DB,"opendspCMh",NULL,0);
    XtVaSetValues (CMh,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC0,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild(CMh);

    CMv = XmCreateSeparatorGadget (DB,"opendspCMv",NULL,0);
    XtVaSetValues (CMv,XmNtopAttachment, XmATTACH_FORM,
                      XmNleftAttachment, XmATTACH_WIDGET,
		      XmNleftWidget, RC0,
		      XmNleftOffset, 5,
                      XmNbottomAttachment, XmATTACH_WIDGET,
		      XmNbottomWidget, CMh,
		      XmNorientation, XmVERTICAL,
                      NULL);
    XtManageChild(CMv);

    RC1 = XmCreateRowColumn (DB,"opendspRC1",NULL,0);
    XtVaSetValues (RC1,XmNtopAttachment, XmATTACH_FORM,
		       XmNleftAttachment, XmATTACH_WIDGET,
		       XmNleftWidget, CMv,
		       XmNleftOffset, 5,
		       XmNtopOffset, 5,
		       XmNrightAttachment, XmATTACH_FORM,
		       XmNrightOffset, 5,
		       XmNorientation, XmVERTICAL,
		       XmNadjustLast, False,
		       NULL);
    XtManageChild(RC1);

    LB0 = XmCreateLabel (RC0,"Left Display",NULL,0);
    XtVaSetValues (LB0,XmNalignment,XmALIGNMENT_CENTER,
                   NULL);
    XtManageChild(LB0);

    RB0 = XmCreateRadioBox (RC0,"opendspRB0",NULL,0);
    XtManageChild(RB0);

    imgRB0 = XmCreateToggleButton (RB0,"Image",NULL,0);
    XmToggleButtonSetState(imgRB0,True,False);
    XtManageChild(imgRB0);
    GBLopendsp.imgRB0 = imgRB0;

    zRB0 = XmCreateToggleButton (RB0,"Elevations (Z)",NULL,0);
    XtManageChild(zRB0);
    GBLopendsp.zRB0 = zRB0;

    dzRB0 = XmCreateToggleButton (RB0,"Z Increment",NULL,0);
    XtManageChild(dzRB0);
    GBLopendsp.dzRB0 = dzRB0;

    synRB0 = XmCreateToggleButton (RB0,"Synth Image from Z",NULL,0);
    XtManageChild(synRB0);
    GBLopendsp.synRB0 = synRB0;

    difRB0 = XmCreateToggleButton (RB0,"Image - Synth",NULL,0);
    XtManageChild(difRB0);
    GBLopendsp.difRB0 = difRB0;

    LB1 = XmCreateLabel (RC1,"Right Display",NULL,0);
    XtVaSetValues (LB1,XmNalignment,XmALIGNMENT_CENTER,
                   NULL);
    XtManageChild(LB1);

    RB1 = XmCreateRadioBox (RC1,"opendspRB1",NULL,0);
    XtManageChild(RB1);

    imgRB1 = XmCreateToggleButton (RB1,"Image",NULL,0);
    XtManageChild(imgRB1);
    GBLopendsp.imgRB1 = imgRB1;

    zRB1 = XmCreateToggleButton (RB1,"Elevations (Z)",NULL,0);
    XmToggleButtonSetState(zRB1,True,False);
    XtManageChild(zRB1);
    GBLopendsp.zRB1 = zRB1;

    dzRB1 = XmCreateToggleButton (RB1,"Z Increment",NULL,0);
    XtManageChild(dzRB1);
    GBLopendsp.dzRB1 = dzRB1;

    synRB1 = XmCreateToggleButton (RB1,"Synth Image from Z",NULL,0);
    XtManageChild(synRB1);
    GBLopendsp.synRB1 = synRB1;

    difRB1 = XmCreateToggleButton (RB1,"Image - Synth",NULL,0);
    XtManageChild(difRB1);
    GBLopendsp.difRB1 = difRB1;

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"opendspRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, CMh,
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
                      XmNspacing, 125,
                      NULL);

    okPB = XmCreatePushButton (RC,"Ok",NULL,0);
    XtAddCallback (okPB,XmNactivateCallback,
                   CBOpenDisplayDBokPB,DB);
    GBLopendsp.okPB = okPB;

    applyPB = XmCreatePushButton (RC,"Apply",NULL,0);
    XtAddCallback (applyPB,XmNactivateCallback,
                   CBOpenDisplayDBokPB,DB);

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,
                   CBqviewCancelExitPB,DB);

    XtManageChild (okPB);
    XtManageChild (applyPB);
    XtManageChild (canPB);
    XtManageChild (RC);
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


void CBOpenDisplayDBokPB (Widget id, XtPointer client_data, 
                          XtPointer call_data)

/***********************************************************************
*
*_Title	CBOpenDisplayDBokPB callback for open display ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	See Xt toolkit reference
*						manual	
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	
*
*_DESC	This routine is called when a user press the ok button on the 
*       open display dialog box. Based on the user's selections, this
*       routine will display the requested data in the left/right displays.
*
*_HIST	Jun 18 2002 Janet Barrett, Original version
*_END
************************************************************************/

{
  void UpdateDisplay (Boolean str);

  if (XmToggleButtonGetState(GBLopendsp.imgRB0)){
    if (LeftDsp[0] != True) {
      LeftDsp[0] = True;
      LeftDsp[1] = False;
      LeftDsp[2] = False;
      LeftDsp[3] = False;
      LeftDsp[4] = False;
      UpdateDisplay(True);
    }
  } else if (XmToggleButtonGetState(GBLopendsp.zRB0)) {
    if (LeftDsp[1] != True) {
      LeftDsp[0] = False;
      LeftDsp[1] = True;
      LeftDsp[2] = False;
      LeftDsp[3] = False;
      LeftDsp[4] = False;
      UpdateDisplay(True);
    }
  } else if (XmToggleButtonGetState(GBLopendsp.dzRB0)) {
    if (LeftDsp[2] != True) {
      LeftDsp[0] = False;
      LeftDsp[1] = False;
      LeftDsp[2] = True;
      LeftDsp[3] = False;
      LeftDsp[4] = False;
      UpdateDisplay(True);
    }
  } else if (XmToggleButtonGetState(GBLopendsp.synRB0)) {
    if (LeftDsp[3] != True) {
      LeftDsp[0] = False;
      LeftDsp[1] = False;
      LeftDsp[2] = False;
      LeftDsp[3] = True;
      LeftDsp[4] = False;
      UpdateDisplay(True);
    }
  } else if (XmToggleButtonGetState(GBLopendsp.difRB0)) {
    if (LeftDsp[4] != True) {
      LeftDsp[0] = False;
      LeftDsp[1] = False;
      LeftDsp[2] = False;
      LeftDsp[3] = False;
      LeftDsp[4] = True;
      UpdateDisplay(True);
    }
  }

  if (XmToggleButtonGetState(GBLopendsp.imgRB1)){
    if (RightDsp[0] != True) {
      RightDsp[0] = True;
      RightDsp[1] = False;
      RightDsp[2] = False;
      RightDsp[3] = False;
      RightDsp[4] = False;
      UpdateDisplay(True);
    }
  } else if (XmToggleButtonGetState(GBLopendsp.zRB1)) {
    if (RightDsp[1] != True) {
      RightDsp[0] = False;
      RightDsp[1] = True;
      RightDsp[2] = False;
      RightDsp[3] = False;
      RightDsp[4] = False;
      UpdateDisplay(True);
    }
  } else if (XmToggleButtonGetState(GBLopendsp.dzRB1)) {
    if (RightDsp[2] != True) {
      RightDsp[0] = False;
      RightDsp[1] = False;
      RightDsp[2] = True;
      RightDsp[3] = False;
      RightDsp[4] = False;
      UpdateDisplay(True);
    }
  } else if (XmToggleButtonGetState(GBLopendsp.synRB1)) {
    if (RightDsp[3] != True) {
      RightDsp[0] = False;
      RightDsp[1] = False;
      RightDsp[2] = False;
      RightDsp[3] = True;
      RightDsp[4] = False;
      UpdateDisplay(True);
    }
  } else if (XmToggleButtonGetState(GBLopendsp.difRB1)) {
    if (RightDsp[4] != True) {
      RightDsp[0] = False;
      RightDsp[1] = False;
      RightDsp[2] = False;
      RightDsp[3] = False;
      RightDsp[4] = True;
      UpdateDisplay(True);
    }
  }

  if (id == GBLopendsp.okPB) XtUnmanageChild (GBLopendsp.DB);
 
  return;
}
