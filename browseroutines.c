#include <Xm/FileSB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <stdio.h>
#include <string.h>

#include "pc2d.h"
#include "spi.h"

struct {
  Widget DB;
} GBLbrowse[V_MAXDSP];

void CreateBrowseDB (int dsp)

/***********************************************************************
*
*_Title	CreateBrowseDB
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	Display Number	

*_DESC	This routine creates the browse dialog box which is a file
*	selection box. It was to work with the colormaps to make sure
*	flashing doesn't occur.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Nov 22 1996 Tracie Sucharski, Added resource for default path and
*                     filter.
*_END
************************************************************************/

{
  Widget DB;
  XmString xstring2;
  XmString xstring3;
  Widget PB;
  Widget FSB;
  Arg argList[5];
  DSPINFO *d;
  Widget widlist[4];
  char string[80];

  void CBbrowseFSBokPB (Widget id, XtPointer client_data, XtPointer call_data);
  void CBbrowseFSBcanPB (Widget id,XtPointer client_data,XtPointer call_data);

/***************************************************************************
* Create the file selection dialog box
***************************************************************************/

  d = &GBLdsp.d[dsp];

  if (d->curapp != 0) {
    strcpy (GBLerr.string,"Only one application can be run at a time ... ");
    strcat (GBLerr.string,"close open app and try again");
    QviewMessage (dsp);
    return;
  }

  d->curapp = V_BROWSE_APP;

  if (GBLbrowse[dsp].DB == NULL) {
    sprintf (string,"Browser Box %d",dsp+1);
    XtSetArg (argList[0],XmNcolormap,d->cmap);
    DB = XtAppCreateShell (string,string,transientShellWidgetClass,
                           xinfo.display,argList,1);
    FSB = XmCreateFileSelectionBox (DB,"browerDB",NULL,0);
    xstring2 = XmStringCreateSimple (GBLdefault.filter);
    xstring3 = XmStringCreateSimple (GBLdefault.path);
    XtVaSetValues (FSB,XmNpattern, xstring2, XmNdirectory, xstring3, NULL);
    XmStringFree (xstring2);
    GBLbrowse[dsp].DB = DB;
    XtManageChild (FSB);

    PB = XmFileSelectionBoxGetChild (FSB,XmDIALOG_OK_BUTTON);
    XtAddCallback (PB,XmNactivateCallback,CBbrowseFSBokPB,
                   (XtPointer)(int)dsp);

    PB = XmFileSelectionBoxGetChild (FSB,XmDIALOG_CANCEL_BUTTON);
    XtAddCallback (PB,XmNactivateCallback,CBbrowseFSBcanPB,
                   (XtPointer)(int)dsp);

    widlist[0] = DB;
    widlist[1] = FSB;
    XtSetWMColormapWindows (DB,widlist,2);
  }

/***************************************************************************
* Manage the dialog box
***************************************************************************/

  XtRealizeWidget (GBLbrowse[dsp].DB);

  return;
}




void CBbrowseFSBokPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBbrowseFSBokPB - File selection OK button Callback routine
*
*_Args	Type       Variable        I/O	Description
*_Parm	Widget     ID;	            I	See Xt toolkit reference manual
*_Parm	XtPointer  client_data;     I	Contains dsp number but needs to
*                                       be cast as an int
*_Parm	XtPointer  call_data;       I	See Xt toolkit reference manual
*
*_DESC	This routine is called when the OK pushbutton for the 
*       file selection dialog box of the browse routine is called.
*
*_HIST	Jan 01 1996 Jeff Anderson, USGS, Flagstaff - Original specs
*       Sep 09 1996 Tracie Sucharski, Read in backplane names and
*                     same the number of backplanes and backplane
*                     names to the file structure in qview.h.
*       Mar 27 1998 TLS,  When loading browse images re-save the
*                     filled display.
*
*_END	
************************************************************************/
{

  Widget fileTF;
  char *ptr;
  int dsp = (int) client_data;

  DSPINFO *d;
  char string[256];
  int i;
  INT4 ret,ifid;

  fileTF = XmFileSelectionBoxGetChild (XtParent(id),XmDIALOG_TEXT);
  ptr = XmTextGetString (fileTF);

  ifid = GBLdsp.d[dsp].f.fid;

  if (OpenCube (dsp,ptr)) {
    XtFree (ptr);
    return;
  }

  q_close (ifid,KEEP_FILE,&ret);

  d = &GBLdsp.d[dsp];
  d->world = True;
  d->save_tmp = True;
  d->read_tmp = False;

  sprintf (string,"Display %d:  ",dsp+1);
  strcat (string,d->f.name);
  XtVaSetValues (d->shell,XmNtitle,string,NULL);

  d->autostr = True;
  d->manstr = False;
  d->band[0] = 1;
  d->band[1] = 0;
  d->band[2] = 0;
  d->scs_sl = 1;
  d->scs_ss = 1;
  d->scs_el = d->f.nl;
  d->scs_es = d->f.ns;
  d->sl = 1;
  d->ss = 1;
/* Fix problem here if browse window size is not 512 */
  d->inc = ((double) d->f.nl) / 512.0;
  if (d->inc < ((double) d->f.ns) / 512.0) d->inc = ((double) d->f.ns)/512.0;
  d->rx_readwrite = False;
  d->rx_undopix = 0;
  for (i=0; i<MAX_BANDS; i++) d->avalid[i] = False;
  QviewWatch (xinfo.topShell,True);
  CalcAutoStr (dsp);
  QviewWatch (xinfo.topShell,False);
  LoadStrLut (dsp);
  LoadCube (dsp);

  XtFree (ptr);

  return;
}




 
void CBbrowseFSBcanPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBbrowerFSBcanPB - cancel button for browse dialog box
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	display number 
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	
*_DESC	This routine is called when the cancel button is pressed. The
*	browse dialog must be destroyed because when the display is 
*	destroyed, the colormap changes.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*_END
************************************************************************/

{
  int dsp = (int) client_data;

  XtDestroyWidget (GBLbrowse[dsp].DB);
  GBLbrowse[dsp].DB = NULL;

  GBLdsp.d[dsp].curapp = 0;

  return;
}
