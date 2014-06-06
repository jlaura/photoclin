#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>

#include "pc2d.h"
#include "pcp.h"
#include "clinom_ipars.h"
#include "clinom_msg.h"

struct {
  Widget DB;
  Widget penaltynumTF;
  Widget alphaasymTF;
  Widget stepasymTF;
  Widget divtolTF;
  Widget etolTF;
  Widget bigtolTF;
  Widget oldtolTF;
  Widget taufacTF;
  Widget okPB;
} GBLpar;

void CreateParamOptionsDB ()


/***********************************************************************
*
*_Title	CreateParamOptionsDB Creates parameter option dialog box
*
*_DESC	This routine creates the parameter option dialog box which 
*	allows the user to set the photoclinometry parameters that 
*	control iteration

*_HIST	Jun 19 2002 Janet Barrett, Original version
*_END
************************************************************************/

{
  static Widget DB = NULL;
  Widget RC,RC2,RC3,RC4,RC5,RC6,RC7,RC8;
  Widget LB;
  Widget penaltynumTF;
  Widget alphaasymTF;
  Widget stepasymTF;
  Widget divtolTF;
  Widget etolTF;
  Widget bigtolTF;
  Widget oldtolTF;
  Widget taufacTF;
  Widget CM;
  Widget okPB;
  Widget applyPB;
  Widget canPB;
  Widget helpPB;

  XmString xstring;
  char *sptr;

  void CBparamOptionsDBokPB (Widget id, XtPointer client_data, 
                               XtPointer call_data);

  sptr = (char *) GBLwork.work[0];

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (DB == NULL) {
    DB = XmCreateFormDialog (xinfo.topShell,"paramDB",NULL,0);
    sprintf (sptr,"Iteration Parameters");
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLpar.DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,DB);

/*------------------------------------------------------------------------------
/  Create the text fields
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"paramRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmHORIZONTAL,
                      XmNadjustLast, False,
                      NULL);

    LB = XmCreateLabel (RC,"PENALTY NUMBER",NULL,0);
    penaltynumTF = XmCreateTextField (RC,"paramTF",NULL,0);
    XtVaSetValues (penaltynumTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (penaltynumTF);
    XtManageChild (RC);

    RC2 = XmCreateRowColumn (DB,"paramRC",NULL,0);
    XtVaSetValues (RC2,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC2,
      "ASYMPTOTIC PENALTY NUMBER",NULL,0);
    alphaasymTF = XmCreateTextField (RC2,"paramTF",NULL,0);
    XtVaSetValues (alphaasymTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (alphaasymTF);
    XtManageChild (RC2);

    RC3 = XmCreateRowColumn (DB,"paramRC",NULL,0);
    XtVaSetValues (RC3,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC2,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC3,
      "TIME CONSTANT (STEPS) TO APPROACH ASYMPTOTE",NULL,0);
    stepasymTF = XmCreateTextField (RC3,"paramTF",NULL,0);
    XtVaSetValues (stepasymTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (stepasymTF);
    XtManageChild (RC3);

    RC4 = XmCreateRowColumn (DB,"paramRC",NULL,0);
    XtVaSetValues (RC4,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC3,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC4,
      "(MAX INC/RMS INC) SIGNALING DIVERGENCE",NULL,0);
    divtolTF = XmCreateTextField (RC4,"paramTF",NULL,0);
    XtVaSetValues (divtolTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (divtolTF);
    XtManageChild (RC4);

    RC5 = XmCreateRowColumn (DB,"paramRC",NULL,0);
    XtVaSetValues (RC5,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC4,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC5,
      "FINAL RMS RHS",NULL,0);
    etolTF = XmCreateTextField (RC5,"paramTF",NULL,0);
    XtVaSetValues (etolTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (etolTF);
    XtManageChild (RC5);

    RC6 = XmCreateRowColumn (DB,"paramRC",NULL,0);
    XtVaSetValues (RC6,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC5,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC6,
      "(RMS RHS/RMS FINE RHS) FOR +RES",NULL,0);
    bigtolTF = XmCreateTextField (RC6,"paramTF",NULL,0);
    XtVaSetValues (bigtolTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (bigtolTF);
    XtManageChild (RC6);

    RC7 = XmCreateRowColumn (DB,"paramRC",NULL,0);
    XtVaSetValues (RC7,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC6,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC7,
      "(RMS RHS/RMS OLD RHS) FOR -RES",NULL,0);
    oldtolTF = XmCreateTextField (RC7,"paramTF",NULL,0);
    XtVaSetValues (oldtolTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (oldtolTF);
    XtManageChild (RC7);

    RC8 = XmCreateRowColumn (DB,"paramRC",NULL,0);
    XtVaSetValues (RC8,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC7,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC8,
      "TAUFAC",NULL,0);
    taufacTF = XmCreateTextField (RC8,"paramTF",NULL,0);
    XtVaSetValues (taufacTF,XmNcolumns, 8, XmNmaxLength, 8, NULL);

    XtManageChild (LB);
    XtManageChild (taufacTF);
    XtManageChild (RC8);

    GBLpar.penaltynumTF = penaltynumTF;
    GBLpar.alphaasymTF = alphaasymTF;
    GBLpar.stepasymTF = stepasymTF;
    GBLpar.divtolTF = divtolTF;
    GBLpar.etolTF = etolTF;
    GBLpar.bigtolTF = bigtolTF;
    GBLpar.oldtolTF = oldtolTF;
    GBLpar.taufacTF = taufacTF;

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    CM = XmCreateSeparatorGadget (DB,"paramCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC8,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

    RC = XmCreateRowColumn (DB,"paramRC",NULL,0);
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

    okPB = XmCreatePushButton (RC,"Ok",NULL,0);
    XtAddCallback (okPB,XmNactivateCallback,
                   CBparamOptionsDBokPB,DB);
    GBLpar.okPB = okPB;

    applyPB = XmCreatePushButton (RC,"Apply",NULL,0);
    XtAddCallback (applyPB,XmNactivateCallback,
                   CBparamOptionsDBokPB,DB);

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,
                   CBqviewCancelExitPB,DB);
 
    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (okPB);
    XtManageChild (applyPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC);
  }

  sprintf (sptr,"%g",1.0/C_IPARS1->al);
  XmTextFieldSetString (GBLpar.penaltynumTF,sptr);
 
  if ((1.0/C_IPARS1->alasym) > (1.0/C_IPARS1->al)) {
    sprintf (sptr,"%g",1.0/C_IPARS1->alasym);
  } else {
    sprintf (sptr,"%g",1.0/C_IPARS1->al);
  }
  XmTextFieldSetString (GBLpar.alphaasymTF,sptr);

  if (C_IPARS1->alrat > 0.999999) {
    sprintf (sptr,"%g",1000000.0);
  } else {
    sprintf (sptr,"%g",-1.0/log(C_IPARS1->alrat));
  }
  XmTextFieldSetString (GBLpar.stepasymTF,sptr);

  sprintf (sptr,"%g",C_IPARS1->divtol);
  XmTextFieldSetString (GBLpar.divtolTF,sptr);
  
  sprintf (sptr,"%g",C_IPARS1->etol);
  XmTextFieldSetString (GBLpar.etolTF,sptr);
  
  sprintf (sptr,"%g",C_IPARS1->bigtol);
  XmTextFieldSetString (GBLpar.bigtolTF,sptr);
  
  sprintf (sptr,"%g",C_IPARS1->oldtol);
  XmTextFieldSetString (GBLpar.oldtolTF,sptr);
  
  sprintf (sptr,"%g",C_IPARS1->taufac);
  XmTextFieldSetString (GBLpar.taufacTF,sptr);
  
  if (XtIsManaged (GBLpar.DB)) {
    XtUnmanageChild (GBLpar.DB);
    XtManageChild (GBLpar.DB);
  }
  else {
    XtManageChild (GBLpar.DB);
  }
 
  return;
}



void CBparamOptionsDBokPB (Widget id, XtPointer client_data, 
                             XtPointer call_data)

/***********************************************************************
*
*_Title	CBparamOptionsDBokPB callback for iteration parameter ok 
*                            pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	See Xt toolkit reference
*						manual	
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when a user press the ok button on the iteration 
*	parameter dialog box. This routine then obtains the iteration parameters 
* 	from the text fields and the checks for validity and reports any 
*	errors to the user. 

*_HIST	Jun 18 2002 Janet Barrett, Original version
*_END
************************************************************************/

{
  char *sptr;
  float penaltynum,oldpenaltynum,newdivtol,newetol,newbigtol,newoldtol;
  float newtaufac,newalphaasym,newstepasym;

  oldpenaltynum = 1.0/C_IPARS1->al;
  sptr = XmTextFieldGetString (GBLpar.penaltynumTF);
  if (sscanf (sptr,"%f",&penaltynum) == 1) {
    if (penaltynum == 0.0) {
      strcpy (C_MSG->pcpmsg,"Invalid value for penalty number.  ");
      strcat (C_MSG->pcpmsg,"Must be a non-zero value.");
      pcpmessage (C_MSG->pcpmsg);
    } else {
      if (penaltynum != oldpenaltynum) aeknown = False;
      C_IPARS1->al = 1.0/penaltynum;
    }
  }
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLpar.alphaasymTF);
  if (sscanf (sptr,"%f",&newalphaasym) == 1) {
    if (newalphaasym > penaltynum) {
      strcpy (C_MSG->pcpmsg,"Invalid value for asymptotic penalty number.  ");
      strcat (C_MSG->pcpmsg,"Must not be greater than penalty number.");
      pcpmessage (C_MSG->pcpmsg);
    } else {
      C_IPARS1->alasym = 1.0/newalphaasym;
    }
  }

  sptr = XmTextFieldGetString (GBLpar.stepasymTF);
  if (sscanf (sptr,"%f",&newstepasym) == 1) {
    if (newstepasym == 0.0) {
      strcpy (C_MSG->pcpmsg,"Invalid value for time constant to approach asymptote.  ");
      strcat (C_MSG->pcpmsg,"Must be a non-zero value.");
      pcpmessage (C_MSG->pcpmsg);
    } else {
      if (newstepasym >= 1000000.0) {
        C_IPARS1->alrat = 1.0;
      } else {
        C_IPARS1->alrat = exp(-1.0/newstepasym);
      }
    }
  }

  sptr = XmTextFieldGetString (GBLpar.divtolTF);
  if (sscanf (sptr,"%f",&newdivtol) == 1) {
    C_IPARS1->divtol = newdivtol;
  }
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLpar.etolTF);
  if (sscanf (sptr,"%f",&newetol) == 1) {
    C_IPARS1->etol = newetol;
  }
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLpar.bigtolTF);
  if (sscanf (sptr,"%f",&newbigtol) == 1) {
    C_IPARS1->bigtol = newbigtol;
  }
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLpar.oldtolTF);
  if (sscanf (sptr,"%f",&newoldtol) == 1) {
    C_IPARS1->oldtol = newoldtol;
  }
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLpar.taufacTF);
  if (sscanf (sptr,"%f",&newtaufac) == 1) {
    C_IPARS1->taufac = newtaufac;
  }
  XtFree (sptr);

  if (id == GBLpar.okPB) XtUnmanageChild (GBLpar.DB);
  
  return;
}
