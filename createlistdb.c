 #include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>

#include "pc2d.h"

void CreateListDB (dsp)

/***********************************************************************
*
*_Title CreateListDB - Create the list dialog box
*
*_Args  Type   Variable        I/O      Description
*_Parm  int     dsp             I       Display number

*_DESC  When this routine is called it will pop up a dialog which
*       contains the listing of the image at the cursor position.
*

*_HIST  Jan 1 1996 Jeff Anderson, Original version
*       Nov 04 1997 Tracie Sucharski, Changed location of dialog box
*_END
************************************************************************/
{
  /*  static Widget list[V_MAXDSP][15];*/
 
  Widget DB;
  Widget RC;
  Widget   LB;
  Widget   TF;
  Widget FM,FM2;
  Widget CM;
  Widget   exitPB;
  Widget   nextPB;
  Widget   savePB;
  Widget   helpPB;

  XmString xstring;
  char *sptr;
  DSPINFO *d;
  Position x,y;

  XFontStruct *font1;
  XmFontList fontlist;

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  x = 200;
  y = 800;
  if (d->listDB == NULL) {
    DB = XmCreateFormDialog (d->shell,"listDB",NULL,0);
    sprintf (sptr,"Display %d List ",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      XmNdefaultPosition,False,
		      XmNx, x,
                      XmNy, y,
                      NULL); 
    XmStringFree (xstring);
    d->listDB = DB;

/*------------------------------------------------------------------------------
/  
/-----------------------------------------------------------------------------*/

    FM = XmCreateForm (DB,"listFM",NULL,0); 
    XtVaSetValues (FM,XmNtopAttachment,XmATTACH_FORM,
                      XmNtopOffset,5,
                      XmNleftAttachment,XmATTACH_FORM,
                      XmNleftOffset,5,
                      XmNrightAttachment,XmATTACH_FORM,
                      XmNrightOffset,5,
                      NULL);
    XtManageChild (FM);

    LB = XmCreateLabel (FM,"Band=N/A",NULL,0);
    XtVaSetValues (LB,XmNtopAttachment,XmATTACH_FORM,
                      XmNleftAttachment,XmATTACH_FORM,
                      XmNrightAttachment,XmATTACH_POSITION,
                      XmNrightPosition,50,
                      XmNbottomAttachment,XmATTACH_FORM,
                      XmNalignment,XmALIGNMENT_BEGINNING,
                      NULL);
    XtManageChild (LB);
    d->list_bandLB = LB;

    LB = XmCreateLabel (FM,"Box Size=N/A",NULL,0);
    XtVaSetValues (LB,XmNtopAttachment,XmATTACH_FORM,
                      XmNleftAttachment,XmATTACH_POSITION,
                      XmNrightAttachment,XmATTACH_FORM,
                      XmNleftPosition,50,
                      XmNbottomAttachment,XmATTACH_FORM,
                      XmNalignment,XmALIGNMENT_END,
                      NULL);
    XtManageChild (LB);
    d->list_boxLB = LB;

    font1 = XLoadQueryFont (xinfo.display,
            "-adobe-courier-medium-r-normal--14-100-100-100-m-90-iso8859-1");
    fontlist = XmFontListCreate (font1,"charset1");

    TF = XmCreateText (DB,"listTF",NULL,0);
    XtVaSetValues (TF,XmNtopAttachment,XmATTACH_WIDGET,
                      XmNtopWidget,FM,
                      XmNleftAttachment,XmATTACH_FORM,
                      XmNleftOffset,5,
                      XmNrightAttachment,XmATTACH_FORM,
                      XmNrightOffset,5,
                      XmNfontList,fontlist,
                      XmNeditable,False,
                      XmNeditMode,XmMULTI_LINE_EDIT,
                      XmNvalue," ",
                      XmNrows,7,
                      XmNcolumns,85, 
                      NULL);
    XtManageChild (TF);
    d->list_dnTF = TF;

    LB = XmCreateLabel (DB,"Samp:Line=N/A",NULL,0);
    XtVaSetValues (LB,XmNtopAttachment,XmATTACH_WIDGET,
                      XmNtopWidget,TF,
                      XmNleftAttachment,XmATTACH_FORM,
                      XmNleftOffset,5,
                      XmNrightAttachment,XmATTACH_FORM,
                      XmNrightOffset,5,
                      XmNalignment,XmALIGNMENT_BEGINNING,
                      NULL);
    XtManageChild (LB);
    d->list_lsLB = LB;

    FM = XmCreateForm (DB,"listFM",NULL,0); 
    XtVaSetValues (FM,XmNtopAttachment,XmATTACH_WIDGET,
                      XmNtopWidget,LB,
                      XmNleftAttachment,XmATTACH_FORM,
                      XmNleftOffset,5,
                      XmNrightAttachment,XmATTACH_FORM,
                      XmNrightOffset,5,
                      NULL);
    XtManageChild (FM);

    LB = XmCreateLabel (FM,"Average=N/A",NULL,0);
    XtVaSetValues (LB,XmNtopAttachment,XmATTACH_FORM,
                      XmNleftAttachment,XmATTACH_FORM,
                      XmNrightAttachment,XmATTACH_POSITION,
                      XmNrightPosition,50,
                      XmNbottomAttachment,XmATTACH_FORM,
                      XmNalignment,XmALIGNMENT_BEGINNING,
                      NULL);
    XtManageChild (LB);
    d->list_avgLB = LB;

    LB = XmCreateLabel (FM,"Minimum=N/A",NULL,0);
    XtVaSetValues (LB,XmNtopAttachment,XmATTACH_FORM,
                      XmNleftAttachment,XmATTACH_POSITION,
                      XmNrightAttachment,XmATTACH_FORM,
                      XmNleftPosition,50,
                      XmNbottomAttachment,XmATTACH_FORM,
                      XmNalignment,XmALIGNMENT_END,
                      NULL);
    XtManageChild (LB);
    d->list_minLB = LB;

    FM2 = XmCreateForm (DB,"listFM",NULL,0); 
    XtVaSetValues (FM2,XmNtopAttachment,XmATTACH_WIDGET,
                       XmNtopWidget,FM,
                       XmNleftAttachment,XmATTACH_FORM,
                       XmNleftOffset,5,
                       XmNrightAttachment,XmATTACH_FORM,
                       XmNrightOffset,5,
                       NULL);
    XtManageChild (FM2);

    LB = XmCreateLabel (FM2,"Standard Deviation=N/A",NULL,0);
    XtVaSetValues (LB,XmNtopAttachment,XmATTACH_FORM,
                      XmNleftAttachment,XmATTACH_FORM,
                      XmNrightAttachment,XmATTACH_POSITION,
                      XmNrightPosition,50,
                      XmNbottomAttachment,XmATTACH_FORM,
                      XmNalignment,XmALIGNMENT_BEGINNING,
                      NULL);
    XtManageChild (LB);
    d->list_stdLB = LB;

    LB = XmCreateLabel (FM2,"Maximum=N/A",NULL,0);
    XtVaSetValues (LB,XmNtopAttachment,XmATTACH_FORM,
                      XmNleftAttachment,XmATTACH_POSITION,
                      XmNrightAttachment,XmATTACH_FORM,
                      XmNleftPosition,50,
                      XmNbottomAttachment,XmATTACH_FORM,
                      XmNalignment,XmALIGNMENT_END,
                      NULL);
    XtManageChild (LB);
    d->list_maxLB = LB;

    CM = XmCreateSeparatorGadget (DB,"listCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, FM2,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"listRC",NULL,0);
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

    exitPB = XmCreatePushButton (RC,"Exit",NULL,0);
    XtAddCallback (exitPB,XmNactivateCallback,CBlistDBexitPB,(XtPointer)dsp);

    nextPB = XmCreatePushButton (RC,"Next",NULL,0);
    XtAddCallback (nextPB,XmNactivateCallback,CBlistDBnextPB,(XtPointer)dsp);

    savePB = XmCreatePushButton (RC,"Save",NULL,0);
    XtAddCallback (savePB,XmNactivateCallback,CBlistDBsavePB,(XtPointer)dsp);

    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (exitPB);
    XtManageChild (nextPB);
    XtManageChild (savePB);
    XtManageChild (helpPB);
    XtManageChild (RC);
  }

  if (!XtIsManaged (d->listDB)) {
    XtManageChild (d->listDB);
  }
 
  return;
}
