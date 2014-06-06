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
} GBLopen;

void CreateOpenDB (void)

/***********************************************************************
*
*_Title	CreateOpenDB - Creates the open dialog box
*
*_DESC	This routine creates the open dialog box which is used to let
*	the user specify how to open/load the display. 
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Feb 04 1998 Tracie Sucharski, Added stretch min/max percentages.
*       Dec 09 1999 TLS, Changed the radio box to a row/column so that
*                      only one of the toggle buttons is active at
*                      any one time.
*       Mar 05 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/
{
  static Widget DB = NULL;

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
 
  char   cmin[6],cmax[6];

  void CBopenDBokPB (Widget id, XtPointer client_data, XtPointer call_data);
  void CBopenDBcancelPB (Widget id, XtPointer client_data, XtPointer call_data);
  void CBopenDBchangeTB (Widget id, XtPointer client_data, XtPointer call_data);

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (DB == NULL) {
    DB = XmCreateFormDialog (xinfo.topShell,"openDB",NULL,0);
    xstring = XmStringCreateSimple ("Load Options");
    XtVaSetValues (DB,XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL,
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLopen.DB = DB;

/*------------------------------------------------------------------------------
/  Create the open toggle buttons and text fields
/-----------------------------------------------------------------------------*/

    RC = XmCreateRowColumn (DB,"openRC",NULL,0);
    XtVaSetValues (RC,XmNtopAttachment, XmATTACH_FORM,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmVERTICAL,
                      XmNradioBehavior, True,
                      NULL);

    noTB = XmCreateToggleButton (RC,"No Stretch",NULL,0);
    XtAddCallback (noTB,XmNvalueChangedCallback,CBopenDBchangeTB,NULL);
    autoTB = XmCreateToggleButton (RC,"Automatic Stretch",NULL,0);
    XtAddCallback (autoTB,XmNvalueChangedCallback,CBopenDBchangeTB,NULL);

    XtManageChild (noTB);
    XtManageChild (autoTB);
    XtManageChild (RC);

    RC2 = XmCreateRowColumn (RC,"openRC2",NULL,0);
    XtVaSetValues (RC2,XmNtopAttachment,XmATTACH_WIDGET,
		       XmNtopWidget,autoTB,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 30, 
		       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC2,"Percent Minimum",NULL,0);
    prestr_minperTF = XmCreateTextField (RC2,"openTF",NULL,0);
    XtVaSetValues (prestr_minperTF,XmNcolumns, 15, XmNmaxLength, 15, NULL);

    XtManageChild (LB);
    XtManageChild (prestr_minperTF);
    XtManageChild (RC2);

    RC3 = XmCreateRowColumn (RC,"openRC3",NULL,0);
    XtVaSetValues (RC3,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC2,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 30, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB2 = XmCreateLabel (RC3,"Percent Maximum",NULL,0);
    prestr_maxperTF = XmCreateTextField (RC3,"openTF",NULL,0);
    XtVaSetValues (prestr_maxperTF,XmNcolumns, 15, XmNmaxLength, 15, NULL);

    XtManageChild (LB2);
    XtManageChild (prestr_maxperTF);
    XtManageChild (RC3);

    manTB = XmCreateToggleButton (RC,"Manual Stretch",NULL,0);
    XtAddCallback (manTB,XmNvalueChangedCallback,CBopenDBchangeTB,NULL);

    XtManageChild (manTB);

    RC4 = XmCreateRowColumn (RC,"openRC4",NULL,0);
    XtVaSetValues (RC4,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,manTB,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 30, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB3 = XmCreateLabel (RC4,"Minimum",NULL,0);
    minTF = XmCreateTextField (RC4,"openTF",NULL,0);
    XtVaSetValues (minTF,XmNcolumns, 15, XmNmaxLength, 15, NULL);

    XtManageChild (LB3);
    XtManageChild (minTF);
    XtManageChild (RC4);

    RC5 = XmCreateRowColumn (RC,"openRC5",NULL,0);
    XtVaSetValues (RC5,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC4,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 30, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB4 = XmCreateLabel (RC5,"Maximum",NULL,0);
    maxTF = XmCreateTextField (RC5,"openTF",NULL,0);
    XtVaSetValues (maxTF,XmNcolumns, 15, XmNmaxLength, 15, NULL);

    XtManageChild (LB4);
    XtManageChild (maxTF);
    XtManageChild (RC5);

    RC7 = XmCreateRowColumn (DB,"openRC7",NULL,0);
    XtVaSetValues (RC7,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC5,
                       XmNtopOffset,15, 
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC7,"Red/Mono",NULL,0);
    redTF = XmCreateTextField (RC7,"openTF",NULL,0);
    XtVaSetValues (redTF,XmNcolumns, 5, XmNmaxLength, 5, NULL);

    XtManageChild (LB);
    XtManageChild (redTF);
    XtManageChild (RC7);


    RC8 = XmCreateRowColumn (DB,"openRC8",NULL,0);
    XtVaSetValues (RC8,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC7,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC8,"Green",NULL,0);
    grnTF = XmCreateTextField (RC8,"openTF",NULL,0);
    XtVaSetValues (grnTF,XmNcolumns, 5, XmNmaxLength, 5, NULL);

    XtManageChild (LB);
    XtManageChild (grnTF);
    XtManageChild (RC8);

    RC9 = XmCreateRowColumn (DB,"openRC9",NULL,0);
    XtVaSetValues (RC9,XmNtopAttachment, XmATTACH_WIDGET,
                       XmNtopWidget,RC8,
                       XmNleftAttachment, XmATTACH_FORM,
                       XmNleftOffset, 5, 
                       XmNorientation, XmHORIZONTAL,
                       XmNadjustLast, False,
                       NULL);

    LB = XmCreateLabel (RC9,"Blue",NULL,0);
    bluTF = XmCreateTextField (RC9,"openTF",NULL,0);
    XtVaSetValues (bluTF,XmNcolumns, 5, XmNmaxLength, 5, NULL);

    XtManageChild (LB);
    XtManageChild (bluTF);
    XtManageChild (RC9);

/*------------------------------------------------------------------------------
/  Create the action area
/-----------------------------------------------------------------------------*/

    CM = XmCreateSeparatorGadget (DB,"openCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC9,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNrightAttachment, XmATTACH_FORM,
                      NULL);
    XtManageChild (CM);

    RC = XmCreateRowColumn (DB,"openRC",NULL,0);
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
                      XmNspacing, 65,
                      NULL);

    okPB = XmCreatePushButton (RC,"Ok",NULL,0);
    XtAddCallback (okPB,XmNactivateCallback,CBopenDBokPB,NULL);

    canPB = XmCreatePushButton (RC,"Cancel",NULL,0);
    XtAddCallback (canPB,XmNactivateCallback,CBopenDBcancelPB,NULL);
 
    helpPB = XmCreatePushButton (RC,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (okPB);
    XtManageChild (canPB);
    XtManageChild (helpPB);
    XtManageChild (RC);

    GBLopen.noTB = noTB;
    GBLopen.autoTB = autoTB;
    GBLopen.manTB = manTB;
    GBLopen.prestr_minperTF = prestr_minperTF;
    GBLopen.prestr_maxperTF = prestr_maxperTF;
    GBLopen.minTF = minTF;
    GBLopen.maxTF = maxTF;
    GBLopen.redTF = redTF;
    GBLopen.grnTF = grnTF;
    GBLopen.bluTF = bluTF;
  }

/*------------------------------------------------------------------------------
/  Load default values in the text fields and toggle buttons
/-----------------------------------------------------------------------------*/

  XmTextFieldSetString (GBLopen.redTF,"1");
  XmTextFieldSetString (GBLopen.grnTF,"0");
  XmTextFieldSetString (GBLopen.bluTF,"0");
  sprintf (cmin,"%g",GBLdefault.prestr_lowper);
  sprintf (cmax,"%g",GBLdefault.prestr_hiper);
  XmTextFieldSetString (GBLopen.prestr_minperTF,cmin);
  XmTextFieldSetString (GBLopen.prestr_maxperTF,cmax);

  if (xinfo.vinfo.class == TrueColor) {
    XmToggleButtonSetState (GBLopen.noTB,False,False);
    XmToggleButtonSetState (GBLopen.autoTB,True,False);
    XmToggleButtonSetState (GBLopen.manTB,False,False);
    XtSetSensitive (GBLopen.prestr_minperTF,True);
    XtSetSensitive (GBLopen.prestr_maxperTF,True);
    XtSetSensitive (GBLopen.minTF,False);
    XtSetSensitive (GBLopen.maxTF,False);
  }
  else if (GBLdsp.d[GBLdsp.free].f.type != 1) {
    XmToggleButtonSetState (GBLopen.noTB,False,False);
    XmToggleButtonSetState (GBLopen.autoTB,True,False);
    XmToggleButtonSetState (GBLopen.manTB,False,False);
    XtSetSensitive (GBLopen.prestr_minperTF,True);
    XtSetSensitive (GBLopen.prestr_maxperTF,True);
    XtSetSensitive (GBLopen.minTF,False);
    XtSetSensitive (GBLopen.maxTF,False);
 }
  else {
    XmToggleButtonSetState (GBLopen.noTB,False,False);
    XmToggleButtonSetState (GBLopen.autoTB,True,False);
    XmToggleButtonSetState (GBLopen.manTB,False,False);
    XtSetSensitive (GBLopen.prestr_minperTF,True);
    XtSetSensitive (GBLopen.prestr_maxperTF,True);
    XtSetSensitive (GBLopen.minTF,False);
    XtSetSensitive (GBLopen.maxTF,False);
  }

  if (XtIsManaged (DB)) {
    XtUnmanageChild (DB);
    XtManageChild (DB);
  }
  else {
    XtManageChild (DB);
  }
 
  return;
}


void CBopenDBokPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBopenDBokPB - callback for ok pushbutton on open dialog
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Display number	
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the user activates the ok pushbutton
*	on the open dialog. It checks for user input error and the calls
*	the routines to create and load the display.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Mar 24 1998 TLS, Copy original min/max values to actual
*                       min/max values.
*       Mar 05 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{
  int dsp;
  int i;
  INT4 ret;
  INT4 dflag;
  FLOAT4 junk;
  DSPINFO *d;
  int b1,b2,b3;
  char *sptr;
  double rmin,gmin,bmin;
  double rmax,gmax,bmax;
  INT4 csize[3],ccont[3];
  INT4 ssize[3],scont[3];
  int minin,maxin;

  dsp = GBLdsp.free;
  d = &GBLdsp.d[dsp];

  d->autostr = XmToggleButtonGetState (GBLopen.autoTB);
  d->manstr = XmToggleButtonGetState (GBLopen.manTB);

  if (d->autostr) {
    sptr = XmTextFieldGetString (GBLopen.prestr_minperTF);
    sscanf (sptr,"%lf",&d->prestr_minper);
    XtFree (sptr);

    sptr = XmTextFieldGetString (GBLopen.prestr_maxperTF);
    sscanf (sptr,"%lf",&d->prestr_maxper);
    XtFree (sptr);
  }

  sptr = XmTextFieldGetString (GBLopen.minTF);
  minin = sscanf (sptr,"%lf %lf %lf",&rmin,&gmin,&bmin); 
  XtFree (sptr);

  sptr = XmTextFieldGetString (GBLopen.maxTF);
  maxin = sscanf (sptr,"%lf %lf %lf",&rmax,&gmax,&bmax); 
  XtFree (sptr);

/*  Red Band   */
  sptr = XmTextFieldGetString (GBLopen.redTF);
  b1 = strtol (sptr,NULL,0);
  XtFree (sptr);

/*  Green Band  */
  sptr = XmTextFieldGetString (GBLopen.grnTF);
  b2 = strtol (sptr,NULL,0);
  XtFree (sptr);

/*  Blue Band   */
  sptr = XmTextFieldGetString (GBLopen.bluTF);
  b3 = strtol (sptr,NULL,0);
  XtFree (sptr);

  if ((b1 < 0) || (b1 > d->f.nb)) {
    strcpy (GBLerr.string,"Invalid band for red/mono");
    QviewMessage (-1);
    return;
  }

  if ((b2 < 0) || (b2 > d->f.nb)) {
    strcpy (GBLerr.string,"Invalid band for green");
    QviewMessage (-1);
    return;
  }

  if ((b3 < 0) || (b3 > d->f.nb)) {
    strcpy (GBLerr.string,"Invalid band for blue");
    QviewMessage (-1);
    return;
  }

  if (!(b1 || b2 || b3)) {
    strcpy (GBLerr.string,"Red, green, and blue can not all be zero");
    QviewMessage (-1);
    return;
  }
  
  if (xinfo.vinfo.class == TrueColor) {
    if (!(d->autostr || d->manstr)) {
      strcpy (GBLerr.string,"Because this display is in TrueColor mode ");
      strcat (GBLerr.string,"an automatic or manual stretch should be ");
      strcat (GBLerr.string,"used when loading the cube");
      QviewMessage (-1);
      return;
    }
  }
  else if (xinfo.vinfo.class == PseudoColor) {
    if (b2 || b3) {
      if (!(d->autostr || d->manstr)) {
        strcpy (GBLerr.string,"Because this display is in PseudoColor mode ");
        strcat (GBLerr.string,"and you have chosen to display a three band ");
        strcat (GBLerr.string,"color composite, ");
        strcat (GBLerr.string,"an automatic or manual stretch should be ");
        strcat (GBLerr.string,"used when loading the cube");
        QviewMessage (-1);
        return;
      }
/*
      strcpy (GBLerr.string,"Can not display color composite on PseudoColor ");
      strcat (GBLerr.string,"device yet (find True or DirectColor device)");
      QviewMessage (-1);
      return;
*/
    }
  }

  if (d->f.type != 1) {
    if (!(d->autostr || d->manstr)) {
      strcpy (GBLerr.string,"Because the cube contains 16-bit or 32-bit ");
      strcat (GBLerr.string,"pixels ");
      strcat (GBLerr.string,"an automatic or manual stretch should be ");
      strcat (GBLerr.string,"used when loading the cube");
      QviewMessage (-1);
      return;
    }
  }

  if (d->manstr) {
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
      QviewMessage (-1);
      return;
    }

    if (b2) {
      if (gmin >= gmax) {
        strcpy (GBLerr.string,"Minimum must be less than maximum");
        QviewMessage (-1);
        return;
      }
    }

    if (b3) {
      if (bmin >= bmax) {
        strcpy (GBLerr.string,"Minimum must be less than maximum");
        QviewMessage (-1);
        return;
      }
    }
  }

  XtUnmanageChild (GBLopen.DB);
  XSync (xinfo.display,False);

  d->band[0] = b1;
  d->band[1] = b2;
  d->band[2] = b3;
/*  d->shell = NULL;*/
  
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

  for (i=0; i<MAX_BANDS; i++) d->avalid[i] = False;

  if (d->manstr) {
    d->rmin = rmin;
    d->gmin = gmin;
    d->bmin = bmin;
    d->rmax = rmax;
    d->gmax = gmax;
    d->bmax = bmax;
  }
  else if (d->autostr) {
    QviewWatch (xinfo.topShell,True);
    CalcAutoStr (dsp);
    QviewWatch (xinfo.topShell,False);
  }
  else {
    d->rmin = 0;
    d->gmin = 0;
    d->bmin = 0;
    d->rmax = 255;
    d->gmax = 255;
    d->bmax = 255;
  } 

/*-----------------------------------------------------------------
/  Save original min/max values.
/----------------------------------------------------------------*/
  d->rmin_real = d->rmin;
  d->rmax_real = d->rmax;
  d->bmin_real = d->bmin;
  d->bmax_real = d->bmax;
  d->gmin_real = d->gmin;
  d->gmax_real = d->gmax;


  LoadStrLut (dsp);
  CreateDisplay ();
  XtSetSensitive (d->shell,True);

  return;
}



void CBopenDBcancelPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBopenDBcancelPB - callback for open dialog cancel pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Not used
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the cancel pushbutton is pressed by
*	the user on the open dialog box.  

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Mar 05 2002 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{
  FILEINFO *f;
  INT4 ret;

  f = &GBLdsp.d[GBLdsp.free].f;
  q_close (f->fid,KEEP_FILE,&ret);
  GBLdsp.used[GBLdsp.free] = False;
  XtUnmanageChild (GBLopen.DB);

  return;
}



void CBopenDBchangeTB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBopenDBokPB - callback for open ok pushbutton
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Not used
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the user presses one of the stretch
*       toggle buttons in the open dialog box.  If the no stretch button
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

  noTB = XmToggleButtonGetState (GBLopen.noTB);
  autoTB = XmToggleButtonGetState (GBLopen.autoTB);
  manTB = XmToggleButtonGetState (GBLopen.manTB);


  if (noTB) {
    XtSetSensitive (GBLopen.prestr_minperTF,False);
    XtSetSensitive (GBLopen.prestr_maxperTF,False);
    XtSetSensitive (GBLopen.minTF,False);
    XtSetSensitive (GBLopen.maxTF,False);
  }

  if (autoTB) {
    XtSetSensitive (GBLopen.prestr_minperTF,True);
    XtSetSensitive (GBLopen.prestr_maxperTF,True);
    XtSetSensitive (GBLopen.minTF,False);
    XtSetSensitive (GBLopen.maxTF,False);
  }
  if (manTB) {
    XtSetSensitive (GBLopen.prestr_minperTF,False);
    XtSetSensitive (GBLopen.prestr_maxperTF,False);
    XtSetSensitive (GBLopen.minTF,True);
    XtSetSensitive (GBLopen.maxTF,True);
  }

  return;

}
