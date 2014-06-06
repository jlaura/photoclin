#include <unistd.h>
#include <Xm/Text.h>
#include "pc2d.h"
#include "pcp.h"
#include "clinom_datum.h"
#include "clinom_aepar.h"
#include "clinom_pgeom.h"
#include "clinom_dnorm.h"
#include "clinom_mem.h"
#include "clinom_ops.h"
#include "clinom_msg.h"
#include "lev.h"

INT4 FirstGuess (void)

/***********************************************************************
*
*_Title	FirstGuess - Generate initial guess at solution of topography
*
*_DESC	First approximation of solution
*
*_HIST	July 27 2001 Janet Barrett, Original version, created by
*                    porting the appropriate sections of pcsi.F
*       July 29 2002 JB, Changed ioct to ioct1 and got rid of code
*                    to reorient the image
*       Feb  01 2005 JB, Added a check to make sure that the input
*                    (FROM) file does not contain any special pixel
*                    values
*_END
************************************************************************/

{
  Boolean err;

  CHAR timdat[20];
  CHAR *sptr;
  CHAR zaxis_name[3][MAXLITLEN+1];
  CHAR zitem_type[MAXLITLEN+1];

  INT4 i,j;
  INT4 nxr,nyr;
  INT4 n1r,n2r,n3r,n4r,n5r,n6r;
  INT4 limits[3][2];
  INT4 ret;
  INT4 ibytes;
  INT4 satcnt[2];
  INT4 invcnt;
  INT4 nvals;
  INT4 slen;
  INT4 ival;
  INT4 nz,n6,n7,n8,n9,n8d,n9d;
  INT4 nrot;   /* Amount of memory needed for rotation + SSIPSF-PI */
  INT4 ioct1;
  INT4 fidz,zaxes,zorder,zcore_size[3],zsuf_size[3],zimage_bytes;

  FLOAT4 tr[6],tri[6];
  FLOAT4 rval;
  FLOAT4 rot45;
  FLOAT4 *ibuf;
  FLOAT4 tmp;
  FLOAT4 cod,sid;
  FLOAT4 border;
  FLOAT4 xmin,xmax,ymin,ymax;
  FLOAT4 xrmin,xrmax,yrmin,yrmax;
  FLOAT4 min,max;
  FLOAT4 zimage_scale[2];

  XmString xstring;

  sptr = (char *) GBLwork.work[0];

  sprintf(sptr,"Az of Characteristics=%f",charazgrid);
  xstring = XmStringCreateSimple(sptr);
  XtVaSetValues(charazLB,XmNlabelString,xstring,NULL);
  XmStringFree(xstring);

/******************************************************************************
* Arrange to flop image to most favorable orientation for SOR (code
* was acquired from pcsi)
******************************************************************************/
  nx = ins;
  ny = inl;
/*  if (charaz < -45.0) {
    ioct = -2;
    nx = inl;
    ny = ins;
    tmp = C_DATUM1->dzx0;
    C_DATUM1->dzx0 = -C_DATUM1->dzy0;
    C_DATUM1->dzy0 = tmp;
    tmp = C_DATUM1->dz10;
    C_DATUM1->dz10 = C_DATUM1->dz20;
    C_DATUM1->dz20 = -tmp;
    tmp = C_DATUM1->xs0;
    C_DATUM1->xs0 = -C_DATUM1->yl0;
    C_DATUM1->yl0 = tmp;
    tmp = C_DATUM1->xs0f;
    C_DATUM1->xs0f = -C_DATUM1->yl0f;
    C_DATUM1->yl0f = tmp;
    C_AEPAR1->dbrat = -C_AEPAR1->dbrat;
    tmp = C_PGEOM->si1[0];
    C_PGEOM->si1[0] = C_PGEOM->si2[0];
    C_PGEOM->si2[0] = -tmp;
    tmp = C_PGEOM->se1[0];
    C_PGEOM->se1[0] = C_PGEOM->se2[0];
    C_PGEOM->se2[0] = -tmp;
  } else if (charaz < 0.0) {
    ioct = -1;
    C_DATUM1->dzy0 = -C_DATUM1->dzy0;
    tmp = C_DATUM1->dz10;
    C_DATUM1->dz10 = C_DATUM1->dz20;
    C_DATUM1->dz20 = tmp;
    C_DATUM1->yl0 = -C_DATUM1->yl0;
    C_DATUM1->yl0f = -C_DATUM1->yl0f;
    C_AEPAR1->delaz = -C_AEPAR1->delaz;
    tmp = C_PGEOM->si1[0];
    C_PGEOM->si1[0] = C_PGEOM->si2[0];
    C_PGEOM->si2[0] = tmp;
    tmp = C_PGEOM->se1[0];
    C_PGEOM->se1[0] = C_PGEOM->se2[0];
    C_PGEOM->se2[0] = tmp;
  } else if (charaz <= 45.0) {*/
    ioct1 = 1;
/*  } else {
    ioct = 2;
    nx = inl;
    ny = ins;
    tmp = C_DATUM1->dzx0;
    C_DATUM1->dzx0 = C_DATUM1->dzy0;
    C_DATUM1->dzy0 = tmp;
    C_DATUM1->dz20 = -C_DATUM1->dz20;
    tmp = C_DATUM1->xs0;
    C_DATUM1->xs0 = C_DATUM1->yl0;
    C_DATUM1->yl0 = tmp;
    tmp = C_DATUM1->xs0f;
    C_DATUM1->xs0f = C_DATUM1->yl0f;
    C_DATUM1->yl0f = tmp;
    C_AEPAR1->delaz = -C_AEPAR1->delaz;
    C_AEPAR1->dbrat = -C_AEPAR1->dbrat;
    C_PGEOM->si2[0] = -C_PGEOM->si2[0];
    C_PGEOM->se2[0] = -C_PGEOM->se2[0];
  }*/

/******************************************************************************
* Calculate size of rotated system for SSIPSF-PI in characteristic
* coordinates. This code was extracted from pcsi to determine the
* size of the rotated image and amount of memory needed for rotation
* + SSIPSF-PI (pcsi appears to do this more correctly than xcsep)
******************************************************************************/
  if (C_AEPAR1->delaz != 0.0 || C_AEPAR1->aspect != 1.0) {
    cod = cos(C_AEPAR1->delaz*L_DEG2RAD);
    sid = sin(C_AEPAR1->delaz*L_DEG2RAD);
    tr[1] = cod*C_AEPAR1->aspect;
    tr[2] = -sid;
    tr[4] = sid*C_AEPAR1->aspect;
    tr[5] = cod;
    border = 0.5;
    xmin = 1.0-border;
    xmax = ((FLOAT4) nx)+border;
    ymin = 1.0-border;
    ymax = ((FLOAT4) ny)+border;
    xrmin = tr[1]*xmin+tr[2]*ymin;
    tmp = tr[1]*xmax+tr[2]*ymin;
    if (tmp < xrmin) xrmin = tmp;
    tmp = tr[1]*xmin+tr[2]*ymax;
    if (tmp < xrmin) xrmin = tmp;
    tmp = tr[1]*xmax+tr[2]*ymax;
    if (tmp < xrmin) xrmin = tmp;
    xrmax = tr[1]*xmin+tr[2]*ymin;
    tmp = tr[1]*xmax+tr[2]*ymin;
    if (tmp > xrmax) xrmax = tmp;
    tmp = tr[1]*xmin+tr[2]*ymax;
    if (tmp > xrmax) xrmax = tmp;
    tmp = tr[1]*xmax+tr[2]*ymax;
    if (tmp > xrmax) xrmax = tmp;
    yrmin = tr[4]*xmin+tr[5]*ymin;
    tmp = tr[4]*xmax+tr[5]*ymin;
    if (tmp < yrmin) yrmin = tmp;
    tmp = tr[4]*xmin+tr[5]*ymax;
    if (tmp < yrmin) yrmin = tmp;
    tmp = tr[4]*xmax+tr[5]*ymax;
    if (tmp < yrmin) yrmin = tmp;
    yrmax = tr[4]*xmin+tr[5]*ymin;
    tmp = tr[4]*xmax+tr[5]*ymin;
    if (tmp > yrmax) yrmax = tmp;
    tmp = tr[4]*xmin+tr[5]*ymax;
    if (tmp > yrmax) yrmax = tmp;
    tmp = tr[4]*xmax+tr[5]*ymax;
    if (tmp > yrmax) yrmax = tmp;
    tr[0] = border-xrmin;
    tr[3] = border-yrmin;
    nxr = (INT4) (xrmax-xrmin+1.0-2.0*border);
    if (((FLOAT4) nxr) < (xrmax-xrmin+1.0-2.0*border)) 
      nxr = nxr+1;
    nyr = (INT4) (yrmax-yrmin+1.0-2.0*border);
    if (((FLOAT4) nyr) < (yrmax-yrmin+1.0-2.0*border))
      nyr = nyr+1;
    /* Trace for the inverse of the above transformation, actually
       used in the interpolation */
    tri[1] = tr[5]/C_AEPAR1->aspect;
    tri[2] = -tr[2]/C_AEPAR1->aspect;
    tri[4] = -tr[4]/C_AEPAR1->aspect;
    tri[5] = tr[1]/C_AEPAR1->aspect;
    tri[0] = -(tri[1]*tr[0]+tri[2]*tr[3]);
    tri[3] = -(tri[4]*tr[0]+tri[5]*tr[3]);
  }

/******************************************************************************
* Allocate memory to accumulate input image(s), build topo solution.
* The amount of memory needed for different solution types came from the
* XCMAIN_INTER.F subroutine.  The DIRECT solution type requires that
* the allocated buffer be at least n9d in size, the SOR solution type
* requires that the allocated buffer be at least n8 in size, and the CGM
* solution type requires that the allocated buffer be at least n9 in
* size.
******************************************************************************/
  nz = (nx+1)*(ny+1);
  rot45 = ((FLOAT4) (nx+ny))/1.4142136;
/*  nrot = 2*nz+2*nxr*nyr+(nxr+1)*(nyr+1)+2*nxr+2*nyr;*/
  nrot = 2*nz+3*rot45*rot45+6*rot45+1;
  n1 = 0;
/*  n1 = 1;*/
  n2 = n1+nx*ny;
  n3 = n2+nz;
  n4 = n3+nz;
  n5 = n4+nz;
  n6 = n5+nz;
  n7 = n6+5*nz;
  n8 = n7+5*nz;
  n9 = n8+nz;
  n8d = n7+nz*(2*(nx+1)+3);
  n9d = n8d+nz*(nx+2); /*Amount of memory needed to do DIRECT*/
                       /*at full resolution                  */
  if (C_AEPAR1->delaz == 0.0 && C_AEPAR1->aspect == 1.0) {
    nrot = n5+2*ny;
  } else {
    n1r = n3;
    n2r = n1r+nxr*nyr;
    n3r = n2r+(nxr+1)*(nyr+1);
    n4r = n3r+nxr*nyr;
    n5r = n4r+2*nxr;
    n6r = n5r+2*nyr;
    nrot = n6r;
  }
  if ((nrot > n8 && C_MEM3->nmax < nrot) ||
      (n8 > nrot && C_MEM3->nmax < n8)) {
    goto more_memory_needed;
  }
  solntype = 1;
/*  cdum = malloc(sizeof (INT4) * C_MEM3->nmax);*/
  cdum = malloc(sizeof (INT4) * (C_MEM3->nmax+1));
  if (cdum == NULL) {
    goto not_enough_memory;
  }
  if (C_MEM3->nmax >= n9) solntype = 2;
  if (C_MEM3->nmax >= n9d) solntype = 3;
  if (solntype == 1) {
    XtSetSensitive(SORbtn,True);
    XtSetSensitive(CGMbtn,False);
    XtSetSensitive(DIRbtn,False);
  } else if (solntype == 2) {
    XtSetSensitive(SORbtn,True);
    XtSetSensitive(CGMbtn,True);
    XtSetSensitive(DIRbtn,False);
  } else {
    XtSetSensitive(SORbtn,True);
    XtSetSensitive(CGMbtn,True);
    XtSetSensitive(DIRbtn,True);
  }
  sordir = 0;

/******************************************************************************
* Fill the allocated memory with the input image
******************************************************************************/
  ibytes = (ins+1)*4;
  if ((ibuf = (FLOAT4 *) u_malloc(ibytes)) == NULL) goto mem_alloc_error;
  satcnt[0] = satcnt[1] = 0;

  for (i=1; i<=inl; i++) {
    limits[0][0] = 1;
    limits[0][1] = ins;
    limits[1][0] = i;
    limits[1][1] = i;
    limits[2][0] = 1;
    limits[2][1] = 1;
    (void) q_lio_cbrick(fid,1,limits,(void *) ibuf,&ret);
    if (ret) goto read_error;
    if (image_bytes != 4) {
      u_convert(image_bytes,3,ibuf,ibuf,ins,image_scale,image_scale,
                1,satcnt,&ret);
      if (ret) goto convert_error;
    }
    (void) chk4inv(ibuf,ins,1,&invcnt);
    if (invcnt > 0) goto in_chk4inv_error;
/*    (void) pblinein1(ibuf,ins,cdum,nx,ny,i,C_DNORM->dnatm,ioct1);*/
    (void) pblinein1(ibuf,ins,&cdum[n1],nx,ny,i,C_DNORM->dnatm,ioct1);
  }

/******************************************************************************
* Now scale the image
******************************************************************************/
  (void) pbscale(&cdum[n1],&cdum[n2],nx,ny,dndatum);

/******************************************************************************
* Write out the scaled image
******************************************************************************/
  u_datime(timdat,19);
  strcpy(pcpdatafile[0],"PC2D_IMAGE_FILE_");
  strcat(pcpdatafile[0],timdat);
  strcat(pcpdatafile[0],".cub");
  pcpdata[0].fileid = 0;
  pcpdata[0].naxes = 3;
  pcpdata[0].order = 1;
  pcpdata[0].core_dims[0] = nx;
  pcpdata[0].suffix_dims[0] = 0;
  (void) strcpy(pcpdata[0].axnames[0],"SAMPLE");
  pcpdata[0].core_dims[1] = ny;
  pcpdata[0].suffix_dims[1] = 0;
  (void) strcpy(pcpdata[0].axnames[1],"LINE");
  pcpdata[0].core_dims[2] = 1;
  pcpdata[0].suffix_dims[2] = 0;
  (void) strcpy(pcpdata[0].axnames[2],"BAND");
  pcpdata[0].pixel_type = 3;
  pcpdata[0].scale[0] = 0.0;
  pcpdata[0].scale[1] = 1.0;
  (void) u_type_keys(pcpdata[0].pixel_type,&pcpdata[0].item_bytes,
                     pcpdata[0].core_type,sizeof(pcpdata[0].core_type),&ret);
  if (ret) goto img_pixtype_error;
  (void) q_set_sys_keys(&pcpdata[0].fileid,pcpdata[0].naxes,pcpdata[0].order,
           pcpdata[0].core_dims,pcpdata[0].suffix_dims,(CHAR *)pcpdata[0].axnames,
	   sizeof(pcpdata[0].axnames[0]),pcpdata[0].item_bytes,
	   pcpdata[0].core_type,pcpdata[0].scale,&ret);
  if (ret) goto img_char_error;
  (void) q_open(&pcpdata[0].fileid,fid,pcpdatafile[0],3,0," ",0,0,1,0,&ret);
  if (ret) goto img_open_error;
/*  nvals = 1;
  slen = strlen("RAW_DATA_NUMBER");
  (void) p_set_lit_key(pcpdata[0].fileid,"QUBE","","CORE_NAME",1,&nvals,
           "RAW_DATA_NUMBER",0,&slen,&ret);
  if (ret) goto img_setcore_error;
  nvals = 1;
  slen = strlen("NONE");
  (void) p_set_lit_key(pcpdata[0].fileid,"QUBE","","CORE_UNIT",1,&nvals,
           "NONE",0,&slen,&ret);
  if (ret) goto img_setcore_error;
  nvals = 1;
  ival = 1;
  (void) p_set_int_key(pcpdata[0].fileid,"QUBE","BAND_BIN",
           "BAND_BIN_ORIGINAL_BAND",1,&nvals,&ival,&ret);
  if (ret) goto img_setband_error;
  nvals = 1;
  slen = strlen("NONE");
  (void) p_set_lit_key(pcpdata[0].fileid,"QUBE","BAND_BIN",
           "BAND_BIN_UNIT",1,&nvals,"NONE",0,&slen,&ret);
  if (ret) goto img_setband_error;
  nvals = 1;
  rval = 0.0;
  (void) p_set_real_key(pcpdata[0].fileid,"QUBE","BAND_BIN",
           "BAND_BIN_CENTER",1,&nvals,&rval,1,6,&ret);
  if (ret) goto img_setband_error;*/
  for (i=1; i<=ny; i++) {
    limits[0][0] = 1;
    limits[0][1] = nx;
    limits[1][0] = i;
    limits[1][1] = i;
    limits[2][0] = 1;
    limits[2][1] = 1;
/*    (void) u_move4(nx,&cdum[(i-1)*nx],ibuf);*/
    (void) u_move4(nx,&cdum[(i-1)*nx+n1],ibuf);
    for (j=0; j<nx; j++) {
      ibuf[j] = ibuf[j] / C_AEPAR1->bnorm;
    }
    (void) q_lio_cbrick(pcpdata[0].fileid,2,limits,(void *)ibuf,&ret);
    if (ret) goto img_wrt_error;
  }
  (void) q_close(pcpdata[0].fileid,1,&ret);
  if (ret) goto img_cls_error;
  min = max = 0;
  if (PreLoadCube (0,pcpdatafile[0],min,max,True,False)) err = True;
  else err = False;
  if (err == False) XtSetSensitive(xinfo.topShell,True);

/******************************************************************************
* Finally, generate the initial guess at the topography
******************************************************************************/
  strcpy(sptr,"The initial guess at the topography is being generated ");
  strcat(sptr,"and loaded.\nThis may take a few minutes.  Please be patient.");
  XmTextSetString (stattext,sptr);
  XmUpdateDisplay (xinfo.topShell);
  C_OPS1->nops = 0;
  if (usezin) {
    if (usedatum) {  /* Start with datum model only, no topo */
      C_OPS2->iguess = 1;
      (void) u_fill4(0.0,(nx+1)*(ny+1),&cdum[n2]);
    } else {         /* Start with stored Z */
      C_OPS2->iguess = 2;
      q_open(&fidz,0,zin,READ_ONLY,0,"",0,0,1,0,&ret);
      if (ret) goto zinopn_error;
      q_check_std(fidz,&ret);
      if (ret) goto zininv_error;
      q_get_sys_keys(fidz,1,&zaxes,&zorder,zcore_size,zsuf_size,
                     (char *)zaxis_name,sizeof(zaxis_name[0]),
		     &zimage_bytes,zitem_type,sizeof(zitem_type),
		     zimage_scale,&ret);
      if (ret) goto zin_bad_sys_keys;
      if (zimage_bytes != 4) goto zpixtyp_error;
      if (zcore_size[0] == ins+1 && zcore_size[1] == inl+1) {
        for (i=1; i<=zcore_size[1]; i++) {
          limits[0][0] = 1;
          limits[0][1] = zcore_size[0];
          limits[1][0] = i;
          limits[1][1] = i;
          limits[2][0] = 1;
          limits[2][1] = 1;
          (void) q_lio_cbrick(fidz,1,limits,(void *) ibuf,&ret);
          if (ret) goto zread_error;
	  for (j=0; j<zcore_size[0]; j++) {
	    if (ibuf[j] >= VALID_MIN4 && ibuf[j] <= VALID_MAX4) {
	      ibuf[j] = ibuf[j] / (scale*cosemi);
            }
	  }
	  (void) u_move4(zcore_size[0],ibuf,&cdum[n2+(i-1)*zcore_size[0]]);
        }
	(void) chk4inv(&cdum[n2],zcore_size[0],zcore_size[1],&invcnt);
	if (invcnt > 0) goto z_chk4inv_error;
      } else if (zcore_size[0] == ins && zcore_size[1] == inl) {
        for (i=1; i<=zcore_size[1]; i++) {
          limits[0][0] = 1;
          limits[0][1] = zcore_size[0];
          limits[1][0] = i;
          limits[1][1] = i;
          limits[2][0] = 1;
          limits[2][1] = 1;
          (void) q_lio_cbrick(fidz,1,limits,(void *) ibuf,&ret);
          if (ret) goto zread_error;
	  (void) u_move4(zcore_size[0],ibuf,&cdum[n3+(i-1)*zcore_size[0]]);
        }
	(void) chk4inv(&cdum[n3],zcore_size[0],zcore_size[1],&invcnt);
	if (invcnt > 0) goto z_chk4inv_error;
        (void) zinterp(&cdum[n3],&cdum[n2],ins,inl,scale,cosemi,&ret);
	if (ret) goto zinterp_error;
      } else {
        goto zinsize_error;
      }
      (void) q_close(fidz,1,&ret);
      if (ret) goto zcls_error;
    }
  } else {           /* Start with SSIPSF-PI */
    C_OPS2->iguess = 3;
    (void) chk4inv(&cdum[n1],nx,ny,&invcnt);
    if (invcnt > 0) goto chk4inv_error;
    (void) xcsep(&cdum[n1],C_MEM3->nmax,n1+1,n2+1,n3+1,n4+1,n5+1,nx,ny,1);
  }

/******************************************************************************
* Initialize arrays for undo files
******************************************************************************/
  for (i=0; i<15; i++) {
    strcpy(pcpprevzfile[i],"PC2D_Z_FILE_");
    strcat(pcpprevzfile[i],timdat);
    sprintf(sptr,".MAG%d.prev.cub",i);
    strcat(pcpprevzfile[i],sptr);
    strcpy(pcpprevrmsfile[i],"PC2D_RMS_FILE_");
    strcat(pcpprevrmsfile[i],timdat);
    sprintf(sptr,".MAG%d.prev.cub",i);
    strcat(pcpprevrmsfile[i],sptr);
    pcpundo[i][0] = 0;
    pcpundo[i][1] = 0;
  }
  pcpresstep = 0;

/******************************************************************************
* Write out the initial guess at topography
******************************************************************************/
  strcpy(pcpdatafile[1],"PC2D_Z_FILE_");
  strcat(pcpdatafile[1],timdat);
  strcat(pcpdatafile[1],".cub");
  pcpdata[1].fileid = 0;
  pcpdata[1].naxes = 3;
  pcpdata[1].order = 1;
  pcpdata[1].core_dims[0] = nx+1;
  pcpdata[1].suffix_dims[0] = 0;
  (void) strcpy(pcpdata[1].axnames[0],"SAMPLE");
  pcpdata[1].core_dims[1] = ny+1;
  pcpdata[1].suffix_dims[1] = 0;
  (void) strcpy(pcpdata[1].axnames[1],"LINE");
  pcpdata[1].core_dims[2] = 1;
  pcpdata[1].suffix_dims[2] = 0;
  (void) strcpy(pcpdata[1].axnames[2],"BAND");
  pcpdata[1].pixel_type = 3;
  pcpdata[1].scale[0] = 0.0;
  pcpdata[1].scale[1] = 1.0;
  (void) u_type_keys(pcpdata[1].pixel_type,&pcpdata[1].item_bytes,
                     pcpdata[1].core_type,sizeof(pcpdata[1].core_type),&ret);
  if (ret) goto topo_pixtype_error;
  (void) q_set_sys_keys(&pcpdata[1].fileid,pcpdata[1].naxes,pcpdata[1].order,
           pcpdata[1].core_dims,pcpdata[1].suffix_dims,(CHAR *)pcpdata[1].axnames,
	   sizeof(pcpdata[1].axnames[0]),pcpdata[1].item_bytes,
	   pcpdata[1].core_type,pcpdata[1].scale,&ret);
  if (ret) goto topo_char_error;
  (void) q_open(&pcpdata[1].fileid,fid,pcpdatafile[1],3,0," ",0,0,1,0,&ret);
  if (ret) goto topo_open_error;
  for (i=1; i<=ny+1; i++) {
    limits[0][0] = 1;
    limits[0][1] = nx+1;
    limits[1][0] = i;
    limits[1][1] = i;
    limits[2][0] = 1;
    limits[2][1] = 1;
    (void) u_move4(nx+1,&cdum[(i-1)*(nx+1)+n2],ibuf);
    for (j=0; j<nx+1; j++) {
      ibuf[j] = ibuf[j] / C_AEPAR1->aspect;
    }
    (void) q_lio_cbrick(pcpdata[1].fileid,2,limits,(void *)ibuf,&ret);
    if (ret) goto topo_wrt_error;
  }
  (void) q_close(pcpdata[1].fileid,1,&ret);
  if (ret) goto topo_cls_error;

  if (dozout) {
    if (u_file_exist(zoutfile)) goto zout_exists;
    zout.fileid = 0;
    zout.naxes = 3;
    zout.order = 1;
    zout.core_dims[0] = nx+1;
    zout.suffix_dims[0] = 0;
    (void) strcpy(zout.axnames[0],"SAMPLE");
    zout.core_dims[1] = ny+1;
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
    if (ret) goto zout_pixtype_error;
    (void) q_set_sys_keys(&zout.fileid,zout.naxes,zout.order,
             zout.core_dims,zout.suffix_dims,(CHAR *)zout.axnames,
	     sizeof(zout.axnames[0]),zout.item_bytes,
	     zout.core_type,zout.scale,&ret);
    if (ret) goto zout_char_error;
    (void) q_open(&zout.fileid,fid,zoutfile,3,0," ",0,0,1,0,&ret);
    if (ret) goto zout_open_error;
    for (i=1; i<=ny+1; i++) {
      limits[0][0] = 1;
      limits[0][1] = nx+1;
      limits[1][0] = i;
      limits[1][1] = i;
      limits[2][0] = 1;
      limits[2][1] = 1;
      (void) u_move4(nx+1,&cdum[(i-1)*(nx+1)+n2],ibuf);
      for (j=0; j<nx+1; j++) {
        ibuf[j] = ibuf[j] * scale * cosemi;
      }
      (void) q_lio_cbrick(zout.fileid,2,limits,(void *)ibuf,&ret);
      if (ret) goto zout_wrt_error;
    }
    (void) q_close(zout.fileid,1,&ret);
    if (ret) goto zout_cls_error;
  }

  min = max = 0;
  if (PreLoadCube (1,pcpdatafile[1],min,max,True,False)) err = True;
  else err = False;
  if (err == False) XtSetSensitive(xinfo.topShell,True);
  strcpy(sptr,"");
  XmTextSetString (stattext,sptr);
  XmUpdateDisplay (xinfo.topShell);

/************************************************************************
  Initialize the arrays that keep track of type of data in each display
************************************************************************/
  LeftDsp[0] = True;
  for (i=1; i<5; i++) {
    LeftDsp[i] = False;
  }
  RightDsp[0] = False;
  RightDsp[1] = True;
  for (i=2; i<5; i++) {
    RightDsp[i] = False;
  }

/************************************************************************
  Generate DZ (topography difference) file name and fill file with
  zeroes
************************************************************************/
  strcpy(pcpdatafile[2],"PC2D_DZ_FILE_");
  strcat(pcpdatafile[2],timdat);
  strcat(pcpdatafile[2],".cub");
  (void) u_fill4(0.0,(nx+1)*(ny+1),&cdum[n4]);
  pcpdata[2] = pcpdata[1];
  pcpdata[2].fileid = 0;
  (void) u_type_keys(pcpdata[2].pixel_type,&pcpdata[2].item_bytes,
                     pcpdata[2].core_type,sizeof(pcpdata[2].core_type),&ret);
  if (ret) goto dz_pixtype_error;
  (void) q_set_sys_keys(&pcpdata[2].fileid,pcpdata[2].naxes,pcpdata[2].order,
           pcpdata[2].core_dims,pcpdata[2].suffix_dims,(CHAR *)pcpdata[2].axnames,
	   sizeof(pcpdata[2].axnames[0]),pcpdata[2].item_bytes,
	   pcpdata[2].core_type,pcpdata[2].scale,&ret);
  if (ret) goto dz_char_error;
  (void) q_open(&pcpdata[2].fileid,fid,pcpdatafile[2],3,0," ",0,0,1,0,&ret);
  if (ret) goto dz_open_error;
  limits[0][0] = 1;
  limits[0][1] = nx+1;
  limits[1][0] = 1;
  limits[1][1] = ny+1;
  limits[2][0] = 1;
  limits[2][1] = 1;
  (void) q_lio_cbrick(pcpdata[2].fileid,2,limits,(void *)(&cdum[n4]),&ret);
  if (ret) goto dz_wrt_error;
  (void) q_close(pcpdata[2].fileid,1,&ret);
  if (ret) goto dz_cls_error;

/************************************************************************
  Generate file names for other types of data
************************************************************************/
  strcpy(pcpdatafile[3],"PC2D_SYN_FILE_");
  strcat(pcpdatafile[3],timdat);
  strcat(pcpdatafile[3],".cub");
  sprintf(sptr,"cp %s %s",pcpdatafile[2],pcpdatafile[3]);
  system(sptr);
  strcpy(pcpdatafile[4],"PC2D_DIF_FILE_");
  strcat(pcpdatafile[4],timdat);
  strcat(pcpdatafile[4],".cub");
  sprintf(sptr,"cp %s %s",pcpdatafile[2],pcpdatafile[4]);
  system(sptr);
  nxlmag = nx;
  nylmag = ny;
  n1lmag = n1;
  n2lmag = n2;
  changeres = False;
  (void) u_free(ibuf);
  return(0);

/*****************************************************
  Error section
*****************************************************/
  mem_alloc_error:
    sprintf(C_MSG->pcpmsg,"Unable to allocate enough memory for processing");
    u_error("FIRSTGUESS-MALLOC",C_MSG->pcpmsg,-1,1);
    return(-1);

  read_error:
    sprintf(C_MSG->pcpmsg,"Unable to read from input image file");
    u_error("FIRSTGUESS-READERR",C_MSG->pcpmsg,-2,1);
    return(-2);

  convert_error:
    sprintf(C_MSG->pcpmsg,"Error converting input image to 32-bit");
    u_error("FIRSTGUESS-CVTERR",C_MSG->pcpmsg,-3,1);
    return(-3);

  xcsto_error:
    sprintf(C_MSG->pcpmsg,"Error creating initial guess from stored Z");
    u_error("FIRSTGUESS-XCSTO",C_MSG->pcpmsg,-4,1);
    return(-4);

  chk4inv_error:
    sprintf(C_MSG->pcpmsg,"SSIPSF-PI cannot be used on an image with invalid pixels");
    u_error("FIRSTGUESS-CHK4INV",C_MSG->pcpmsg,-5,1);
    return(-5);

  img_open_error:
    sprintf(C_MSG->pcpmsg,"Error opening temporary image file: %s",
            pcpdatafile[0]);
    u_error("FIRSTGUESS-IMGOPEN",C_MSG->pcpmsg,-6,1);
    return(-6);

  img_pixtype_error:
    sprintf(C_MSG->pcpmsg,
      "Error getting pixel type keywords for temporary image file: %s",
      pcpdatafile[0]);
    u_error("FIRSTGUESS-IMGPIXKEY",C_MSG->pcpmsg,-7,1);
    return(-7);

  img_char_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting file characteristics for temporary image file: %s",
      pcpdatafile[0]);
    u_error("FIRSTGUESS-IMGSETCHR",C_MSG->pcpmsg,-8,1);
    return(-8);

  img_setcore_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting CORE keywords in temporary image file: %s",
      pcpdatafile[0]);
    u_error("FIRSTGUESS-IMGSETCORE",C_MSG->pcpmsg,-9,1);
    return(-9);

  img_setband_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting BAND_BIN keywords in temporary image file: %s",
      pcpdatafile[0]);
    u_error("FIRSTGUESS-IMGSETBAND",C_MSG->pcpmsg,-10,1);
    return(-10);

  img_wrt_error:
    sprintf(C_MSG->pcpmsg,"Error writing to temporary image file: %s",
            pcpdatafile[0]);
    u_error("FIRSTGUESS-IMGWRT",C_MSG->pcpmsg,-11,1);
    return(-11);

  img_cls_error:
    sprintf(C_MSG->pcpmsg,"Error closing temporary image file: %s",
            pcpdatafile[0]);
    u_error("FIRSTGUESS-IMGCLS",C_MSG->pcpmsg,-12,1);
    return(-12);

  topo_char_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting file characteristics for temporary zout file: %s",
      pcpdatafile[1]);
    u_error("FIRSTGUESS-TOPOSETCHR",C_MSG->pcpmsg,-13,1);
    return(-13);

  topo_open_error:
    sprintf(C_MSG->pcpmsg,"Error opening temporary zout file: %s",
            pcpdatafile[1]);
    u_error("FIRSTGUESS-TOPOOPEN",C_MSG->pcpmsg,-14,1);
    return(-14);

  topo_pixtype_error:
    sprintf(C_MSG->pcpmsg,
      "Error getting pixel type keywords for temporary zout file: %s",
      pcpdatafile[1]);
    u_error("FIRSTGUESS-TOPOPIXKEY",C_MSG->pcpmsg,-15,1);
    return(-15);

  topo_setcore_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting CORE keywords in temporary zout file: %s",
      pcpdatafile[1]);
    u_error("FIRSTGUESS-TOPOSETCORE",C_MSG->pcpmsg,-16,1);
    return(-16);

  topo_setband_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting BAND_BIN keywords in temporary zout file: %s",
      pcpdatafile[1]);
    u_error("FIRSTGUESS-TOPOSETBAND",C_MSG->pcpmsg,-17,1);
    return(-17);

  topo_wrt_error:
    sprintf(C_MSG->pcpmsg,"Error writing to temporary zout file: %s",
            pcpdatafile[1]);
    u_error("FIRSTGUESS-TOPOWRT",C_MSG->pcpmsg,-18,1);
    return(-18);

  topo_cls_error:
    sprintf(C_MSG->pcpmsg,"Error closing temporary zout file: %s",
            pcpdatafile[1]);
    u_error("FIRSTGUESS-TOPOCLS",C_MSG->pcpmsg,-19,1);
    return(-19);

  not_enough_memory:
    sprintf(C_MSG->pcpmsg,
      "Error allocating enough memory for SOR or DIRECT iteration.");
    u_error("FIRSTGUESS-NOMEM",C_MSG->pcpmsg,-20,1);
    return(-20);

  more_memory_needed:
    sprintf(C_MSG->pcpmsg,
      "The amount of memory specified (MAXMEM) is too small for this image.");
    u_error("FIRSTGUESS-MAXMEM",C_MSG->pcpmsg,-21,1);
    return(-21);
    
  zout_exists:
    sprintf(C_MSG->pcpmsg,"The zout file already exists: %s",zoutfile);
    u_error("FIRSTGUESS-ZOUTEXST",C_MSG->pcpmsg,-22,1);
    return(-22);

  dz_pixtype_error:
    sprintf(C_MSG->pcpmsg,
      "Error getting pixel type keywords for temporary dz file: %s",
      pcpdatafile[2]);
    u_error("FIRSTGUESS-DZPIXKEY",C_MSG->pcpmsg,-23,1);
    return(-23);

  dz_char_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting file characteristics for temporary dz file: %s",
      pcpdatafile[2]);
    u_error("FIRSTGUESS-DZSETCHR",C_MSG->pcpmsg,-24,1);
    return(-24);

  dz_open_error:
    sprintf(C_MSG->pcpmsg,"Error opening temporary dz file: %s",
            pcpdatafile[2]);
    u_error("FIRSTGUESS-DZOPEN",C_MSG->pcpmsg,-25,1);
    return(-25);

  dz_setcore_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting CORE keywords in temporary dz file: %s",
      pcpdatafile[2]);
    u_error("FIRSTGUESS-DZSETCORE",C_MSG->pcpmsg,-26,1);
    return(-26);

  dz_setband_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting BAND_BIN keywords in temporary dz file: %s",
      pcpdatafile[2]);
    u_error("FIRSTGUESS-DZSETBAND",C_MSG->pcpmsg,-27,1);
    return(-27);

  dz_wrt_error:
    sprintf(C_MSG->pcpmsg,"Error writing to temporary dz file: %s",
            pcpdatafile[2]);
    u_error("FIRSTGUESS-DZWRT",C_MSG->pcpmsg,-28,1);
    return(-28);

  dz_cls_error:
    sprintf(C_MSG->pcpmsg,"Error closing temporary dz file: %s",
            pcpdatafile[2]);
    u_error("FIRSTGUESS-DZCLS",C_MSG->pcpmsg,-29,1);
    return(-29);

  zinterp_error:
    sprintf(C_MSG->pcpmsg,"Error creating initial guess from stored Z");
    u_error("FIRSTGUESS-ZINTERP",C_MSG->pcpmsg,-30,1);
    return(-30);

  zinopn_error:
    sprintf(C_MSG->pcpmsg,"Error opening stored Z file %s",zin);
    u_error("FIRSTGUESS-ZOPEN",C_MSG->pcpmsg,-31,1);
    return(-31);

  zininv_error:
    sprintf(C_MSG->pcpmsg,"%s is not a standard cube file",zin);
    u_error("FIRSTGUESS-ZINV",C_MSG->pcpmsg,-32,1);
    return(-32);

  zin_bad_sys_keys:
    sprintf(C_MSG->pcpmsg,"Error obtaining system keywords from %s",zin);
    u_error("FIRSTGUESS-ZSYSKEY",C_MSG->pcpmsg,-33,1);
    return(-33);

  zinsize_error:
    sprintf(C_MSG->pcpmsg,"Stored Z file is an invalid size");
    u_error("FIRSTGUESS-ZINVSIZ",C_MSG->pcpmsg,-34,1);
    return(-34);

  zpixtyp_error:
    sprintf(C_MSG->pcpmsg,"Stored Z file is not 32 bit");
    u_error("FIRSTGUESS-ZPIXTYP",C_MSG->pcpmsg,-35,1);
    return(-35);

  zread_error:
    sprintf(C_MSG->pcpmsg,"Unable to read from stored Z file");
    u_error("FIRSTGUESS-ZREADERR",C_MSG->pcpmsg,-36,1);
    return(-36);

  zcls_error:
    sprintf(C_MSG->pcpmsg,"Error closing stored Z file");
    u_error("FIRSTGUESS-ZCLS",C_MSG->pcpmsg,-37,1);
    return(-37);

  z_chk4inv_error:
    sprintf(C_MSG->pcpmsg,"Stored Z file contains invalid pixels");
    u_error("FIRSTGUESS-ZCHK4INV",C_MSG->pcpmsg,-38,1);
    return(-38);

  zout_pixtype_error:
    sprintf(C_MSG->pcpmsg,
      "Error getting pixel type keywords for zout file: %s",
      zoutfile);
    u_error("FIRSTGUESS-ZOUTPIXKEY",C_MSG->pcpmsg,-39,1);
    return(-39);

  zout_char_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting file characteristics for zout file: %s",
      zoutfile);
    u_error("FIRSTGUESS-ZOUTSETCHR",C_MSG->pcpmsg,-40,1);
    return(-40);

  zout_open_error:
    sprintf(C_MSG->pcpmsg,"Error opening zout file: %s",
            zoutfile);
    u_error("FIRSTGUESS-ZOUTOPEN",C_MSG->pcpmsg,-41,1);
    return(-41);

  zout_wrt_error:
    sprintf(C_MSG->pcpmsg,"Error writing to zout file: %s",
            zoutfile);
    u_error("FIRSTGUESS-ZOUTWRT",C_MSG->pcpmsg,-42,1);
    return(-42);

  zout_cls_error:
    sprintf(C_MSG->pcpmsg,"Error closing zout file: %s",
            zoutfile);
    u_error("FIRSTGUESS-ZOUTCLS",C_MSG->pcpmsg,-43,1);
    return(-43);

  in_chk4inv_error:
    sprintf(C_MSG->pcpmsg,"Input (FROM) file contains invalid pixels");
    u_error("FIRSTGUESS-INCHK4INV",C_MSG->pcpmsg,-44,1);
    return(-44);
}
