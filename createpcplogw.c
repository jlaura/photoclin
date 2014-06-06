#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ScrolledW.h>
#include <Xm/MainW.h>

#include "pc2d.h"
#include "pcp.h"

void CreatePcpLogW ()

/***********************************************************************
*
*_Title CreatePcpLogW - Create the photoclinometry log window
*
*_DESC  When this routine is called it will pop up a window which
*       contains a running log of iteration parameters and status
*       messages.
*
*
*_HIST  Jun 18 2002 Janet Barrett, Original version
*_END
************************************************************************/
{
  Widget MW;
  Widget LB;
  Widget CM;
  Widget RC;
  Widget VSB,HSB,SW;

  Arg argList[1];

  XmString xstring;
  char *sptr;
  Position x,y;
  char string[256];

  XFontStruct *font1;
  XmFontList fontlist;

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  sptr = (char *) GBLwork.work[0];
  strcpy(string,"PC2D LOG");

/*------------------------------------------------------------------------------
/  Create the window.
/-----------------------------------------------------------------------------*/

  XtSetArg (argList[0],XmNallowShellResize,True);
  logshell = XtAppCreateShell (string,string,applicationShellWidgetClass,
                               xinfo.display,argList,1);
  XtVaGetValues (xinfo.topShell,
                 XmNx,&x,
		 XmNy,&y,
		 NULL);
  x = x + 200;
  y = y + 500;
  MW = XmCreateMainWindow (logshell,"pcplogMW",NULL,0);
  XtVaSetValues (logshell,XmNx,x,
                    XmNy,y,
                    NULL); 

  RC = XmCreateRowColumn (MW,"pcplogRC",NULL,0);
  XtVaSetValues (RC,XmNorientation,XmVERTICAL,
                    NULL);
  XtManageChild (RC);

  font1 = XLoadQueryFont (xinfo.display,
          "-adobe-courier-medium-r-normal--10-100-100-100-m-90-iso8859-1");
  fontlist = XmFontListCreate (font1,"charset1");

  LB = XmCreateLabel (RC,"pcplogLB",NULL,0);
  strcpy(sptr," IT#   M OPS   MAG  CONTRST          RMS  E");
  strcat(sptr,"           RMS DB        RMS  Z");
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues (LB,XmNalignment,XmALIGNMENT_BEGINNING,
		    XmNlabelString,xstring,
		    XmNfontList,fontlist,
                    NULL);
  XmStringFree(xstring);
  XtManageChild (LB);

  logtext = XmCreateScrolledText (RC,"pcplogTF",NULL,0);
  XtVaSetValues (logtext,XmNrows,5,
                    XmNcolumns,90,
		    XmNeditable,False,
		    XmNeditMode,XmMULTI_LINE_EDIT,
		    /*XmNcursorPositionVisible,False,*/
		    XmNscrollingPolicy,XmAUTOMATIC,
		    XmNfontList,fontlist,
                    NULL);
  XtManageChild (logtext);
  SW = XtParent(logtext);
  XtVaGetValues(SW,XmNhorizontalScrollBar,&HSB,
                   XmNverticalScrollBar,&VSB,NULL);
  XtManageChild (HSB);
  XtManageChild (VSB);
  XtManageChild (SW);

  CM = XmCreateSeparatorGadget (RC,"pcplogCM",NULL,0);
  XtManageChild (CM);

  LB = XmCreateLabel (RC,"PC2D Status",NULL,0);
  XtVaSetValues (LB,XmNalignment,XmALIGNMENT_CENTER,
/*		    XmNfontList,fontlist,*/
                    NULL);
  XtManageChild (LB);

  stattext = XmCreateScrolledText (RC,"pcpstatTF",NULL,0);
  XtVaSetValues (stattext,XmNrows,5,
                    XmNcolumns,90,
                    XmNeditable,False,
                    XmNeditMode,XmMULTI_LINE_EDIT,
		    /*XmNcursorPositionVisible,False,*/
		    XmNscrollingPolicy,XmAUTOMATIC,
		    XmNfontList,fontlist,
                    NULL);
  XtManageChild (stattext);
  SW = XtParent(stattext);
  XtVaGetValues(SW,XmNhorizontalScrollBar,&HSB,
                   XmNverticalScrollBar,&VSB,NULL);
  XtManageChild (HSB);
  XtManageChild (VSB);
  XtManageChild (SW);

  XmMainWindowSetAreas (MW,NULL,NULL,NULL,NULL,RC);

  XtManageChild (MW);
  XtRealizeWidget (logshell);
  XSync (xinfo.display,False);

  return;
}
