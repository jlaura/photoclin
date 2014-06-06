#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "isismain.h"
#include "isistypes.h"
#include "isismain_c.h"
#include "u.h"
#include "chardip.h"
#include "spi.h"
#include "lev.h"
#include "clinom_datum.h"
#include "clinom_pgeom.h"
#include "clinom_aepar.h"
#include "clinom_psnpar.h"
#include "clinom_ppars.h"

#define NUMOFSCAN  3
#define NUMOFFRAME  10
#define SQ2 1.414213562

#define CHARDIP main

int CHARDIP (int argc, char *argv[])

/***********************************************************************
*
*_Title	CHARDIP - Create a unit tilt toward sun file
*
*_DESC	Creates the "unit tilt toward sun" file needed by the
*       pc_fit_inverse Perl script.
*
*_HIST	Apr 9 2003 Janet Barrett, Used the pc2d subroutine initpcp 
*       to write this program.
*
*_END
************************************************************************/

{

  BOOLEAN levels,frame;
  INT4 level,spi_level;
  INT4 len,count,ret,length,icount;
  INT4 naxes,order,core_size[3],suf_size[3];
  INT4 issi,isli;
  INT4 iesi,ieli;
  INT4 iflag,jflag;
  INT4 lpos;
  INT4 icard;
  INT4 ccont[3];
  INT4 scont[3];
  INT4 dflag;
  INT4 icountx,icounty;
  INT4 levgroups;
  INT4 found;
  INT4 i,j;
  int (*p)();

  FLOAT4 dzxm,dzym;
  FLOAT4 z01,z02,z03,z04;
  FLOAT4 xsinci,xlinci;
  FLOAT4 dzdip;
  FLOAT4 arg;
  FLOAT4 si0;
  FLOAT4 se0;
  FLOAT4 bclipin;
  FLOAT4 db1,db2;
  FLOAT4 cardaz;
  FLOAT4 delaza,delazb;
  FLOAT4 xcenter,ycenter;
  FLOAT4 clat,clon;

  FLOAT8 azi;
  FLOAT8 lat,lon;
  FLOAT8 inc,emi,pha;
  FLOAT8 scaz,sunaz;
  FLOAT8 ssclat,ssclon;
  FLOAT8 ssunlat,ssunlon;
  FLOAT8 radii[3];
  FLOAT8 slantrange;

  CHAR pcpmsg[512];
  CHAR axis_name[3][MAXLITLEN+1];
  CHAR item_type[MAXLITLEN+1];
  CHAR prmname[30];
  CHAR tmpstr[256];
  CHAR planet[11];
  CHAR sfrom[136];
  CHAR scan_sc[NUMOFSCAN][LEV_SPACECRAFT_NAME_SIZE] = 
       {"MARS_GLOBAL_SURVEYOR",
        "MARS_GLOBAL_SURVEYOR",
	"MARS_ODYSSEY"};
  CHAR scan_inst[NUMOFSCAN][LEV_INSTRUMENT_NAME_SIZE] =
       {"MOC_NA_A",
        "MOC_WA_A",
	"THEMIS_IR"};
  CHAR frame_sc[NUMOFFRAME][LEV_SPACECRAFT_NAME_SIZE] =
       {"GALILEO_1",
        "VIKING_ORBITER_1",
	"VIKING_ORBITER_1",
	"VIKING_ORBITER_2",
	"VIKING_ORBITER_2",
	"MARS_ODYSSEY",
	"VOYAGER_1",
	"VOYAGER_1",
	"VOYAGER_2",
	"VOYAGER_2"};
/*	"CASSINI",
	"LUNAR_ORBITER_4"};*/
  CHAR frame_inst[NUMOFFRAME][LEV_INSTRUMENT_NAME_SIZE] =
       {"SSI",
        "VISUAL_IMAGING_SUBSYSTEM_CAMERA_A",
	"VISUAL_IMAGING_SUBSYSTEM_CAMERA_B",
        "VISUAL_IMAGING_SUBSYSTEM_CAMERA_A",
	"VISUAL_IMAGING_SUBSYSTEM_CAMERA_B",
	"THEMIS_VIS",
	"WA",
	"NA",
	"WA",
	"NA"};
/*	"VIMS",
	"24_INCH_FOCAL_LENGTH_CAMERA"};*/
  FLOAT8 frame_flinpix[NUMOFFRAME] =
       {98455.811,
        40323.83,
	40328.08,
	40341.85,
	40298.585,
	22222.222,
        16989.138,
	127248.26,
	17209.598,
	127528.17};

  LevData data,tmpdata;
  Lev both;
  FLOAT8 sampres,lineres,azres;

  CHAR obj_name[5]="QUBE";
  CHAR grp_name[1] = "";
  CHAR grp2_name[21]="IMAGE_MAP_PROJECTION";
  INT4 whcnt,hg1cnt,hg2cnt,hhcnt,b0cnt,thetacnt,bhcnt,chcnt,kcnt,lcnt;
  FLOAT4 dz1,dz2,be,bdif,dzx,dzy,z0,be0;
  INT4 limits[3][2];

/******************************************************************************
* Initialize program
******************************************************************************/
  u_std_init("CHARDIP","2003-04-09",
             "Create a unit tilt toward sun file");
  C_PPARS1->nps = 1;

/******************************************************************************
* Get input file parameter from TAE
******************************************************************************/
  strcpy(prmname,"FROM");
  (void) u_get_file_parm("FROM",1,(CHAR *)from,sizeof(from),&len,&count,&ret);
  if (ret) goto user_parameter_error;

/******************************************************************************
* Get subcube specifier
******************************************************************************/
  strcpy(prmname,"SFROM");
  u_get_str_parm("SFROM",1,(CHAR *)sfrom,sizeof(sfrom),&len,&icount,&ret);
  if (ret) goto user_parameter_error;
  if (icount <= 0) (void) strcpy(sfrom," ");

/******************************************************************************
* Open the FROM file and get information
******************************************************************************/
  q_open(&fid,0,from,READ_ONLY,0,"",0,0,1,0,&ret);
  if (ret) goto cube_open_error;
  q_check_std(fid,&ret);
  if (ret) goto invalid_cube;
  q_get_sys_keys(fid,1,&naxes,&order,core_size,suf_size,
		 (char *)axis_name,sizeof(axis_name[0]),&image_bytes,
		 item_type,sizeof(item_type),image_scale,&ret);
  if (ret) goto bad_sys_keys;

/******************************************************************************
* Parse the SFROM subcube specification and apply to the FROM file and
* then read system keywords again to reflect the virtual cube
******************************************************************************/
  (void) u_parse_isub(fid,sfrom,1,core_size,suf_size,ccont,scont,&ret);
  if (ret) goto parse_sfrom_error;
  (void) q_set_virtual(fid,&ret);
  if (ret) goto sfrom_error;
  q_get_sys_keys(fid,1,&naxes,&order,core_size,suf_size,
		 (char *)axis_name,sizeof(axis_name[0]),&image_bytes,
		 item_type,sizeof(item_type),image_scale,&ret);
  if (ret) goto bad_sys_keys;
  ins = core_size[0];
  inl = core_size[1];

/******************************************************************************
* Set the variables describing the FROM subcube
******************************************************************************/
  u_get_isub_desc(fid,"SAMPLE",&issi,&iesi,&xsinci,&dflag,&ret);
  if (ret) goto bad_subsamp;
  if (dflag != 1) goto subsamp_error;
  u_get_isub_desc(fid,"LINE",&isli,&ieli,&xlinci,&dflag,&ret);
  if (ret) goto bad_subline;
  if (dflag != 1) goto subline_error;
  if (xsinci != xlinci) goto inc_error; /* Need to be equal or
                                map scale will not be accurate */

/******************************************************************************
* Open the TO file and set it to same size as FROM virtual cube
******************************************************************************/
  strcpy(prmname,"TO");
  u_get_file_parm("TO",1,(CHAR *)tiltfile,sizeof(tiltfile),&length,&icount,&ret);
  if (ret) goto user_parameter_error;
  tilt.fileid = 0;
  tilt.naxes = 3;
  tilt.order = 1;
  tilt.core_dims[0] = ins;
  tilt.suffix_dims[0] = 0;
  (void) strcpy(tilt.axnames[0],"SAMPLE");
  tilt.core_dims[1] = inl;
  tilt.suffix_dims[1] = 0;
  (void) strcpy(tilt.axnames[1],"LINE");
  tilt.core_dims[2] = 1;
  tilt.suffix_dims[2] = 0;
  (void) strcpy(tilt.axnames[2],"BAND");
  tilt.pixel_type = 3;
  tilt.scale[0] = 0.0;
  tilt.scale[1] = 1.0;
  (void) u_type_keys(tilt.pixel_type,&tilt.item_bytes,
    tilt.core_type,sizeof(tilt.core_type),&ret);
  if (ret) goto pixtype_error;
  (void) q_set_sys_keys(&tilt.fileid,tilt.naxes,tilt.order,
    tilt.core_dims,tilt.suffix_dims,(CHAR *)tilt.axnames,
    sizeof(tilt.axnames[0]),tilt.item_bytes,
    tilt.core_type,tilt.scale,&ret);
  if (ret) goto setsys_error;
  (void) q_open(&tilt.fileid,fid,tiltfile,3,0," ",0,0,1,0,&ret);
  if (ret) goto tilt_open_error; 

/******************************************************************************
* Set center of virtual subcube
******************************************************************************/
  xcenter = 0.5*(ins+1);
  ycenter = 0.5*(inl+1);

/******************************************************************************
* Get photoclinometry parameters from TAE
******************************************************************************/
  strcpy(prmname,"DATUMTYP");
  u_get_int_parm("DATUMTYP",1,&C_DATUM2->datumtyp,&icount,&ret);
  if (ret) goto user_parameter_error;

  nucenter = FALSE; /* Reset to True if X,Y inputs given */

  strcpy(prmname,"X");
  u_get_dbl_parm("X",1,1,6,&x,&icountx,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"Y");
  u_get_dbl_parm("Y",1,1,6,&y,&icounty,&ret);
  if (ret) goto user_parameter_error;
  if ((icountx >= 1) && (icounty >= 1)) nucenter = TRUE;

  strcpy(prmname,"DISTORTD");
  u_get_str_parm("DISTORTD",1,(CHAR *)tmpstr,sizeof(tmpstr),&len,&icount,&ret);
  if (ret) goto user_parameter_error;

  distortd = TRUE;
  if (strncmp(tmpstr,"NO",1) == 0) 
    distortd = FALSE;

  strcpy(prmname,"PHOFUNC");
  u_get_str_parm("PHOFUNC",1,(CHAR *)C_PPARS4->phofunc,sizeof(C_PPARS4->phofunc),
    &length,&icount,&ret);
  if (ret) goto user_parameter_error;

  C_PPARS1->piopt = 0;
  if (strncmp(C_PPARS4->phofunc,"HAPLEG",6) == 0)
    C_PPARS1->piopt = 1;
  if (strncmp(C_PPARS4->phofunc,"HAPL_S",6) == 0)
    C_PPARS1->piopt = 1;

  C_PPARS3->hapke = FALSE;
  if (strncmp(C_PPARS4->phofunc,"H",1) == 0)
    C_PPARS3->hapke = TRUE;

  strcpy(prmname,"WH");
  u_get_real_parm("WH",1,1,6,&C_PPARS5->pwh,&whcnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"HG1");
  u_get_real_parm("HG1",1,1,6,&C_PPARS5->phg1,&hg1cnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"HG2");
  u_get_real_parm("HG2",1,1,6,&C_PPARS5->phg2,&hg2cnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"HH");
  u_get_real_parm("HH",1,1,6,&C_PPARS5->phh,&hhcnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"B0");
  u_get_real_parm("B0",1,1,6,&C_PPARS5->pb0,&b0cnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"THETA");
  u_get_real_parm("THETA",1,1,6,&C_PPARS5->ptheta,&thetacnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"BH");
  u_get_real_parm("BH",1,1,6,&C_PPARS5->pbh,&bhcnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"CH");
  u_get_real_parm("CH",1,1,6,&C_PPARS5->pch,&chcnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"K");
  u_get_real_parm("K",1,1,6,&C_PPARS2->pex,&kcnt,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"L");
  u_get_real_parm("L",1,1,6,&C_PPARS2->pl2,&lcnt,&ret);
  if (ret) goto user_parameter_error;

  if (strncmp(C_PPARS4->phofunc,"HAPHEN",6) == 0) {
    if (whcnt < 1 || thetacnt < 1 || b0cnt < 1 || hhcnt < 1 ||
        hg1cnt < 1 || hg2cnt < 1) goto haphen_error;
  } else if (strncmp(C_PPARS4->phofunc,"HAPLEG",6) == 0) {
    if (whcnt < 1 || thetacnt < 1 || b0cnt < 1 || hhcnt < 1 ||
        bhcnt < 1 || chcnt < 1) goto hapleg_error;
  } else if (strncmp(C_PPARS4->phofunc,"HAPH_S",6) == 0) {
    if (whcnt < 1 || hg1cnt < 1 || hg2cnt < 1) goto haph_s_error;
  } else if (strncmp(C_PPARS4->phofunc,"HAPL_S",6) == 0) {
    if (whcnt < 1 || bhcnt < 1 || chcnt < 1) goto hapl_s_error;
  } else if (strncmp(C_PPARS4->phofunc,"LAMBER",6) == 0) {
    C_PPARS2->pex = 1.0;
    C_PPARS2->pl2 = 0.0;
  } else if (strncmp(C_PPARS4->phofunc,"LOMSEL",6) == 0) {
    C_PPARS2->pex = 1.0;
    C_PPARS2->pl2 = 1.0;
  } else if (strncmp(C_PPARS4->phofunc,"MIN",3) == 0) {
    C_PPARS2->pl2 = 0.0;
    if (kcnt < 1) goto min_error;
  } else if (strncmp(C_PPARS4->phofunc,"LUNLAM",6) == 0) {
    C_PPARS2->pex = 1.0;
    if (lcnt < 1) goto lunlam_error;
  }

  (void) u_end_parm();

/******************************************************************************
* Get label information from input file
******************************************************************************/
/******************************************************************************
* Evaluate default (center) location in image if needed. This default is in
* ALL cases now the center of the virtual subcube, transferred to full cube.
******************************************************************************/
  if (!nucenter) {
    x = (FLOAT8) (issi) + (xcenter-1.0) * xsinci;
    y = (FLOAT8) (isli) + (ycenter-1.0) * xlinci;
  }

/******************************************************************************
* Check for the 3 levels Level 1 keyword groups (ISIS_INSTRUMENT, 
* ISIS_GEOMETRY, ISIS_TARGET)
******************************************************************************/
  levgroups = 0;
  (void) p_check_key(fid,"QUBE","ISIS_INSTRUMENT","",&icount,&ret);
  if (!ret) levgroups = levgroups + 1;
  (void) p_check_key(fid,"QUBE","ISIS_GEOMETRY","",&icount,&ret);
  if (!ret) levgroups = levgroups + 1;
  (void) p_check_key(fid,"QUBE","ISIS_TARGET","",&icount,&ret);
  if (!ret) levgroups = levgroups + 1;

/******************************************************************************
* All 3 levels Level 1 keyword groups are missing. This is either a 
* pre-levels Level 1 file or a Level 2 file that has had the groups erased
* from the labels (or, under the old system, never got them). If the file
* contains either ISIS_MOSAIC or IMAGE_MAP_PROJECTION keyword groups, then
* it is not a Level 1 image and cannot be used. Otherwise, try using old
* SPICE routines.
******************************************************************************/
  if (levgroups == 0) {
    (void) p_check_key(fid,"QUBE","ISIS_MOSAIC","",&icount,&ret);
    if (!ret) {
    /* Level 2 mosaic without required groups */
      goto mosaic_group_error;
    } else {
      (void) p_check_key(fid,"QUBE","IMAGE_MAP_PROJECTION","",&icount,&ret);
      if (!ret) {
      /* Level 2 image without required groups */
	goto level2_group_error;
      }
    }

    /* Pre-levels Level 1 image */
    iflag = 0;
    jflag = 1;
    if (!distortd) jflag = 0;
    spi_phosun(fid,obj_name,sizeof(obj_name),grp_name,sizeof(grp_name),
      grp2_name,sizeof(grp2_name),jflag,0,&iflag,
      &y,&x,&lat,&lon,&emi,&inc,&pha,&ssclat,&ssclon,
      &scaz,&ssunlat,&ssunlon,&sunaz,&aznor,&res,&ret);
    if (ret) goto spi_phosun_error;

/******************************************************************************
* If we got to here without an error, this is a valid pre-levels Level 1
* file. Make sure the user did not specify two input files.
******************************************************************************/
    levels = FALSE;
    level = 1;

/******************************************************************************
* All pre-levels instruments are framing cameras.
******************************************************************************/
    frame = TRUE;

/******************************************************************************
* Now assign emi, inc, pha, scaz, sunaz to variables used below as
* appropriate for the framing camera case:  Z axis is viewing direction, so
* emission angle is used to calculate datum derivatives rather than emission
* vector.
******************************************************************************/
    res = res * xsinci; /* xsinci=xlinci guaranteed */
    C_AEPAR1->aspect = 1.0;
    C_PSNPAR->phase[0] = pha;
    clinc = pha;
    azinc = sunaz;
    clemi = 0.0;
    azemi = 0.0;
    dip = emi;
    az = scaz + 180.0;
    cosemi = cos(emi*L_DEG2RAD);

/******************************************************************************
* Obtain parameters needed for spherical datum
******************************************************************************/
    if (C_DATUM2->datumtyp == 2) {
      spi_lbplan(fid,"QUBE","","IMAGE_MAP_PROJECTION",planet,
	sizeof(planet),radii,&lpos,&ret);
      if (ret) goto spi_lbplan_error;
      if (radii[1] == 0.0) radii[1] = radii[0];
      if (radii[2] == 0.0) radii[2] = radii[0];
      radius = sqrt((radii[0]*radii[0]+radii[1]*radii[1]+
		  radii[2]*radii[2])/3.0);
      C_DATUM1->r0 = radius/res;  /* Planet radius in pixels */

/******************************************************************************
* Obtain planet center in full frame and convert to subframe. xs0 and yl0 are
* with respect to the pixels of the subcube; xs0f and yl0f are computed by
* geomset at each grid resolution and refer to coordinates equal to indices
* in the DEM.
******************************************************************************/
      spi_findlin(fid,(FLOAT4) ssclat,(FLOAT4) ssclon,
        &C_DATUM1->xs0,&C_DATUM1->yl0,&ret);
      if (ret) goto spi_findlin_error;
      C_DATUM1->xs0=(C_DATUM1->xs0 - (FLOAT4) issi)/xsinci
	+ 1.0;
      C_DATUM1->yl0=(C_DATUM1->yl0 - (FLOAT4) isli)/xlinci
	+ 1.0;

/******************************************************************************
* Must calculate z00s for sphere, putting middle of image at height 0
******************************************************************************/
      C_DATUM1->z00s = -sqrt(C_DATUM1->r0*C_DATUM1->r0 - 
	pow((xcenter-C_DATUM1->xs0)*C_AEPAR1->aspect,2.0) -
	pow((ycenter-C_DATUM1->yl0),2.0));
    }

/******************************************************************************
* All 3 levels Level 1 keyword groups were found. This is a levels file,
* either Level 1 or Level 2 with the Level 1 groups included.
******************************************************************************/
  } else if (levgroups == 3) {
    levels = TRUE;
    spi_level = lev_find_level(fid);
    if (spi_level == 2) {
      if (lev2_init_from_labels(fid,&both.map)) goto lev_init_error;
      if (lev1_init(fid,&both.spi,0)) goto lev_init_error;
    } else {
      if (lev1_init(fid,&both.spi,0)) goto lev_init_error;
    }
/*    if (lev_init(fid,&both)) goto lev_init_error;*/

/******************************************************************************
* If input file is level 2 (map projected), then get the center latitude
* and center longitude.
******************************************************************************/
    if (both.level > 1 && strcmp(both.map.proj.name,"ORTHOGRAPHIC") == 0) {
      (void) p_check_key(fid,"QUBE","IMAGE_MAP_PROJECTION","CENTER_LATITUDE",
        &icount,&ret);
      if (ret) goto clat_error;
      icount = 1;
      p_get_real_key(fid,"QUBE","IMAGE_MAP_PROJECTION","CENTER_LATITUDE",
        2,&icount,&clat,&ret);
      if (ret) goto clat_error;
      (void) p_check_key(fid,"QUBE","IMAGE_MAP_PROJECTION","CENTER_LONGITUDE",
        &icount,&ret);
      if (ret) goto clon_error;
      icount = 1;
      p_get_real_key(fid,"QUBE","IMAGE_MAP_PROJECTION","CENTER_LONGITUDE",
        2,&icount,&clon,&ret);
      if (ret) goto clon_error;
    }

/******************************************************************************
* This is a levels Level 1 image. Make sure that only one input file was
* specified.
******************************************************************************/
    if (both.level == 1) {
      level = 1;
      data.samp = x;
      data.line = y;
      if (lev1_linesamp_to_latlon(&both.spi,&data)) goto ls_to_latlon;

/******************************************************************************
* Return emission, incidence, phase as emi, inc, pha respectively. These
* angles are evaluated at location x,y relative to the full image cube.
******************************************************************************/
      lev1_calc_emission_angle(&data,&emi);
      lev1_calc_incidence_angle(&data,&inc);
      lev1_calc_phase_angle(&data,&pha);

/******************************************************************************
* Check spacecraft/instrument to determine if it's a scanner or a
* framing camera .
******************************************************************************/
      found = -1;
      for (icount=0; icount<NUMOFSCAN; icount++) {
        if (strcmp(both.spi.inst.spacecraft,scan_sc[icount]) == 0
	    && strcmp(both.spi.inst.name,scan_inst[icount]) == 0) {
          frame = FALSE;
	  found = icount;
        }
      }
      if (found == -1) {
	for (icount=0; icount<NUMOFFRAME; icount++) {
	  if (strcmp(both.spi.inst.spacecraft,frame_sc[icount]) == 0
	      && strcmp(both.spi.inst.name,frame_inst[icount]) == 0) {
            frame = TRUE;
	    found = icount;
          }
	}
      }
      /* Invalid instrument */
      if (found == -1) goto unknown_inst;
	
/******************************************************************************
* This is a scanner. Spherical datum model does not make sense.
******************************************************************************/
      if (frame == FALSE) {

/******************************************************************************
* Return sample resolution, line resolution, spacecraft azimuth, sun azimuth 
* as sampres, lineres, scaz, sunaz respectively. All of these values are
* evaluated at location x,y relative to the full image cube.
******************************************************************************/
        if (lev1_calc_resolution(&both.spi,&data,&sampres,&lineres)) 
          goto res_error;
        if (sampres > lineres) azres = sampres;
        else azres = lineres;
        lev1_calc_spacecraft_azimuth(&both.spi,&data,azres,&scaz);
        lev1_calc_sun_azimuth(&both.spi,&data,azres,&sunaz);

/******************************************************************************
* Now assign emi, inc, pha, scaz, sunaz to variables used below as
* appropriate for the scanner case:  Z axis is local vertical, so
* emission angle is used to calculate emission vector rather than datum
* derivatives.
******************************************************************************/
        res = lineres*xlinci;
	C_AEPAR1->aspect = sampres/lineres;
	C_PSNPAR->phase[0] = pha;
	clinc = inc;
	azinc = sunaz;
	clemi = emi;
	azemi = scaz;
	dip = 0.0;
	az = 0.0;
	C_DATUM2->datumtyp = 1;
        cosemi = 1.0;

/******************************************************************************
* This is a framing camera. 
******************************************************************************/
      } else {
        lev1_calc_slant_distance(&data,&slantrange);

/******************************************************************************
* Return resolution, spacecraft azimuth, sun azimuth as res,
* scaz, sunaz respectively. All of these values are evaluated at
* location x,y relative to the full image cube.
******************************************************************************/
        res = (slantrange/frame_flinpix[found])*xlinci;
        azres = res;
        lev1_calc_spacecraft_azimuth(&both.spi,&data,azres,&scaz);
        lev1_calc_sun_azimuth(&both.spi,&data,azres,&sunaz);

/******************************************************************************
* Now assign emi, inc, pha, scaz, sunaz to variables used below as
* appropriate for the framing camera case:  Z axis is viewing direction, so
* emission angle is used to calculate datum derivatives rather than 
* emission vector.
******************************************************************************/
        C_AEPAR1->aspect = 1.0;
	C_PSNPAR->phase[0] = pha;
	clinc = pha;
	azinc = sunaz;
	clemi = 0.0;
	azemi = 0.0;
	dip = emi;
	az = scaz + 180.0;
        cosemi = cos(emi*L_DEG2RAD);

/******************************************************************************
* Obtain parameters needed for spherical datum
******************************************************************************/
	if (C_DATUM2->datumtyp == 2) {
          radii[0] = both.spi.target.radii[0];
	  radii[1] = both.spi.target.radii[1];
	  radii[2] = both.spi.target.radii[2];
          if (radii[1] == 0.0) radii[1] = radii[0];
          if (radii[2] == 0.0) radii[2] = radii[0];
          radius = sqrt((radii[0]*radii[0]+radii[1]*radii[1]+
		   radii[2]*radii[2])/3.0);
          C_DATUM1->r0 = radius/res;  /* Planet radius in pixels */
	  lev1_calc_subspace(&both.spi,&data,&tmpdata);
	  ssclat = tmpdata.lat;
	  ssclon = tmpdata.lon;
	  data.lat = ssclat;
	  data.lon = ssclon;
	  if (lev1_latlon_to_linesamp(&both.spi,&data)) goto latlon_to_ls;
          C_DATUM1->xs0 = data.samp;
          C_DATUM1->yl0 = data.line;

/******************************************************************************
* We have planet center in terms of full image cube, so convert to 
* subcube.
******************************************************************************/
          C_DATUM1->xs0=(C_DATUM1->xs0 - (FLOAT4) issi)/xsinci
	    + 1.0;
          C_DATUM1->yl0=(C_DATUM1->yl0 - (FLOAT4) isli)/xlinci
	    + 1.0;

/******************************************************************************
* Must calculate z00s for sphere, putting middle of image at height 0
******************************************************************************/
          C_DATUM1->z00s = -sqrt(C_DATUM1->r0*C_DATUM1->r0 - 
	    pow((xcenter-C_DATUM1->xs0)*C_AEPAR1->aspect,2.0) -
	    pow((ycenter-C_DATUM1->yl0),2.0));
        }
      }

/******************************************************************************
* This is a levels Level 2 image. Having two Level 2 files is permitted
* in software. The two image subcubes are the same size and they have
* the same projection parameters. It doesn't matter if the scalar datum
* variables are set twice from labels for both images; the results are
* guaranteed to be the same.
******************************************************************************/
    } else {
      level = 2;

/******************************************************************************
* Next, check the projection. Orthographic projection allows a set of
* images to be treated like one image. The coverage could be highly
* localized (setting projection center to subspacecraft point minimizes
* parallax) or hemispheric in scale. Spherical datum type is always used
* in this projection, with center at center of projection. Coordinate
* system has Z axis out of planet at center of projection. This point is
* known by lat and lon. First find its Level 1 coordinates so we can find
* ema, inc, phase here. NOTE that azimuths are returned but they are with
* respect to Level 1 space so they don't get used.
******************************************************************************/
      if (strcmp(both.map.proj.name,"ORTHOGRAPHIC") == 0) {
	data.lat = (FLOAT8) clat;
	data.lon = (FLOAT8) clon;
	if (lev1_latlon_to_linesamp(&both.spi,&data)) 
	  goto latlon_to_ls;
        lev1_calc_emission_angle(&data,&emi);
        lev1_calc_incidence_angle(&data,&inc);
        lev1_calc_phase_angle(&data,&pha);

/******************************************************************************
* Now find the Level 2 coordinates of the center so we can find the
* azimuths. This routine gives line and sample of a given lat,lon in
* terms of the full image. Will later calculate position relative to
* subcube.
******************************************************************************/
        if (lev2_latlon_to_linesamp(&both.map,&data))
	  goto latlon_to_ls;
        C_DATUM1->xs0 = data.samp;
        C_DATUM1->yl0 = data.line;

/******************************************************************************
* Find azimuth to sun and spacecraft in Level 2.
******************************************************************************/
        /* Convert mapscale to units of resolution */
        res = both.map.proj.kmperpix * xlinci;
        azres = res;
	tmpdata = data;
        lev2_calc_spacecraft_azimuth(&both.spi,&data,
	  &both.map,&tmpdata,azres,&scaz);
	tmpdata = data;
        lev2_calc_sun_azimuth(&both.spi,&data,
	  &both.map,&tmpdata,azres,&sunaz);

/******************************************************************************
* Now assign emi, inc, pha, scaz, sunaz to variables used below as
* appropriate for the map projected case:  Z axis is the local vertical,
* so emission angle is used to calculate emission vector rather than datum
* derivatives.
******************************************************************************/
        C_AEPAR1->aspect = 1.0;
	C_PSNPAR->phase[0] = pha;
	clinc = inc;
	azinc = sunaz;
	clemi = emi;
	azemi = scaz;
	C_DATUM2->datumtyp = 2;
	cosemi = 1.0;

/******************************************************************************
* Obtain parameters needed for spherical datum
******************************************************************************/
        radii[0] = both.spi.target.radii[0];
	radii[1] = both.spi.target.radii[1];
	radii[2] = both.spi.target.radii[2];
        if (radii[1] == 0.0) radii[1] = radii[0];
        if (radii[2] == 0.0) radii[2] = radii[0];
        radius = sqrt((radii[0]*radii[0]+radii[1]*radii[1]+
	  radii[2]*radii[2])/3.0);
        C_DATUM1->r0 = radius/res;  /* Planet radius in pixels */

/******************************************************************************
* Call above gave planet center in full image cube, so convert to subcube
******************************************************************************/
        C_DATUM1->xs0=(C_DATUM1->xs0 - (FLOAT4) issi)/xsinci
	  + 1.0;
        C_DATUM1->yl0=(C_DATUM1->yl0 - (FLOAT4) isli)/xlinci
	  + 1.0;

/******************************************************************************
* Must calculate z00s for sphere, putting middle of image at height 0
******************************************************************************/
        C_DATUM1->z00s = -sqrt(C_DATUM1->r0*C_DATUM1->r0 - 
	  pow((xcenter-C_DATUM1->xs0)*C_AEPAR1->aspect,2.0) -
	  pow((ycenter-C_DATUM1->yl0),2.0));

/******************************************************************************
* Finally, calculate dip and azimuth of dip for sphere at reference
* point. Do this by calling datum to get the elevation differences across
* the pixel near the reference point (converted from full image cube to 
* subcube coordinates.
******************************************************************************/
        (void) geomset(1.0); /* Setup needed before datum */
	i = (INT4) ((x - (FLOAT4) issi)/
	           xsinci + .5) + 1;
	j = (INT4) ((y - (FLOAT4) isli)/
	           xlinci + .5) + 1;
        (void) datum(i,j,4,&z01,&z02,&z03,&z04);
	dzxm = 0.5*(-z01+z02+z03-z04);
	dzym = 0.5*(-z01-z02+z03+z04);
        dip = atan(sqrt((dzxm/C_AEPAR1->aspect)*(dzxm/C_AEPAR1->aspect)+
	      dzym*dzym))*L_RAD2DEG;
	az = atan2(dzym*C_AEPAR1->aspect*C_AEPAR1->aspect,dzxm)*L_RAD2DEG;

/******************************************************************************
* Non-orthographic projections can be used to rectify scanner images.
* Intent is to represent a small region only, so datum type is planar and
* in fact has zero dip, similar to handling of Level 1 scanner images. No
* error checking is done to see if the projection has severe skew or other
* distortions that would make the calculation nonsensical.
*
* User input or default reference point location is in Level 2 space, so
* convert to Level 1 space using lat,lon coordinates.
******************************************************************************/
      } else {
        data.samp = x;
        data.line = y;
        if (lev2_linesamp_to_latlon(&both.map,&data)) goto ls_to_latlon;
        if (lev2_latlon_to_linesamp(&both.map,&data)) goto latlon_to_ls;
	tmpdata = data;
	if (lev1_latlon_to_linesamp(&both.spi,&data)) goto latlon_to_ls;
        else if (lev1_linesamp_to_latlon(&both.spi,&data)) goto ls_to_latlon;

/******************************************************************************
* Return emission, incidence, phase as emi, inc, pha respectively. These
* angles are evaluated at a location relative to the whole image.
******************************************************************************/
        lev1_calc_emission_angle(&data,&emi);
        lev1_calc_incidence_angle(&data,&inc);
        lev1_calc_phase_angle(&data,&pha);

/******************************************************************************
* Get ground resolutions of map at the reference point.
******************************************************************************/
        if (lev2_calc_map_resolution(&both.spi,&data,
	  &both.map,&tmpdata,&sampres,&lineres)) 
	  goto res_error;

/******************************************************************************
* Now, find azimuth to sun and spacecraft in Level 2. 
******************************************************************************/
        res = lineres*xlinci;
        azres = res;
        lev2_calc_spacecraft_azimuth(&both.spi,&data,
	  &both.map,&tmpdata,azres,&scaz);
        lev2_calc_sun_azimuth(&both.spi,&data,
	  &both.map,&tmpdata,azres,&sunaz);

/******************************************************************************
* Now assign emi, inc, pha, scaz, sunaz to variables used below as
* appropriate for the map projected case:  Z axis is the local vertical,
* so emission angle is used to calculate emission vector rather than datum
* derivatives.
******************************************************************************/
        C_AEPAR1->aspect = sampres/lineres;
	C_PSNPAR->phase[0] = pha;
	clinc = inc;
	azinc = sunaz;
	clemi = emi;
	azemi = scaz;
	dip = 0.0;
	az = 0.0;
	C_DATUM2->datumtyp = 1;
	cosemi = 1.0;
      }
    }

/******************************************************************************
* Only some of the needed levels Level 1 keyword groups found. Image is
* invalid.
******************************************************************************/
  } else {
    goto labels_not_complete;
  }

/******************************************************************************
* Now proceed to calculate stored values with label information. Still
* need to calculate derivatives of the plane datum.
******************************************************************************/
  dzdip = tan(dip*L_DEG2RAD)/sqrt(pow(cos(az*L_DEG2RAD)*C_AEPAR1->aspect,2.0)
    +pow(sin(az*L_DEG2RAD),2.0));
  C_DATUM1->dzx0 = -cos(az*L_DEG2RAD)*dzdip*C_AEPAR1->aspect*C_AEPAR1->aspect;
  C_DATUM1->dzy0 = -sin(az*L_DEG2RAD)*dzdip;
  C_DATUM1->z00 = -(C_DATUM1->dzx0*xcenter+C_DATUM1->dzy0*ycenter);
  C_DATUM1->dz10 = -(C_DATUM1->dzx0+C_DATUM1->dzy0);
  C_DATUM1->dz20 = -(C_DATUM1->dzx0-C_DATUM1->dzy0);
  (void) geomset(1.0);
  scale = 1000.0*res;

  C_PGEOM->ci[0] = cos(clinc*L_DEG2RAD)*SQ2;
  si0 = sin(clinc*L_DEG2RAD)/
    sqrt(pow(cos(azinc*L_DEG2RAD)*C_AEPAR1->aspect,2.0)
    +pow(sin(azinc*L_DEG2RAD),2.0));
  C_PGEOM->si1[0] = si0*cos((45.0-azinc)*L_DEG2RAD);
  C_PGEOM->si2[0] = si0*sin((45.0-azinc)*L_DEG2RAD);
  C_PGEOM->ce[0] = cos(clemi*L_DEG2RAD)*SQ2;
  se0 = sin(clemi*L_DEG2RAD)/
    sqrt(pow(cos(azemi*L_DEG2RAD)*C_AEPAR1->aspect,2.0)
    +pow(sin(azemi*L_DEG2RAD),2.0));
  C_PGEOM->se1[0] = se0*cos((45.0-azemi)*L_DEG2RAD);
  C_PGEOM->se2[0] = se0*sin((45.0-azemi)*L_DEG2RAD);

/******************************************************************************
* Calculations above are purely GEOMETRIC; those below are PHOTOMETRIC,
* i.e., depending on surface properties and description thereof.
******************************************************************************/
  if (C_PPARS3->hapke) {
    if (strncmp(C_PPARS4->phofunc,"HAPH_S",6) == 0) {
      C_PPARS2->falpha[0] = (1.0-C_PPARS5->phg2)*
	(1.0-C_PPARS5->phg1*C_PPARS5->phg1)/
        (pow(1.0+C_PPARS5->phg1*C_PPARS5->phg1+2.0*
	C_PPARS5->phg1*cos(C_PSNPAR->phase[0]*L_DEG2RAD),1.5))+
        C_PPARS5->phg2*(1.0-C_PPARS5->phg1*C_PPARS5->phg1)/
	(pow(1.0+C_PPARS5->phg1*C_PPARS5->phg1-2.0*C_PPARS5->phg1*
        cos(C_PSNPAR->phase[0]*L_DEG2RAD),1.5))-1.0;
    } else if (strncmp(C_PPARS4->phofunc,"HAPL_S",6) == 0) {
      C_PPARS2->falpha[0] = 1.0+C_PPARS5->pbh*cos(C_PSNPAR->phase[0]*
	L_DEG2RAD)+C_PPARS5->pch*(1.5*cos(C_PSNPAR->phase[0]*L_DEG2RAD)+
	0.5);
    }
    C_PPARS2->twogam = 2.0*sqrt(1.0-C_PPARS5->pwh);
  } else {
    C_PPARS2->pex1 = C_PPARS2->pex-1.0;
    C_PPARS2->pl2 = C_PPARS2->pl2+C_PPARS2->pl2;
    C_PPARS2->pl1 = 1.0-.5*C_PPARS2->pl2;
  }

/******************************************************************************
* End of material-dependent code. Find photometric function
* normalization, etc. (AEPARs)
******************************************************************************/
  bclipin = 1.0e30;
  C_AEPAR1->bmax = 1.0e30;
  i = (INT4) (xcenter+.5);
  j = (INT4) (ycenter+.5);
  (void) pbder(i,j,0.0,0.0,&C_AEPAR1->bnorm,&db1,&db2,1);
  (void) findbmax();
  if (C_AEPAR1->bmax < C_AEPAR1->bnorm*bclipin) C_AEPAR1->bclip = C_AEPAR1->bmax;
  else C_AEPAR1->bclip = C_AEPAR1->bnorm*bclipin;
  C_AEPAR2->flop = FALSE;
  C_AEPAR2->logimg = FALSE;

/******************************************************************************
* Find azimuth of characteristic strips. This is calculated in terms of
* true Cartesian azimuth, whereas the other azimuths are those on the
* pixel grid, which may not have a unit aspect ratio. We will output the
* azimuth of characteristics transformed to a grid azimuth. Starting point
* for the search is the (numerator) sun azimuth, transformed to Cartesian.
******************************************************************************/
  azi = atan2(sin(azinc*L_DEG2RAD)/C_AEPAR1->aspect,cos(azinc*L_DEG2RAD))*
    L_RAD2DEG;
  if (azi >= 0) icount = (INT4) (azi/90.0+.5);
  else icount = (INT4) (azi/90.0-.5);
  icard = icount-4*(icount/4);
  cardaz = 90.0*((FLOAT4) icard);
  delaza = cardaz-azi-45.0;
  delazb = cardaz-azi+45.0;
  p = ddberr;
  C_AEPAR1->delaz = zbrent(p,delaza,delazb,1.0e-6);
  if (ins > inl) arg = ins;
  else arg = inl;
  if (fabs(sin(C_AEPAR1->delaz*L_DEG2RAD)*cos(C_AEPAR1->delaz*L_DEG2RAD))*
      arg <= 0.5) C_AEPAR1->delaz = 0.0;
  while (icard < 0) {
    icard = icard+4;
    cardaz = cardaz+360.;
  }
  charaz = cardaz-C_AEPAR1->delaz;
  if (C_AEPAR1->dbrat*(0.5-(icard%2)) < 0.0) charaz = charaz+90.;
  charaz = charaz-360.0*((INT4) (charaz/360.0));
  if (charaz > 180.) charaz = charaz-360.;
  if (charaz >= 0.) arg = 180.;
  else arg = -180.;
  if (fabs(charaz) > 90.) charaz = charaz-arg;

/******************************************************************************
* We use Cartesian charaz but print grid-relative charazgrid for the
* user. No longer need to use ioct in main program to control geometric
* transformation of images on input. Instead, set ioct1=1 and use it in
* calls to pblinein1, etc. By setting the octant of the characteristics
* ioct here and using it to control the SOR sweep direction, we get
* optimal results for any charaz. ioct is passed via common.
******************************************************************************/
  charazgrid = atan2(sin(charaz*L_DEG2RAD)*C_AEPAR1->aspect,
               cos(charaz*L_DEG2RAD))*L_RAD2DEG;

  cdum = malloc(sizeof (FLOAT4) * (ins*inl+(ins+1)*(inl+1)));
  if (cdum == NULL) goto not_enough_memory;

  dz1 = -0.001*cos((charazgrid-45.0)*L_DEG2RAD); /* The 0.001 is to keep the
                                                    plane from being shadowed */
  dz2 = 0.001*sin((charazgrid-45.0)*L_DEG2RAD);  /* which would spoil the scaling
                                                    below */
  C_DATUM2->datumtyp = 1;
  (void) pbder(0,0,0.0,0.0,&be0,&db1,&db2,0);
  (void) pbder(0,0,dz1,dz2,&be,&db1,&db2,0);
  bdif = 0.01;        /* Make a plane this much brighter than the datum */
  dz1 = dz1*bdif*be0/(be-be0); /* This normalization takes care of the sign */
  dz2 = dz2*bdif*be0/(be-be0); /* (downhill direction) as well as magnitude of tilt */
  dzx = -0.5*(dz1+dz2);
  dzy = -0.5*(dz1-dz2);
  C_DATUM1->z00 = -0.5*(ins)*dzx-0.5*(inl)*dzy;
  for (j=0; j<=inl; j++) {
    z0 = C_DATUM1->z00+dzy*j;
    for (i=0; i<=ins; i++) {
      cdum[j*(ins+1)+i] = z0+dzx*i;
    }
  }

  (void) zlineout(&cdum[0],ins,inl,&cdum[(ins+1)*(inl+1)],
    ins,inl,scale,1,cosemi);
  limits[0][0] = 1;
  limits[0][1] = ins;
  limits[1][0] = 1;
  limits[1][1] = inl;
  limits[2][0] = 1;
  limits[2][1] = 1;
  (void) q_lio_cbrick(tilt.fileid,2,limits,
    (void *)(&cdum[(ins+1)*(inl+1)]),&ret);
  if (ret) goto wrt_tilt_error;
  (void) q_close(tilt.fileid,1,&ret);

  (void) q_close(fid,KEEP_FILE,&ret);

  return(0);

/*****************************************************
  Error section
*****************************************************/
  user_parameter_error:
    sprintf(pcpmsg,"Error retrieving TAE parameter %s",prmname);
    u_error("CHARDIP-TAEERR",pcpmsg,-1,1);
    return(-1);

  cube_open_error:
    sprintf(pcpmsg,"Error opening input cube file %s",from);
    u_error("CHARDIP-OPENERR",pcpmsg,-2,1);
    return(-2);

  invalid_cube:
    sprintf(pcpmsg,"%s is not a standard cube file",from);
    u_error("CHARDIP-INVCUBE",pcpmsg,-3,1);
    return(-3);

  bad_sys_keys:
    sprintf(pcpmsg,"Error obtaining system keywords from %s",from);
    u_error("CHARDIP-SYSKEYS",pcpmsg,-4,1);
    return(-4);

  spi_phosun_error:
    sprintf(pcpmsg,
      "Unable to get spacecraft and sun position information for %s",from);
    u_error("CHARDIP-SPIPHOSUN",pcpmsg,-5,1);
    return(-5);

  spi_findlin_error:
    sprintf(pcpmsg,"Unable to convert lat,lon to line,samp for %s",
            from);
    u_error("CHARDIP-SPIFINDLIN",pcpmsg,-6,1);
    return(-6);

  spi_lbplan_error:
    sprintf(pcpmsg,"Unable to read planet radii and lpos from %s",
            from);
    u_error("CHARDIP-SPILBP",pcpmsg,-7,1);
    return(-7);

  lev_init_error:
    sprintf(pcpmsg,"Unable to initialize input file %s",from);
    u_error("CHARDIP-LEVINIT",pcpmsg,-8,1);
    return(-8);

  ls_to_latlon:
    sprintf(pcpmsg,"Unable to convert line,sample to lat,lon for %s",
            from);
    u_error("CHARDIP-LSTOLATLON",pcpmsg,-9,1);
    return(-9);

  latlon_to_ls:
    sprintf(pcpmsg,"Unable to convert lat,lon to line,sample for %s",
            from);
    u_error("CHARDIP-LATLONTOLS",pcpmsg,-10,1);
    return(-10);

  res_error:
    sprintf(pcpmsg,
    "Unable to determine line,sample resolution for %s",from);
    u_error("CHARDIP-LSRES",pcpmsg,-11,1);
    return(-11);

  parse_sfrom_error:
    sprintf(pcpmsg,"Unable to parse SFROM parameter %s for %s",sfrom,from);
    u_error("CHARDIP-PRSSFROM",pcpmsg,-12,1);
    return(-12);

  sfrom_error:
    sprintf(pcpmsg,"Unable to apply SFROM %s to input file %s",sfrom,from);
    u_error("CHARDIP-SFROMERR",pcpmsg,-13,1);
    return(-13);

  bad_subsamp:
    sprintf(pcpmsg,"Error getting SFROM %s sample information for %s",
            sfrom,from);
    u_error("CHARDIP-SUBSAMP",pcpmsg,-14,1);
    return(-14);

  subsamp_error:
    sprintf(pcpmsg,"Subcube specifier SFROM %s is invalid",sfrom);
    u_error("CHARDIP-INVSAMP",pcpmsg,-15,1);
    return(-15);

  bad_subline:
    sprintf(pcpmsg,"Error getting SFROM %s line information for %s",
            sfrom,from);
    u_error("CHARDIP-SUBLINE",pcpmsg,-16,1);
    return(-16);

  subline_error:
    sprintf(pcpmsg,"Subcube specifier SFROM %s is invalid",sfrom);
    u_error("CHARDIP-INVLINE",pcpmsg,-17,1);
    return(-17);

  inc_error:
    sprintf(pcpmsg,"Line and sample increment of file %s are not equal",from);
    u_error("CHARDIP-LINCSINC",pcpmsg,-18,1);
    return(-18);

  mosaic_group_error:
    sprintf(pcpmsg,
    "Mosaicked images must have all of groups: ISIS_INSTRUMENT, ISIS_GEOMETRY, ISIS_TARGET");
    u_error("CHARDIP-MOSGRP",pcpmsg,-19,1);
    return(-19);

  level2_group_error:
    sprintf(pcpmsg,
    "Level 2 images must have all of groups: ISIS_INSTRUMENT, ISIS_GEOMETRY, ISIS_TARGET");
    u_error("CHARDIP-LEV2GRP",pcpmsg,-20,1);
    return(-20);

  unknown_inst:
    sprintf(pcpmsg,
    "Unknown spacecraft/instrument found in %s",from);
    u_error("CHARDIP-BILEV1",pcpmsg,-21,1);
    return(-21);

  labels_not_complete:
    sprintf(pcpmsg,
    "Necessary Level 1 keyword groups not found in %s",from);
    u_error("CHARDIP-INCLAB",pcpmsg,-22,1);
    return(-22);

  clat_error:
    sprintf(pcpmsg,"Unable to get clat from labels of %s",from);
    u_error("CHARDIP-CLAT",pcpmsg,-23,1);
    return(-23);
	       
  clon_error:
    sprintf(pcpmsg,"Unable to get clon from labels of %s",from);
    u_error("CHARDIP-CLON",pcpmsg,-24,1);
    return(-24);

  haphen_error:
    sprintf(pcpmsg,
    "The following values must be provided for function HAPHEN: wh,theta,b0,hh,hg1,hg2");
    u_error("CHARDIP-HAPHEN",pcpmsg,-25,1);
    return(-25);

  hapleg_error:
    sprintf(pcpmsg,
    "The following values must be provided for function HAPLEG: wh,theta,b0,hh,bh,ch");
    u_error("CHARDIP-HAPLEG",pcpmsg,-26,1);
    return(-26);

  haph_s_error:
    sprintf(pcpmsg,
    "The following values must be provided for function HAPH_S: wh,hg1,hg2");
    u_error("CHARDIP-HAPH_S",pcpmsg,-27,1);
    return(-27);

  hapl_s_error:
    sprintf(pcpmsg,
    "The following values must be provided for function HAPL_S: wh,bh,ch");
    u_error("CHARDIP-HAPL_S",pcpmsg,-28,1);
    return(-28);

  min_error:
    sprintf(pcpmsg,
    "The following values must be provided for function MIN: k");
    u_error("CHARDIP-MIN",pcpmsg,-29,1);
    return(-29);

  lunlam_error:
    sprintf(pcpmsg,
    "The following values must be provided for function LUNLAM: l");
    u_error("CHARDIP-LUNLAM",pcpmsg,-30,1);
    return(-30);

  pixtype_error:
    sprintf(pcpmsg,
    "Error getting pixel type keywords for output tilt file");
    u_error("CHARDIP-PIXTYP",pcpmsg,-31,1);
    return(-31);
  
  setsys_error:
    sprintf(pcpmsg,
    "Error setting file characteristics for output tilt file");
    u_error("CHARDIP-SETSYS",pcpmsg,-32,1);
    return(-32);

  tilt_open_error:
    sprintf(pcpmsg,"Error opening output tilt file");
    u_error("CHARDIP-OPEN",pcpmsg,-33,1);
    return(-33);

  not_enough_memory:
    sprintf(pcpmsg,"Error allocating memory for tilt data");
    u_error("CHARDIP-NOMEM",pcpmsg,-34,1);
    return(-34);

  wrt_tilt_error:
    sprintf(pcpmsg,"Error writing to output tilt file");
    u_error("CHARDIP-WRITE",pcpmsg,-35,1);
    return(-35);
}
