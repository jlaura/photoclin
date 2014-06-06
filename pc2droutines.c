#include <stdlib.h>
#include <string.h>
#include <Xm/MessageB.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "pc2d.h"
#include "pc2d_bitmaps.h"

struct {
  Cursor watch;
  Cursor question;
} GBLcursor;

void QviewInit (void)

/***********************************************************************
*
*_Title	QviewInit - Initializations for qview	
*

*_DESC	This routine initializes the global structures, sets up
*	the watch cursor, and allocates some standard colors.
*	It also allocates dynamic memory for working buffers.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Nov 07 1996 Tracie Sucharski,  Added resource defaults.
*       May 08 1998 TLS, Added green to the global colors.
*       Oct 25 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{
  unsigned int i;
  Pixmap watchPM, maskPM;
  XColor other;

  XAllocNamedColor (xinfo.display,xinfo.cmap,"green",&GBLcolor.green,&other);
  XAllocNamedColor (xinfo.display,xinfo.cmap,"red",&GBLcolor.red,&other);
  XAllocNamedColor (xinfo.display,xinfo.cmap,"white",&GBLcolor.white,&other);
  XAllocNamedColor (xinfo.display,xinfo.cmap,"black",&GBLcolor.black,&other);
  XAllocNamedColor (xinfo.display,xinfo.cmap,"yellow",&GBLcolor.yellow,&other);

  for (i=0; i<V_MAXDSP; i++) {
    GBLdsp.used[i] = False;
    GBLdsp.linked[i] = False;
  }

  GBLdsp.blink_auto = GBLdefault.blink_auto;
  GBLdsp.blink_rate = GBLdefault.blink_rate;
  GBLdsp.blink_display = -1;
  GBLdsp.blink_cmap = False;

  for (i=0; i<4; i++) {
    GBLwork.work[i] = (char *) malloc (V_WORK_SIZE);
  }
  GBLwork.big = (char *) malloc (524288);

  watchPM = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                                         (char *)watch_bits,
                                         watch_height,watch_width,1,0,1);

  maskPM = XCreatePixmapFromBitmapData (xinfo.display,xinfo.root,
                                        (char *)watch_mask_bits,
                                        watch_mask_height,watch_mask_width,
                                        1,0,1);

  GBLcursor.watch = XCreatePixmapCursor (xinfo.display,watchPM,maskPM,
                                         &GBLcolor.white,&GBLcolor.black,
                                         watch_x_hot,watch_y_hot);

  GBLcursor.question = XCreateFontCursor (xinfo.display,XC_question_arrow);


  return;
}



void QviewMessage (int dsp)

/***********************************************************************
*
*_Title	QviewMessage - Used to report warnings to the user	
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	Display number	

*_DESC	When this routine is called it will pop up a dialog which
*	contains the error message in the global variable GBLerr.string
*

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Nov 21 1996 Tracie Sucharski, Check for newline character.
*       Oct 25 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{
  int pos,i,adsp;
  int comma,colon,brace,slash,newline;
  char *errstring,newstring[512],save[4],tmpstring[40];
  Widget shell;
  Widget DB;
  XmString xstring;
  static Widget shellDB = NULL;
  static Widget dspDB[V_MAXDSP];
  static Boolean firstime = True;

/****************************************************************************
* Initialization
*****************************************************************************/

  if (firstime) {
    for (i=0; i<V_MAXDSP; i++) dspDB[i] = NULL;
    firstime = False;
  }

/****************************************************************************
* Figure out which display to pop the error message up on
*****************************************************************************/

  if (dsp == -1) {
    adsp = 0;
    shell = xinfo.topShell;
    DB = shellDB; 
  }
  else {
    adsp = dsp + 1;
    shell = GBLdsp.d[dsp].shell;
    DB = dspDB[dsp];
  }

/****************************************************************************
* Create the dialog if it has not already been done
*****************************************************************************/

  if (DB == NULL) {
    DB = XmCreateWarningDialog (shell,"warningDB",NULL,0);
    if (dsp == -1) {
      shellDB = DB;
    }
    else {
      dspDB[dsp] = DB;
    }

    if (dsp == -1) {
      xstring = XmStringCreateSimple ("Warning");
    }
    else {
      sprintf (tmpstring,"Display %d warning",adsp);
      xstring = XmStringCreateSimple (tmpstring);
    }
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
  errstring = GBLerr.string;

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



void QviewWatch (Widget shell, Boolean watch)

/***********************************************************************
*
*_Title	QviewWatch - Replaces/Removes watch pointer 
*
*_Args	Type   Variable        I/O	Description
*_Parm	Widget	Shell		I	Not used
*_Parm 	Boolean	watch		I	True = watch pointer
*					False = regular pointer	

*_DESC	This routine changes the cursor from its default shape to
*	a watch and vice versa.
*

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Oct 25 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/
{

  Window win;
  int dsp;

  for (dsp=0; dsp<=V_MAXDSP; dsp++) {
    if (dsp == 0) {
      win = XtWindow (xinfo.topShell);
    }
    else {
      if (!GBLdsp.used[dsp-1]) continue;
      if (GBLdsp.d[dsp-1].shell == NULL) continue;
      win = XtWindow (GBLdsp.d[dsp-1].shell);
    }

    if (watch) {
      XDefineCursor (xinfo.display, win, GBLcursor.watch);
    }
    else {
      XUndefineCursor (xinfo.display, win);
    }
  }

  XSync (xinfo.display,False);
  XmUpdateDisplay (xinfo.topShell);

  return;
}



void QviewExit (void)

/***********************************************************************
*
*_Title	QviewExit - Call this routine to exit qview nicely
*
*_DESC	This routine should be called for qview to exit nicely
*	including closing files properly and calling the standard
*	ISIS exit procedure
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Mar 25 1998  TLS, Changed names of variables pertaining
*                      to world button from temp to fill.
*       Feb 12 1999  TLS, If reseau application has been used, write
*                      reseau keywords to label.
*       Apr 25 2000  TLS, Before trying to close fill file, make sure
*                      there is a valid file pointer.
*       Oct 25 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{
  int i;
  INT4 ret;
  CHAR tempname1[128];
  CHAR tempname2[128];
  CHAR *sptr;
  CHAR username[128];
  CHAR psinfo[512];
  FILE *fp1,*fp2;
  CHAR psid[25];

  sptr = (char *) GBLwork.work[0];

  for (i=0; i<V_MAXDSP; i++) {
    if (GBLdsp.used[i]) {
      q_close (GBLdsp.d[i].f.fid,KEEP_FILE,&ret);
      if (GBLdsp.d[i].fill_fp != NULL) {
	fclose (GBLdsp.d[i].fill_fp);
	remove (GBLdsp.d[i].fillfile);
      }
    }
  }

  tmpnam(tempname1);
  sprintf(sptr,"whoami > %s",tempname1);
  system(sptr);

  fp1 = fopen(tempname1,"r");
  fgets(username,sizeof(username),fp1);
  fclose(fp1);
  sprintf(sptr,"/bin/rm %s",tempname1);
  system(sptr);
  i = strlen(username);
  username[i-1] = EOS;

  tmpnam(tempname2);
  sprintf(sptr,"ps -ef | grep %s > %s",username,tempname2);
  system(sptr);
  fp2 = fopen(tempname2,"r");
  while (!feof(fp2)) {
    fgets(psinfo,sizeof(psinfo),fp2);
    i = strlen(psinfo);
    if (strstr(psinfo,"/usgs/dev/pgplot/bin/pgxwin_serv") != NULL) {
      sscanf(psinfo,"%s %s",username,psid);
      sprintf(sptr,"kill -9 %s",psid);
      system(sptr);
      goto pc2d_exit;
    }
  }

  pc2d_exit:
  fclose(fp2);
  sprintf(sptr,"/bin/rm %s",tempname2);
  system(sptr);
  u_std_exit ();

  return;
}



void QviewResources (void)

/***********************************************************************
*
*_Title	QviewResources - Obtains user defaults from resource file
*
*_DESC	THis routine obtain user defaults from the resource file
*	$HOME/.qview-defaults. These resources allow the user to
*	set default values for the zoom, roam, stretch, etc
*	tools.
*
*_HIST	Oct 31 1996 Jeff Anderson, Original version
*       Feb 17 1997 James M Anderson - fixed int* arg to scanf
*       Feb 06 1998 Tracie Sucharski, Changed prestretch low percent
*                    from 5.0 to 0.5.
*       Mar 18 1998 TLS,  Added measure tool option default.
*       Oct 25 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*
*_END
************************************************************************/

{
  XrmDatabase dbase;
  XrmValue value;
  char resfile[256];
  char *homedir;
  char *str_type[20];
  char string[512];
  double dval;
  float rval;
  int ival;
  int i;

/****************************************************************************
* Set up the default values 
*****************************************************************************/

  GBLdefault.zoom_factor = 2.0;
  GBLdefault.roam_percent = .75;
  GBLdefault.stretch_loper = 0.5;
  GBLdefault.stretch_hiper = 99.5;
  GBLdefault.showpos = False;
  strcpy (GBLdefault.rx_mode,"Point");
  strcpy (GBLdefault.rx_value,"Null");
  GBLdefault.rx_dn = 0.0;
  GBLdefault.rx_all = True;

  strcpy (GBLdefault.list,"List Core");
  GBLdefault.list_box_nl = 5;
  GBLdefault.list_box_ns = 5;
  strcpy (GBLdefault.list_file,"");

/*  GBLdefault.display_nl = 512;
  GBLdefault.display_ns = 512;*/
  GBLdefault.display_nl = 340;
  GBLdefault.display_ns = 340;
  GBLdefault.blink_auto = True;
  GBLdefault.blink_rate = 1.0;

  GBLdefault.prestr_lowper = 0.5;
  GBLdefault.prestr_hiper = 99.5;
  GBLdefault.prestr_linc = 1;
  GBLdefault.prestr_sinc = 1;

  strcpy (GBLdefault.filter,"*.cub*");
  strcpy (GBLdefault.path,"");
  strcpy (GBLdefault.font,"");
  GBLdefault.cursor = -1;

  strcpy (GBLdefault.measure_mode,"ISIS");

/****************************************************************************
* Try to open the resource file .qview-defaults
*****************************************************************************/

  homedir = getenv ("HOME");
  if (homedir == NULL) return; 

  strcpy (resfile,homedir);
  strcat (resfile,"/.qview-defaults"); 
  dbase = XrmGetFileDatabase (resfile);
  if (dbase == NULL) return;

/*--------------------------------------------------------------------------
/   Zero out error buffer.
/-------------------------------------------------------------------------*/
  for (i = 0; i <= 17; i++) GBLdefault.error[i] = 0;

/****************************************************************************
* Check for each of the resources 
*****************************************************************************/

  if (XrmGetResource (dbase,"qview.zoomFactor","Qview.ZoomFactor",
                      str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%lf",&dval) == 1) {
      if (dval > 1.0 && dval <= 10.0) 
	GBLdefault.zoom_factor = dval;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[0] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.roamPercent","Qview.RoamPercent",
                      str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%lf",&dval) == 1) {
      if (dval >= 25.0 && dval <= 100.0) 
	GBLdefault.roam_percent = dval/100.;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[1] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.stretchLowPercent",
                      "Qview.StretchLowPercent",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%lf",&dval) == 1) {
      if (dval <= 25.0) 
	GBLdefault.stretch_loper = dval;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[2] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.stretchHiPercent",
                      "Qview.StretchHiPercent",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%lf",&dval) == 1) {
      if (dval >= 75.0) 
	GBLdefault.stretch_hiper = dval;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[3] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.showPosition",
                      "Qview.ShowPosition",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (strcmp(string, "True") == 0) 
      GBLdefault.showpos = True;
  }

  if (XrmGetResource (dbase,"qview.doctorMode",
                      "Qview.DoctorMode",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if ( (strstr(string, "Point") != NULL) || 
         (strstr(string, "Horizontal line") != NULL) || 
         (strstr(string, "Start/End line") != NULL) ||       
         (strstr(string, "Rectangle") != NULL) )      
      strcpy(GBLdefault.rx_mode, string);
    else {
      GBLdefault.error[17] = 1;
      GBLdefault.error[4] = 1;
    }
  }

  if (XrmGetResource (dbase,"qview.doctorValue",
                      "Qview.DoctorValue",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if ( (strstr(string, "Null") != NULL) || 
         (strstr(string, "Low Saturation") != NULL) || 
         (strstr(string, "High Saturation") != NULL) ||       
         (strstr(string, "Dn") != NULL) )       
      strcpy(GBLdefault.rx_value, string);
    else {
      GBLdefault.error[17] = 1;
      GBLdefault.error[5] = 1;
    }
  }

  if (XrmGetResource (dbase,"qview.doctorDn",
                      "Qview.DoctorDn",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%f",&rval) == 1) {
      if (rval >= VALID_MIN4 && rval <= VALID_MAX4)
        GBLdefault.rx_dn = rval;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[6] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.doctorAll",
                      "Qview.DoctorAll",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (strcmp(string, "False") == 0) 
      GBLdefault.rx_all = False;
  }

  if (XrmGetResource (dbase,"qview.list",
                      "Qview.List",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if ( (strstr(string, "List Core") != NULL) || 
         (strstr(string, "List Backplanes") != NULL) )  
      strcpy(GBLdefault.list, string);
    else {
      GBLdefault.error[17] = 1;
      GBLdefault.error[7] = 1;
    }
  }

  if (XrmGetResource (dbase,"qview.listBoxNL",
                      "Qview.ListBoxNL",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%d",&ival) == 1) {
      if (ival >= 1 && ival <= 101 && ival % 2 != 0)
        GBLdefault.list_box_nl = ival;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[8] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.listBoxNS",
                      "Qview.ListBoxNS",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%d",&ival) == 1) {
      if (ival >= 1 && ival <= 101)
        GBLdefault.list_box_ns = ival;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[9] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.listFile",
                      "Qview.ListFile",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (strcmp(string,"") != 0) strcpy(GBLdefault.list_file, string);
  }

   if (XrmGetResource (dbase,"qview.displayNL",
                      "Qview.DisplayNL",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%d",&ival) == 1) {
      if (ival >= 100 && ival <= 1024)
        GBLdefault.display_nl = ival;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[10] = 1;
      }
    }
  }

   if (XrmGetResource (dbase,"qview.displayNS",
                      "Qview.DisplayNS",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%d",&ival) == 1) {
      if (ival >= 100 && ival <= 1024)
        GBLdefault.display_ns = ival;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[11] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.blinkAuto",
                      "Qview.BlinkAuto",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (strcmp(string, "False") == 0) GBLdefault.blink_auto = False;
  }

  if (XrmGetResource (dbase,"qview.blinkRate",
                      "Qview.BlinkRate",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%lf",&dval) == 1) {
      if (dval >= .1 && dval <= 5.0)
        GBLdefault.blink_rate = (int)(dval * 10);
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[12] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.prestretchLowPercent",
                      "Qview.PreStretchLowPercent",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%lf",&dval) == 1) {
      if (dval <= 25.0)
        GBLdefault.prestr_lowper = dval;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[13] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.prestretchHiPercent",
                      "Qview.PreStretchHiPercent",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%lf",&dval) == 1) {
      if (dval >= 75.0)
        GBLdefault.prestr_hiper = dval;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[14] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.prestretchLinc",
                      "Qview.PreStretchLinc",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%d",&ival) == 1) {
      if (ival >= 1)
        GBLdefault.prestr_linc = ival;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[15] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.prestretchSinc",
                      "Qview.PreStretchSinc",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (sscanf (string,"%d",&ival) == 1) {
      if (ival >= 1)
        GBLdefault.prestr_sinc = ival;
      else {
	GBLdefault.error[17] = 1;
	GBLdefault.error[16] = 1;
      }
    }
  }

  if (XrmGetResource (dbase,"qview.filter",
                      "Qview.Filter",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (strcmp(string, "") != 0) strcpy(GBLdefault.filter, string);
  }

  if (XrmGetResource (dbase,"qview.path",
                      "Qview.Path",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (strcmp(string, "") != 0) strcpy(GBLdefault.path, string);
  }

  if (XrmGetResource (dbase,"qview.font",
                      "Qview.Font",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (strcmp(string, "") != 0) strcpy(GBLdefault.font, string);
  }

  if (XrmGetResource (dbase,"qview.cursor",
                      "Qview.Cursor",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if (strcmp(string, "") != 0) {
      if (strstr(string, "X_cursor") != NULL) GBLdefault.cursor = 0;
      if (strstr(string, "arrow") != NULL) GBLdefault.cursor = 2;
      if (strstr(string, "cross") != NULL) GBLdefault.cursor = 30;
      if (strstr(string, "cross_reverse") != NULL) GBLdefault.cursor = 32;
      if (strstr(string, "crosshair") != NULL) GBLdefault.cursor = 34;
    }
  }

  if (XrmGetResource (dbase,"qview.measureMode",
                      "Qview.MeasureMode",str_type,&value) == True) {
    strncpy (string,value.addr,(int)value.size);
    if ( (strstr(string, "ISIS") != NULL) || 
         (strstr(string, "Cartographic") != NULL) ) 
      strcpy(GBLdefault.measure_mode, string);
    else {
      GBLdefault.error[17] = 1;
      GBLdefault.error[18] = 1;
    }
  }

 return;
}


Boolean QviewResourceErrors (XtPointer client_data)

/***********************************************************************
*
*_Title	QviewResourceErrors - Prints errors for QviewResources
*
*_DESC	This routine will create pop-up error messages when the user
*       has invalid values in their qview resource file.
*
*_HIST	Nov 18 1996 Tracie Sucharski, Original version
*       Mar 18 1998 TLS,  Added measure mode default errors.
*
*_END
************************************************************************/

{

  if (GBLdefault.error[17] == 0) return (True);

  strcpy (GBLerr.string, "The following resources are invalid in the ");
  strcat (GBLerr.string, "qview resource file - the default values shown will ");
  strcat (GBLerr.string, "be used:\n\n");


  if (GBLdefault.error[0] == 1) {
    strcat (GBLerr.string, "Qview.ZoomFactor:2.0\n");
  }
  if (GBLdefault.error[1] == 1) {
    strcat (GBLerr.string, "Qview.RoamPercent:75\n");
  }
  if (GBLdefault.error[2] == 1) {
    strcat (GBLerr.string, "Qview.StretchLowPercent:50\n");
  }    
  if (GBLdefault.error[3] == 1) {
    strcat (GBLerr.string, "Qview.StretchHiPercent:99.5\n");
  }    
  if (GBLdefault.error[4] == 1) {
    strcat (GBLerr.string, "Qview.DoctorMode:Point\n");
  }    
  if (GBLdefault.error[5] == 1) {
    strcat (GBLerr.string, "Qview.DoctorValue:Null\n");
  }    
  if (GBLdefault.error[6] == 1) {
    strcat (GBLerr.string, "Qview.DoctorDn:0.0\n");
  }    
  if (GBLdefault.error[7] == 1) {
    strcat (GBLerr.string, "Qview.List:List Core\n");
  }    
  if (GBLdefault.error[8] == 1) {
    strcat (GBLerr.string, "Qview.ListBoxNL:5\n");
  }    
  if (GBLdefault.error[9] == 1) {
    strcat (GBLerr.string, "Qview.ListBoxNS:5\n");
  }    
  if (GBLdefault.error[10] == 1) {
    strcat (GBLerr.string, "Qview.DisplayNL:512\n");
  }    
  if (GBLdefault.error[11] == 1) {
    strcat (GBLerr.string, "Qview.DisplayNS:512\n");
  }    
  if (GBLdefault.error[12] == 1) {
    strcat (GBLerr.string, "Qview.BlinkRate:1.0\n");
  }    
  if (GBLdefault.error[13] == 1) {
    strcat (GBLerr.string, "Qview.PreStretchLowPercent:5\n");
  }    
  if (GBLdefault.error[14] == 1) {
    strcat (GBLerr.string, "Qview.PreStretchHiPercent:99.5\n");
  }    
  if (GBLdefault.error[15] == 1) {
    strcat (GBLerr.string, "Qview.PreStretchHiLinc:2\n");
  }    
  if (GBLdefault.error[16] == 1) {
    strcat (GBLerr.string, "Qview.PreStretchHiSinc:2\n");
  }    
  if (GBLdefault.error[18] == 1) {
    strcat (GBLerr.string, "Qview.MeasureMode:Point\n");
  }    
  QviewMessage (-1);


  return (True);
}
