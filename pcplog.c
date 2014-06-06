#include "pc2d.h"
#include "pcp.h"
#include <Xm/Text.h>

void pcplog (CHAR ptext[],INT4 ptype)

/***********************************************************************
*
*_Title pcplog - Write text to Pcp log text field
*
*_DESC  Write text to the specified Pcp log text field.  There is a
*       status field and a log field.  If ptype=0, text goes into the
*       log field.  If ptype=1, text goes into the status field.
*
*_HIST  July 27 2001 Janet Barrett, Original version
*_END
************************************************************************/
{
  long i,icnt;
  char *logptr;
  char *statptr;
  char *txtptr;

  txtptr = (char *) malloc(70000);
 
  if (ptype == LOGTEXT) {
    logptr = XmTextGetString (logtext);
    i = 0;
    if (strlen(logptr) > 68999) {
      icnt = 0;
      while (icnt < 5) {
        if (logptr[i++] == '\n') icnt++;
      }
    }
    strcpy(txtptr,&logptr[i]);
    if (strlen(txtptr) != 0) strcat(txtptr,"\n");
    strcat(txtptr,ptext);
    XmTextSetString (logtext,txtptr);
    XtFree(logptr);
    i = strlen(txtptr);
    XmTextSetInsertionPosition(logtext,i);
  } else {
    statptr = XmTextGetString (stattext);
    i = 0;
    if (strlen(statptr) > 68999) {
      icnt = 0;
      while (icnt < 5) {
        if (statptr[i++] == '\n') icnt++;
      }
    }
    strcpy(txtptr,&statptr[i]);
    if (strlen(txtptr) != 0) strcat(txtptr,"\n");
    strcat(txtptr,ptext);
    XmTextSetString (stattext,txtptr);
    XtFree(statptr);
    i = strlen(txtptr);
    XmTextSetInsertionPosition(stattext,i);
  }

  XmUpdateDisplay (xinfo.topShell);

  free(txtptr);

  return;
}
