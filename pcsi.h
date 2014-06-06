/*********************************************************************
*_TITLE pcsi.h Define pcsi globals
*
*_DESC  Common block for pcsi program
*
*_HIST Mar 19 2003  Janet Barrett, USGS Flagstaff, Original Version
*      Jun 11 2004  Kris Becker Reworked globals so they no longer
*                    have multiple instances using extern configuration
**********************************************************************/
#ifndef PCSI_H
#define PCSI_H 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "isistypes.h"
#include "isismain.h"
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"
#include "lev.h"
#include "pcsi.p"

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


#if defined(DECLARE_GLOBALS)
INT4 sordir;               /* Determines which solution type is used:
                              0 - SOR is used
			      1 - DIR is used
			      2 - CGM is used */
INT4 nx,ny;                /* Sample,line dimension of image in internal
                              orientation */
INT4 ins,inl;              /* Number of samples, number of lines in input
                              image */
INT4 nsmooth;              /* # of times to smooth initial solution with
                              3x3 boxcar before iterating; if -ve, smooth
	                      until residuals top decreasing */
INT1 dozout;               /* Determines if zout file will be created */
FLOAT4 dndatum;            /* Image value for datum orientation, after DNATM
		              subtracted */ 
INT4 *cdum;                /* Array containing all final and
                              intermediate pcp data */
CHAR from[129];            /* Input image file */
INT4 fid;                  /* File id for from file */
CHAR topofile[129];        /* File that final topo result is written to */
CUBE_SPECS topo;           /* Image information for topofile */
CHAR zin[129];             /* File containing initial guess for topo */
BOOLEAN usezin;            /* Length of zin file name */
BOOLEAN usedatum;          /* Indicates if we start with datum model
			      only, no topo */
CHAR zoutfile[129];        /* File that the last full-res topo estimate
                              will be written to when the program
			      terminates */
CUBE_SPECS zout;           /* Image information for zoutfile */
CHAR logfile[129];         /* File to receive history of the iteration 
			      process */
CHAR note[81];             /* A note to put into the log file */
INT1 nucenter;             /* Change from default center line/sample? (1) for
			      SPICE geometry info, (2) for center of spherical
			      datum */
FLOAT8 x;                  /* User-specified sample at which to calculate
			      geometry, relative to FULL frame. */
FLOAT8 y;                  /* User-specified line at which to calculate
			      geometry, relative to FULL frame. */
INT1 distortd;             /* Does each image contain distortions? */
FLOAT4 radius;             /* Planetary radius in km; user input overrides value
			      from planet.sav */
FLOAT8 aznor;              /* Azimuth to north */
FLOAT8 res;                /* Image resolution in km/pixel */
FLOAT8 clemi;              /* Emission angles for image */
FLOAT4 clinc;              /* Phase angle */
FLOAT8 azemi;              /* Azimuth of surface normal vector */
FLOAT8 azinc;              /* Azimuth of incidence vector */
FLOAT4 dip;                /* Dip of datum (degrees) */
FLOAT4 az;                 /* Azimuth of datum dip */
FLOAT4 scale;              /* Scale factor to convert final output
                              to units of m/dn */
FLOAT4 charazgrid;         /* Azimuth of characteristic strips -
                              this value is displayed on the main
			      gui window */
INT4 image_bytes;
FLOAT4 image_scale[2];

INT4 n1,n2,n3,n4,n5;             /* Indices to data in cdum array */
INT4 solntype;                   /* Determines which solution types are
                                    possible based on available memory:
				    1 - SOR is possible
				    2 - SOR and CGM are possible 
				    3 - SOR, CGM, and DIRECT are possible */
INT4 n1lmag,n2lmag,n4lmag,nxlmag,nylmag;
CHAR pcpdatafile[5][256];        /* Names of files containing image and
                                    topography data */
CUBE_SPECS pcpdata[5];           /* Image information for data files */
BOOLEAN changeres;
FLOAT4 cosemi;                   /* Used for scaling from line-of-sight to
                                    vertical for ZOUT and TO. The Z values are
				    multiplied by cosemi on output. Used for
				    scaling from vertical to line-of-sight for
				    ZIN. The Z values are divided by cosemi on
				    input. */
CHAR method[4];                  /* Type of solution method to use (SOR,
                                    CGM,DIR) */
FLOAT4 *ibuf;

#else

extern INT4 sordir;
extern INT4 nx,ny;
          
extern INT4 ins,inl;
extern INT4 nsmooth;
extern INT1 dozout;
extern FLOAT4 dndatum;
extern INT4 *cdum;
extern CHAR from[129];
extern INT4 fid;
extern CHAR topofile[129];
extern CUBE_SPECS topo;
extern CHAR zin[129];
extern BOOLEAN usezin;
extern BOOLEAN usedatum;
extern CHAR zoutfile[129];
extern CUBE_SPECS zout;
extern CHAR logfile[129];
extern CHAR note[81];
extern INT1 nucenter;
extern FLOAT8 x;
extern FLOAT8 y;
extern INT1 distortd;
extern FLOAT4 radius;
extern FLOAT8 aznor;
extern FLOAT8 res;
extern FLOAT8 clemi;
extern FLOAT4 clinc;
extern FLOAT8 azemi;
extern FLOAT8 azinc;
extern FLOAT4 dip;
extern FLOAT4 az;
extern FLOAT4 scale;
extern FLOAT4 charazgrid;
extern INT4 image_bytes;
extern FLOAT4 image_scale[2];
extern INT4 n1,n2,n3,n4,n5;
extern INT4 solntype;
extern INT4 n1lmag,n2lmag,n4lmag,nxlmag,nylmag;
extern CHAR pcpdatafile[5][256];
extern CUBE_SPECS pcpdata[5];
extern BOOLEAN changeres;
extern FLOAT4 cosemi;
extern CHAR method[4];
extern FLOAT4 *ibuf;

#endif
#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
