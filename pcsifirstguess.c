#include <unistd.h>
#include "pcsi.h"
#include "clinom_aepar.h"
#include "clinom_dnorm.h"
#include "clinom_mem.h"
#include "clinom_ops.h"
#include "clinom_msg.h"
#include "lev.h"

INT4 PcsiFirstGuess (void)

/***********************************************************************
*
*_Title	PcsiFirstGuess - Generate initial guess at solution of topography
*
*_DESC	First approximation of solution
*
*_HIST	Mar 19 2003 Janet Barrett, Original version, created by
*                   modifying the FirstGuess subroutine used by pc2d
*_END
************************************************************************/

{
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
  INT4 nz,n6,n7,n8,n9,n8d,n9d;
  INT4 nrot;   /* Amount of memory needed for rotation + SSIPSF-PI */
  INT4 ioct1;
  INT4 fidz,zaxes,zorder,zcore_size[3],zsuf_size[3],zimage_bytes;

  FLOAT4 tr[6],tri[6];
  FLOAT4 rot45;
  FLOAT4 tmp;
  FLOAT4 cod,sid;
  FLOAT4 border;
  FLOAT4 xmin,xmax,ymin,ymax;
  FLOAT4 xrmin,xrmax,yrmin,yrmax;
  FLOAT4 zimage_scale[2];

/******************************************************************************
* Arrange to flop image to most favorable orientation for SOR (code
* was acquired from pcsi)
******************************************************************************/
  nx = ins;
  ny = inl;
  ioct1 = 1;

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
  nrot = 2*nz+3*rot45*rot45+6*rot45+1;
  n1 = 0;
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
  cdum = malloc(sizeof (INT4) * (C_MEM3->nmax+1));
  if (cdum == NULL) {
    goto not_enough_memory;
  }
  if (C_MEM3->nmax >= n9) solntype = 2;
  if (C_MEM3->nmax >= n9d) solntype = 3;
  if (strncmp(method,"CGM",3) == 0) {
    if (solntype < 2) goto not_enough_memory;
  }
  if (strncmp(method,"DIR",3) == 0) {
    if (solntype < 3) goto not_enough_memory;
  }

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
    (void) pblinein1(ibuf,ins,&cdum[n1],nx,ny,i,C_DNORM->dnatm,ioct1);
  }

/******************************************************************************
* Now scale the image
******************************************************************************/
  (void) pbscale(&cdum[n1],&cdum[n2],nx,ny,dndatum);

/******************************************************************************
* Finally, generate the initial guess at the topography
******************************************************************************/
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
    (void) pcsi_xcsep(&cdum[n1],C_MEM3->nmax,n1+1,n2+1,n3+1,n4+1,n5+1,nx,ny,1);
  }

/******************************************************************************
* Write out the initial guess at topography
******************************************************************************/
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

/************************************************************************
  Return to calling routine
************************************************************************/
  return(0);

/*****************************************************
  Error section
*****************************************************/
  mem_alloc_error:
    sprintf(C_MSG->pcpmsg,"Unable to allocate enough memory for processing");
    u_error("PCSIFIRSTGUESS-MALLOC",C_MSG->pcpmsg,-1,1);
    return(-1);

  read_error:
    sprintf(C_MSG->pcpmsg,"Unable to read from input image file");
    u_error("PCSIFIRSTGUESS-READERR",C_MSG->pcpmsg,-2,1);
    return(-2);

  convert_error:
    sprintf(C_MSG->pcpmsg,"Error converting input image to 32-bit");
    u_error("PCSIFIRSTGUESS-CVTERR",C_MSG->pcpmsg,-3,1);
    return(-3);

  chk4inv_error:
    sprintf(C_MSG->pcpmsg,"SSIPSF-PI cannot be used on an image with invalid pixels");
    u_error("PCSIFIRSTGUESS-CHK4INV",C_MSG->pcpmsg,-4,1);
    return(-4);

  not_enough_memory:
    sprintf(C_MSG->pcpmsg,
      "Error allocating enough memory for SOR, ICCG, or DIRECT iteration.");
    u_error("PCSIFIRSTGUESS-NOMEM",C_MSG->pcpmsg,-5,1);
    return(-5);

  more_memory_needed:
    sprintf(C_MSG->pcpmsg,
      "The amount of memory specified (MAXMEM) is too small for this image.");
    u_error("PCSIFIRSTGUESS-MAXMEM",C_MSG->pcpmsg,-6,1);
    return(-6);
    
  zout_exists:
    sprintf(C_MSG->pcpmsg,"The zout file already exists: %s",zoutfile);
    u_error("PCSIFIRSTGUESS-ZOUTEXST",C_MSG->pcpmsg,-7,1);
    return(-7);

  zinterp_error:
    sprintf(C_MSG->pcpmsg,"Error creating initial guess from stored Z");
    u_error("PCSIFIRSTGUESS-ZINTERP",C_MSG->pcpmsg,-8,1);
    return(-8);

  zinopn_error:
    sprintf(C_MSG->pcpmsg,"Error opening stored Z file %s",zin);
    u_error("PCSIFIRSTGUESS-ZOPEN",C_MSG->pcpmsg,-9,1);
    return(-9);

  zininv_error:
    sprintf(C_MSG->pcpmsg,"%s is not a standard cube file",zin);
    u_error("PCSIFIRSTGUESS-ZINV",C_MSG->pcpmsg,-10,1);
    return(-10);

  zin_bad_sys_keys:
    sprintf(C_MSG->pcpmsg,"Error obtaining system keywords from %s",zin);
    u_error("PCSIFIRSTGUESS-ZSYSKEY",C_MSG->pcpmsg,-11,1);
    return(-11);

  zinsize_error:
    sprintf(C_MSG->pcpmsg,"Stored Z file is an invalid size");
    u_error("PCSIFIRSTGUESS-ZINVSIZ",C_MSG->pcpmsg,-12,1);
    return(-12);

  zpixtyp_error:
    sprintf(C_MSG->pcpmsg,"Stored Z file is not 32 bit");
    u_error("PCSIFIRSTGUESS-ZPIXTYP",C_MSG->pcpmsg,-13,1);
    return(-13);

  zread_error:
    sprintf(C_MSG->pcpmsg,"Unable to read from stored Z file");
    u_error("PCSIFIRSTGUESS-ZREADERR",C_MSG->pcpmsg,-14,1);
    return(-14);

  zcls_error:
    sprintf(C_MSG->pcpmsg,"Error closing stored Z file");
    u_error("PCSIFIRSTGUESS-ZCLS",C_MSG->pcpmsg,-15,1);
    return(-15);

  z_chk4inv_error:
    sprintf(C_MSG->pcpmsg,"Stored Z file contains invalid pixels");
    u_error("PCSIFIRSTGUESS-ZCHK4INV",C_MSG->pcpmsg,-16,1);
    return(-16);

  zout_pixtype_error:
    sprintf(C_MSG->pcpmsg,
      "Error getting pixel type keywords for zout file: %s",
      zoutfile);
    u_error("PCSIFIRSTGUESS-ZOUTPIXKEY",C_MSG->pcpmsg,-17,1);
    return(-17);

  zout_char_error:
    sprintf(C_MSG->pcpmsg,
      "Error setting file characteristics for zout file: %s",
      zoutfile);
    u_error("PCSIFIRSTGUESS-ZOUTSETCHR",C_MSG->pcpmsg,-18,1);
    return(-18);

  zout_open_error:
    sprintf(C_MSG->pcpmsg,"Error opening zout file: %s",
            zoutfile);
    u_error("PCSIFIRSTGUESS-ZOUTOPEN",C_MSG->pcpmsg,-19,1);
    return(-19);

  zout_wrt_error:
    sprintf(C_MSG->pcpmsg,"Error writing to zout file: %s",
            zoutfile);
    u_error("PCSIFIRSTGUESS-ZOUTWRT",C_MSG->pcpmsg,-20,1);
    return(-20);

  zout_cls_error:
    sprintf(C_MSG->pcpmsg,"Error closing zout file: %s",
            zoutfile);
    u_error("PCSIFIRSTGUESS-ZOUTCLS",C_MSG->pcpmsg,-21,1);
    return(-21);
}
