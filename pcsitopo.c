#include <unistd.h>
#include "pcsi.h"
#include "clinom_msg.h"

INT4 PcsiTopo (void)

/***********************************************************************
*
*_Title	PcsiTopo - Write final topography solution to file
*
*_DESC	Write final topography solution to file
*
*_HIST	June 13 2002 Janet Barrett, Original version
*       July 29 2002 JB, Replaced ioct with ioct1 to prevent the image
*                    from being reoriented
*_END
************************************************************************/

{
  INT4 limits[3][2];
  INT4 ret;
  INT4 ioct1;

  topo.fileid = 0;
  topo.naxes = 3;
  topo.order = 1;
  topo.core_dims[0] = ins;
  topo.suffix_dims[0] = 0;
  (void) strcpy(topo.axnames[0],"SAMPLE");
  topo.core_dims[1] = inl;
  topo.suffix_dims[1] = 0;
  (void) strcpy(topo.axnames[1],"LINE");
  topo.core_dims[2] = 1;
  topo.suffix_dims[2] = 0;
  (void) strcpy(topo.axnames[2],"BAND");
  topo.pixel_type = 3;
  topo.scale[0] = 0.0;
  topo.scale[1] = 1.0;
  (void) u_type_keys(topo.pixel_type,&topo.item_bytes,
           topo.core_type,sizeof(topo.core_type),&ret);
  if (ret) {
    strcpy(C_MSG->pcpmsg,"Error getting pixel type keywords for output topo file.");
    u_error("PCSITOPO-PIXTYP",C_MSG->pcpmsg,-1,1);
    return(-1);
  }
  (void) q_set_sys_keys(&topo.fileid,topo.naxes,topo.order,
           topo.core_dims,topo.suffix_dims,(CHAR *)topo.axnames,
	   sizeof(topo.axnames[0]),topo.item_bytes,
	   topo.core_type,topo.scale,&ret);
  if (ret) {
    strcpy(C_MSG->pcpmsg,"Error setting file characteristics for output topo file.");
    u_error("PCSITOPO-SETSYS",C_MSG->pcpmsg,-2,1);
    return(-2);
  }
  (void) q_open(&topo.fileid,fid,topofile,3,0," ",0,0,1,0,&ret);
  if (ret) {
    strcpy(C_MSG->pcpmsg,"Error opening output topo file.");
    u_error("PCSITOPO-OPEN",C_MSG->pcpmsg,-3,1);
    return(-3);
  }

  ioct1 = 1;
  (void) zlineout(&cdum[n2],nx,ny,&cdum[n3],ins,inl,scale,ioct1,cosemi);

  limits[0][0] = 1;
  limits[0][1] = ins;
  limits[1][0] = 1;
  limits[1][1] = inl;
  limits[2][0] = 1;
  limits[2][1] = 1;
  (void) q_lio_cbrick(topo.fileid,2,limits,(void *)(&cdum[n3]),&ret);
  if (ret) {
    strcpy(C_MSG->pcpmsg,"Error writing data to output topo file.");
    u_error("PCSITOPO-WRITE",C_MSG->pcpmsg,-4,1);
    return(-4);
  }

  (void) q_close(topo.fileid,1,&ret);
  if (ret) {
    strcpy(C_MSG->pcpmsg,"Error closing output topo file.");
    u_error("PCSITOPO-CLOSE",C_MSG->pcpmsg,-5,1);
    return(-5);
  }

  return(0);
}
