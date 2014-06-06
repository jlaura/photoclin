#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <stdio.h>
#include <string.h>

#include "pc2d.h"
#include "spi.h"

void GetCubeName (int dsp)

/***********************************************************************
*
*_Title	GetCubeName - Creates file selection box for getting cube name
*
*_DESC	This routine creates a file selection box which allows the user
*	to peruse directory trees and select the file they wish to
*	display.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Nov 07 1996 Tracie Sucharski,  Added the qview resource defaults
*                    for the path and filter.
*       Mar 10 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{
  static Widget FSB = NULL;

  XmString xstring;
  XmString xstring2;
  XmString xstring3;
  Widget PB;
  int i;

  void CBopenFSBokPB (Widget id, XtPointer client_data, XtPointer call_data);

/***************************************************************************
* Make sure too many displays haven't been opened
***************************************************************************/

  for (i=0; i<V_MAXDSP; i++) if (!GBLdsp.used[i]) break;
  if (i >= V_MAXDSP) {
    sprintf (GBLerr.string,"Only %d displays can be open",V_MAXDSP);
    QviewMessage (-1);
    return;
  }
  GBLdsp.free = dsp;

/***************************************************************************
* Create the file selection dialog box
***************************************************************************/

  if (FSB == NULL) {
    FSB = XmCreateFileSelectionDialog (xinfo.topShell,"openFSB",NULL,0);
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

    PB = XmFileSelectionBoxGetChild (FSB,XmDIALOG_OK_BUTTON);
    XtAddCallback (PB,XmNactivateCallback,CBopenFSBokPB,NULL);

    PB = XmFileSelectionBoxGetChild (FSB,XmDIALOG_CANCEL_BUTTON);
    XtAddCallback (PB,XmNactivateCallback,CBqviewCancelExitPB,(XtPointer)FSB);
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




void CBopenFSBokPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBopenFSBokPB - File selection OK button callback
*
*_Args	Type       Variable        I/O	Description
*_Parm	Widget     ID;	            I	See Xt toolkit reference manual
*_Parm	XtPointer  client_data;     I	not used
*_Parm	XtPointer  call_data;       I	See Xt toolkit reference manual
*
*_DESC	This routine is called when the OK button of the file
*       selection button is pushed. It opens the file and verifies
*	it is a valid is cube. Then it calls other routines to obtain
*	loading parameters.
*
*_HIST	Jan 01 1996 Jeff Anderson, USGS, Flagstaff - Original version
*	Mar 10 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*
*_END
************************************************************************/

{

  Widget fileTF;
  char *ptr;

  void CreateOpenDB (void);

  fileTF = XmFileSelectionBoxGetChild (XtParent(id),XmDIALOG_TEXT);
  ptr = XmTextGetString (fileTF);

  if (OpenCube (GBLdsp.free,ptr) == False) {
    GBLdsp.used[GBLdsp.free] = True; 
    XtUnmanageChild (XtParent(id));
    CreateOpenDB ();
  }

  XtFree (ptr);
  return;
}
