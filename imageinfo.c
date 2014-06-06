#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>

#include "pc2d.h"


struct {
  Widget DB;
  Widget   FileNameVLB;
  Widget   BitTypeVLB;
  Widget   NsVLB;
  Widget   NlVLB;
  Widget   NbVLB;
  Widget   CoreBaseVLB;
  Widget   CoreMultVLB;
  Widget   NbckVLB;
  Widget   BackNameVLB;
  Widget   DispMinVLB;
  Widget   DispMaxVLB;
} GBLimageinfo[V_MAXDSP];


XmString xstring;

DSPINFO *dp;
void CBimageInfoDBexitPB(Widget id, XtPointer client_data, XtPointer call_data);


void CreateImageInfoDB (int dsp)
/***********************************************************************
*
*	---------------------------
*	|    CreateImageInfoDB    |
*	---------------------------
*
*_Title	CreateImageInfoDB - Create the Image Information dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm  int    dsp              I       display number
*			
*_DESC	This routine creates the dialog box when Image Information
*	is chosen.
*
*_HIST	Sep 04 1996 Tracie Sucharski, USGS, Flagstaff, Original version
*       Sep 16 1996 TLS, Added backplane names, display min and max.
*       Mar 24 1998 TLS, Print out actual display  min/max rather
*                     than original display min/max.
*	
*_END	
************************************************************************/

{
  Widget DB;
  Widget RC;
  Widget   FileNameLB;
  Widget   BitTypeLB;
  Widget   CoreBaseLB;
  Widget   CoreMultLB;
  Widget   NsLB;
  Widget   NlLB;
  Widget   NbLB;
  Widget   DispMinLB;
  Widget   DispMaxLB;
  Widget   FileNameVLB;
  Widget   BitTypeVLB;
  Widget   CoreBaseVLB;
  Widget   CoreMultVLB;
  Widget   NsVLB;
  Widget   NlVLB;
  Widget   NbVLB;
  Widget   DispMinVLB;
  Widget   DispMaxVLB;
  Widget RC2;
  Widget   NbckLB;
  Widget   BackNameLB;
  Widget   NbckVLB;
  Widget   BackNameVLB;
  Widget CM;
  Widget RC3;
  Widget   exitPB;
  Widget   helpPB;
 
  char *sptr;
  char string[32];
  int i;
  static Boolean firstime = True;

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  dp = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLimageinfo[i].DB = NULL;
  }
/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (GBLimageinfo[dsp].DB == NULL) {
    DB = XmCreateFormDialog (dp->shell,"ImageInfoDB",NULL,0);
    sprintf (sptr,"Display %d Image Information",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_MODELESS,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLimageinfo[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,
                   &GBLimageinfo[dsp].DB);

/*------------------------------------------------------------------------------
/  Create the row/column with image information
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"ImageRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmVERTICAL,
                      XmNadjustLast, False,
                      XmNpacking, XmPACK_COLUMN,
                      XmNnumColumns, 2,
                      NULL);

    FileNameLB = XmCreateLabel (RC,"File Name:",NULL,0);
    BitTypeLB = XmCreateLabel (RC,"Bit Type:",NULL,0);
    CoreBaseLB = XmCreateLabel (RC,"Core Base:",NULL,0);
    CoreMultLB = XmCreateLabel (RC,"Core Multiplier:",NULL,0);
    NsLB = XmCreateLabel (RC,"Number of Samples:",NULL,0);
    NlLB = XmCreateLabel (RC,"Number of Lines:",NULL,0);
    NbLB = XmCreateLabel (RC,"Number of Bands:",NULL,0);
    DispMinLB = XmCreateLabel (RC, "Display Minimum:", NULL, 0);
    DispMaxLB = XmCreateLabel (RC, "Display Maximum:", NULL, 0);

    FileNameVLB = XmCreateLabel (RC, dp->f.fname,NULL,0);
    sprintf(sptr,"%d",dp->f.type);
    if (dp->f.type == 1) strcat(sptr," (8-bit)");
    if (dp->f.type == 2) strcat(sptr," (16-bit)");
    if (dp->f.type == 3) strcat(sptr," (32-bit)");
    BitTypeVLB = XmCreateLabel (RC,sptr,NULL,0);
    sprintf(sptr,"%g",dp->f.scale[0]);
    CoreBaseVLB = XmCreateLabel (RC,sptr,NULL,0);
    sprintf(sptr,"%g",dp->f.scale[1]);
    CoreMultVLB = XmCreateLabel (RC,sptr,NULL,0);
    sprintf(sptr,"%d",dp->f.ns);
    NsVLB = XmCreateLabel (RC,sptr,NULL,0);
    sprintf(sptr,"%d",dp->f.nl);
    NlVLB = XmCreateLabel (RC,sptr,NULL,0);
    sprintf(sptr,"%d",dp->f.nb);
    NbVLB = XmCreateLabel (RC,sptr,NULL,0);
    sprintf (sptr, "%g", dp->rmin_real);
    DispMinVLB = XmCreateLabel (RC, sptr, NULL, 0);
    sprintf (sptr, "%g", dp->rmax_real);
    DispMaxVLB = XmCreateLabel (RC, sptr, NULL, 0);

    RC2 = XmCreateRowColumn (DB,"ImageRC2",NULL,0);
    XtVaSetValues (RC2,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmVERTICAL,
                      XmNadjustLast, False,
                      XmNpacking, XmPACK_COLUMN,
                      XmNnumColumns, 2,
                      NULL);

    NbckLB = XmCreateLabel (RC2,"Number of Backplanes:",NULL,0);
    if (dp->f.nbck > 0)
        BackNameLB = XmCreateLabel (RC2, "Backplane Names:", NULL, 0);
    sprintf(sptr,"%d",dp->f.nbck);
    NbckVLB = XmCreateLabel (RC2,sptr,NULL,0);
    if (dp->f.nbck > 0) {
      strcpy (sptr, "");
      for (i=0; i<dp->f.nbck; i++) {
	sprintf (string, "%s\n", dp->f.bck_names[i]);
	strcat (sptr, string);
      }
      BackNameVLB = XmCreateLabel (RC2, "BackNameVLB", NULL, 0);
      xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
      XtVaSetValues (BackNameVLB, 
		     XmNlabelString, xstring,
		     NULL);
      XmStringFree (xstring);
    }


    GBLimageinfo[dsp].FileNameVLB = FileNameVLB;
    GBLimageinfo[dsp].BitTypeVLB = BitTypeVLB;
    GBLimageinfo[dsp].CoreBaseVLB = CoreBaseVLB;
    GBLimageinfo[dsp].CoreMultVLB = CoreMultVLB;
    GBLimageinfo[dsp].NsVLB = NsVLB;
    GBLimageinfo[dsp].NlVLB = NlVLB;
    GBLimageinfo[dsp].NbVLB = NbVLB;
    GBLimageinfo[dsp].NbckVLB = NbckVLB;
    if (dp->f.nbck > 0) GBLimageinfo[dsp].BackNameVLB = NbckVLB;
    GBLimageinfo[dsp].DispMinVLB = DispMinVLB;
    GBLimageinfo[dsp].DispMaxVLB = DispMaxVLB;

    XtManageChild (DispMaxVLB);
    XtManageChild (DispMaxLB);
    XtManageChild (DispMinVLB);
    XtManageChild (DispMinLB);
    if (dp->f.nbck > 0) {
      XtManageChild (BackNameVLB);
      XtManageChild (BackNameLB);
    }
    XtManageChild (NbckVLB);
    XtManageChild (NbckLB);
    XtManageChild (CoreMultVLB);
    XtManageChild (CoreMultLB);
    XtManageChild (CoreBaseVLB);
    XtManageChild (CoreBaseLB);
    XtManageChild (NbVLB);
    XtManageChild (NbLB);
    XtManageChild (NlVLB);
    XtManageChild (NlLB);
    XtManageChild (NsVLB);
    XtManageChild (NsLB);
    XtManageChild (BitTypeVLB);
    XtManageChild (BitTypeLB);
    XtManageChild (FileNameVLB);
    XtManageChild (FileNameLB);

    XtManageChild (RC);
    XtManageChild (RC2);

    CM = XmCreateSeparatorGadget (DB,"ImageInfoCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC2,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    RC3 = XmCreateRowColumn (DB,"ImageRC",NULL,0);
    XtVaSetValues (RC3,XmNtopAttachment, XmATTACH_WIDGET,
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

    exitPB = XmCreatePushButton (RC3,"Exit",NULL,0);
    XtAddCallback (exitPB,XmNactivateCallback, 
                   CBqviewCancelExitPB,DB);

    helpPB = XmCreatePushButton (RC3,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (exitPB);
    XtManageChild (helpPB);
    XtManageChild (RC3);

  }


  if (XtIsManaged (GBLimageinfo[dsp].DB)) {
    XtUnmanageChild (GBLimageinfo[dsp].DB);
    XtManageChild (GBLimageinfo[dsp].DB);
  }
  else {
    XtManageChild (GBLimageinfo[dsp].DB);
  }
 
  return;
}







void UpdateImageInfoDB (int dsp)
/***********************************************************************
*
*	---------------------------
*	|    UpdateImageInfoDB    |
*	---------------------------
*
*_Title	UpdateImageInfoDB - Update the image information dialog box
*
*_Args	Type   Variable        I/O	Description
*Parm	INT4   dsp;		I	Display Number
*
*_DESC	This routine updates the ImageInfo dialog box.
*
*_HIST	Sep 05 1996 Tracie Sucharski, USGS, Flagstaff, Original version
*	
*_END	
************************************************************************/

{

  dp = &GBLdsp.d[dsp];


  sprintf (GBLwork.work[0],dp->f.fname);
  xstring = XmStringCreateSimple (GBLwork.work[0]);
  XtVaSetValues (GBLimageinfo[dsp].FileNameVLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLwork.work[0],"%d",dp->f.type);
  xstring = XmStringCreateSimple (GBLwork.work[0]);
  XtVaSetValues (GBLimageinfo[dsp].BitTypeVLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLwork.work[0],"%d",dp->f.ns);
  xstring = XmStringCreateSimple (GBLwork.work[0]);
  XtVaSetValues (GBLimageinfo[dsp].NsVLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLwork.work[0],"%d",dp->f.nl);
  xstring = XmStringCreateSimple (GBLwork.work[0]);
  XtVaSetValues (GBLimageinfo[dsp].NlVLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLwork.work[0],"%d",dp->f.nb);
  xstring = XmStringCreateSimple (GBLwork.work[0]);
  XtVaSetValues (GBLimageinfo[dsp].NbVLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLwork.work[0],"%f",dp->f.scale[0]);
  xstring = XmStringCreateSimple (GBLwork.work[0]);
  XtVaSetValues (GBLimageinfo[dsp].CoreBaseVLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  sprintf (GBLwork.work[0],"%f",dp->f.scale[1]);
  xstring = XmStringCreateSimple (GBLwork.work[0]);
  XtVaSetValues (GBLimageinfo[dsp].CoreMultVLB,XmNlabelString,xstring,NULL);
  XmStringFree (xstring);

  return;
}

