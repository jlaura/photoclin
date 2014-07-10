/*****************************************************************
*_TITLE clinom_rgeom.h 
*
*_DESC  Common block defining the imaging geometry for the radar
*       scene
*
*_HIST Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_RGEOM_H
#define CLINOM_RGEOM_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_rgeom_f_common {
  FLOAT4 h;      /* Elevation of radar track above datum
                    plane in units of pixels on finest mesh */
  FLOAT4 x0r;    /* Horizontal distance (on datum plane) from
                    sub-radar track to far edge of image in
		    units of pixels on finest mesh */
  FLOAT4 scent;  /* Slant range to center of image, in units
                    of pixels on finest mesh */
  FLOAT4 hf;     /* As h, but in units of pixels on current
                    mesh */
  FLOAT4 hf2;    /* Square of hf */
  FLOAT4 x0rn;   /* Coeff giving horizontal distance from
                    sub-radar track to *node* I in units of
		    pixels on current mesh: Xnode=x0rn - I */
  FLOAT4 x0re;   /* As x0rn but for center of element I */
  FLOAT4 scentf; /* As scent but in units of current pixels */
  FLOAT4 thsf;   /* 2*hf*scentf; thsf/x is twice the difference
                    in arc length (datum angle times scentf)
		    on the datum across a pixel in the range
		    direction
} CLINOM_RGEOM_F_COMMON;

extern CLINOM_RGEOM_F_COMMON     FTN_COMMON(rgeom);

static CLINOM_RGEOM_F_COMMON *C_RGEOM = &FTN_COMMON(rgeom);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
