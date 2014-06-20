#include <stdlib.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#define QV_DECLARE_GLOBALS
#include "pc2d.h"
#define PCP_DECLARE_GLOBALS
#include "pcp.h"
#include "clinom_ppars.h"

#include "isismain.h"
#include "isistypes.h"
#include "isismain_c.h"
 
#define PC2D main

int PC2D (int argc, char *argv[])

/************************************************************************
*
*_Title PC2D - Interactive photoclinometry program
*
*_Hist  May 18 2001 Janet Barrett U.S.G.S. Flagstaff, Original Version
*                   adapted from qview.c
*_End
***********************************************************************/

{

  Widget   qviewMW;
  Widget   qviewMB;

  Status status;
  Visual *visual;

  INT4 len,count,ret;

  char *fall_res[2];

  void QviewInit (void);
  void ColormapInit (void);
  void InitDisplay (void);
  INT4 InitPcp (void);
  void CreatePcpLogW (void);

  void QviewResources (void);

/*******************************************************************************
*  Start of function
*******************************************************************************/

  u_std_init ("PC2D",
              "2008-04-10",
              "Perform interactive photoclinometry");

  u_get_file_parm ("FROM",1,(CHAR *)from,sizeof(from),&len,&count,&ret);
/*  u_get_file_parm ("FROM",1,(CHAR *)from[0],sizeof(from[0]),&len,&count,&ret);
  u_get_file_parm ("FROM2",1,(CHAR *)from[1],sizeof(from[1]),&len,&count,&ret);
  if (count > 0) 
    C_PPARS1->nps = 2;
  else*/
    C_PPARS1->nps = 1;

  QviewResources ();
/*---------------------------------------------------------------------------/
/  Create the topshell and get information about the it and then
/  set up the MWM close menu item to call pulldown exit button routine
/---------------------------------------------------------------------------*/
  if ( strcmp(GBLdefault.font,"") != 0) {
    fall_res[0] = (char *) malloc(80); 
    strcpy (fall_res[0], "*fontList: "); 
    strcat (fall_res[0], GBLdefault.font);
    fall_res[1] = NULL;
  }
  else {
    fall_res[0] = NULL;
  }
  xinfo.topShell = XtAppInitialize (&xinfo.appContext,"qview",
                                    NULL,0,&argc,argv,fall_res,NULL,0);

  XtVaSetValues (xinfo.topShell,XmNtitle,"PC2D",
                                XmNiconName,"PC2D",
		                XmNx, 5,
                                XmNy, 5,
                                NULL);

  xinfo.display = XtDisplay (xinfo.topShell);
  xinfo.screen = XtScreen (xinfo.topShell);
  xinfo.scr_num = DefaultScreen (xinfo.display);
  xinfo.root = RootWindowOfScreen (xinfo.screen);
  xinfo.depth = DefaultDepthOfScreen (xinfo.screen);

  xinfo.imgGC = XCreateGC (xinfo.display,xinfo.root,0,NULL);
  xinfo.cmap = DefaultColormapOfScreen (xinfo.screen);
  xinfo.height = DisplayHeight (xinfo.display,xinfo.scr_num);
  xinfo.width = DisplayWidth (xinfo.display,xinfo.scr_num);

  visual = DefaultVisual (xinfo.display,xinfo.scr_num);
  status = XMatchVisualInfo (xinfo.display,xinfo.scr_num,xinfo.depth,
                             DirectColor,&xinfo.vinfo);
  if ((!status) || (xinfo.vinfo.visual != visual)) {
    status = XMatchVisualInfo (xinfo.display,xinfo.scr_num,xinfo.depth,
                               TrueColor,&xinfo.vinfo);
    if ((!status) || (xinfo.vinfo.visual != visual)) {
      status = XMatchVisualInfo (xinfo.display,xinfo.scr_num,xinfo.depth,
                                 PseudoColor,&xinfo.vinfo);
      if ((!status) || (xinfo.vinfo.visual != visual)) {
        u_error ("BADVISUAL","Qview does not support default visual",-1,2);
        u_std_exit ();
      }
    }
  }

/*----------------------------------------------------------------------------
/  Initialize the qview structures
/---------------------------------------------------------------------------*/

  QviewInit ();

/*----------------------------------------------------------------------------
/  Create a main window manager with a work area and a menu bar only
/---------------------------------------------------------------------------*/

  qviewMW = XmCreateMainWindow (xinfo.topShell,"qviewMW",NULL,0);
  qviewMB = CreateDisplayMenu (qviewMW);
  qviewFM = XmCreateForm(qviewMW,"qviewFM",NULL,0);
  InitDisplay ();

/*----------------------------------------------------------------------------
/  Manage the children
/---------------------------------------------------------------------------*/
 
  XmMainWindowSetAreas (qviewMW, qviewMB, NULL, NULL, NULL, qviewFM);

  XtManageChild (qviewFM);
  XtManageChild (qviewMB);
  XtManageChild (qviewMW);

  XtRealizeWidget (xinfo.topShell);
  XSync (xinfo.display,False);
  ColormapInit ();
  (void) CreatePcpLogW();

  if (InitPcp()) u_std_exit();
  u_end_parm ();

/*----------------------------------------------------------------------------
/  Initialize the photoclinometry driver 
/---------------------------------------------------------------------------*/

  printf(n1+1)
  printf(nx)
  printf(ny)
  printf(&weknown)
  printf(&n1lmag)
  printf(&n2lmag)
  printf( &n4lmag,&nxlmag,&nylmag,&wrtimg,&mag,&minmag,res[0],azinc[0],
    fastinj,&ret)
  (void) xcmain_gui(&cdum[n1],n1+1,nx,ny,0,0,&aeknown,&n1lmag,&n2lmag,
    &n4lmag,&nxlmag,&nylmag,&wrtimg,&mag,&minmag,res[0],azinc[0],
    fastinj,&ret);

  XtAppMainLoop (xinfo.appContext);

  return 0; 
}
