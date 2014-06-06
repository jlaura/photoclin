#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include "pc2d.h"

Widget CreatePanel (Widget MW)

/***********************************************************************
*
*_Title	CreatePanel - Creates panel buttons on main window
*
*_Args	Type   Variable        I/O	Description
*_Parm	Widget	MW		I	Main Window widget id
*_Parm	Widget	CreatePanel	O	Widget id of output panel	

*_DESC	This routine creates the link/blink/etc buttons on the 
*	main window.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       May 6 2002 Janet Barrett, Modified to work with the pc2d
*                  program.
*_END
************************************************************************/

{
  Widget RC;
    Widget linkPB;
    Widget junkPB;

  void CBpanelLinkPB (Widget id, XtPointer client_data, XtPointer call_data);

  RC = XmCreateRowColumn (MW,"panelRC",NULL,0);
  XtVaSetValues (RC,XmNadjustLast,False,
                    XmNentryAlignment,XmALIGNMENT_CENTER,
                    XmNorientation,XmHORIZONTAL,
                    NULL);

  linkPB = XmCreatePushButton (RC,"Link",NULL,0);
  XtAddCallback (linkPB,XmNactivateCallback,CBpanelLinkPB,NULL);
  XtManageChild (linkPB);

  junkPB = XmCreateLabel (RC,"      ",NULL,0);
  XtManageChild (junkPB);

  return RC;
}


void CBpanelLinkPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBpanelLinkPB - callback for link pushbutton	
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Not Used
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the link pushbutton is pressed. It
*	calls the routine to create the link dialog box.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       May 6 2002 Janet Barrett, Modified to work with the pc2d
*                  program.
*_END
************************************************************************/

{
  void CreateLinkDB (void);

  CreateLinkDB ();
  return;
}
