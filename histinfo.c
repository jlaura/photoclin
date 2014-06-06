#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/ScrolledW.h>

#include "pc2d.h"


typedef struct {
  INT4 bandindx;
  INT4 band;
  INT4 nbins;
  INT4 npix;
  FLOAT4 min;
  FLOAT4 max;
  FLOAT8 mean;
  FLOAT8 stdev;
  FLOAT8 skew;
  INT4   mode;
  FLOAT4 median;
  FLOAT4 bin[256];
  INT4   hist[256];
  INT4   tpix[256];
  FLOAT4 pct[256];
  FLOAT4 cumpct[256];
  INT4   sppix[6];
} HISTSTATS;

struct {
  Widget DB;
  Widget     DA;
  Widget     FileNameVLB;
  Widget     NpixVLB;
  Widget     BandVLB;
  Widget     MinVLB;
  Widget     MaxVLB;
  Widget     MeanVLB;
  Widget     StdevVLB;
  Widget     SkewVLB;
  Widget     MedianVLB;
  Widget     NonValidVLB;
  Widget     NullCountVLB;
  Widget     LRepCountVLB;
  Widget     LInsCountVLB;
  Widget     HRepCountVLB;
  Widget     HInsCountVLB;
  Widget     DNListLB;
  Widget     NpixListLB;
  Widget     TpixListLB;
  Widget     PctListLB;
  Widget     ApctListLB;
  HISTSTATS  S;
} GBLhistinfo[V_MAXDSP];




void CalcHist (INT4 dsp, HISTSTATS *S); 
void UpdateHistInfoDB (Widget id,XtPointer client_data,XtPointer call_data);
void CBexposeHistDA (Widget id,XtPointer client_data,XtPointer call_data);


void CreateHistInfoDB (int dsp)
/***********************************************************************
*
*	--------------------------
*	|    CreateHistInfoDB    |
*	--------------------------
*
*_Title	CreateHistInfoDB - Create the Histogram Information dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm  int    dsp              I       display number
*			
*_DESC	This routine creates the dialog box when Histogram Information
*	is chosen.
*
*_HIST	Sep 09 1996 Tracie Sucharski, USGS, Flagstaff, Original version
*       Sep 20 1996 TLS, Added separate row/column for the statistical
*                     information.
*       Dec 17 1996 TLS, Added special pixel counts.
*       Mar 05 1999 TLS, Added exit callback, so that a new histogram
*                     will be calculated when the dialog box has been
*                     deleted and then restarted.
*       Apr 25 2000 TLS, Allow more characters for DN value.
*       Dec 14 2000 TLS, Create graphical histogram in additon to tabular
*                        form.
*	
*_END	
************************************************************************/

{
  Widget DB;
  Widget histDA;
  Widget RC1;
  Widget   FileNameLB;
  Widget   BandLB;
  Widget   FileNameVLB;
  Widget   BandVLB;
  Widget   NpixLB;
  Widget   MinLB;
  Widget   MaxLB;
  Widget   MeanLB;
  Widget   StdevLB;
  Widget   SkewLB;
  Widget   MedianLB;
  Widget   NonValidLB;
  Widget   NullCountLB;
  Widget   LRepCountLB;
  Widget   LInsCountLB;
  Widget   HRepCountLB;
  Widget   HInsCountLB;
  Widget   NpixVLB;
  Widget   MinVLB;
  Widget   MaxVLB;
  Widget   MeanVLB;
  Widget   StdevVLB;
  Widget   SkewVLB;
  Widget   MedianVLB;
  Widget   NonValidVLB;
  Widget   NullCountVLB;
  Widget   LRepCountVLB;
  Widget   LInsCountVLB;
  Widget   HRepCountVLB;
  Widget   HInsCountVLB;
  Widget HistHeaderLB;
  Widget HistSW;
  Widget   FM;
  Widget     DNListLB;
  Widget     NpixListLB;
  Widget     TpixListLB;
  Widget     PctListLB;
  Widget     ApctListLB;
  Widget CM;
  Widget RC3;
  Widget   exitPB;
  Widget   nextPB;
  Widget   helpPB;

  Window   win;
 
  Arg argList;
  char string[40];
  int i;
  int sx,ex,sy,ey;

  char *sptr;
  static Boolean firstime = True;
  XmString xstring;

  DSPINFO *d;

  void CBHistInfoDBexitPB(Widget id, XtPointer client_data, XtPointer call_data);


/*------------------------------------------------------------------------------
/  Initialization
/-----------------------------------------------------------------------------*/

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];

  if (firstime) {
    firstime = False;
    for (i=0; i<V_MAXDSP; i++) GBLhistinfo[i].DB = NULL;
  }

  GBLhistinfo[dsp].S.band = d->band[0];
/*-----------------------------------------------------------------------------
/  Calculate histogram data and statistics.
/----------------------------------------------------------------------------*/
  CalcHist (dsp,&GBLhistinfo[dsp].S);

/*------------------------------------------------------------------------------
/  Create the dialog box if it is not already created.
/-----------------------------------------------------------------------------*/
 
  if (GBLhistinfo[dsp].DB == NULL) {
    DB = XmCreateFormDialog (d->shell,"HistInfoDB",NULL,0);
    sprintf (sptr,"Display %d Histogram Information",dsp+1);
    xstring = XmStringCreateSimple (sptr);
    XtVaSetValues (DB,/*XmNdialogStyle,XmDIALOG_MODELESS,*/
                      XmNnoResize,True,
                      XmNdialogTitle,xstring,
                      NULL); 
    XmStringFree (xstring);
    GBLhistinfo[dsp].DB = DB;
    XtAddCallback (DB,XmNdestroyCallback,CBqviewDestroyWidget,
                   &GBLhistinfo[dsp].DB);

    XtSetArg (argList,XmNcolormap,xinfo.cmap);
    histDA = XmCreateDrawingArea (DB,"HistDA",&argList,1);
    XtVaSetValues (histDA,XmNtopAttachment,XmATTACH_FORM,
		   XmNtopOffset,10,
		   XmNleftAttachment,XmATTACH_FORM,
		   XmNleftOffset,10,
		   XmNbackground,BlackPixelOfScreen(xinfo.screen),
		   XmNheight,256,
		   XmNwidth,512,
		   NULL);
    XtManageChild (histDA);
    GBLhistinfo[dsp].DA = histDA;
    XtAddCallback (GBLhistinfo[dsp].DA,XmNexposeCallback,
                   CBexposeHistDA,(XtPointer)dsp);

/*------------------------------------------------------------------------------
/  Create the labels with various stats information.
/-----------------------------------------------------------------------------*/

    RC1 = XmCreateRowColumn (DB,"HistRC",NULL,0);
    XtVaSetValues (RC1,XmNtopAttachment, XmATTACH_WIDGET,
		   XmNtopWidget,histDA,
                      XmNtopOffset, 5,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,
                      XmNorientation, XmVERTICAL,
		      XmNadjustLast, False,
		      XmNadjustMargin,False,
		      XmNpacking, XmPACK_COLUMN,
                      XmNnumColumns, 2,
                      NULL);

    FileNameLB = XmCreateLabel (RC1,"Filename:",NULL,0);
    BandLB = XmCreateLabel (RC1,"Band:",NULL,0);
    NpixLB = XmCreateLabel (RC1,"Total pixels:",NULL,0);
    MinLB = XmCreateLabel (RC1,"Min:",NULL,0);
    MaxLB = XmCreateLabel (RC1,"Max:",NULL,0);
    MeanLB = XmCreateLabel (RC1,"Mean:",NULL,0);
    StdevLB = XmCreateLabel (RC1,"Stdev:",NULL,0);
    SkewLB = XmCreateLabel (RC1,"Skew:",NULL,0);
    MedianLB = XmCreateLabel (RC1,"Median:",NULL,0);
    NonValidLB = XmCreateLabel (RC1,"NonValid pixel count:",NULL,0);
    NullCountLB = XmCreateLabel (RC1,"Null count:",NULL,0);
    LRepCountLB = XmCreateLabel (RC1,"Low Repr count:",NULL,0);
    LInsCountLB = XmCreateLabel (RC1,"Low Instr count:",NULL,0);
    HRepCountLB = XmCreateLabel (RC1,"High Repr count:",NULL,0);
    HInsCountLB = XmCreateLabel (RC1,"High Instr count:",NULL,0);

    FileNameVLB = XmCreateLabel (RC1, d->f.name,NULL,0);
    GBLhistinfo[dsp].S.bandindx = 0;
    strcpy (sptr, "");
    sprintf (sptr, "%d", d->band[0]);
    BandVLB = XmCreateLabel (RC1,sptr,NULL,0);
    strcpy (sptr, "");
    sprintf (sptr, "%d", GBLhistinfo[dsp].S.npix);
    NpixVLB = XmCreateLabel (RC1,sptr,NULL,0);
    strcpy (sptr, "");
    sprintf (sptr, "%g", GBLhistinfo[dsp].S.min);
    MinVLB = XmCreateLabel (RC1,sptr,NULL,0);
    strcpy (sptr, "");
    sprintf (sptr, "%g", GBLhistinfo[dsp].S.max);
    MaxVLB = XmCreateLabel (RC1,sptr,NULL,0);
    strcpy (sptr, "");
    sprintf (sptr, "%g", GBLhistinfo[dsp].S.mean);
    MeanVLB = XmCreateLabel (RC1,sptr,NULL,0);
    strcpy (sptr, "");
    sprintf (sptr, "%g", GBLhistinfo[dsp].S.stdev);
    StdevVLB = XmCreateLabel (RC1,sptr,NULL,0);
    strcpy (sptr, "");
    sprintf (sptr, "%g", GBLhistinfo[dsp].S.skew);
    SkewVLB = XmCreateLabel (RC1,sptr,NULL,0);
    strcpy (sptr, "");
    sprintf (sptr, "%g", GBLhistinfo[dsp].S.median);
    MedianVLB = XmCreateLabel (RC1,sptr,NULL,0);
    sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[0]);
    NonValidVLB = XmCreateLabel (RC1,sptr,NULL,0);
    sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[1]);
    NullCountVLB = XmCreateLabel (RC1,sptr,NULL,0);
    sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[2]);
    LRepCountVLB = XmCreateLabel (RC1,sptr,NULL,0);
    sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[3]);
    LInsCountVLB = XmCreateLabel (RC1,sptr,NULL,0);
    sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[4]);
    HRepCountVLB = XmCreateLabel (RC1,sptr,NULL,0);
    sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[5]);
    HInsCountVLB = XmCreateLabel (RC1,sptr,NULL,0);
    

    GBLhistinfo[dsp].FileNameVLB = FileNameVLB;
    GBLhistinfo[dsp].BandVLB = BandVLB;
    GBLhistinfo[dsp].NpixVLB = NpixVLB;
    GBLhistinfo[dsp].MinVLB = MinVLB;
    GBLhistinfo[dsp].MaxVLB = MaxVLB;
    GBLhistinfo[dsp].MeanVLB = MeanVLB;
    GBLhistinfo[dsp].StdevVLB = StdevVLB;
    GBLhistinfo[dsp].SkewVLB = SkewVLB;
    GBLhistinfo[dsp].MedianVLB = MedianVLB;
    GBLhistinfo[dsp].NonValidVLB = NonValidVLB;
    GBLhistinfo[dsp].NullCountVLB = NullCountVLB;
    GBLhistinfo[dsp].LRepCountVLB = LRepCountVLB;
    GBLhistinfo[dsp].LInsCountVLB = LInsCountVLB;
    GBLhistinfo[dsp].HRepCountVLB = HRepCountVLB;
    GBLhistinfo[dsp].HInsCountVLB = HInsCountVLB;

    XtManageChild (FileNameLB);
    XtManageChild (FileNameVLB);
    XtManageChild (BandLB);
    XtManageChild (BandVLB);

    XtManageChild (NpixLB);
    XtManageChild (NpixVLB);
    XtManageChild (MinLB);
    XtManageChild (MaxLB);
    XtManageChild (MeanLB);
    XtManageChild (StdevLB);
    XtManageChild (SkewLB);
    XtManageChild (MedianLB);
    XtManageChild (NonValidLB);
    XtManageChild (NullCountLB);
    XtManageChild (LRepCountLB);
    XtManageChild (LInsCountLB);
    XtManageChild (HRepCountLB);
    XtManageChild (HInsCountLB);
    XtManageChild (MinVLB);
    XtManageChild (MaxVLB);
    XtManageChild (MeanVLB);
    XtManageChild (StdevVLB);
    XtManageChild (SkewVLB);
    XtManageChild (MedianVLB);
    XtManageChild (NonValidVLB);
    XtManageChild (NullCountVLB);
    XtManageChild (LRepCountVLB);
    XtManageChild (LInsCountVLB);
    XtManageChild (HRepCountVLB);
    XtManageChild (HInsCountVLB);

    XtManageChild (RC1);

/*-------------------------------------------------------------------------
/  Create heading for actual histogram.
/------------------------------------------------------------------------*/
    strcpy (sptr, "");
    strcpy (sptr,"          DN   ");
    strcat (sptr,"   Npix   ");
    strcat (sptr,"Total   ");
    strcat (sptr,"    Pct   ");
    strcat (sptr,"  AccPct   ");
    
    HistHeaderLB = XmCreateLabel (DB,sptr,NULL,0);
    XtVaSetValues (HistHeaderLB,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, RC1,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5,NULL);
    XtManageChild (HistHeaderLB);

/*-------------------------------------------------------------------------
/  Create scrolled window containing histogram data.
/------------------------------------------------------------------------*/
    XtSetArg (argList,XmNscrollingPolicy,XmAUTOMATIC);
    HistSW = XmCreateScrolledWindow (DB,"HistSW",&argList,1);
    XtVaSetValues (HistSW,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, HistHeaderLB,
                      XmNleftAttachment, XmATTACH_FORM,
                      XmNleftOffset, 5, 
                      XmNrightAttachment, XmATTACH_FORM,
                      XmNrightOffset, 5, NULL);
    XtManageChild (HistSW);
    
    FM = XmCreateForm (HistSW, "scrolledFM", NULL, 0);
    XtManageChild (FM);

    DNListLB = XmCreateLabel (FM, "DNListLB", NULL, 0);
    strcpy (sptr, "");
    strcpy (string, "");
    for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
      if (GBLhistinfo[dsp].S.hist[i] > 0) {
	sprintf (string, "%15.6f\n", GBLhistinfo[dsp].S.bin[i]);
	strcat(sptr, string);
      }
    }

    xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues (DNListLB, XmNtopAttachment, XmATTACH_FORM,
                   XmNbottomAttachment, XmATTACH_FORM,
                   XmNleftAttachment, XmATTACH_FORM,
                   XmNlabelString, xstring,
                   NULL);
    XmStringFree (xstring);
    GBLhistinfo[dsp].DNListLB = DNListLB;
    XtManageChild (DNListLB);

    NpixListLB = XmCreateLabel (FM, "NpixListLB", NULL, 0);
    strcpy (sptr, "");
    strcpy (string, "");
    for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
      if (GBLhistinfo[dsp].S.hist[i] > 0) {
	sprintf (string, "%7d\n", GBLhistinfo[dsp].S.hist[i]);
	strcat(sptr, string);
      }
    }

    xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues (NpixListLB, XmNtopAttachment, XmATTACH_FORM,
                   XmNbottomAttachment, XmATTACH_FORM,
                   XmNleftAttachment, XmATTACH_WIDGET,
                   XmNleftWidget, DNListLB,
                   XmNlabelString, xstring,
                   NULL);
    XmStringFree (xstring);
    GBLhistinfo[dsp].NpixListLB = NpixListLB;
    XtManageChild (NpixListLB);

    TpixListLB = XmCreateLabel (FM, "TpixListLB", NULL, 0);
    strcpy (sptr, "");
    strcpy (string, "");
    for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
      if (GBLhistinfo[dsp].S.hist[i] > 0) {
	sprintf (string, "%7d\n", GBLhistinfo[dsp].S.tpix[i]);
	strcat(sptr, string);
      }
    }

    xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues (TpixListLB, XmNtopAttachment, XmATTACH_FORM,
                   XmNbottomAttachment, XmATTACH_FORM,
                   XmNleftAttachment, XmATTACH_WIDGET,
                   XmNleftWidget, NpixListLB,
                   XmNlabelString, xstring,
                   NULL);
    XmStringFree (xstring);
    GBLhistinfo[dsp].TpixListLB = TpixListLB;
    XtManageChild (TpixListLB);

    PctListLB = XmCreateLabel (FM, "PctListLB", NULL, 0);
    strcpy (sptr, "");
    strcpy (string, "");
    for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
      if (GBLhistinfo[dsp].S.hist[i] > 0) {
	sprintf (string, "%10.2f\n", GBLhistinfo[dsp].S.pct[i]);
	strcat(sptr, string);
      }
    }

    xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues (PctListLB, XmNtopAttachment, XmATTACH_FORM,
                   XmNbottomAttachment, XmATTACH_FORM,
                   XmNleftAttachment, XmATTACH_WIDGET,
                   XmNleftWidget, TpixListLB,
                   XmNlabelString, xstring,
                   NULL);
    XmStringFree (xstring);
    GBLhistinfo[dsp].PctListLB = PctListLB;
    XtManageChild (PctListLB);

    ApctListLB = XmCreateLabel (FM, "ApctListLB", NULL, 0);
    strcpy (sptr, "");
    strcpy (string, "");
    for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
      if (GBLhistinfo[dsp].S.hist[i] > 0) {
	sprintf (string, "%10.2f\n", GBLhistinfo[dsp].S.cumpct[i]);
	strcat(sptr, string);
      }
    }

    xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
    XtVaSetValues (ApctListLB, XmNtopAttachment, XmATTACH_FORM,
                   XmNbottomAttachment, XmATTACH_FORM,
                   XmNleftAttachment, XmATTACH_WIDGET,
                   XmNleftWidget, PctListLB,
                   XmNrightAttachment, XmATTACH_FORM,
                   XmNlabelString, xstring,
                   NULL);
    XmStringFree (xstring);
    GBLhistinfo[dsp].ApctListLB = ApctListLB;
    XtManageChild (ApctListLB);




    CM = XmCreateSeparatorGadget (DB,"HistInfoCM",NULL,0);
    XtVaSetValues (CM,XmNtopAttachment, XmATTACH_WIDGET,
                      XmNtopWidget, HistSW,
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
                      XmNrightOffset, 300,
                      XmNorientation, XmHORIZONTAL,
                      XmNpacking, XmPACK_COLUMN,
                      XmNentryAlignment, XmALIGNMENT_CENTER,
                      XmNspacing, 35,
                      NULL);

    exitPB = XmCreatePushButton (RC3,"Exit",NULL,0);
    XtAddCallback (exitPB,XmNactivateCallback, 
                   CBHistInfoDBexitPB,(XtPointer)dsp);

    nextPB = XmCreatePushButton (RC3,"Next",NULL,0);
    XtAddCallback (nextPB, XmNactivateCallback, 
                   UpdateHistInfoDB, (XtPointer)(int)dsp);

    helpPB = XmCreatePushButton (RC3,"Help",NULL,0);
    XtAddCallback (helpPB,XmNactivateCallback,CBqviewHelp,"");
 
    XtManageChild (exitPB);
    XtManageChild (nextPB);
    XtManageChild (helpPB);
    XtManageChild (RC3);

  }


  if (XtIsManaged (GBLhistinfo[dsp].DB)) {
    XtUnmanageChild (GBLhistinfo[dsp].DB);
    XtManageChild (GBLhistinfo[dsp].DB);
  }
  else {
    XtManageChild (GBLhistinfo[dsp].DB);
  }
    /*---------------------------------------------------------------
      /  Draw histogram in drawing area.
      /------------------------------------------------------------*/


    XSetForeground (xinfo.display,xinfo.imgGC,GBLcolor.red.pixel);
    win = XtWindow(histDA);
    sx = 0;
    for (i=0; i<255; i++) {
      ex = sx + 2;
      if (GBLhistinfo[dsp].S.mode >= 0) { 
        sy = 255 - 255.0 / 
             GBLhistinfo[dsp].S.hist[GBLhistinfo[dsp].S.mode] * 
             GBLhistinfo[dsp].S.hist[i];
	ey = 255 - 255.0 / 
	     GBLhistinfo[dsp].S.hist[GBLhistinfo[dsp].S.mode] * 
             GBLhistinfo[dsp].S.hist[i+1];
	if (sy < 0) sy = 0;
	if (ey < 0) ey = 0;
      }
      else {
	sy = 255;
	ey = 255;
      }
      XDrawLine (xinfo.display,win,xinfo.imgGC,sx,sy,ex,ey);
      sx = ex;
    }
 
  return;
}






void UpdateHistInfoDB (Widget id, XtPointer client_data, XtPointer call_data)
/***********************************************************************
*
*	---------------------------
*	|    UpdateHistInfoDB    |
*	---------------------------
*
*_Title	UpdateHistInfoDB - Update the histogram information dialog box
*
*_Args	Type   Variable        I/O	Description
*_Parm  Widget     ID;              I   See Xt toolkit reference manual
*_Parm  XtPointer  client_data;     I   Contains dsp number but needs to
*                                       be cast as an int
*_Parm  XtPointer  call_data;       I   See Xt toolkit reference manual
*
*_DESC	This routine updates the HistInfo dialog box.
*
*_HIST	Sep 20 1996 Tracie Sucharski, USGS, Flagstaff, Original version
*       Sep 24 1996 TLS, Changed prototype.
*       Dec 17 1996 TLS, Added special pixel counts.
*	
*_END	
************************************************************************/

{

  int i;
  char string[40];
  XmString xstring;
  char *sptr;
  DSPINFO *d;

  int dsp = (int) client_data;

  d = &GBLdsp.d[dsp];
  sptr = (char *) GBLwork.work[0];


/*----------------------------------------------------------------------------
/  Increment band number.  If it doesn't exist, display error.
/---------------------------------------------------------------------------*/
  GBLhistinfo[dsp].S.bandindx++;
  GBLhistinfo[dsp].S.band = d->band[GBLhistinfo[dsp].S.bandindx];
  if (GBLhistinfo[dsp].S.band == 0) {
    GBLhistinfo[dsp].S.bandindx = 0;
    GBLhistinfo[dsp].S.band = d->band[GBLhistinfo[dsp].S.bandindx];
    strcpy (GBLerr.string,"There are no more bands displayed-");
    strcat (GBLerr.string,"Histograms will be calculated starting at ");
    strcat (GBLerr.string,"the first band displayed");
    QviewMessage (dsp);
  }

/*-----------------------------------------------------------------------------
/  Calculate histogram data and statistics.
/----------------------------------------------------------------------------*/
  CalcHist (dsp, &GBLhistinfo[dsp].S);

  strcpy (sptr, "");
  sprintf (sptr, "%d", GBLhistinfo[dsp].S.band);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].BandVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);
  
  strcpy (sptr, "");
  sprintf (sptr, "%g", GBLhistinfo[dsp].S.min);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].MinVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%g", GBLhistinfo[dsp].S.max);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].MaxVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%g", GBLhistinfo[dsp].S.mean);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].MeanVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%g", GBLhistinfo[dsp].S.stdev);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].StdevVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%g", GBLhistinfo[dsp].S.skew);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].SkewVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%g", GBLhistinfo[dsp].S.median);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].MedianVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[0]);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].NonValidVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[1]);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].NullCountVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[2]);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].LRepCountVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[3]);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].LInsCountVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[4]);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].HRepCountVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  sprintf (sptr, "%d", GBLhistinfo[dsp].S.sppix[5]);
  xstring = XmStringCreateSimple (sptr);
  XtVaSetValues (GBLhistinfo[dsp].HInsCountVLB,
                 XmNlabelString, xstring,
                 NULL);
  XmStringFree (xstring);

/*-----------------------------------------------------------------------------
/  Update histogram window.
/----------------------------------------------------------------------------*/
  strcpy (sptr, "");
  strcpy (string, "");
  for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
    if (GBLhistinfo[dsp].S.hist[i] > 0) {
      sprintf (string, "%15.6f\n", GBLhistinfo[dsp].S.bin[i]);
      strcat(sptr, string);
    }
  }
  
  xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues (GBLhistinfo[dsp].DNListLB, 
		 XmNlabelString, xstring,
		 NULL);
  XmStringFree (xstring);
  

  strcpy (sptr, "");
  strcpy (string, "");
  for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
    if (GBLhistinfo[dsp].S.hist[i] > 0) {
      sprintf (string, "%7d\n", GBLhistinfo[dsp].S.hist[i]);
      strcat(sptr, string);
    }
  }
  
  xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues (GBLhistinfo[dsp].NpixListLB, 
		 XmNlabelString, xstring,
		 NULL);
  XmStringFree (xstring);
  
  strcpy (sptr, "");
  strcpy (string, "");
  for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
    if (GBLhistinfo[dsp].S.hist[i] > 0) {
      sprintf (string, "%7d\n", GBLhistinfo[dsp].S.tpix[i]);
      strcat(sptr, string);
    }
  }
  
  xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues (GBLhistinfo[dsp].TpixListLB, 
		 XmNlabelString, xstring,
		 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  strcpy (string, "");
  for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
    if (GBLhistinfo[dsp].S.hist[i] > 0) {
      sprintf (string, "%10.2f\n", GBLhistinfo[dsp].S.pct[i]);
      strcat(sptr, string);
    }
  }
  
  xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues (GBLhistinfo[dsp].PctListLB, 
		 XmNlabelString, xstring,
		 NULL);
  XmStringFree (xstring);

  strcpy (sptr, "");
  strcpy (string, "");
  for (i=0; i<GBLhistinfo[dsp].S.nbins; i++) {
    if (GBLhistinfo[dsp].S.hist[i] > 0) {
      sprintf (string, "%10.2f\n", GBLhistinfo[dsp].S.cumpct[i]);
      strcat(sptr, string);
    }
  }
  
  xstring = XmStringCreateLtoR (sptr, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues (GBLhistinfo[dsp].ApctListLB, 
		 XmNlabelString, xstring,
		 NULL);
  XmStringFree (xstring);


  return;
}

void CBHistInfoDBexitPB (Widget id, XtPointer client_data, 
                             XtPointer call_data)
 
{
  int dsp = (int) client_data;
  DSPINFO *d;
 
  d = &GBLdsp.d[dsp];

  GBLhistinfo[dsp].DB = NULL;
  XtUnmanageChild (XtParent(XtParent(id)));
  
  return;
}

void CBexposeHistDA (Widget id,XtPointer client_data,XtPointer call_data)
/***********************************************************************
*
*_Title CBexposeHistDA - Callback when histogram drawing area is exposed.
*
*_Args  Type            Variable        I/O     Description
*_Parm  Widget          id              I       See Xt toolkit reference 
*                                               manual
*_Parm  XtPointer       client_data     I       Display number
*_Parm  XtPointer       call_data       I       See Xt toolkit reference
*                                               manual
 
*_DESC  This routine is called when the drawing area of the histogram
*       dialog is exposed.
*
*_HIST  Nov 05 2000  Tracie Sucharski, Original version
*       Dec 14 2000 TLS, Create graphical histogram in additon to tabular
*                        form.
*    
*_END
************************************************************************/

{
  
  int dsp = (int) client_data;

  INT4 i;
  int sx,ex,sy,ey;
  Window win;

  win = XtWindow (GBLhistinfo[dsp].DA);
  sx = 0;
  for (i=0; i<255; i++) {
    ex = sx + 2;
    if (GBLhistinfo[dsp].S.mode >= 0) { 
      sy = 255 - 255.0 / 
	GBLhistinfo[dsp].S.hist[GBLhistinfo[dsp].S.mode] * 
	GBLhistinfo[dsp].S.hist[i];
      ey = 255 - 255.0 / 
	GBLhistinfo[dsp].S.hist[GBLhistinfo[dsp].S.mode] * 
	GBLhistinfo[dsp].S.hist[i+1];
      if (sy < 0) sy = 0;
      if (ey < 0) ey = 0;
    }
    else {
      sy = 255;
      ey = 255;
    }
    XDrawLine (xinfo.display,win,xinfo.imgGC,sx,sy,ex,ey);
    sx = ex;
  }

  return;
}


