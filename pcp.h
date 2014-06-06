/*********************************************************************
*_Title pcp.h Define pc2d globals
*_VARS  None
*
*_HIST Jun 01 2001  Janet Barrett, USGS Flagstaff, Original Version
*      Jul 29 2002  JB, Got rid of ioct
**********************************************************************/
#ifndef PCP_H
#define PCP_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"
#include "pcp.p"
#include "Xm/Xm.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#define V_GO_BTN      1
#define V_GO1_BTN     2
#define V_PAUSE_BTN   3
#define V_RESDN_BTN   4
#define V_RESUP_BTN   5
#define V_DONE_BTN    6
/*#define V_SMOOTH_BTN  7
#define V_PARAM_BTN   8*/
#define V_BACK_BTN    7
#define V_SOR_BTN     8
#define V_DIRECT_BTN  9
#define V_CGM_BTN     10
#define V_DSPON_BTN   11
#define V_DSPOFF_BTN  12
#define V_EXACT_BTN   13
#define V_FAST_BTN    14

#define PI          3.141592653589793
#define RAD_TO_DEG  180.0/PI
#define DEG_TO_RAD  PI/180.0

#define LOGTEXT 0
#define STATTEXT 1


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

/*CHAR pcpmsg[512];*/          /* String used for pcp messages */
/*CHAR from[2][MAXFILNAME+1];*/ /* Input image file */
/*CHAR from[MAXFILNAME+1];  Input image file */
/*INT4 fid[2];*/                /* File id for from file(s) */
/*INT4 fid;                 File id for from file(s) */
/*CHAR topofile[MAXFILNAME+1]; File that final topo result is written to */
/*CUBE_SPECS topo;             Image information for topofile */
/*CHAR zin[MAXFILNAME+1];     File containing initial guess for topo */
/*BOOLEAN usezin;                Length of zin file name */
/*BOOLEAN usedatum;              Indicates if we start with datum model
*			      only, no topo */
/*CHAR zoutfile[MAXFILNAME+1]; File that the last full-res topo estimate
*                              will be written to when the program
*			       terminates */
/*CUBE_SPECS zout;            Image information for zoutfile */
/*BOOLEAN dozout;             Determines if zout file will be created */
/*CHAR logfile[MAXFILNAME+1]; File to receive history of the iteration 
*		              process */
/*CHAR note[80];              A note to put into the log file */
/*CHAR from2[MAXFILNAME+1];*/  /* Second input file to use for biscopic
			      photocliometry (not used) */
/*FLOAT4 dndatum;             Image value for datum orientation, after DNATM
*		              subtracted */ 
/*INT1 nucenter;              Change from default center line/sample? (1) for
*		              SPICE geometry info, (2) for center of spherical
*		              datum */
/*FLOAT8 x[3];                (1), (2) user-specified sample at which to 
*			      calculate geometry, relative to FULL frame(s)
*			      (3) user-specified sample at center of spherical
*			      datum, relative to input image, which may be less
*			      than full frame because of previous processing */
/*FLOAT8 y[3];                (1), (2) user-specified line at which to 
*			      calculate geometry, relative to FULL frame(s)
*			      (3) user-specified line at center of spherical
*			      datum, relative to input image, which may be less
*			      than full frame because of previous processing */
/*INT1 distortd;              Does each image contain distortions? */
/*FLOAT4 radius;              Planetary radius in km; user input overrides value
*		              from planet.sav */
/*INT1 marsfit[2];            Use Minnaert photometric function with k(phase) fit
*		              to Mars whole disk data? */
/*CHAR phofunc[2][15];*/       /* Names of photometric functions to use */
/*FLOAT4 walbedo[2];                Single-scattering albedo */
/*FLOAT4 gasym[2];                Asymmetry parameter */
/*FLOAT8 aznor[2];            Azimuth(s) to north */
/*FLOAT8 res[2];              Image resolution(s) in km/pixel */
/* clemi, clinc, azemi, and azinc will be converted from the
   usual photometric angles in image 2 into angles describing the
   emission incidence vectors of image 2 relative to coordinates
   centered on the emission directions for image 1 */
/*FLOAT8 clemi[2];            Emission angles for image(s) */
/*FLOAT4 clinc[2];            Phase angle(s) */
/*FLOAT8 azemi[2];            Azimuth(s) of surface normal vector(s) */
/*FLOAT8 azinc[2];            Azimuth(s) of incidence vector(s) */
/*FLOAT4 dip;                 Dip of datum (degrees) */
/*FLOAT4 az;                  Azimuth of datum dip */
/*FLOAT4 scale;               Scale factor to convert final output
*                             to units of m/dn */
/*FLOAT4 charaz;*/             /* Azimuth of characteristic strips */
/*FLOAT4 charazgrid;          Azimuth of characteristic strips -
*                             this value is displayed on the main
*		              gui window */
/*INT4 nsmooth;               # of times to smooth initial solution with
*                             3x3 boxcar before iterating; if -ve, smooth
*		              until residuals top decreasing */
/*INT4 ins,inl;               Number of samples, number of lines in input
*                             image */
/*INT4 *cdum;                 Array containing all final and
*                             intermediate pcp data */
/*INT4 nx,ny;                 Sample,line dimension of image in internal
*                             orientation */

/*CHAR pcpprevrmsfile[15][256];  Name of previous full-res topo
*                                estimate work file */
/*CHAR pcpprevzfile[15][256];    Name of previous full-res topo
*                                estimate work file */
/*INT4 pcpundo[15][2];           Determines if solution can be undone to previous
*                             step: 0=cannot undo, 1=can undo to last step within
*		              current magnification level without skipping steps
*		              done at lower magnification levels, 2=can undo to
*		              last step within current magnification level, but
*		              must skip steps done at lower magnification levels
*		              (this cancels out all undos at lower magnification
*		              levels) */
/*CUBE_SPECS pcpprevz;*/             /* Image information for previous full-res
                                    topo estimate work file */
/*INT4 n1,n2,n3,n4,n5;              Indices to data in cdum array */
/*INT2 solntype;                    Determines which solution types are
*                                   possible based on available memory:
*			            1 - SOR is possible
*			            2 - SOR and CGM are possible 
*			            3 - SOR, CGM, and DIRECT are possible */
static INT4 mag,minmag;          /* Current resolution magnitude and minimum
                                    resolution magnitude */
/*INT4 ioct;*/                       /* Determines which 45 degree "octant" that
                                    charaz lies in. Data is transformed on
				    input to put characteristics in octant +1
				    ([0,45]). The data is transformed back on
				    output. */
/*BOOLEAN LeftDsp[5];               Keeps track of which type of data is
*                                   currently displayed in the Left display
*			            window. Only one index of the array is
*		                    set to True at a time. The indices in
*			            the array correspond to the 5 types of
*			            data as follows:
*			            0 = Image data
*                                   1 = Estimate of topography
*			            2 = RMS NR increment to the estimate of
*			                topography (DZ = difference between
*				        successive Z (topo) files)
*                                   3 = Synthesized image
*			            4 = Difference between synthesized and
*			                target image */
/*BOOLEAN RightDsp[5];              See description for LeftDsp */
/*CHAR pcpdatafile[5][256];         Names of files containing image and
*                                   topography data */
/*CUBE_SPECS pcpdata[5];            Image information for data files */
/*FLOAT4 cosemi;                    Used for scaling from line-of-sight to
*                                   vertical for ZOUT and TO. The Z values are
*			            multiplied by cosemi on output. Used for
*			            scaling from vertical to line-of-sight for
*			            ZIN. The Z values are divided by cosemi on
*			            input. */
#if defined(PCP_DECLARE_GLOBALS)
Widget logshell;
Widget logtext;
Widget stattext;
Widget counterlabel;
Widget SORbtn;
Widget DIRbtn;
Widget CGMbtn;
Widget WMAXslider;
Widget ITMAXslider;
Widget hintsLB;
Widget errorsLB;
Widget charazLB;
Widget fmagLB;
Widget rmsrhsberLB;
Widget dipLB;
Widget plnslpLB;
Widget rmsresLB;
Widget DSPONbtn;
Widget DSPOFFbtn;
Widget FASTbtn;
Widget EXACTbtn;
BOOLEAN LeftDsp[5];
BOOLEAN RightDsp[5];
BOOLEAN aeknown;
BOOLEAN fastinj;
FLOAT4 az;
FLOAT8 azemi[2];
FLOAT8 azinc[2];
FLOAT8 aznor[2];
INT4 *cdum;
BOOLEAN changeres;
FLOAT4 charazgrid;
FLOAT8 clemi[2];
FLOAT4 clinc[2];
FLOAT4 cosemi;
FLOAT4 dip;
INT1 distortd;
FLOAT4 dndatum;
BOOLEAN dozout;
INT4 fid;
CHAR from[MAXFILNAME+1];
/*FLOAT4 gasym[2];*/
INT4 image_bytes;
FLOAT4 image_scale[2];
INT4 ins,inl;
CHAR logfile[MAXFILNAME+1];
INT1 marsfit[2];
INT4 n1,n2,n3,n4,n5;
INT4 n1lmag,n2lmag,n4lmag,nxlmag,nylmag;
CHAR note[80];
INT4 nsmooth;
INT1 nucenter;
INT4 nx,ny;
CUBE_SPECS pcpdata[5];
CHAR pcpdatafile[5][256];
CHAR pcpprevrmsfile[15][256];
CHAR pcpprevzfile[15][256];
INT4 pcpresstep;
INT4 pcpundo[15][2];
FLOAT4 radius;
FLOAT8 res[2];
FLOAT4 scale;
INT2 solntype;
INT4 sordir;
CHAR topofile[MAXFILNAME+1];
CUBE_SPECS topo;
BOOLEAN usedatum;
BOOLEAN usezin;
/*FLOAT4 walbedo[2];*/
BOOLEAN wrtimg;
FLOAT8 x[3],y[3];
CHAR zin[MAXFILNAME+1];
CUBE_SPECS zout;
CHAR zoutfile[MAXFILNAME+1];
#else
extern Widget logshell;
extern Widget logtext;
extern Widget stattext;
extern Widget counterlabel;
extern Widget SORbtn;
extern Widget DIRbtn;
extern Widget CGMbtn;
extern Widget WMAXslider;
extern Widget ITMAXslider;
extern Widget hintsLB;
extern Widget errorsLB;
extern Widget charazLB;
extern Widget fmagLB;
extern Widget rmsrhsberLB;
extern Widget dipLB;
extern Widget plnslpLB;
extern Widget rmsresLB;
extern Widget DSPONbtn;
extern Widget DSPOFFbtn;
extern Widget FASTbtn;
extern Widget EXACTbtn;
extern BOOLEAN LeftDsp[5];
extern BOOLEAN RightDsp[5];
extern BOOLEAN aeknown;
extern BOOLEAN fastinj;
extern FLOAT4 az;
extern FLOAT8 azemi[2];
extern FLOAT8 azinc[2];
extern FLOAT8 aznor[2];
extern INT4 *cdum;
extern BOOLEAN changeres;
extern FLOAT4 charazgrid;
extern FLOAT8 clemi[2];
extern FLOAT4 clinc[2];
extern FLOAT4 cosemi;
extern FLOAT4 dip;
extern INT1 distortd;
extern FLOAT4 dndatum;
extern BOOLEAN dozout;
extern INT4 fid;
extern CHAR from[MAXFILNAME+1];
/*extern FLOAT4 gasym[2];*/
extern INT4 image_bytes;
extern FLOAT4 image_scale[2];
extern INT4 ins,inl;
extern CHAR logfile[MAXFILNAME+1];
extern INT1 marsfit[2];
extern INT4 n1,n2,n3,n4,n5;
extern INT4 n1lmag,n2lmag,n4lmag,nxlmag,nylmag;
extern CHAR note[80];
extern INT4 nsmooth;
extern INT1 nucenter;
extern INT4 nx,ny;
extern CUBE_SPECS pcpdata[5];
extern CHAR pcpdatafile[5][256];
extern CHAR pcpprevrmsfile[15][256];
extern CHAR pcpprevzfile[15][256];
extern INT4 pcpresstep;
extern INT4 pcpundo[15][2];
extern FLOAT4 radius;
extern FLOAT8 res[2];
extern FLOAT4 scale;
extern INT2 solntype;
extern INT4 sordir;
extern CHAR topofile[MAXFILNAME+1];
extern CUBE_SPECS topo;
extern BOOLEAN usedatum;
extern BOOLEAN usezin;
/*extern FLOAT4 walbedo[2];*/
extern BOOLEAN wrtimg;
extern FLOAT8 x[3],y[3];
extern CHAR zin[MAXFILNAME+1];
extern CUBE_SPECS zout;
extern CHAR zoutfile[MAXFILNAME+1];
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
