#include <stdlib.h>
#include <string.h>
#include <Xm/MessageB.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "pc2d.h"
#include "pcp.h"

void pcpmessage (CHAR *pcpmsg)

/***********************************************************************
*
*_Title	pcpmessage - Used to report warnings to the user	
*
*_DESC	When this routine is called it will pop up a dialog which
*	contains the error message in the global variable pcpmsg
*

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Nov 21 1996 Tracie Sucharski, Check for newline character.
*       Apr 30 2002 Janet Barrett, Renamed to pcpmessage and added
*                   modifications so that it will work with the
*                   pc2d program.
*_END
************************************************************************/

{
  int pos;
  int comma,colon,brace,slash,newline;
  char *errstring,newstring[512],save[4];
  Widget shell;
  XmString xstring;
  static Widget DB = NULL;

  shell = xinfo.topShell;

/****************************************************************************
* Create the dialog if it has not already been done
*****************************************************************************/

  if (DB == NULL) {
    DB = XmCreateWarningDialog (shell,"warningDB",NULL,0);

    xstring = XmStringCreateSimple ("Warning");
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL,
                      XmNdialogTitle,xstring,
                      XmNnoResize,True,
                      NULL); 
    XmStringFree (xstring);
  }

/****************************************************************************
* Load the message into the error dialog 
*****************************************************************************/

  strcpy (newstring,"");
  errstring = pcpmsg;

  while ((int) strlen(errstring) > 35) {
    pos = 35;
    comma = 0;
    colon = 0;
    brace = 0;
    slash = 0;
    newline = 0;

    while ((errstring[pos] != ' ') && (pos > 0)) {
      if ((errstring[pos] == ',') && (!comma)) comma = pos;
      if ((errstring[pos] == ':') && (!colon)) colon = pos;
      if ((errstring[pos] == ']') && (!brace)) brace = pos;
      if ((errstring[pos] == '/') && (!slash)) slash = pos;
      if ((errstring[pos] == '\n') && (!newline)) newline = pos;
      pos--;
    }

    if (comma > pos) pos = comma;
    if (colon > pos) pos = colon;
    if (brace > pos) pos = brace;
    if (slash > pos) pos = slash;
    if (newline) pos = newline;
    if (pos == 0) pos = 35;

    if (pos - comma < 5) pos = comma;
    while (errstring[pos+1] == ' ') pos++;

    save[0] = errstring[pos];
    save[1] = 0;
    errstring[pos] = 0;

    strcat (newstring,errstring);
    strcat (newstring,save);
    if (save[0]!='\n') strcat (newstring,"\n");
    strcpy (errstring,&errstring[pos+1]);
  }

  if ((int) strlen (errstring) > 0) strcat (newstring,errstring);
 
  xstring = XmStringCreateLtoR (newstring, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues (DB,XmNmessageString,xstring,NULL);
  XmStringFree (xstring);

/****************************************************************************
* Pop up the error message 
*****************************************************************************/

  if (XtIsManaged (DB)) {
    XtUnmanageChild (DB);
    XtManageChild (DB);
  }
  else {
    XtManageChild (DB);
  }

  return;
}
