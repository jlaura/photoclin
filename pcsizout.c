#include "pcsi.h"
#include "maplib.h"
#include "clinom_msg.h"
#include "clinom_ipars.h"
#include "clinom_mem.h"
#include "clinom_dnorm.h"
#include "clinom_aepar.h"

void pcsizout (void)

/***********************************************************************
*
*_Title pcsizout - Write zout file to disk
*
*_DESC  Write zout file to disk.
*
*_HIST  Mar 20 2003 Janet Barrett, USGS Flagstaff, Original version 
*    
*_END
************************************************************************/
 
{
  int i,j;
  INT4 ret;
  INT4 limits[3][2],nx1,ny1;

/**************************************************************
* Write zout file to disk
***************************************************************/
  if (u_file_exist(zoutfile)) remove(zoutfile);
  nx1 = nx + 1;
  ny1 = ny + 1;
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
    u_error("PCSIZOUT-PIXTYP",C_MSG->pcpmsg,-1,1);
    return;
  }
  (void) q_set_sys_keys(&zout.fileid,zout.naxes,zout.order,
           zout.core_dims,zout.suffix_dims,(CHAR *)zout.axnames,
	   sizeof(zout.axnames[0]),zout.item_bytes,
	   zout.core_type,zout.scale,&ret);
  if (ret) {
    sprintf(C_MSG->pcpmsg,"Error setting file characteristics for zout file: %s",
      zoutfile);
    u_error("PCSIZOUT-SETSYS",C_MSG->pcpmsg,-2,1);
    return;
  }
  (void) q_open(&zout.fileid,fid,zoutfile,3,0," ",0,0,1,0,&ret);
  if (ret) {
    sprintf(C_MSG->pcpmsg,"Error opening zout file: %s",zoutfile);
    u_error("PCSIZOUT-OPEN",C_MSG->pcpmsg,-3,1);
    return;
  }
  for (i=1; i<=ny1; i++) {
    limits[0][0] = 1;
    limits[0][1] = nx1;
    limits[1][0] = i;
    limits[1][1] = i;
    limits[2][0] = 1;
    limits[2][1] = 1;
    (void) u_move4(nx1,&cdum[(i-1)*nx1+n2],ibuf);
    for (j=0; j<nx1; j++) {
      ibuf[j] = ibuf[j] * scale * cosemi;
    }
    (void) q_lio_cbrick(zout.fileid,2,limits,(void *)ibuf,&ret);
    if (ret) {
      sprintf(C_MSG->pcpmsg,"Error writing to zout file: %s",zoutfile);
      u_error("PCSIZOUT-WRITE",C_MSG->pcpmsg,-4,1);
      return;
    }
  }
  (void) q_close(zout.fileid,1,&ret);
  if (ret) {
    sprintf(C_MSG->pcpmsg,"Error closing zout file: %s",zoutfile);
    u_error("PCSIZOUT-CLOSE",C_MSG->pcpmsg,-5,1);
    return;
  }

  return;
}
