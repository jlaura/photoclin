#include <unistd.h>
#include "pcsi.h"
#include "clinom_ppars.h"
#include "clinom_dnorm.h"
#include "clinom_datum.h"
#include "clinom_pgeom.h"
#include "clinom_aepar.h"
#include "clinom_psnpar.h"
#include "clinom_mem.h"
#include "clinom_msg.h"
#include "spi.h"
#include "lev.h"

#define NUMOFSCAN  3
#define NUMOFFRAME  10
#define SQ2 1.414213562

INT4 InitPcsi (void)

/***********************************************************************
*
*_Title	InitPcsi - Initialize pcsi parameters 
*
*_DESC	Initialize pcsi parameters 
*
*_HIST	Mar 9 2003 Janet Barrett, Created by modifying the initpcp
*                  subroutine used by the pc2d program.
*       Jul 9 2003 JB - Fixed a problem with handling Level 1 labels
*                  in a Level 2 image.
*       Jan 11 2010 JB - The both.level element is not being set by
*                  calls to lev1_init or lev2_init. This was fixed by
*                  using the spi_level value in place of it.
*                  
*_END
************************************************************************/

{

  BOOLEAN levels,frame;
  INT4 level,spi_level;
  INT4 lzin,llog,lnote;
  INT4 length,icount,ret;
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
  FLOAT8 sampres,lineres,azres,sres;

  INT4 PcsiFirstGuess (void);
  CHAR obj_name[5]="QUBE";
  CHAR grp_name[1] = "";
  CHAR grp2_name[21]="IMAGE_MAP_PROJECTION";
  INT4 mulpht;
  INT4 whcnt,hg1cnt,hg2cnt,hhcnt,b0cnt,thetacnt,bhcnt,chcnt,kcnt,lcnt;

/******************************************************************************
* Get input and output file parameters from TAE
******************************************************************************/
  strcpy(prmname,"TO");
  u_get_file_parm("TO",1,(CHAR *)topofile,sizeof(topofile),&length,&icount,&ret);
  if (ret) goto user_parameter_error;
  if (icount <= 0 || strlen(topofile) == 0) goto to_file_error;
  (void) u_strtrim(topofile,topofile);
  if (strlen(topofile) == 0) goto to_file_error;
  if (u_file_exist(topofile)) goto to_file_exist;
  if (strncmp(&(topofile[strlen(topofile)-1]),"/",1) == 0) goto to_file_error;
  (void) u_ver_flspec(topofile,"cub",topofile,sizeof(topofile),&ret);
  if (ret) goto to_file_invalid;

  strcpy(prmname,"ZOUT");
  u_get_file_parm("ZOUT",1,(CHAR *)zoutfile,sizeof(zoutfile),&length,&icount,&ret);
  if (ret) goto user_parameter_error;
  if (icount <= 0 || strlen(zoutfile) == 0) {
    dozout = FALSE;
  } else {
    (void) u_strtrim(zoutfile,zoutfile);
    (void) u_ver_flspec(zoutfile,"cub",zoutfile,sizeof(zoutfile),&ret);
    if (ret) goto flspec_error;
    if (u_file_exist(zoutfile)) {
      if (remove(zoutfile)) goto zout_remove_error;
    }
    dozout = TRUE;
  }

  (void) xctae_files(zin,sizeof(zin),&lzin,&usezin,&usedatum,logfile,
	      sizeof(logfile),&llog,note,sizeof(note),&lnote);

/******************************************************************************
* Get subcube specifiers
******************************************************************************/
  strcpy(prmname,"SFROM");
  u_get_str_parm("SFROM",1,(CHAR *)sfrom,sizeof(sfrom),&length,&icount,&ret);
  if (ret) goto user_parameter_error;
  if (icount <= 0) (void) strcpy(sfrom," ");

/******************************************************************************
* Open the FROM file and get information
******************************************************************************/
  strcpy(prmname,"FROM");
  u_get_file_parm("FROM",1,(CHAR *)from,sizeof(from),&length,&icount,&ret);
  if (ret) goto user_parameter_error;
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
  if (dflag != 1) goto bad_subsamp;
  u_get_isub_desc(fid,"LINE",&isli,&ieli,&xlinci,&dflag,&ret);
  if (ret) goto bad_subline;
  if (dflag != 1) goto bad_subline;
  if (xsinci != xlinci) goto inc_error; /* Need to be equal or
                               map scale will not be accurate */

/******************************************************************************
* Set center of virtual subcube
******************************************************************************/
  xcenter = 0.5*(ins+1);
  ycenter = 0.5*(inl+1);

/******************************************************************************
* Get photoclinometry parameters from TAE
******************************************************************************/
  strcpy(prmname,"MAXMEM");
  u_get_int_parm("MAXMEM",1,&C_MEM3->nmax,&icount,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"DNATM");
  u_get_real_parm("DNATM",1,1,6,&C_DNORM->dnatm,&icount,&ret);
  if (ret) goto user_parameter_error;

  strcpy(prmname,"DNDATUM");
  u_get_real_parm("DNDATUM",1,1,6,&dndatum,&icount,&ret);
  if (ret) goto user_parameter_error;

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
  u_get_str_parm("DISTORTD",1,(CHAR *)tmpstr,sizeof(tmpstr),&length,&icount,&ret);
  if (ret) goto user_parameter_error;

  distortd = TRUE;
  if (strncmp(tmpstr,"NO",1) == 0) 
    distortd = FALSE;

  strcpy(prmname,"METHOD");
  u_get_str_parm("METHOD",1,(CHAR *)method,sizeof(method),&length,&icount,&ret);
  if (ret) goto user_parameter_error;
  if (strncmp(method,"SOR",3) == 0) {
    sordir = 0;
  } else if (strncmp(method,"DIR",3) == 0) {
    sordir = 1;
  } else {
    sordir = 2;
  }

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
        (void) p_check_key(fid,"QUBE","IMAGE_MAP_PROJECTION",
	  "MAP_PROJECTION_TYPE",&icount,&ret);
        if (!ret) {
        /* Level 2 image without required groups */
	  goto level2_group_error;
        }
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
* file. 
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
* vector. Also calculate the scaling variable, cosemi, to be used for 
* line-of-sight to vertical on output and vertical to line-of-sight on
* input.
******************************************************************************/
    res = res * xsinci; /* xsinci=xlinci guaranteed */
    sres = res;
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
/*      if (lev_init(fid,&both)) goto lev_init_error;*/

/******************************************************************************
* If input file is level 2 (map projected), then get the center latitude
* and center longitude.
******************************************************************************/
    if (spi_level > 1 && strcmp(both.map.proj.name,"ORTHOGRAPHIC") == 0) {
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
* This is a levels Level 1 image. 
******************************************************************************/
    if (spi_level == 1) {
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
* derivatives. Also calculate the scaling variable, cosemi, to be used 
* for line-of-sight to vertical on output and vertical to line-of-sight on
* input (cosemi=1.0 for scanners).
******************************************************************************/
        res = lineres*xlinci;
	sres = sampres*xsinci;
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
* Return resolution, spacecraft azimuth, sun azimuth as res[index],
* scaz, sunaz respectively. All of these values are evaluated at
* location x,y relative to the full image cube.
******************************************************************************/
        res = (slantrange/frame_flinpix[found])*xlinci;
        sres = res;
        azres = res;
        lev1_calc_spacecraft_azimuth(&both.spi,&data,azres,&scaz);
        lev1_calc_sun_azimuth(&both.spi,&data,azres,&sunaz);

/******************************************************************************
* Now assign emi, inc, pha, scaz, sunaz to variables used below as
* appropriate for the framing camera case:  Z axis is viewing direction, so
* emission angle is used to calculate datum derivatives rather than 
* emission vector. Also calculate the scaling variable, cosemi, to be used 
* for line-of-sight to vertical on output and vertical to line-of-sight on
* input.
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
	sres = res;
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
* derivatives. Also calculate the scaling variable, cosemi, to be used 
* for line-of-sight to vertical on output and vertical to line-of-sight on
* input (cosemi=1.0 for map-projected images).
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
	if (lev1_latlon_to_linesamp(&both.spi,&data)) 
	  goto latlon_to_ls;
        else if (lev1_linesamp_to_latlon(&both.spi,&data)) 
	  goto ls_to_latlon;

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
	sres = sampres*xsinci;
        azres = res;
        lev2_calc_spacecraft_azimuth(&both.spi,&data,
	  &both.map,&tmpdata,azres,&scaz);
        lev2_calc_sun_azimuth(&both.spi,&data,
	  &both.map,&tmpdata,azres,&sunaz);

/******************************************************************************
* Now assign emi, inc, pha, scaz, sunaz to variables used below as
* appropriate for the map projected case:  Z axis is the local vertical,
* so emission angle is used to calculate emission vector rather than datum
* derivatives. Also calculate the scaling variable, cosemi, to be used 
* for line-of-sight to vertical on output and vertical to line-of-sight on
* input (cosemi=1.0 for map-projected images).
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
    if (strncmp(C_PPARS4->phofunc,"HAPLEG",6) == 0 ||
        strncmp(C_PPARS4->phofunc,"HAPHEN",6) == 0) {
      mulpht = 62;
      if (strncmp(C_PPARS4->phofunc,"HAPHEN",6) == 0) 
	mulpht = mulpht + 23;
      if (C_PPARS5->phh != 0.0) mulpht = mulpht + 3;
      if (C_PPARS5->ptheta != 0.0) mulpht = mulpht + 127;
      C_PPARS1->mulps = (16+mulpht)*2;
      C_PPARS1->mulpsp = (42+mulpht*4)*2;
    } else {
      C_PPARS1->mulps = 15;
      C_PPARS1->mulpsp = 15;
    }
  } else {
    C_PPARS2->pex1 = C_PPARS2->pex-1.0;
    C_PPARS2->pl2 = C_PPARS2->pl2+C_PPARS2->pl2;
    C_PPARS2->pl1 = 1.0-.5*C_PPARS2->pl2;
    if (C_PPARS2->pl2 == 0.0) {
      if (C_PPARS2->pex == 1.0) {
        C_PPARS1->mulps = 0;
	C_PPARS1->mulpsp = 0;
      } else {
        C_PPARS1->mulps = 15;
        C_PPARS1->mulpsp = 4;
      }
    } else if (C_PPARS2->pl2 == 2.0) {
      C_PPARS1->mulps = 3;
      C_PPARS1->mulpsp = 4;
    } else {
      if (C_PPARS2->pex == 1.0) {
        C_PPARS1->mulps = 5;
	C_PPARS1->mulpsp = 7;
      } else {
        C_PPARS1->mulps = 21;
	C_PPARS1->mulpsp = 14;
      }
    }
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
  C_AEPAR1->charaz = cardaz-C_AEPAR1->delaz;
  if (C_AEPAR1->dbrat*(0.5-(icard%2)) < 0.0) C_AEPAR1->charaz = 
                                             C_AEPAR1->charaz+90.;
  C_AEPAR1->charaz = C_AEPAR1->charaz-360.0*((INT4) (C_AEPAR1->charaz/360.0));
  if (C_AEPAR1->charaz > 180.) C_AEPAR1->charaz = C_AEPAR1->charaz-360.;
  if (C_AEPAR1->charaz >= 0.) arg = 180.;
  else arg = -180.;
  if (fabs(C_AEPAR1->charaz) > 90.) C_AEPAR1->charaz = C_AEPAR1->charaz-arg;

/******************************************************************************
* We use Cartesian charaz but print grid-relative charazgrid for the
* user. No longer need to use ioct in main program to control geometric
* transformation of images on input. Instead, set ioct1=1 and use it in
* calls to pblinein1, etc. By setting the octant of the characteristics
* ioct here and using it to control the SOR sweep direction, we get
* optimal results for any charaz. ioct is passed via common.
******************************************************************************/
  charazgrid = atan2(sin(C_AEPAR1->charaz*L_DEG2RAD)*C_AEPAR1->aspect,
               cos(C_AEPAR1->charaz*L_DEG2RAD))*L_RAD2DEG;
  if ((-90.0 <= C_AEPAR1->charaz) && (C_AEPAR1->charaz < -45.0)) 
    C_AEPAR3->ioct = -2;
  else if ((-45.0 <= C_AEPAR1->charaz) && (C_AEPAR1->charaz < 0.0)) 
    C_AEPAR3->ioct = -1;
  else if ((45.0 < C_AEPAR1->charaz) && (C_AEPAR1->charaz <= 90.0))
    C_AEPAR3->ioct = 2;
  else
    C_AEPAR3->ioct = 1;

  sprintf(C_MSG->pcpmsg,"X= %f",x);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Y= %f",y);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"RADIUS= %f",radius);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Dip of datum= %f",dip);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Azimuth of dip= %f",az);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Azimuth of characteristics= %f",charazgrid);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Resolution (km/pix) = %f",sres);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Aspect = %f",1.0/C_AEPAR1->aspect);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Emission angle = %f",emi);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Incidence angle = %f",inc);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Phase angle = %f",pha);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Spacecraft azimuth = %f",scaz);
  u_write_msg(3,C_MSG->pcpmsg);
  sprintf(C_MSG->pcpmsg,"Sun azimuth = %f",sunaz);
  u_write_msg(3,C_MSG->pcpmsg);

/******************************************************************************
* Finish getting TAE parameters
******************************************************************************/
  (void) pcsi_ipars(&nsmooth);

  (void) write2log(1,ins,inl,note,lnote,from,clinc,
                   azinc,clemi,azemi,sres,
		   dip,az,charazgrid,radius,dndatum,
		   1.0/C_AEPAR1->aspect);

  if (PcsiFirstGuess()) goto first_guess_error;

  (void) write2log(2,ins,inl,note,lnote,from,clinc,
                   azinc,clemi,azemi,sres,
		   dip,az,charazgrid,radius,dndatum,
		   1.0/C_AEPAR1->aspect);

  return(0);

/*****************************************************
  Error section
*****************************************************/
  user_parameter_error:
    sprintf(C_MSG->pcpmsg,"Error retrieving TAE parameter %s",prmname);
    u_error("INITPCSI-TAEERR",C_MSG->pcpmsg,-1,1);
    return(-1);

  spi_phosun_error:
    sprintf(C_MSG->pcpmsg,
      "Unable to get spacecraft and sun position information for %s",from);
    u_error("INITPCSI-SPIPHOSUN",C_MSG->pcpmsg,-2,1);
    return(-2);

  spi_findlin_error:
    sprintf(C_MSG->pcpmsg,"Unable to convert lat,lon to line,samp for %s",
            from);
    u_error("INITPCSI-SPIFINDLIN",C_MSG->pcpmsg,-3,1);
    return(-3);

  spi_lbplan_error:
    sprintf(C_MSG->pcpmsg,"Unable to read planet radii and lpos from %s",
            from);
    u_error("INITPCSI-SPILBP",C_MSG->pcpmsg,-4,1);
    return(-4);

  first_guess_error:
    sprintf(C_MSG->pcpmsg,"Unable to generate an initial solution for topography");
    u_error("INITPCSI-SOLNERR",C_MSG->pcpmsg,-5,1);
    return(-5);
  
  lev_init_error:
    sprintf(C_MSG->pcpmsg,"Unable to initialize input file %s",from);
    u_error("INITPCSI-LEVINIT",C_MSG->pcpmsg,-6,1);
    return(-6);

  ls_to_latlon:
    sprintf(C_MSG->pcpmsg,"Unable to convert line,sample to lat,lon for %s",
            from);
    u_error("INITPCSI-LSTOLATLON",C_MSG->pcpmsg,-7,1);
    return(-7);

  latlon_to_ls:
    sprintf(C_MSG->pcpmsg,"Unable to convert lat,lon to line,sample for %s",
            from);
    u_error("INITPCSI-LATLONTOLS",C_MSG->pcpmsg,-8,1);
    return(-8);

  res_error:
    sprintf(C_MSG->pcpmsg,
    "Unable to determine line,sample resolution for %s",from);
    u_error("INITPCSI-LSRES",C_MSG->pcpmsg,-9,1);
    return(-9);

  bad_subsamp:
    sprintf(C_MSG->pcpmsg,"Error getting sub-sample information for %s",
            from);
    u_error("INITPCSI-SUBSAMP",C_MSG->pcpmsg,-10,1);
    return(-10);

  bad_subline:
    sprintf(C_MSG->pcpmsg,"Error getting sub-line information for %s",
            from);
    u_error("INITPCSI-SUBLINE",C_MSG->pcpmsg,-11,1);
    return(-11);

  inc_error:
    sprintf(C_MSG->pcpmsg,"Line and sample increment of file %s are not equal",
            from);
    u_error("INITPCSI-LINCSINC",C_MSG->pcpmsg,-12,1);
    return(-12);

  mosaic_group_error:
    sprintf(C_MSG->pcpmsg,
    "Mosaicked images must have all of groups: ISIS_INSTRUMENT, ISIS_GEOMETRY, ISIS_TARGET");
    u_error("INITPCSI-MOSGRP",C_MSG->pcpmsg,-13,1);
    return(-13);

  level2_group_error:
    sprintf(C_MSG->pcpmsg,
    "Level 2 images must have all of groups: ISIS_INSTRUMENT, ISIS_GEOMETRY, ISIS_TARGET");
    u_error("INITPCSI-LEV2GRP",C_MSG->pcpmsg,-14,1);
    return(-14);

  unknown_inst:
    sprintf(C_MSG->pcpmsg,
    "Unknown spacecraft/instrument found in %s",from);
    u_error("INITPCSI-BILEV1",C_MSG->pcpmsg,-15,1);
    return(-15);

  labels_not_complete:
    sprintf(C_MSG->pcpmsg,
    "Necessary Level 1 keyword groups not found in %s",from);
    u_error("INITPCSI-INCLAB",C_MSG->pcpmsg,-16,1);
    return(-16);

  haphen_error:
    sprintf(C_MSG->pcpmsg,
    "The following values must be provided for function HAPHEN: wh,theta,b0,hh,hg1,hg2");
    u_error("INITPCSI-HAPHEN",C_MSG->pcpmsg,-17,1);
    return(-17);

  hapleg_error:
    sprintf(C_MSG->pcpmsg,
    "The following values must be provided for function HAPLEG: wh,theta,b0,hh,bh,ch");
    u_error("INITPCSI-HAPLEG",C_MSG->pcpmsg,-18,1);
    return(-18);

  haph_s_error:
    sprintf(C_MSG->pcpmsg,
    "The following values must be provided for function HAPH_S: wh,hg1,hg2");
    u_error("INITPCSI-HAPH_S",C_MSG->pcpmsg,-19,1);
    return(-19);

  hapl_s_error:
    sprintf(C_MSG->pcpmsg,
    "The following values must be provided for function HAPL_S: wh,bh,ch");
    u_error("INITPCSI-HAPL_S",C_MSG->pcpmsg,-20,1);
    return(-20);

  min_error:
    sprintf(C_MSG->pcpmsg,
    "The following values must be provided for function MIN: k");
    u_error("INITPCSI-MIN",C_MSG->pcpmsg,-21,1);
    return(-21);

  lunlam_error:
    sprintf(C_MSG->pcpmsg,
    "The following values must be provided for function LUNLAM: l");
    u_error("INITPCSI-LUNLAM",C_MSG->pcpmsg,-22,1);
    return(-22);

  zout_remove_error:
    sprintf(C_MSG->pcpmsg,
    "Unable to delete pre-existing ZOUT file");
    u_error("INITPCSI-ZOUTREM",C_MSG->pcpmsg,-23,1);
    return(-23);

  flspec_error:
    sprintf(C_MSG->pcpmsg,
    "The ZOUT filename is invalid");
    u_error("INITPCSI-FLSPEC",C_MSG->pcpmsg,-24,1);
    return(-24);

  to_file_error:
    sprintf(C_MSG->pcpmsg,
    "TO file was not specified by user");
    u_error("INITPCSI-TOERR",C_MSG->pcpmsg,-25,1);
    return(-25);

  to_file_exist:
    sprintf(C_MSG->pcpmsg,
    "TO file already exists");
    u_error("INITPCSI-TOEXIST",C_MSG->pcpmsg,-26,1);
    return(-26);

  to_file_invalid:
    sprintf(C_MSG->pcpmsg,
    "TO file name invalid");
    u_error("INITPCSI-TOINV",C_MSG->pcpmsg,-27,1);
    return(-27);

  cube_open_error:
    sprintf(C_MSG->pcpmsg,"Error opening input cube file %s",from);
    u_error("INITPCSI-OPENERR",C_MSG->pcpmsg,-28,1);
    return(-28);

  invalid_cube:
    sprintf(C_MSG->pcpmsg,"%s is not a standard cube file",from);
    u_error("INITPCSI-INVCUBE",C_MSG->pcpmsg,-29,1);
    return(-29); 

  bad_sys_keys:
    sprintf(C_MSG->pcpmsg,"Error obtaining system keywords from %s",from);
    u_error("INITPCSI-SYSKEYS",C_MSG->pcpmsg,-30,1);
    return(-30);

  parse_sfrom_error:
    sprintf(C_MSG->pcpmsg,"Unable to parse SFROM parameter %s for %s",sfrom,from);
    u_error("INITPCSI-PRSSFROM",C_MSG->pcpmsg,-31,1);
    return(-31); 

  sfrom_error:
    sprintf(C_MSG->pcpmsg,"Unable to apply SFROM %s to input file %s",sfrom,from);
    u_error("INITPCSI-SFROMERR",C_MSG->pcpmsg,-32,1);
    return(-32); 

  clat_error:
    sprintf(C_MSG->pcpmsg,"Unable to get clat from labels of %s",from);
    u_error("INITPCSI-CLAT",C_MSG->pcpmsg,-33,1);
    return(-33); 

  clon_error:
    sprintf(C_MSG->pcpmsg,"Unable to get clon from labels of %s",from);
    u_error("INITPCSI-CLON",C_MSG->pcpmsg,-34,1);
    return(-34); 
}
