/*********************************************************************
*_Title chardip.h Define chardip globals
*_VARS  None
*
*_HIST Apr 09 2003  Janet Barrett, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CHARDIP_H
#define CHARDIP_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"
#include "chardip.p"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#define PI          3.141592653589793
#define RAD_TO_DEG  180.0/PI
#define DEG_TO_RAD  PI/180.0

typedef struct cube_specs {
  INT4 fileid;                   /* File identifier */
  INT4 record_bytes;             /* Size of a data record */
  INT4 order;                    /* Order of data storage */
  INT4 naxes;                    /* Number of axes in file */
  INT4 item_bytes;               /* Number bytes per pixel item */
  INT4 suffix_bytes;             /* Number bytes per suffix item */
  CHAR core_type[MAXLITLEN+2];   /* Type of pixel data */
  FLOAT4 scale[2];               /* Pixel scaling values */
  INT4 pixel_type;               /* Pixel type code */
  CHAR axnames[MAX_AXES][MAXLITLEN+2]; /* Names of each axis */
  INT4 core_dims[MAX_AXES];      /* Dimensions of core region */
  INT4 suffix_dims[MAX_AXES];    /* "           " suffix region */
  INT4 vcore_dims[MAX_AXES];     /* Virtual dimensions of core region */
  INT4 vsuffix_dims[MAX_AXES];   /* "                " suffix region */
  INT4 bandno;                   /* Band number to process */
  BOOLEAN need_min;              /* MINIMUM keyword (if needed) */
  BOOLEAN need_max;              /* MAXIMUM keyword (if needed) */
  FLOAT4 dmin;                   /* Data minimum */
  FLOAT4 dmax;                   /* Data maximum */
} CUBE_SPECS;

CHAR from[MAXFILNAME+1]; /* Input image file */
INT4 fid;                /* File id for from file(s) */
CHAR tiltfile[MAXFILNAME+1]; /* File that the unit tilt toward sun is written to */
CUBE_SPECS tilt;         /* Image information for tiltfile */
INT1 nucenter;             /* Change from default center line/sample? (1) for
			      SPICE geometry info, (2) for center of spherical
			      datum */
FLOAT8 x;                  /* (1), (2) user-specified sample at which to 
			      calculate geometry, relative to FULL frame(s)
			      (3) user-specified sample at center of spherical
			      datum, relative to input image, which may be less
			      than full frame because of previous processing */
FLOAT8 y;                  /* (1), (2) user-specified line at which to 
			      calculate geometry, relative to FULL frame(s)
			      (3) user-specified line at center of spherical
			      datum, relative to input image, which may be less
			      than full frame because of previous processing */
INT1 distortd;             /* Does each image contain distortions? */
FLOAT4 radius;             /* Planetary radius in km; user input overrides value
			      from planet.sav */
FLOAT8 aznor;              /* Azimuth(s) to north */
FLOAT8 res;                /* Image resolution(s) in km/pixel */
/* clemi, clinc, azemi, and azinc will be converted from the
   usual photometric angles in image 2 into angles describing the
   emission incidence vectors of image 2 relative to coordinates
   centered on the emission directions for image 1 */
FLOAT8 clemi;              /* Emission angles for image(s) */
FLOAT4 clinc;              /* Phase angle(s) */
FLOAT8 azemi;              /* Azimuth(s) of surface normal vector(s) */
FLOAT8 azinc;              /* Azimuth(s) of incidence vector(s) */
FLOAT4 dip;                /* Dip of datum (degrees) */
FLOAT4 az;                 /* Azimuth of datum dip */
FLOAT4 scale;              /* Scale factor to convert final output
                              to units of m/dn */
FLOAT4 charaz;             /* Azimuth of characteristic strips */
FLOAT4 charazgrid;         /* Azimuth of characteristic strips -
                              this value is displayed on the main
			      gui window */
INT4 ins,inl;              /* Number of samples, number of lines in input
                              image */
FLOAT4 *cdum;              /* Array containing all final and intermediate
                              pcp data */
INT4 image_bytes;
FLOAT4 image_scale[2];
FLOAT4 cosemi;             /* Used for scaling from line-of-sight to
                              vertical for the tilt file */

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
