#include <Xm/RowColumn.h>

#include "pc2d.h"
#include "pcp.h"

Widget CreateDisplayMenu (Widget MW)

/***********************************************************************
*
*_Title	CreateDisplayMenu - Creates pulldowns for a display
*
*_Args	Type   Variable		        I/O	Description
*_Parm	Widget	Mw			I	Main window id 
*_Parm	Widget	CreateDisplayMenu	O	Output menu widget id

*_DESC	This routine creates the pulldown menus (file,options,show,etc)
*	for each display.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Sep 26 1996 Tracie Sucharski,  Added photo statistics.
*       Oct 09 1997 TLS, Added limbfit application.
*       Mar 18 1998 TLS,  Added options for measure tool.
*       Nov 12 1998 TLS, Add reseau tool.
*       Nov 14 1998 TLS, Add options for reseau tool.
*       Aug 19 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/
 
{

  Widget MB,RC;
  Widget widget;

  XmString file;
  XmString   opendsp;
  XmString   viewleft;
  XmString   viewright;
  XmString opts;
  XmString   zoom;
  XmString   roam;
  XmString   stretch;
  XmString   list;
  XmString   pcparam;
  XmString   size;
  XmString info;
  XmString   image;
  XmString   histogramleft;
  XmString   histogramright;
  XmString apps;
  XmString   smooth;
  XmString help;
  XmString   onpc2d;

  void CBdisplayFileMB (Widget id, XtPointer client_data, XtPointer call_data);
  void CBdisplayOptionsMB (Widget id, XtPointer client_data, 
                           XtPointer call_data);
  void CBapplicationsMB (Widget id, XtPointer client_data, XtPointer call_data);
  void CBinformationMB (Widget id, XtPointer client_data, XtPointer call_data);

/*------------------------------------------------------------------------------
/ Create the horizontal menu bar
/-----------------------------------------------------------------------------*/

  file = XmStringCreateSimple ("File");
  opts = XmStringCreateSimple ("Options");
  info = XmStringCreateSimple ("Show");
  apps = XmStringCreateSimple ("Applications");
  help = XmStringCreateSimple ("Help");

  MB = XmVaCreateSimpleMenuBar (MW,"qviewMB",
                                XmVaCASCADEBUTTON,file,'F',
                                XmVaCASCADEBUTTON,opts,'O',
                                XmVaCASCADEBUTTON,info,'S',
                                XmVaCASCADEBUTTON,apps,'A',
                                XmVaCASCADEBUTTON,help,'H',
                                NULL);

  XmStringFree (file);
  XmStringFree (opts);
  XmStringFree (info);
  XmStringFree (apps);
  XmStringFree (help);

/*------------------------------------------------------------------------------
/ Create the File pulldown menu
/-----------------------------------------------------------------------------*/

  opendsp = XmStringCreateSimple ("Open Display...");
  viewleft = XmStringCreateSimple ("Left View...");
  viewright = XmStringCreateSimple ("Right View...");

  RC = XmVaCreateSimplePulldownMenu (MB,"QviewMB",0,CBdisplayFileMB,
                                     XmVaPUSHBUTTON,opendsp,'O',NULL,NULL,
                                     XmVaPUSHBUTTON,viewleft,'L',NULL,NULL,
                                     XmVaPUSHBUTTON,viewright,'R',NULL,NULL,
                                     NULL);

  widget = XtNameToWidget (RC,"button_0");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  widget = XtNameToWidget (RC,"button_1");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  widget = XtNameToWidget (RC,"button_2");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);

  XmStringFree (viewleft);
  XmStringFree (viewright);
  XmStringFree (opendsp);

/*------------------------------------------------------------------------------
/ Create the Options pulldown menu
/-----------------------------------------------------------------------------*/

  zoom = XmStringCreateSimple ("Zoom Options...");
  roam = XmStringCreateSimple ("Roam Options...");
  stretch = XmStringCreateSimple ("Stretch Options...");
  list = XmStringCreateSimple ("List Options...");
  pcparam = XmStringCreateSimple ("PhotoClinometry Options...");
  size = XmStringCreateSimple ("Display size...");

  RC = XmVaCreateSimplePulldownMenu (MB,"QviewMB",1,CBdisplayOptionsMB,
                                     XmVaPUSHBUTTON,zoom,'Z',NULL,NULL,
                                     XmVaPUSHBUTTON,roam,'R',NULL,NULL,
                                     XmVaPUSHBUTTON,stretch,'S',NULL,NULL,
                                     XmVaPUSHBUTTON,list,'L',NULL,NULL,
                                     XmVaPUSHBUTTON,pcparam,'P',NULL,NULL,
/*                                     XmVaSEPARATOR,
                                     XmVaPUSHBUTTON,size,'i',NULL,NULL,*/
                                     NULL);


  widget = XtNameToWidget (RC,"button_0");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  widget = XtNameToWidget (RC,"button_1");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  widget = XtNameToWidget (RC,"button_2");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  widget = XtNameToWidget (RC,"button_3");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  widget = XtNameToWidget (RC,"button_4");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
/*  widget = XtNameToWidget (RC,"button_5");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);*/

  XmStringFree (zoom);
  XmStringFree (roam);
  XmStringFree (stretch);
  XmStringFree (list);
  XmStringFree (pcparam);
  XmStringFree (size);

/*-----------------------------------------------------------------------------
/ Create the show pulldown menu
/----------------------------------------------------------------------------*/

  image = XmStringCreateSimple ("Image Information");
  histogramleft = XmStringCreateSimple ("Left Histogram");
  histogramright = XmStringCreateSimple ("Right Histogram");

  RC = XmVaCreateSimplePulldownMenu (MB,"QviewMB",2,CBinformationMB,
                                     XmVaPUSHBUTTON,image,'I',NULL,NULL,
                                     XmVaPUSHBUTTON,histogramleft,'L',NULL,NULL,
                                     XmVaPUSHBUTTON,histogramright,'R',NULL,NULL,
                                     NULL);

  widget = XtNameToWidget (RC,"button_0");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  widget = XtNameToWidget (RC,"button_1");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  widget = XtNameToWidget (RC,"button_2");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);

  XmStringFree (image);
  XmStringFree (histogramleft);
  XmStringFree (histogramright);

/*------------------------------------------------------------------------------
/ Create the Applications pulldown menu
/-----------------------------------------------------------------------------*/

  smooth = XmStringCreateSimple ("Smooth");

  RC = XmVaCreateSimplePulldownMenu (MB,"QviewMB",3,CBapplicationsMB,
                                     XmVaPUSHBUTTON,smooth,'S',NULL,NULL,
                                     NULL);

  widget = XtNameToWidget (RC,"button_0");
  XtVaSetValues (widget,XmNuserData,GBLdsp.free,NULL);
  
  XmStringFree (smooth);

/*------------------------------------------------------------------------------
/ Create the Help pulldown menu
/-----------------------------------------------------------------------------*/

  onpc2d = XmStringCreateSimple ("On PC2D...");

  RC = XmVaCreateSimplePulldownMenu (MB,"QviewMB",4,NULL,
                                     XmVaPUSHBUTTON,onpc2d,'P',NULL,NULL,
                                     NULL);

  widget = XtNameToWidget (RC,"button_0");
  XtAddCallback (widget,XmNactivateCallback,CBqviewHelp,"");
  XtAddCallback (widget,XmNhelpCallback,CBqviewHelp,"");

  XmStringFree (onpc2d);

/*------------------------------------------------------------------------------
/ Tell the menubar which button is the help menu
/-----------------------------------------------------------------------------*/

  if ((widget = XtNameToWidget (MB,"button_4"))) {
    XtVaSetValues (MB,XmNmenuHelpWidget,widget,NULL);
  }

  return MB;

}



void CBdisplayFileMB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBdisplayFileMB - callback for display file menu
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Selected menu item
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the user selects an item from the
*	file pulldown menu of a display. It either changes the view
*	or sets the type of data displayed in the left/right display
*       areas.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Jan 06 1998 Tracie Sucharski, Update link dialog when closing 
*                       single display.
*       Mar 23 1998 TLS, Close temp file.
*       Mar 25 1998  TLS, Changed names of variables pertaining
*                      to world button from temp to fill.
*       Feb 16 1999 TLS, If reseau application is active, write reseaus
*                      to image labels before closing display.
*       Aug 19 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

{
  int button = (int) client_data;

  void CreateViewDB (int dsp);
  void CreateOpenDisplayDB (void);

  switch (button) {
/*  Open display */
    case 0:
      CreateOpenDisplayDB();
    break;

/*  Change left view */
    case 1:
      CreateViewDB (0);
    break;

/*  Change right view */
    case 2:
      CreateViewDB (1);
    break;
  }

  return;
}




void CBdisplayOptionsMB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBdisplayOptionsMB - callback for display options menu
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Selected menu item
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the user selects an item from the
*	options pulldown menu of a display. It allows the user to change
*	how tools operate.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Mar 18 1998 Tracie Sucharski, Added measure option dialog
*       Nov 14 1998 TLS, Add reseau option dialog.
*       Aug 19 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/
 
{
  int button = (int) client_data;
  int dsp;
  DSPINFO *d;

  void CreateZoomOptionsDB (int dsp);
  void CreateRoamOptionsDB (int dsp);
  void CreateListOptionsDB (int dsp);
  void CreateStretchOptionsDB (int dsp);
  void CreateSizeDB (int dsp);
  void CreatMeasureOptionsDB (int dsp);

  XtVaGetValues (id,XmNuserData,&dsp,NULL);
  d = &GBLdsp.d[dsp]; 

  switch (button) {
    case 0:
      CreateZoomOptionsDB (dsp);
    break;

    case 1:
      CreateRoamOptionsDB (dsp);
    break;

    case 2:
      CreateStretchOptionsDB (dsp);
    break;

    case 3:
      CreateListOptionsDB (dsp);
    break;

    case 4:
      CreateParamOptionsDB (dsp);
    break;

    case 5:
      CreateSizeDB (dsp);
    break;

  }

  return;
}



void CBapplicationsMB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBapplicationsMB - callback for display applications menu
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Selected menu item
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	

*_DESC	This routine is called when the user selects an item from the
*	applications pulldown menu of a display. It allows the user to 
*	select an application to apply to the display.

*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Oct 09 1997  Tracie Sucharski, Added limbfit application.
*       Nov 12 1998  TLS, Add reseaux application.
*       Aug 19 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*_END
************************************************************************/

 
{
  DSPINFO *d;
  int button = (int) client_data;
  int dsp = (int) client_data;
  INT4 ret;


  void UpdateDisplay (Boolean str);

  d = &GBLdsp.d[dsp];


  XtVaGetValues (id,XmNuserData,&dsp,NULL);

  switch (button) {
  /* Smooth */
    case 0:
      (void) xcmain_gui(&cdum[n1],n1,nx,ny,3,sordir,&aeknown,&n1lmag,
        &n2lmag,&n4lmag,&nxlmag,&nylmag,&wrtimg,&mag,&minmag,res[0],
	azinc[0],fastinj,&ret);
      if (wrtimg) UpdateDisplay(False);
    break;
  }


  return;
}



void CBinformationMB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title	CBinformationMB - Callback routine for the Information Menu
*
*_Args	Type   		Variable        I/O	Description
*_Parm	Widget		id		I	See Xt toolkit reference 
*						manual
*_Parm	XtPointer	client_data	I	Selected menu item
*_Parm	XtPointer	call_data	I	See Xt toolkit reference
*						manual	
*
*_DESC	This routine is called when the information menu is selected
*
*_HIST	Sep 04 1996 Tracie Sucharski, USGS, Flagstaff - Original specs
*       Sep 26 1996 TLS, Added photo statistics.
*       Aug 20 2001 Janet Barrett, Modified to work with the pc2d
*                   program.
*
*_END
************************************************************************/
 
{
  int button = (int) client_data;
  int dsp;

  void CreateImageInfoDB (int dsp);
  void CreateHistInfoDB (int dsp);

  XtVaGetValues (id,XmNuserData,&dsp,NULL);

  switch (button) {
    case 0:
      CreateImageInfoDB (dsp);
      break;

    case 1:
      CreateHistInfoDB (0);
      break;

    case 2:
      CreateHistInfoDB (1);
      break;
  }

  return;
}

