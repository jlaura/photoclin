#include "pc2d.h"
#include "spi.h"

Boolean OpenCube (int dsp, char *ptr)

/***********************************************************************
*
*_Title	OpenCube - Opens a cube for display
*
*_Args	Type   Variable        I/O	Description
*_Parm	int	dsp		I	Display number
*_Parm	char	ptr		I	name of file to open
*_Parm	Boolean	OpenCube	O	False = opened ok
*					True = error occured
*
*_DESC	This routine will attempt to open a cube. If successful it
*	will load the "f" structure associated with the display and
*	return False. If an error occurs it will return True.
*
*_HIST	Jan 1 1996 Jeff Anderson, Original version
*       Oct 16 1998 Tracie Sucharski,  Changed the error code checking
*                      for spi_minitspc, so that qview doesn't core
*                      dump for images with no spice info.
*       Nov 02 1998 TLS, Added more checks for print lat/lon info.
*       Nov 19 1998 TLS, If level0 do not call spi stuff.
*       Mar 30 1999 TLS, Updated check for spi_checklev return value.
*_END
************************************************************************/

{
  FILEINFO *f;

  INT4 ret,ifid;
  INT4 core_size[3];
  INT4 suf_size[3];
  INT4 naxes,order;
  INT4 item_bytes;
  FLOAT4 iscale[2];
  CHAR axis_name[3][MAXLITLEN+1];
  CHAR item_type[MAXLITLEN+1];
  INT4 itype;
  INT4 fsc;
  INT4 maxns;
  INT4 total_bck;
  INT4 nbck;
  CHAR bck_names[MAX_BACKPLANES][MAXLITLEN+1];
  INT4 i;
  INT4 level;

/*------------------------------------------------------------------
/  Try to open the file
/-----------------------------------------------------------------*/

  u_set_erract (3,3);
  q_open (&ifid,0,ptr,READ_ONLY,0,"",0,0,1,0,&ret);
  if (ret) {
    strcpy (GBLerr.string,ptr);
    strcat (GBLerr.string," can not be opened");
    QviewMessage (-1);
    u_set_erract (3,1);
    return True;
  }

/*------------------------------------------------------------------
/  Make sure its a standard cube
/-----------------------------------------------------------------*/

  q_check_std (ifid,&ret);
  if (ret) {
    strcpy (GBLerr.string,ptr);
    if (ret == -8) {
      strcat (GBLerr.string," is not in the proper computer architecture. ");
      strcat (GBLerr.string," Please use the ISIS program convert.");
    }
    else {
      strcat (GBLerr.string," is not a standard 3-D cube");
    }
    QviewMessage (-1);
    q_close (ifid, 1, &ret);
    u_set_erract (3,1);
    return True;
  }

/*------------------------------------------------------------------
/  Get the system keys from the labels
/-----------------------------------------------------------------*/

  q_get_sys_keys (ifid,1,&naxes,&order,core_size,suf_size,
                  (char *)axis_name,sizeof(axis_name[0]),&item_bytes,
                  item_type,sizeof(item_type),iscale,&ret);
  if (ret) {
    strcpy (GBLerr.string,"Unable to retreive system keywords from ");
    strcat (GBLerr.string,ptr);
    QviewMessage (-1);
    q_close (ifid, 1, &ret);
    u_set_erract (3,1);
    return True;
  }

/*------------------------------------------------------------------
/  Check to make sure the cube isn't too big in the sample direction
/-----------------------------------------------------------------*/

  u_type_code (1,item_bytes,item_type,&itype);

  maxns = V_WORK_SIZE / item_bytes;
  if (core_size[0] > maxns) {
    strcpy (GBLerr.string,"Too many samples in ");
    strcat (GBLerr.string,ptr);
    strcat (GBLerr.string," to display");
    QviewMessage (-1);
    q_close (ifid, 1, &ret);
    u_set_erract (3,1);
    return True;
  }

/*------------------------------------------------------------------
/  Get backplane information
/-----------------------------------------------------------------*/

  nbck = MAX_BACKPLANES;
  q_get_suffix_names(ifid, "BAND", &total_bck, &nbck, (char *)bck_names,
                     sizeof(bck_names[0]), &ret);
  if (ret != 0) {
    strcpy (GBLerr.string,"Unable to read backplane information from ");
    strcat (GBLerr.string,ptr);
    QviewMessage (-1);
    q_close (ifid, 1, &ret);
    u_set_erract (3,1);
    return True;
  }

/*------------------------------------------------------------------
/ Load the file structure
/-----------------------------------------------------------------*/

  f = &GBLdsp.d[dsp].f;
  f->fid = ifid;
  f->ns = core_size[0];
  f->nl = core_size[1];
  f->nb = core_size[2];
  f->type = itype;
  f->scale[0] = iscale[0];
  f->scale[1] = iscale[1];
  f->nbck = nbck;
  strcpy (f->fname,ptr);
  if (strrchr (ptr,'/') == NULL) {
    strcpy (f->name,ptr);
  }
  else {
    strcpy (f->name,strrchr (ptr,'/')+1);
  }

  for (i=0; i<nbck; i++) {
    strcpy (f->bck_names[i],bck_names[i]);
  }

/*------------------------------------------------------------------
/ Find out if the cube has lev software mapping info 
/-----------------------------------------------------------------*/

  f->printlat = False;
  f->printpho = False;

  f->use_level = 0;
  level = lev_find_level (ifid);

  if (level == 1) {
    ret = lev1_init (ifid,&f->spi,0);
    if (ret == 0) {
      f->use_level = 1;
      f->printlat = True;
      f->printpho = True;
    }
  }
  else if (level == 2) {
    ret = lev2_init_from_labels (ifid,&f->map);
    if (ret == 0) {
      f->use_level = 2;
      f->printlat = True;
      f->printpho = False;
    }
  }

/*------------------------------------------------------------------
/ Find out if the cube has mapping information
/-----------------------------------------------------------------*/

  if (!f->use_level) {
    f->printlat = True;
    f->printpho = True;

    spi_checklev (f->fid,"QUBE","","IMAGE_MAP_PROJECTION",&level,
                  &ret);
    if (ret == -1 || level == 0) {
      f->printlat = False;
      f->printpho = False;
    }

    else {
      spi_minitspc ((INT4)(GBLdsp.free+1),f->fid,&fsc,&ret);
      if ((ret == 1) || (ret >= 1000) || (ret < 0)) {
        f->printlat = False;
      }
      if ((ret == 1) || (ret == 10) || (ret == 1010)) {
        f->printpho = False;
      }
      if (level == 1 && ret == 10) f->printlat = False;
    }
  }

  u_set_erract (3,1);
  return False;
}
