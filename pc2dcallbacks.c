#include "pc2d.h"

void CBqviewDestroyWidget (Widget id, XtPointer client_data, 
                           XtPointer call_data)

/***********************************************************************
*
*_Title	CBqviewDestroyWidget - reinitialize widget when it is destroyed	
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget	i	id		I	See X Toolkit Reference Manual
*_Parm	XtPointer	client_data	I	Widget to initialize
*_Parm	XtPointer	call_data	I	See X Toolkit Reference Manaul	
*
*_DESC	This routine will initialize a widget to null when it is destroyed.
*	It is useful for routines which create dialog boxes for a display,
*	and when the display and DB are destroyed the DB widget can be set
*	to NULL to indicate it should be created again.
*
*_HIST	Sep 11 1996 Jeff Anderson, USGS, Flagstaff - Original version
*
*_END
************************************************************************/

{
  Widget *wid = (Widget *) client_data;

  *wid = NULL;

  return;
}



void CBqviewCancelExitPB (Widget id, XtPointer client_data, 
                          XtPointer call_data)

/***********************************************************************
*
*_Title	CBqviewCancelExitPB - Unmanages dialog box due to cancel/exit
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Contains dialog box widget
*						to unmanage
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a user presses the cancel or exit
*	button on a dialog box. It simply unmanages the dialog box which
*	is passed in as client data.

*_HIST	Sep 11 1996 Jeff Anderson, Original version
*_END
************************************************************************/


{
  Widget wid = (Widget) client_data;

  XtUnmanageChild (wid);
  return;
}




void CBqviewHelp (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBqviewHelp - Qview help callback routine for help pushbuttons
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Contains help string??
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a user presses the help button or
*	requests help from qview in some other manner.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Feb 4 2003 Janet Barrett, Modified the message so that it applies
*                  to the pc2d program.
*_END
************************************************************************/

{

  strcpy (GBLerr.string,"Currently there is no online-help for pc2d. ");
  strcat (GBLerr.string,"That is, all help buttons in pc2d will bring up ");
  strcat (GBLerr.string,"this message. To obtain help on pc2d, type ");
  strcat (GBLerr.string,"tutor pc2d in TAE and then help *.");

  QviewMessage (-1);

  return;
}
