#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>

#include <stdio.h>
#include <string.h>

#include "pc2d.h"
#include "pcp.h"
#include "clinom_msg.h"

struct {
  Widget DB;
  Widget savefileTB;
  Widget savenofileTB;
  Widget okPB;
} GBLdon;

void CreateDoneOptionsDB (void)

/***********************************************************************
*
*_Title CreateDoneOptionsDB Creates done option dialog box
*
*_DESC  This routine creates the done option dialog box which
*       allows the user to determine if an output file will be
*       created right before the program quits
*
*_HIST  Jan 13 2002 Janet Barrett, USGS, Original version
*_END
************************************************************************/

{
  static Widget DB = NULL;
  Widget RC,RC1,RB;
  Widget savefileTB;
  Widget savenofileTB;
  Widget CM;
  Widget okPB;
  Widget canPB;
  Widget helpPB;

  XmString xstring;
  char *sptr;

  void CBdoneOptionsDBokPB (Widget id, XtPointer client_data,
                               XtPointer call_data);

  sptr = (char *) GBLwork.work[0];

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/

  if (DB == NULL) {
    DB = XmCreateFormDialog (xinfo.topShell,"doneDB",NULL,0);
    sprintf (sptr,"Output File Options");
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
		      XmNdialogTitle,xstring,
		      NULL);
    XmStringFree (xstring);
    GBLdon.DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,DB);

/*------------------------------------------------------------------------------
/  Create the radio box and radio buttons
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"doneRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
		      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
		      XmNorientation, XmVERTICAL,
		      XmNadjustLast, False,
		      NULL);

    RB = XmCreateRadioBox (RC,"doneRB",NULL,0);

    savefileTB = XmCreateToggleButton (RB,"Save final topographic model to a file",NULL,0);
    XmToggleButtonSetState(savefileTB,True,False);
    savenofileTB = XmCreateToggleButton (RB,"Don't save final topographic model",NULL,0);
    XtSetSensitive(savenofileTB,True);

    GBLdon.savefileTB = savefileTB;
    GBLdon.savenofileTB = savenofileTB;

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    CM = XmCreateSeparatorGadget (DB,"doneCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
	              XmNtopWidget, RC,
                      XmNtopOffset, 5,
		      XmNleftAttachment, XmATTACH_FORM,
		      XmNrightAttachment, XmATTACH_FORM,
		      NULL);

    RC1 = XmCreateRowColumn (DB,"doneRC1",NULL,0);
    XtVaSetValues (RC1,XmNtopAttachment, XmATTACH_WIDGET,
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

    okPB = XmCreatePushButton (RC1,"Ok",NULL,0);
    XtAddCallback (okPB,XmNactivateCallback,
	                CBdoneOptionsDBokPB,DB);
    GBLdon.okPB = okPB;

    canPB = XmCreatePushButton (RC1,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,
                   CBqviewCancelExitPB,DB);

    helpPB = XmCreatePushButton (RC1,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");

    XtManageChild (savenofileTB);
    XtManageChild (savefileTB);
    XtManageChild (RB);
    XtManageChild (RC);
    XtManageChild (CM);
    XtManageChild (okPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC1);
  }

  if (XtIsManaged(DB)) {
    XtUnmanageChild(DB);
    XtManageChild(DB);
  } else {
    XtManageChild(DB);
  }

  return;
}


void CBdoneOptionsDBokPB (Widget id, XtPointer client_data,
                             XtPointer call_data)

/***********************************************************************
*
*_Title CBdoneOptionsDBokPB callback for Save Output File ok
*                            pushbutton
*
*_Args  Type            Variable        I/O     Description
*_Parm  Widget          id              I       See Xt toolkit reference
*                                               manual
*_Parm  XtPointer       client_data     I       See Xt toolkit reference
*                                               manual
*_Parm  XtPointer       call_data       I       See Xt toolkit reference
*                                               manual

*_DESC  This routine is called when a user presses the ok button on the
*       done dialog box. This routine then obtains the user preference
*       from the radio buttons and saves the output file if specified.

*_HIST  Jan 13 2002 Janet Barrett, USGS, Original version
*_END
************************************************************************/

{
  INT4 i,ret;
  void GetTopoName();
  INT4 WriteTopo(void);

  XtUnmanageChild(GBLdon.DB);

  if (XmToggleButtonGetState(GBLdon.savefileTB)) {
    if (mag != 0) {
      strcpy(C_MSG->pcpmsg,
        "The image must be at maximum resolution before it can be saved.");
	pcpmessage(C_MSG->pcpmsg);
	return;
    } else {
/*      GetTopoName();*/
      if (WriteTopo()) return;
    }
  } 
  remove(pcpdatafile[0]);
  remove(pcpdatafile[1]);
  remove(pcpdatafile[2]);
  remove(pcpdatafile[3]);
  remove(pcpdatafile[4]);
  for (i=0; i<15; i++) {
    if (u_file_exist(pcpprevzfile[i])) 
      remove(pcpprevzfile[i]);
    if (u_file_exist(pcpprevrmsfile[i])) 
      remove(pcpprevrmsfile[i]);
  }
  (void) q_close(fid,1,&ret);
  QviewExit();

  return;
}


void GetTopoName(void)

/***********************************************************************
*
*_Title	GetTopoName - Creates file selection box for getting topo name
*
*_DESC	This routine creates a file selection box which allows the user
*	to peruse directory trees and select the file they wish to
*	display.
*
*_HIST	Jan 13 2002 Janet Barrett, USGS, Original version
*_END
************************************************************************/

{
  static Widget FSB = NULL;
  Widget FSBtext;
  XmString xstring;
  XmString xstring2;
  XmString xstring3;

  void CBgetFSBokPB (Widget id, XtPointer client_data, XtPointer call_data);

/***************************************************************************
* Create the file selection dialog box
***************************************************************************/

  if (FSB == NULL) {
    FSB = XmCreateFileSelectionDialog (xinfo.topShell,"getFSB",NULL,0);
    xstring = XmStringCreateSimple ("File Selection");
    xstring2 = XmStringCreateSimple (GBLdefault.filter);
    xstring3 = XmStringCreateSimple (GBLdefault.path);
    XtVaSetValues (FSB,
                   XmNdialogStyle, XmDIALOG_MODELESS,
                   XmNdialogTitle, xstring,
                   XmNpattern, xstring2,
		   XmNdirectory, xstring3,
                   XmNautoUnmanage,False,
                   NULL);
    XmStringFree (xstring);
    XmStringFree (xstring2);
    XmStringFree (xstring3);

    FSBtext = XmFileSelectionBoxGetChild(FSB,XmDIALOG_TEXT);

    XtAddCallback (FSB,XmNokCallback,CBgetFSBokPB,FSBtext);
    XtAddCallback (FSB,XmNcancelCallback,CBqviewCancelExitPB,(XtPointer)FSB);
  }

/***************************************************************************
* Manage the dialog box
***************************************************************************/

  if (XtIsManaged (FSB)) {
    XtUnmanageChild (FSB);
    XtManageChild (FSB);
  }
  else {
    XtManageChild (FSB);
  }

  return;
}


void CBgetFSBokPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBgetFSBokPB - File selection OK button callback
*
*_Args	Type       Variable        I/O	Description
*_Parm	Widget     id;	            I	See Xt toolkit reference manual
*_Parm	XtPointer  client_data;     I	not used
*_Parm	XtPointer  call_data;       I	See Xt toolkit reference manual
*
*_DESC	This routine is called when the OK button of the file
*       selection button is pushed. It gets the name of the file that
*       will be used to store the final topographic model in.
*
*_HIST	Jan 13 2002 Janet Barrett, USGS, Flagstaff - Original version
*
*_END
************************************************************************/

{
  char *ptr;
  INT4 i,ret;
  INT4 WriteTopo(void);

  ptr = XmTextGetString ((Widget) client_data);

  strcpy(topofile,ptr);
  XtUnmanageChild (XtParent((Widget) client_data));
  if (strlen(topofile) == 0 || 
      strncmp(&(topofile[strlen(topofile)-1]),"/",1) == 0) {
    strcpy(C_MSG->pcpmsg,"No output file name was specified");
    pcpmessage(C_MSG->pcpmsg);
    return;
  }
  (void) u_ver_flspec(topofile,"cub",topofile,sizeof(topofile),&ret);
  if (ret != 0) {
    strcpy(C_MSG->pcpmsg,"Output file name is invalid:        ");
    strcat(C_MSG->pcpmsg,topofile);
    pcpmessage(C_MSG->pcpmsg);
    return;
  }
  if (u_file_exist(topofile)) {
    strcpy(C_MSG->pcpmsg,"Output file already exists:         ");
    strcat(C_MSG->pcpmsg,topofile);
    pcpmessage(C_MSG->pcpmsg);
    return;
  }

  if (WriteTopo()) return;

  XtFree (ptr);

  remove(pcpdatafile[0]);
  remove(pcpdatafile[1]);
  remove(pcpdatafile[2]);
  remove(pcpdatafile[3]);
  remove(pcpdatafile[4]);
  for (i=0; i<15; i++) {
    if (u_file_exist(pcpprevzfile[i])) 
      remove(pcpprevzfile[i]);
    if (u_file_exist(pcpprevrmsfile[i])) 
      remove(pcpprevrmsfile[i]);
  }
  (void) q_close(fid,1,&ret);
  QviewExit();

  return;
}
