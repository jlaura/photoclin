#include <Xm/Scale.h>
#include <Xm/ToggleB.h>
#include "pc2d.h"
#include "pcp.h"
#include "maplib.h"
#include "clinom_msg.h"
#include "clinom_ipars.h"
#include "clinom_mem.h"
#include "clinom_dnorm.h"
#include "clinom_aepar.h"

void CBtoolPcpPB (Widget id, XtPointer client_data, XtPointer call_data)

/***********************************************************************
*
*_Title CBtoolPcpPB - callback for pcp tool push buttons
*
*_Args  Type            Variable        I/O     Description
*_Parm  Widget          id              I       See Xt toolkit reference 
*                                               manual
*_Parm  XtPointer       client_data     I       Display number
*_Parm  XtPointer       call_data       I       See Xt toolkit reference
*                                               manual
* 
*_DESC  This routine is called when the user presses a pcp tool push button.
*
*_HIST  Jun 18 2002 Janet Barrett, Original version
*    
*_END
************************************************************************/
 
{
  Widget *pcplist = (Widget *) client_data;
  int button;
  int i,j;
  INT4 ret;
  INT4 nz,dirmem;
  XEvent event;
  Boolean gobtn;
  CHAR *sptr;
  Boolean err;
  FLOAT4 min,max;
  Boolean str;
  INT4 resfactor;
  XmString xstring;
  INT4 Ldsp,Rdsp;
  INT4 fidp,naxes,order,core_size[3],suf_size[3];
  CHAR axis_name[3][MAXLITLEN+1];
  CHAR item_type[MAXLITLEN+1];
  FLOAT4 *fbuf;
  FLOAT4 fmag;
  INT4 limits[3][2],nx1,ny1;

  void UpdateDisplay (Boolean str);
 
  sptr = (char *) GBLwork.work[0];
  fbuf = (FLOAT4 *) GBLwork.big;

  XtVaGetValues (id,XmNuserData,&button,NULL);

  XmScaleGetValue (WMAXslider,&i);
  C_IPARS1->wmax = ((float) i)/10.;
  XmScaleGetValue (ITMAXslider,&i);
  C_IPARS2->itmax = i;
  C_IPARS1->ddztol = 1./((float) C_IPARS2->itmax);

  switch (button) {
    case V_RESUP_BTN :
      hinterror("","",0);
      for (i=1; i<8; i++) 
        XtVaSetValues (pcplist[i],XmNpushButtonEnabled,True,NULL);

      for (i=1; i<8; i++) {
        XtUnmapWidget (pcplist[i]);
        XtMapWidget (pcplist[i]);
      }

      pcpresstep = pcpresstep + 1;
      pcpundo[mag][0] = 2;
      pcpundo[mag][1] = pcpresstep;
      if (u_file_exist(pcpprevzfile[mag])) {
        remove(pcpprevzfile[mag]);
      } 
      sprintf(sptr,"cp %s %s",pcpdatafile[1],pcpprevzfile[mag]);
      system(sptr);
      if (u_file_exist(pcpprevrmsfile[mag])) {
        remove(pcpprevrmsfile[mag]);
      } 
      sprintf(sptr,"cp %s %s",pcpdatafile[2],pcpprevrmsfile[mag]);
      system(sptr);

      if (XmToggleButtonGetState(FASTbtn)) {
        fastinj = True;
      } else if (XmToggleButtonGetState(EXACTbtn)) {
        fastinj = False;
      }

      (void) xcmain_gui(&cdum[n1],n1+1,nx,ny,1,sordir,&aeknown,&n1lmag,
        &n2lmag,&n4lmag,&nxlmag,&nylmag,&wrtimg,&mag,&minmag,
	res[0],azinc[0],fastinj,&ret);
      resfactor = (INT4) (pow(2.0,mag));
      sprintf(sptr,"Resolution factor=%d",resfactor);
      xstring = XmStringCreateSimple(sptr);
      XtVaSetValues (fmagLB,XmNlabelString,xstring,NULL);
      XmStringFree(xstring);
      if (mag == 0) {
        XtSetSensitive(pcplist[5],False);
      } else {
        XtSetSensitive(pcplist[5],True);
      }
      XtSetSensitive(pcplist[4],True);
      if (wrtimg) {
        changeres = True;
	UpdateDisplay(False);
      }
      nz = (nxlmag+1)*(nylmag+1);
      dirmem = 10*nz + nz*(2*(nxlmag+1)+3) + nz*(nxlmag+2);
      if (dirmem <= C_MEM3->nmax) {
        XtSetSensitive(DIRbtn,True);
      } else {
        XtSetSensitive(DIRbtn,False);
        if (XmToggleButtonGetState(DIRbtn)) {
	  XmToggleButtonSetState(DIRbtn,False,False);
	  XmToggleButtonSetState(SORbtn,True,False);
        }
      }
    break;
    
    case V_RESDN_BTN :
      hinterror("","",0);

      pcpresstep = pcpresstep + 1;
      pcpundo[mag][0] = 2;
      pcpundo[mag][1] = pcpresstep;
      if (u_file_exist(pcpprevzfile[mag])) {
        remove(pcpprevzfile[mag]);
      } 
      sprintf(sptr,"cp %s %s",pcpdatafile[1],pcpprevzfile[mag]);
      system(sptr);
      if (u_file_exist(pcpprevrmsfile[mag])) {
        remove(pcpprevrmsfile[mag]);
      } 
      sprintf(sptr,"cp %s %s",pcpdatafile[2],pcpprevrmsfile[mag]);
      system(sptr);

      for (i=1; i<8; i++) 
        XtVaSetValues (pcplist[i],XmNpushButtonEnabled,True,NULL);

      for (i=1; i<8; i++) {
        XtUnmapWidget (pcplist[i]);
        XtMapWidget (pcplist[i]);
      }

      if (XmToggleButtonGetState(FASTbtn)) {
        fastinj = True;
      } else if (XmToggleButtonGetState(EXACTbtn)) {
        fastinj = False;
      }

      (void) xcmain_gui(&cdum[n1],n1+1,nx,ny,2,sordir,&aeknown,&n1lmag,
        &n2lmag,&n4lmag,&nxlmag,&nylmag,&wrtimg,&mag,&minmag,
	res[0],azinc[0],fastinj,&ret);
      resfactor = (INT4) (pow(2.0,mag));
      sprintf(sptr,"Resolution factor=%d",resfactor);
      xstring = XmStringCreateSimple(sptr);
      XtVaSetValues (fmagLB,XmNlabelString,xstring,NULL);
      XmStringFree(xstring);
      if (mag == minmag) {
        XtSetSensitive(pcplist[4],False);
      } else {
        XtSetSensitive(pcplist[4],True);
      }
      XtSetSensitive(pcplist[5],True);
      if (wrtimg) {
        changeres = True;
	UpdateDisplay(False);
      }
      nz = (nxlmag+1)*(nylmag+1);
      dirmem = 10*nz + nz*(2*(nxlmag+1)+3) + nz*(nxlmag+2);
      if (dirmem <= C_MEM3->nmax) {
        XtSetSensitive(DIRbtn,True);
      } else {
        XtSetSensitive(DIRbtn,False);
        if (XmToggleButtonGetState(DIRbtn)) {
	  XmToggleButtonSetState(DIRbtn,False,False);
	  XmToggleButtonSetState(SORbtn,True,False);
        }
      }
    break;

    case V_GO_BTN :
      hinterror("","",0);
      gobtn = True;

      for (i=1; i<8; i++) 
        XtVaSetValues (pcplist[i],XmNpushButtonEnabled,False,NULL);
  
      if (XmToggleButtonGetState(SORbtn)) {
        sordir = 0;
      } else if (XmToggleButtonGetState(DIRbtn)) {
        sordir = 1;
      } else {
        sordir = 2;
      }

      pcpundo[mag][0] = 1;
      pcpundo[mag][1] = 0;
      if (u_file_exist(pcpprevzfile[mag])) {
        remove(pcpprevzfile[mag]);
      } 
      sprintf(sptr,"cp %s %s",pcpdatafile[1],pcpprevzfile[mag]);
      system(sptr);
      if (u_file_exist(pcpprevrmsfile[mag])) {
        remove(pcpprevrmsfile[mag]);
      } 
      sprintf(sptr,"cp %s %s",pcpdatafile[2],pcpprevrmsfile[mag]);
      system(sptr);

      if (XmToggleButtonGetState(FASTbtn)) {
        fastinj = True;
      } else if (XmToggleButtonGetState(EXACTbtn)) {
        fastinj = False;
      }

      while (gobtn) {
        if (XCheckMaskEvent(xinfo.display,ButtonPressMask,&event)) {
          gobtn = False;
        } else {
          (void) xcmain_gui(&cdum[n1],n1+1,nx,ny,4,sordir,&aeknown,&n1lmag,
	    &n2lmag,&n4lmag,&nxlmag,&nylmag,&wrtimg,&mag,&minmag,
	    res[0],azinc[0],fastinj,&ret);
          if (wrtimg) UpdateDisplay(False);
        }
      }
    break;

    case V_GO1_BTN :
      hinterror("","",0);
      for (i=1; i<8; i++) 
        XtVaSetValues (pcplist[i],XmNpushButtonEnabled,False,NULL);

      if (XmToggleButtonGetState(SORbtn)) {
        sordir = 0;
      } else if (XmToggleButtonGetState(DIRbtn)) {
        sordir = 1;
      } else {
        sordir = 2;
      }

      pcpundo[mag][0] = 1;
      pcpundo[mag][1] = 0;
      if (u_file_exist(pcpprevzfile[mag])) {
        remove(pcpprevzfile[mag]);
      } 
      sprintf(sptr,"cp %s %s",pcpdatafile[1],pcpprevzfile[mag]);
      system(sptr);
      if (u_file_exist(pcpprevrmsfile[mag])) {
        remove(pcpprevrmsfile[mag]);
      } 
      sprintf(sptr,"cp %s %s",pcpdatafile[2],pcpprevrmsfile[mag]);
      system(sptr);

      if (XmToggleButtonGetState(FASTbtn)) {
        fastinj = True;
      } else if (XmToggleButtonGetState(EXACTbtn)) {
        fastinj = False;
      }

      (void) xcmain_gui(&cdum[n1],n1+1,nx,ny,4,sordir,&aeknown,&n1lmag,
        &n2lmag,&n4lmag,&nxlmag,&nylmag,&wrtimg,&mag,&minmag,
	res[0],azinc[0],fastinj,&ret);
      if (wrtimg) UpdateDisplay(False);
      XtRealizeWidget(xinfo.topShell);
      XSync(xinfo.display,False);
    break;

    case V_PAUSE_BTN :
    break;

    case V_DONE_BTN:
      hinterror("","",0);
      CreateDoneOptionsDB();
    break;

    case V_BACK_BTN:
      hinterror("","",0);
      if (pcpundo[mag][0] == 2) {
        for (i=0; i<15; i++) {
	  if (pcpundo[i][1] > pcpundo[mag][1]) {
	    pcpundo[i][0] = 0;
	    pcpundo[i][1] = 0;
          }
        }
      }
      remove(pcpdatafile[1]);
      sprintf(sptr,"cp %s %s",pcpprevzfile[mag],pcpdatafile[1]);
      system(sptr);
      remove(pcpdatafile[2]);
      sprintf(sptr,"cp %s %s",pcpprevrmsfile[mag],pcpdatafile[2]);
      system(sptr);
      pcpresstep = pcpundo[mag][1] - 1;
      pcpundo[mag][0] = 0;
      pcpundo[mag][1] = 0;
      fmag = pow(2.0,mag);
      q_open(&fidp,0,pcpprevzfile[mag],READ_ONLY,0,"",0,0,1,0,&ret);
      if (ret) {
        sprintf(C_MSG->pcpmsg,"Error opening backup file %s",pcpprevzfile[mag]);
	u_error("PCPTOOLCALLBACKS-OPENERR",C_MSG->pcpmsg,-2,1);
	break;
      }
      q_get_sys_keys(fidp,1,&naxes,&order,core_size,suf_size,
        (char *)axis_name,sizeof(axis_name[0]),&image_bytes,
	item_type,sizeof(item_type),image_scale,&ret);
      if (ret) {
        sprintf(C_MSG->pcpmsg,"Error reading from backup file %s",pcpprevzfile[mag]);
	u_error("PCPTOOLCALLBACKS-RDERR",C_MSG->pcpmsg,-2,1);
	break;
      }
      nx1 = nxlmag + 1;
      ny1 = nylmag + 1;
      for (i=1; i<=ny1; i++) {
        limits[0][0] = 1;
        limits[0][1] = nx1;
        limits[1][0] = i;
        limits[1][1] = i;
        limits[2][0] = 1;
        limits[2][1] = 1;
        (void) q_lio_cbrick(fidp,1,limits,(void *) fbuf,&ret);
        if (ret) {
          sprintf(C_MSG->pcpmsg,"Error reading from backup file %s",pcpprevzfile[mag]);
	  u_error("PCPTOOLCALLBACKS-RDERR",C_MSG->pcpmsg,-2,1);
	  break;
	}
	for (j=0; j<nx1; j++) {
	  fbuf[j] = (fbuf[j] * C_AEPAR1->aspect) / fmag;
        }
	(void) u_move4(nx1,fbuf,&cdum[n2lmag+(i-1)*nx1]);
      }
      (void) q_close(fidp,1,&ret);
      if (ret) {
        sprintf(C_MSG->pcpmsg,"Error reading from backup file %s",pcpprevzfile[mag]);
	u_error("PCPTOOLCALLBACKS-RDERR",C_MSG->pcpmsg,-2,1);
	break;
      }
      q_open(&fidp,0,pcpprevrmsfile[mag],READ_ONLY,0,"",0,0,1,0,&ret);
      if (ret) {
        sprintf(C_MSG->pcpmsg,"Error opening backup file %s",pcpprevrmsfile[mag]);
	u_error("PCPTOOLCALLBACKS-OPENERR",C_MSG->pcpmsg,-2,1);
	break;
      }
      q_get_sys_keys(fidp,1,&naxes,&order,core_size,suf_size,
        (char *)axis_name,sizeof(axis_name[0]),&image_bytes,
	item_type,sizeof(item_type),image_scale,&ret);
      if (ret) {
        sprintf(C_MSG->pcpmsg,"Error reading from backup file %s",pcpprevrmsfile[mag]);
	u_error("PCPTOOLCALLBACKS-RDERR",C_MSG->pcpmsg,-2,1);
	break;
      }
      nx1 = nxlmag + 1;
      ny1 = nylmag + 1;
      for (i=1; i<=ny1; i++) {
        limits[0][0] = 1;
        limits[0][1] = nx1;
        limits[1][0] = i;
        limits[1][1] = i;
        limits[2][0] = 1;
        limits[2][1] = 1;
        (void) q_lio_cbrick(fidp,1,limits,(void *) fbuf,&ret);
        if (ret) {
          sprintf(C_MSG->pcpmsg,"Error reading from backup file %s",pcpprevrmsfile[mag]);
	  u_error("PCPTOOLCALLBACKS-RDERR",C_MSG->pcpmsg,-2,1);
	  break;
	}
	for (j=0; j<nx1; j++) {
	  fbuf[j] = (fbuf[j] * C_AEPAR1->aspect) / fmag;
        }
	(void) u_move4(nx1,fbuf,&cdum[n4lmag+(i-1)*nx1]);
      }
      (void) q_close(fidp,1,&ret);
      if (ret) {
        sprintf(C_MSG->pcpmsg,"Error reading from backup file %s",pcpprevrmsfile[mag]);
	u_error("PCPTOOLCALLBACKS-RDERR",C_MSG->pcpmsg,-2,1);
	break;
      }
      UpdateDisplay(True);
    break;
  }

  if (mag == 0 && dozout) {
    if (u_file_exist(zoutfile)) remove(zoutfile);
    nx1 = nxlmag + 1;
    ny1 = nylmag + 1;
    zout.fileid = 0;
    zout.naxes = 3;
    zout.order = 1;
    zout.core_dims[0] = nx1;
    zout.suffix_dims[0] = 0;
    (void) strcpy(zout.axnames[0],"SAMPLE");
    zout.core_dims[1] = ny1;
    zout.suffix_dims[1] = 0;
    (void) strcpy(zout.axnames[1],"LINE");
    zout.core_dims[2] = 1;
    zout.suffix_dims[2] = 0;
    (void) strcpy(zout.axnames[2],"BAND");
    zout.pixel_type = 3;
    zout.scale[0] = 0.0;
    zout.scale[1] = 1.0;
    (void) u_type_keys(zout.pixel_type,&zout.item_bytes,
                       zout.core_type,sizeof(zout.core_type),&ret);
    if (ret) {
      sprintf(C_MSG->pcpmsg,"Error getting pixel type keywords for zout file: %s",
      zoutfile);
      pcpmessage(C_MSG->pcpmsg);
      goto skipzout;
    }
    (void) q_set_sys_keys(&zout.fileid,zout.naxes,zout.order,
             zout.core_dims,zout.suffix_dims,(CHAR *)zout.axnames,
	     sizeof(zout.axnames[0]),zout.item_bytes,
	     zout.core_type,zout.scale,&ret);
    if (ret) {
      sprintf(C_MSG->pcpmsg,"Error setting file characteristics for zout file: %s",
      zoutfile);
      pcpmessage(C_MSG->pcpmsg);
      goto skipzout;
    }
    (void) q_open(&zout.fileid,fid,zoutfile,3,0," ",0,0,1,0,&ret);
    if (ret) {
      sprintf(C_MSG->pcpmsg,"Error opening zout file: %s",zoutfile);
      pcpmessage(C_MSG->pcpmsg);
      goto skipzout;
    }
    for (i=1; i<=ny1; i++) {
      limits[0][0] = 1;
      limits[0][1] = nx1;
      limits[1][0] = i;
      limits[1][1] = i;
      limits[2][0] = 1;
      limits[2][1] = 1;
      (void) u_move4(nx1,&cdum[(i-1)*nx1+n2],fbuf);
      for (j=0; j<nx1; j++) {
	fbuf[j] = fbuf[j] * scale * cosemi;
      }
      (void) q_lio_cbrick(zout.fileid,2,limits,(void *)fbuf,&ret);
      if (ret) {
        sprintf(C_MSG->pcpmsg,"Error writing to zout file: %s",zoutfile);
        pcpmessage(C_MSG->pcpmsg);
        goto skipzout;
      }
    }
    (void) q_close(zout.fileid,1,&ret);
    if (ret) {
      sprintf(C_MSG->pcpmsg,"Error closing zout file: %s",zoutfile);
      pcpmessage(C_MSG->pcpmsg);
      goto skipzout;
    }
  }

skipzout:
  if (pcpundo[mag][0] != 0) {
    XtSetSensitive(pcplist[7],True);
  } else {
    XtSetSensitive(pcplist[7],False);
  }

  return;
}


void UpdateDisplay (Boolean str)
{
  INT4 limits[3][2],i,j,Ldsp,Rdsp;
  INT4 nvals,ret,slen,ival;
  Boolean err;
  FLOAT4 min,max,rval;
  INT4 pcpdata_nxlmag[5],pcpdata_nylmag[5],pcpdata_cdumloc[5];
  FLOAT4 *fbuf;
  FLOAT4 fmag;

  fbuf = (FLOAT4 *) GBLwork.big;
  fmag = pow(2.0,mag);

  pcpdata_nxlmag[0] = nxlmag;
  pcpdata_nxlmag[1] = nxlmag + 1;
  pcpdata_nxlmag[2] = nxlmag + 1;
  pcpdata_nxlmag[3] = nxlmag;
  pcpdata_nxlmag[4] = nxlmag;
  pcpdata_nylmag[0] = nylmag;
  pcpdata_nylmag[1] = nylmag + 1;
  pcpdata_nylmag[2] = nylmag + 1;
  pcpdata_nylmag[3] = nylmag;
  pcpdata_nylmag[4] = nylmag;
  pcpdata_cdumloc[0] = n1lmag;
  pcpdata_cdumloc[1] = n2lmag;
  pcpdata_cdumloc[2] = n4lmag;

  if (LeftDsp[3] || RightDsp[3]) {
    (void) synthetic(pcpdatafile[3],GBLwork.big,&cdum[n2lmag],
             nxlmag,nylmag,dndatum,C_DNORM->dnatm,fmag,fid);
  }
  if (LeftDsp[4] || RightDsp[4]) {
    (void) synthetic(pcpdatafile[3],GBLwork.big,&cdum[n2lmag],
             nxlmag,nylmag,dndatum,C_DNORM->dnatm,fmag,fid);
    (void) synthdiff(pcpdatafile[4],pcpdatafile[3],&cdum[n1lmag],
             GBLwork.big,nxlmag,nylmag,fmag,fid);
  }

  if (q_close(GBLdsp.d[0].f.fid,1,&ret)) {
    strcpy(C_MSG->pcpmsg,"Fatal error (-1) closing left display file.");
    pcpmessage(C_MSG->pcpmsg);
    QviewExit();
  }
  for (i=0; i<5; i++) {
    if (LeftDsp[i]) Ldsp = i;
  }
  if (Ldsp < 3) {
    if (q_open(&pcpdata[Ldsp].fileid,0,pcpdatafile[Ldsp],2,0," ",0,0,1,0,&ret) != 0) {
      strcpy(C_MSG->pcpmsg,"Fatal error (-2) opening left display file.");
      pcpmessage(C_MSG->pcpmsg);
      QviewExit();
    } else {
      if (q_close(pcpdata[Ldsp].fileid,2,&ret) != 0) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-3) deleting left display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      pcpdata[Ldsp].fileid = 0;
      pcpdata[Ldsp].naxes = 3;
      pcpdata[Ldsp].order = 1;
      pcpdata[Ldsp].core_dims[0] = pcpdata_nxlmag[Ldsp];
      pcpdata[Ldsp].suffix_dims[0] = 0;
      (void) strcpy(pcpdata[Ldsp].axnames[0],"SAMPLE");
      pcpdata[Ldsp].core_dims[1] = pcpdata_nylmag[Ldsp];
      pcpdata[Ldsp].suffix_dims[1] = 0;
      (void) strcpy(pcpdata[Ldsp].axnames[1],"LINE");
      pcpdata[Ldsp].core_dims[2] = 1;
      pcpdata[Ldsp].suffix_dims[2] = 0;
      (void) strcpy(pcpdata[Ldsp].axnames[2],"BAND");
      pcpdata[Ldsp].pixel_type = 3;
      pcpdata[Ldsp].scale[0] = 0.0;
      pcpdata[Ldsp].scale[1] = 1.0;
      (void) u_type_keys(pcpdata[Ldsp].pixel_type,&pcpdata[Ldsp].item_bytes,
               pcpdata[Ldsp].core_type,sizeof(pcpdata[Ldsp].core_type),&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-4) writing labels to left ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      (void) q_set_sys_keys(&pcpdata[Ldsp].fileid,pcpdata[Ldsp].naxes,
               pcpdata[Ldsp].order,pcpdata[Ldsp].core_dims,
	       pcpdata[Ldsp].suffix_dims,(CHAR *)pcpdata[Ldsp].axnames,
	       sizeof(pcpdata[Ldsp].axnames[0]),pcpdata[Ldsp].item_bytes,
	       pcpdata[Ldsp].core_type,pcpdata[Ldsp].scale,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-5) writing labels to left ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      (void) q_open(&pcpdata[Ldsp].fileid,fid,pcpdatafile[Ldsp],3,0," ",0,0,1,0,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-6) opening left display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      map_fixgrp(fid,pcpdata[Ldsp].fileid,1,1,fmag,fmag,0,&ret);
/*      nvals = 1;
      slen = strlen("RAW_DATA_NUMBER");
      (void) p_set_lit_key(pcpdata[Ldsp].fileid,"QUBE","","CORE_NAME",1,&nvals,
               "RAW_DATA_NUMBER",0,&slen,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-7) writing labels to left ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      nvals = 1;
      slen = strlen("NONE");
      (void) p_set_lit_key(pcpdata[Ldsp].fileid,"QUBE","","CORE_UNIT",1,&nvals,
	       "NONE",0,&slen,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-7) writing labels to left ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      nvals = 1;
      ival = 1;
      (void) p_set_int_key(pcpdata[Ldsp].fileid,"QUBE","BAND_BIN",
	       "BAND_BIN_ORIGINAL_BAND",1,&nvals,&ival,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-7) writing labels to left ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      nvals = 1;
      slen = strlen("NONE");
      (void) p_set_lit_key(pcpdata[Ldsp].fileid,"QUBE","BAND_BIN",
	       "BAND_BIN_UNIT",1,&nvals,"NONE",0,&slen,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-7) writing labels to left ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      nvals = 1;
      rval = 0.0;
      (void) p_set_real_key(pcpdata[Ldsp].fileid,"QUBE","BAND_BIN",
	       "BAND_BIN_CENTER",1,&nvals,&rval,1,6,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-7) writing labels to left ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }*/
      for (i=1; i<=pcpdata_nylmag[Ldsp]; i++) {
        limits[0][0] = 1;
        limits[0][1] = pcpdata_nxlmag[Ldsp];
        limits[1][0] = i;
        limits[1][1] = i;
        limits[2][0] = 1;
        limits[2][1] = 1;
	(void) u_move4(pcpdata_nxlmag[Ldsp],
	  &cdum[pcpdata_cdumloc[Ldsp]+(i-1)*pcpdata_nxlmag[Ldsp]],
	  fbuf);
	for (j=0; j<pcpdata_nxlmag[Ldsp]; j++) {
	  if (Ldsp != 0) {
	    fbuf[j] = (fbuf[j] * fmag) / C_AEPAR1->aspect;
          } else {
	    fbuf[j] = fbuf[j] / C_AEPAR1->bnorm;
	  }
        }
        (void) q_lio_cbrick(pcpdata[Ldsp].fileid,2,limits,
          (void *) fbuf,&ret);
        if (ret) {
          strcpy(C_MSG->pcpmsg,"Fatal error (-8) writing image data ");
          strcat(C_MSG->pcpmsg,"to left display file.");
          pcpmessage(C_MSG->pcpmsg);
          QviewExit();
        }
      } 
      (void) q_close(pcpdata[Ldsp].fileid,1,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-9) closing left display ");
	strcat(C_MSG->pcpmsg,"file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      if (str) {
        min = 0;
        max = 0;
      } else {
        min = GBLdsp.d[0].rmin;
        max = GBLdsp.d[0].rmax;
      }
      if (PreLoadCube(0,pcpdatafile[Ldsp],min,max,str,changeres)) err = True;
      else err = False;
      if (err == False) XtSetSensitive(xinfo.topShell,True);
    } 
  } else {
    if (str) {
      min = 0;
      max = 0;
    } else {
      min = GBLdsp.d[0].rmin;
      max = GBLdsp.d[0].rmax;
    }
    if (PreLoadCube(0,pcpdatafile[Ldsp],min,max,str,changeres)) err = True;
    else err = False;
    if (err == False) XtSetSensitive(xinfo.topShell,True);
  }

  if (q_close(GBLdsp.d[1].f.fid,1,&ret)) {
    strcpy(C_MSG->pcpmsg,"Fatal error (-10) closing right display ");
    strcat(C_MSG->pcpmsg,"file.");
    pcpmessage(C_MSG->pcpmsg);
    QviewExit();
  }
  for (i=0; i<5; i++) {
    if (RightDsp[i]) Rdsp = i;
  }
  if (Rdsp < 3) {
    if (q_open(&pcpdata[Rdsp].fileid,0,pcpdatafile[Rdsp],2,0," ",0,0,1,0,&ret) != 0) {
      strcpy(C_MSG->pcpmsg,"Fatal error (-11) opening right display ");
      strcat(C_MSG->pcpmsg,"file.");
      pcpmessage(C_MSG->pcpmsg);
      QviewExit();
    } else {
      if (q_close(pcpdata[Rdsp].fileid,2,&ret) != 0) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-12) deleting right display ");
        strcat(C_MSG->pcpmsg,"file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      pcpdata[Rdsp].fileid = 0;
      pcpdata[Rdsp].naxes = 3;
      pcpdata[Rdsp].order = 1;
      pcpdata[Rdsp].core_dims[0] = pcpdata_nxlmag[Rdsp];
      pcpdata[Rdsp].suffix_dims[0] = 0;
      (void) strcpy(pcpdata[Rdsp].axnames[0],"SAMPLE");
      pcpdata[Rdsp].core_dims[1] = pcpdata_nylmag[Rdsp];
      pcpdata[Rdsp].suffix_dims[1] = 0;
      (void) strcpy(pcpdata[Rdsp].axnames[1],"LINE");
      pcpdata[Rdsp].core_dims[2] = 1;
      pcpdata[Rdsp].suffix_dims[2] = 0;
      (void) strcpy(pcpdata[Rdsp].axnames[2],"BAND");
      pcpdata[Rdsp].pixel_type = 3;
      pcpdata[Rdsp].scale[0] = 0.0;
      pcpdata[Rdsp].scale[1] = 1.0;
      (void) u_type_keys(pcpdata[Rdsp].pixel_type,&pcpdata[Rdsp].item_bytes,
               pcpdata[Rdsp].core_type,sizeof(pcpdata[Rdsp].core_type),&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-13) setting system ");
        strcat(C_MSG->pcpmsg,"keywords for right display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      (void) q_set_sys_keys(&pcpdata[Rdsp].fileid,pcpdata[Rdsp].naxes,
               pcpdata[Rdsp].order,pcpdata[Rdsp].core_dims,
	       pcpdata[Rdsp].suffix_dims,(CHAR *)pcpdata[Rdsp].axnames,
	       sizeof(pcpdata[Rdsp].axnames[0]),pcpdata[Rdsp].item_bytes,
	       pcpdata[Rdsp].core_type,pcpdata[Rdsp].scale,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-14) writing system ");
        strcat(C_MSG->pcpmsg,"keywords to right display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      (void) q_open(&pcpdata[Rdsp].fileid,fid,pcpdatafile[Rdsp],3,0," ",0,0,1,0,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-15) opening right display ");
        strcat(C_MSG->pcpmsg,"file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      map_fixgrp(fid,pcpdata[Rdsp].fileid,1,1,fmag,fmag,0,&ret);
/*      nvals = 1;
      slen = strlen("RAW_DATA_NUMBER");
      (void) p_set_lit_key(pcpdata[Rdsp].fileid,"QUBE","","CORE_NAME",1,&nvals,
               "RAW_DATA_NUMBER",0,&slen,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-16) writing labels to right ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      nvals = 1;
      slen = strlen("NONE");
      (void) p_set_lit_key(pcpdata[Rdsp].fileid,"QUBE","","CORE_UNIT",1,&nvals,
  	       "NONE",0,&slen,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-16) writing labels to right ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      nvals = 1;
      ival = 1;
      (void) p_set_int_key(pcpdata[Rdsp].fileid,"QUBE","BAND_BIN",
	       "BAND_BIN_ORIGINAL_BAND",1,&nvals,&ival,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-16) writing labels to right ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      nvals = 1;
      slen = strlen("NONE");
      (void) p_set_lit_key(pcpdata[Rdsp].fileid,"QUBE","BAND_BIN",
	       "BAND_BIN_UNIT",1,&nvals,"NONE",0,&slen,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-16) writing labels to right ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      nvals = 1;
      rval = 0.0;
      (void) p_set_real_key(pcpdata[Rdsp].fileid,"QUBE","BAND_BIN",
	       "BAND_BIN_CENTER",1,&nvals,&rval,1,6,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-16) writing labels to right ");
        strcat(C_MSG->pcpmsg,"display file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }*/
      for (i=1; i<=pcpdata_nylmag[Rdsp]; i++) {
        limits[0][0] = 1;
        limits[0][1] = pcpdata_nxlmag[Rdsp];
        limits[1][0] = i;
        limits[1][1] = i;
        limits[2][0] = 1;
        limits[2][1] = 1;
	(void) u_move4(pcpdata_nxlmag[Rdsp],
	  &cdum[pcpdata_cdumloc[Rdsp]+(i-1)*pcpdata_nxlmag[Rdsp]],
	  fbuf);
	for (j=0; j<pcpdata_nxlmag[Rdsp]; j++) {
	  if (Rdsp != 0) {
	    fbuf[j] = (fbuf[j] * fmag) / C_AEPAR1->aspect;
          } else {
	    fbuf[j] = fbuf[j] / C_AEPAR1->bnorm;
	  }
        }
        (void) q_lio_cbrick(pcpdata[Rdsp].fileid,2,limits,
          (void *) fbuf,&ret);
        if (ret) {
          strcpy(C_MSG->pcpmsg,"Fatal error (-17) writing image data ");
          strcat(C_MSG->pcpmsg,"to right display file.");
          pcpmessage(C_MSG->pcpmsg);
          QviewExit();
        }
      } 
      (void) q_close(pcpdata[Rdsp].fileid,1,&ret);
      if (ret) {
        strcpy(C_MSG->pcpmsg,"Fatal error (-18) closing right display ");
	strcat(C_MSG->pcpmsg,"file.");
        pcpmessage(C_MSG->pcpmsg);
        QviewExit();
      }
      if (str) {
        min = 0;
        max = 0;
      } else {
        min = GBLdsp.d[1].rmin;
        max = GBLdsp.d[1].rmax;
      }
      if (PreLoadCube(1,pcpdatafile[Rdsp],min,max,str,changeres)) err = True;
      else err = False;
      if (err == False) XtSetSensitive(xinfo.topShell,True);
    }
  } else {
    if (str) {
      min = 0;
      max = 0;
    } else {
      min = GBLdsp.d[1].rmin;
      max = GBLdsp.d[1].rmax;
    }
    if (PreLoadCube(1,pcpdatafile[Rdsp],min,max,str,changeres)) err = True;
    else err = False;
    if (err == False) XtSetSensitive(xinfo.topShell,True);
  }
  changeres = False;
  return;
}
