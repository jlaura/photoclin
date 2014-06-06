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
  Widget noTB;
  Widget autoTB;
  Widget prestr_minperTF;
  Widget prestr_maxperTF;
  Widget manTB;
  Widget minTF;
  Widget maxTF;
  Widget redTF;
  Widget grnTF;
  Widget bluTF;
  Widget okPB;
} GBLview[V_MAXDSP];

void CBviewDBchangeTB (Widget id, XtPointer client_data, XtPointer call_data);



void CreateViewDB (int dsp)

/***********************************************************************
*
*_Title	CreateViewDB - Creates dialog for changing cube view
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	Display number

*_DESC	This routine creates the view dialog box which allows the user
*	to select which band to view, select the stretch to load the
*	cube with, etc.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Jan 05 1998 Tracie Sucharski, Added sub-cube specifier
*       Feb 05 1998 Tracie Sucharski, Added stretch min/max percentages.
*       Mar 06 1998 TLS, Changed the ratio box to a row/column so that
*                      only one of the toggle buttons is active at
*                      any one time.
*       Mar 24 1998 TLS, Print out actual display  min/max rather
*                     than original display min/max.
*       Apr 12 2002 Janet Barrett, Modified to work with pc2d program.
*_END
************************************************************************/

{
  Widget DB;
  Widget RC;
  Widget   noTB;
  Widget   autoTB;
  Widget RC2,RC3;
  Widget   LB,LB2;
  Widget   prestr_minperTF;
  Widget   prestr_maxperTF;
  Widget   manTB;
  Widget RC4,RC5;
  Widget   LB3,LB4;
  Widget   minTF;
  Widget   maxTF;
  Widget RC7,RC8,RC9;
  Widget   redTF;
  Widget   grnTF;
  Widget   bluTF;
  Widget CM;
  Widget   okPB;
  Widget   canPB;
  Widget   helpPB;

  XmString xstring;
  char *sptr;
  int i;
  int b1,b2,b3;
  DSPINFO *d;
  static Boolean firstime = True;
  char   cmin[6],cmax[6];

  void CBviewDBokPB (Widget id, XtPointer client_data, XtPointer call_data);

/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLview[i].DB = NULL;
  }

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (GBLview[dsp].DB == NULL) {
    DB = XmCreateFormDialog (d->shell,"viewDB",NULL,0);
    sprintf (sptr,"Display %d View Options",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLview[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,&GBLview[dsp].DB);

/*------------------------------------------------------------------------------
/ Create the radio buttons and text fields 
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"viewRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmVERTICAL,
                      XmNradioBehavior, True,
                      NULL);

    noTB = XmCreateToggleButton (RC,"No Stretch",NULL,0);
    XtAddCallback (noTB,XmNvalueChangedCallback,CBviewDBchangeTB,(XtPointer)(int)dsp);
    autoTB = XmCreateToggleButton (RC,"Automatic Stretch",NULL,0);
    XtAddCallback (autoTB,XmNvalueChangedCallback,CBviewDBchangeTB,(XtPointer)(int)dsp);

    RC2 = XmCreateRowColumn (RC,"viewRC",NULL,0);
    XtVaSetValues (RC2,XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC2,"Percent Minimum",NULL,0);
    XtVaSetValues (LB,XmNmarginLeft,20,
                   NULL);
    prestr_minperTF = XmCreateTextField (RC2,"viewTF",NULL,0);
    XtVaSetValues (prestr_minperTF,XmNcolumns, 15, XmNmaxLength, 15, NULL);

    RC3 = XmCreateRowColumn (RC,"viewRC",NULL,0);
    XtVaSetValues (RC3,XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB2 = XmCreateLabel (RC3,"Percent Maximum",NULL,0);
    XtVaSetValues (LB2,XmNmarginLeft,20,
                   NULL);
    prestr_maxperTF = XmCreateTextField (RC3,"viewTF",NULL,0);
    XtVaSetValues (prestr_maxperTF,XmNcolumns, 15, XmNmaxLength, 15, NULL);

    manTB = XmCreateToggleButton (RC,"Manual Stretch",NULL,0);
    XtAddCallback (manTB,XmNvalueChangedCallback,CBviewDBchangeTB,(XtPointer)(int)dsp);

    RC4 = XmCreateRowColumn (RC,"viewRC",NULL,0);
    XtVaSetValues (RC4,XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB3 = XmCreateLabel (RC4,"Minimum",NULL,0);
    XtVaSetValues (LB3,XmNmarginLeft,20,
                   NULL);
    minTF = XmCreateTextField (RC4,"viewTF",NULL,0);
    XtVaSetValues (minTF,XmNcolumns, 15, XmNmaxLength, 30, NULL);

    RC5 = XmCreateRowColumn (RC,"viewRC",NULL,0);
    XtVaSetValues (RC5,XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB4 = XmCreateLabel (RC5,"Maximum",NULL,0);
    XtVaSetValues (LB4,XmNmarginLeft,20,
                   NULL);
    maxTF = XmCreateTextField (RC5,"viewTF",NULL,0);
    XtVaSetValues (maxTF,XmNcolumns, 15, XmNmaxLength, 30, NULL);

    XtManageChild (noTB);
    XtManageChild (autoTB);
    XtManageChild (LB);
    XtManageChild (prestr_minperTF);
    XtManageChild (RC2);
    XtManageChild (LB2);
    XtManageChild (prestr_maxperTF);
    XtManageChild (RC3);
    XtManageChild (manTB);
    XtManageChild (LB3);
    XtManageChild (minTF);
    XtManageChild (RC4);
    XtManageChild (LB4);
    XtManageChild (maxTF);
    XtManageChild (RC5);
    XtManageChild (RC);

    RC7 = XmCreateRowColumn (DB,"viewDB",NULL,0);
    XtVaSetValues (RC7,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopOffset,15, 
                       XmNtopWidget,RC5,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC7,"Red/Mono",NULL,0);
    redTF = XmCreateTextField (RC7,"viewTF",NULL,0);
    XtVaSetValues (redTF,XmNcolumns, 5, XmNmaxLength, 5, NULL);

    XtManageChild (LB);
    XtManageChild (redTF);
    XtManageChild (RC7);


    RC8 = XmCreateRowColumn (DB,"viewDB",NULL,0);
    XtVaSetValues (RC8,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC7,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC8,"Green",NULL,0);
    grnTF = XmCreateTextField (RC8,"viewTF",NULL,0);
    XtVaSetValues (grnTF,XmNcolumns, 5, XmNmaxLength, 5, NULL);

    XtManageChild (LB);
    XtManageChild (grnTF);
    XtManageChild (RC8);

    RC9 = XmCreateRowColumn (DB,"viewDB",NULL,0);
    XtVaSetValues (RC9,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC8,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC9,"Blue",NULL,0);
    bluTF = XmCreateTextField (RC9,"viewTF",NULL,0);
    XtVaSetValues (bluTF,XmNcolumns, 5, XmNmaxLength, 5, NULL);

    XtManageChild (LB);
    XtManageChild (bluTF);
    XtManageChild (RC9);

/*------------------------------------------------------------------------------
/ Create the action area 
/-----------------------------------------------------------------------------*/

    CM = XmCreateSeparatorGadget (DB,"viewDB",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC9,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

    RC = XmCreateRowColumn (DB,"viewDB",NULL,0);
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
    XtAddCallback (okPB,XmNactivateCallback,CBviewDBokPB,(XtPointer)((int)dsp));

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,CBqviewCancelExitPB,(XtPointer)DB);
 
    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (okPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC);

    GBLview[dsp].noTB = noTB;
    GBLview[dsp].autoTB = autoTB;
    GBLview[dsp].manTB = manTB;
    GBLview[dsp].prestr_minperTF = prestr_minperTF;
    GBLview[dsp].prestr_maxperTF = prestr_maxperTF;
    GBLview[dsp].minTF = minTF;
    GBLview[dsp].maxTF = maxTF;
    GBLview[dsp].redTF = redTF;
    GBLview[dsp].grnTF = grnTF;
    GBLview[dsp].bluTF = bluTF;
    GBLview[dsp].okPB = okPB;
  }

/*------------------------------------------------------------------------------
/ Load the defaults into the text fields and toggle buttons
/-----------------------------------------------------------------------------*/

  b1 = d->band[0];
  b2 = d->band[1];
  b3 = d->band[2];

  sprintf (sptr,"%d",b1);
  XmTextFieldSetString (GBLview[dsp].redTF,sptr);
  
  sprintf (sptr,"%d",b2);
  XmTextFieldSetString (GBLview[dsp].grnTF,sptr);
  
  sprintf (sptr,"%d",b3);
  XmTextFieldSetString (GBLview[dsp].bluTF,sptr);

  sprintf (cmin,"%g",d->prestr_minper);
  sprintf (cmax,"%g",d->prestr_maxper);
  XmTextFieldSetString (GBLview[dsp].prestr_minperTF,cmin);
  XmTextFieldSetString (GBLview[dsp].prestr_maxperTF,cmax);

  if (d->autostr) {
    XmToggleButtonSetState (GBLview[dsp].noTB,False,False);
    XmToggleButtonSetState (GBLview[dsp].autoTB,True,False);
    XmToggleButtonSetState (GBLview[dsp].manTB,False,False);
    XtSetSensitive (GBLview[dsp].minTF,False);
    XtSetSensitive (GBLview[dsp].maxTF,False);
  }
  else if (d->manstr) {
    XmToggleButtonSetState (GBLview[dsp].noTB,False,False);
    XmToggleButtonSetState (GBLview[dsp].autoTB,False,False);
    XmToggleButtonSetState (GBLview[dsp].manTB,True,False);
    XtSetSensitive (GBLview[dsp].prestr_minperTF,False);
    XtSetSensitive (GBLview[dsp].prestr_maxperTF,False);
  }
  else {
    XmToggleButtonSetState (GBLview[dsp].noTB,True,False);
    XmToggleButtonSetState (GBLview[dsp].autoTB,False,False);
    XmToggleButtonSetState (GBLview[dsp].manTB,False,False);
    XtSetSensitive (GBLview[dsp].minTF,False);
    XtSetSensitive (GBLview[dsp].maxTF,False);
    XtSetSensitive (GBLview[dsp].prestr_minperTF,False);
    XtSetSensitive (GBLview[dsp].prestr_maxperTF,False);
  } 

  if (!(d->band[1] || d->band[2])) {
    sprintf (sptr,"%g",d->rmin_real);
    XmTextFieldSetString (GBLview[dsp].minTF,sptr);
    sprintf (sptr,"%g",d->rmax_real);
    XmTextFieldSetString (GBLview[dsp].maxTF,sptr);
  }
  else {
    sprintf (sptr,"%g %g %g",d->rmin_real,d->gmin_real,d->bmin_real);
    XmTextFieldSetString (GBLview[dsp].minTF,sptr);
    sprintf (sptr,"%g %g %g",d->rmax_real,d->gmax_real,d->bmax_real);
    XmTextFieldSetString (GBLview[dsp].maxTF,sptr);
  }

/*------------------------------------------------------------------------------
/ Manage the view dialog
/-----------------------------------------------------------------------------*/

  if (XtIsManaged (GBLview[dsp].DB)) {
    XtUnmanageChild (GBLview[dsp].DB);
    XtManageChild (GBLview[dsp].DB);
  }
  else {
    XtManageChild (GBLview[dsp].DB);
  }
 
  return;
}



void CBviewDBokPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBviewDBokPB - callback for view ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Not used
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the user press the ok pushbutton on
*	the view dialog box. It obtains the viewing parameters from the
*	text fields/toggle buttons and checks for errors. If everything
*	is ok it will update the view of the display.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Jan 05 1998 Tracie Sucharski, Added sub-cube specifier.
*       Mar 06 1998 TLS, Added prestretch min/max percent.
*       Apr 12 2002 Janet Barrett, Modified to work with pc2d program.
*_END
************************************************************************/

{
  int dsp = (int) client_data;
  DSPINFO *d;
  int b1,b2,b3;
  char *sptr;
  Boolean autostr,manstr;
  double rmin,gmin,bmin;
  double rmax,gmax,bmax;
  double minper,maxper;
  int minin,maxin;
  int i;
  INT4 ret;
  INT4 dflag;
  FLOAT4 junk;
  INT4 csize[3],ccont[3];
  INT4 ssize[3],scont[3];

  d = &GBLdsp.d[dsp];

  autostr = XmToggleButtonGetState (GBLview[dsp].autoTB);
  manstr = XmToggleButtonGetState (GBLview[dsp].manTB);

  if (autostr) {
    sptr = XmTextFieldGetString (GBLview[dsp].prestr_minperTF);
    sscanf (sptr,"%lf",&minper);
    XtFree (sptr);

    sptr = XmTextFieldGetString (GBLview[dsp].prestr_maxperTF);
    sscanf (sptr,"%lf",&maxper);
    XtFree (sptr);
    if (minper != d->prestr_minper || maxper != d->prestr_maxper)
      for (i=0;i<3;i++) d->avalid[i] = False;
    d->prestr_minper = minper;
    d->prestr_maxper = maxper;
  }

  if (manstr) {
    sptr = XmTextFieldGetString (GBLview[dsp].minTF);
    minin = sscanf (sptr,"%lf %lf %lf",&rmin,&gmin,&bmin); 
    XtFree (sptr);

    sptr = XmTextFieldGetString (GBLview[dsp].maxTF);
    maxin = sscanf (sptr,"%lf %lf %lf",&rmax,&gmax,&bmax); 
    XtFree (sptr);
  }


  sptr = XmTextFieldGetString (GBLview[dsp].redTF);
  b1 = strtol (sptr,NULL,0);
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLview[dsp].grnTF);
  b2 = strtol (sptr,NULL,0);
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLview[dsp].bluTF);
  b3 = strtol (sptr,NULL,0);
  XtFree (sptr);

  if ((b1 < 0) || (b1 > d->f.nb)) {
    strcpy (GBLerr.string,"Invalid band for red/mono");
    QviewMessage (dsp);
    return;
  }

  if ((b2 < 0) || (b2 > d->f.nb)) {
    strcpy (GBLerr.string,"Invalid band for green");
    QviewMessage (dsp);
    return;
  }

  if ((b3 < 0) || (b3 > d->f.nb)) {
    strcpy (GBLerr.string,"Invalid band for blue");
    QviewMessage (dsp);
    return;
  }

  if (!(b1 || b2 || b3)) {
    strcpy (GBLerr.string,"Red, green, and blue can not all be zero");
    QviewMessage (dsp);
    return;
  }

  if (xinfo.vinfo.class == TrueColor) {
    if (!(autostr || manstr)) {
      strcpy (GBLerr.string,"Because this display is in TrueColor mode ");
      strcat (GBLerr.string,"an automatic or manual stretch should be ");
      strcat (GBLerr.string,"used when loading the cube");
      QviewMessage (dsp);
      return;
    }
  }
  else if (xinfo.vinfo.class == PseudoColor) {
    if (b2 || b3) {
      if (!(autostr || manstr)) {
        strcpy (GBLerr.string,"Because this display is in PseudoColor mode ");
        strcat (GBLerr.string,"and you have chosen to display a three band ");
        strcat (GBLerr.string,"color composite, ");
        strcat (GBLerr.string,"an automatic or manual stretch should be ");
        strcat (GBLerr.string,"used when loading the cube");
        QviewMessage (dsp);
        return;
      }
    }
  }

  if (d->f.type != 1) {
    if (!(autostr || manstr)) {
      strcpy (GBLerr.string,"Because the cube contains 16-bit or 32-bit ");
      strcat (GBLerr.string,"pixels ");
      strcat (GBLerr.string,"an automatic or manual stretch should be ");
      strcat (GBLerr.string,"used when loading the cube");
      QviewMessage (dsp);
      return;
    }
  }

  if (manstr) {
    if (b2 || b3) {
      if ((minin != 3) || (maxin != 3)) {
        strcpy (GBLerr.string,"A minimum/maximum must be entered for ");
        strcat (GBLerr.string,"each band. Separate each by a blank ");
        strcat (GBLerr.string,"(e.g., .2 .3 .15)");
        QviewMessage (-1);
        return;
      }
    }
    else if ((minin != 1) || (maxin != 1)) {
      strcpy (GBLerr.string,"Exactly one minimum/maximum must be entered for ");
      strcat (GBLerr.string,"for the manual stretch when displaying one band");
      QviewMessage (-1);
      return;
    }

    if (rmin >= rmax) {
      strcpy (GBLerr.string,"Minimum must be less than maximum");
      QviewMessage (dsp);
      return;
    }

    if (b2) {
      if (gmin >= gmax) {
        strcpy (GBLerr.string,"Minimum must be less than maximum");
        QviewMessage (dsp);
        return;
      }
    }

    if (b3) {
      if (bmin >= bmax) {
        strcpy (GBLerr.string,"Minimum must be less than maximum");
        QviewMessage (dsp);
        return;
      }
    }
  }

  d->band[0] = b1;
  d->band[1] = b2;
  d->band[2] = b3;
  d->autostr = autostr;
  d->manstr = manstr;

  if (!d->band[0]) {
    rmin = 0;
    rmax = 255;
  }
  if (!d->band[1]) {
    gmin = 0;
    gmax = 255;
  }
  if (!d->band[2]) {
    bmin = 0;
    bmax = 255;
  }

  XtUnmanageChild (XtParent(XtParent(id)));
  XSync (xinfo.display,False);

  if (manstr) {
    d->rmin = rmin;
    d->gmin = gmin;
    d->bmin = bmin;
    d->rmax = rmax;
    d->gmax = gmax;
    d->bmax = bmax;
  }
  else if (autostr) {
    QviewWatch (d->shell,True);
    CalcAutoStr (dsp);
    QviewWatch (d->shell,False);
  }
  else {
    d->rmin = 0;
    d->gmin = 0;
    d->bmin = 0;
    d->rmax = 255;
    d->gmax = 255;
    d->bmax = 255;
  } 

  StretchReset (dsp);
  LoadStrLut (dsp);
  if (xinfo.vinfo.class == PseudoColor) {
    if (d->band[1] || d->band[2]) {
      LoadPseudoColor (d);
    }
    else {
      LoadPseudoBW (d);
    }
  }
  d->read_tmp = False;
  LoadCube (dsp);

  return;
}


void CBviewDBchangeTB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBviewDBokPB - callback for view ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Not used
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the user presses one of the stretch
*       toggle buttons in the view dialog box.  If the no stretch button
*       is pushed, the min/max percent and min/max text fields are set
*       to insensitive.  If the auto stretch button is pushed, the min/max
*       text fields for the manual stretch are set to insensitive.  If
*       the manual stretch button is pushed, the min/max percent text
*       fields for the auto stretch are set to insensitive.

*_HIST	Mar 06 1998 Tracie Sucharski, Original version
*       Jan 27 1999 TLS, Get toggle button state rather than just
*                     looking at the Widget ID to determine which
*                     button was chosen.
*
*_END
************************************************************************/

{
  int dsp = (int) client_data;

  Boolean noTB,autoTB,manTB;

  dsp = (int) client_data;

  noTB = XmToggleButtonGetState (GBLview[dsp].noTB);
  autoTB = XmToggleButtonGetState (GBLview[dsp].autoTB);
  manTB = XmToggleButtonGetState (GBLview[dsp].manTB);


  if (noTB) {
    XtSetSensitive (GBLview[dsp].prestr_minperTF,False);
    XtSetSensitive (GBLview[dsp].prestr_maxperTF,False);
    XtSetSensitive (GBLview[dsp].minTF,False);
    XtSetSensitive (GBLview[dsp].maxTF,False);
  }

  if (autoTB) {
    XtSetSensitive (GBLview[dsp].prestr_minperTF,True);
    XtSetSensitive (GBLview[dsp].prestr_maxperTF,True);
    XtSetSensitive (GBLview[dsp].minTF,False);
    XtSetSensitive (GBLview[dsp].maxTF,False);
  }

  if (manTB) {
    XtSetSensitive (GBLview[dsp].prestr_minperTF,False);
    XtSetSensitive (GBLview[dsp].prestr_maxperTF,False);
    XtSetSensitive (GBLview[dsp].minTF,True);
    XtSetSensitive (GBLview[dsp].maxTF,True);
  }

  return;

}
