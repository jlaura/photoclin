#include <Xm/RowColumn.h>

#include "qview.h"

Widget CreateMenu (Widget MW)

/***********************************************************************
*
*_Title	CreateMenu - Creates pulldown menus for initial window
*
*_Args	Type   Variable        I/O	Description
*_Parm	Widget	MW		I	Widget for the main window
*_Parm	Widget	CreateMenu	O	Widget of the pulldown menu

*_DESC	This routine creates the pulldown menus for the intial display
*	containing the panel buttons (e.g., link,blink,etc).

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Sep 21 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{

  Widget MB,RC;
  Widget widget;

  XmString file;
  XmString   opendsp;
  XmString   exit;
  XmString help;
  XmString   onpc2d;

  void CBfileMB (Widget id, XtPointer client_data, XtPointer call_data);

/*------------------------------------------------------------------------------
/ Create the horizontal menu bar
/-----------------------------------------------------------------------------*/

  file = XmStringCreateSimple ("File");
  help = XmStringCreateSimple ("Help");

  MB = XmVaCreateSimpleMenuBar (MW,"qviewMB",
                                XmVaCASCADEBUTTON,file,'F',
                                XmVaCASCADEBUTTON,help,'H',
                                NULL);
/*
  widget = XtNameToWidget (MB,"button_0");
  XtAddCallback (widget,XmNhelpCallback,CBqviewHelp,"File");

  widget = XtNameToWidget (MB,"button_1");
  XtAddCallback (widget,XmNhelpCallback,CBqviewHelp,"Help");
*/

  XmStringFree (file);
  XmStringFree (help);

/*------------------------------------------------------------------------------
/ Create the File pulldown menu
/
/ Note: if more items are added to this list then qview.c needs to be modified.
/       A change should be made to the code in qview.c which sets up the
/       callback when the program exits due to the close requested by the window
/	manager.
/-----------------------------------------------------------------------------*/

  opendsp = XmStringCreateSimple ("Open Display...");
  exit = XmStringCreateSimple ("Exit");

  RC = XmVaCreateSimplePulldownMenu (MB,"QviewMB",0,CBfileMB,
                                     XmVaPUSHBUTTON,opendsp,'O',NULL,NULL,
                                     XmVaPUSHBUTTON,exit,'E',NULL,NULL,
                                     NULL);
/*
  widget = XtNameToWidget (RC,"button_0");
  XtAddCallback (widget,XmNhelpCallback,CBqviewHelp,"FileOpenBwDB");

  widget = XtNameToWidget (RC,"button_1");
  XtAddCallback (widget,XmNhelpCallback,CBqviewHelp,"FileExit");
*/

  XmStringFree (opendsp);
  XmStringFree (exit);

/*------------------------------------------------------------------------------
/ Create the Help pulldown menu
/-----------------------------------------------------------------------------*/

  onpc2d = XmStringCreateSimple ("On PC2D...");

  RC = XmVaCreateSimplePulldownMenu (MB,"QviewMB",1,NULL,
                                     XmVaPUSHBUTTON,onpc2d,'P',NULL,NULL,
                                     NULL);

  widget = XtNameToWidget (RC,"button_0");
  XtAddCallback (widget,XmNactivateCallback,CBqviewHelp,"");
  XtAddCallback (widget,XmNhelpCallback,CBqviewHelp,"");

  XmStringFree (onpc2d);

/*------------------------------------------------------------------------------
/ Tell the menubar which button is the help menu
/-----------------------------------------------------------------------------*/

  if (widget = XtNameToWidget (MB,"button_1")) {
    XtVaSetValues (MB,XmNmenuHelpWidget,widget,NULL);
  }

  return MB;

}


void CBfileMB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBfileMB - callback for the file pulldown menu 
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I 	Button pressed	
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a menu item is selected from the
*	file pulldown menu. If open... was selected then the client_data
*	contains a 0 if exit was selected then client_data contains a 1.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Sep 21 2001 Janet Barrett, Modified to work with the pc2d 
*                   program.
*_END
************************************************************************/

{
  int button = (int) client_data;
  void GetCubeName (void);

  switch (button) {
    case 0:		/* Open display */
      GetCubeName (); 
    break;

    case 1:		/* Exit qview */
      QviewExit ();
    break;
  }

  return;
}
