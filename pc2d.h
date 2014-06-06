/*****************************************************************
*_Title qview.h Define qview globals
*_VARS  None
*
*_HIST Jan 01 1996  Jeff Anderson, USGS Flagstaff, Original Version
*      Jan 20 1998  Tracie Sucharski, Added variables to save fill display.
*      Mar 18 1998  TLS, Added measure tool option default.
*      May 05 1998  TLS, Added measure save file.
*      Nov 14 1998  TLS, Add reseau options.
*      Jul 20 2007  Janet Barrett, Added Steven Lambright's modifications
*                   so the software will work on both Macs and Linux
*      Jun 18 2008 Kris Becker Update BYTEDEPTH and BITMAP_PAD Mac conditinal
*                           declarations to work properly on (Intel/Leopard) 
*                           Macs 
**********************************************************************/
#ifndef QVIEW_H
#define QVIEW_H 1

#include <Xm/Xm.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "isistypes.h"
#include "isismain.h"

#include "lev.h"

typedef struct {
  Widget topShell;
  XtAppContext appContext;
  Display *display;
  Screen *screen;
  int scr_num;
  Window root;
  XVisualInfo vinfo;
  Cardinal depth;
  GC imgGC;
  Colormap cmap;
  int height;
  int width;
  int cpt;
} QVXINFO;

typedef struct {
  CHAR name[256];	/* Short name of cube (no directory) */
  CHAR fname[256];	/* Full name of cube */
  INT4 fid;		/* Fid for cube i/o */
  INT4 ns;		/* Number of samples in cube */
  INT4 nl;		/* Number of lines in cube */
  INT4 nb;		/* Number of bands in cube */
  INT4 nbck;            /* Number of backplanes in cube */
  INT4 type;		/* Cube type 1,2,3 = 8,16,32-bit */
  CHAR bck_names[MAX_BACKPLANES][MAXLITLEN+1]; /* Backplane names */
  FLOAT4 scale[2];	/* Cube base and multiplier */
  Boolean printlat;	/* Map proj info is available and can be output */
  Boolean printpho;     /* Info is available for printing photo statistics */

  INT4 use_level;
  Lev1 spi;
  Lev2 map;
} FILEINFO;

typedef struct {
  Widget shell;			/* Top shell of display */
  Widget DA;			/* Display drawing area */
  Widget posLB;			/* Position label for left image */
  Widget dnLB;			/* Dn at position label for left image */
  Widget bandLB;                /* Band information for left image */
  Widget infoLB;		/* Information label (for sl,ss,el,es) for left image */
  Widget scaleLB;		/* Scale label (for inc) for left image */
  Widget latLB;
  Widget pieLB;

  Pixmap pmap;			/* Pixmap for displaying in drawing area */
  XImage *ximg;			/* XImage structure used for loading */
  char *ximgptr;		/* Memory for ximage structure */
  Colormap cmap;		/* Colormap for drawing area */
  unsigned long cmap_pixs[256];	/* Colormap pixels */
  int cmap_count;		/* Number of pixels allocated in colormap */
  XColor cmap_cells[256];	/* Colors in the colormap */

  FILE *fill_fp;                /* File pointer for saved fill display */
  char fillfile[128];           /* File name of temp file */
  Boolean save_tmp;             /* Save filled display to temp file */
  Boolean read_tmp;             /* Read filled display from temp file */

  INT4 scs_sl;			/* Subcube specifier starting line */
  INT4 scs_el;			/* Subcube specifier ending line */
  INT4 scs_ss;			/* Subcube specifier starting sample */
  INT4 scs_es;			/* Subcube specifier ending sample */

  /* CUBE RELATED LOADING VARIABLES */
  FILEINFO f;			/* Info about file in display */
  int band[3];			/* Bands to display */
  double sl;			/* Starting line to display */
  double ss;			/* Starting sample to display */
  double inc;			/* line/sample increment */
  double *linelut;		/* Lut in=screen line out=cube line */
  double *samplut;		/* Lut in=screen samp out=cube samp */

  int map[256];			/* B/W lut in=8-bit pixel out=screen pixel */
  int rmap[256];		/* Red lut in=8-bit pixel out=screen pixel */ 
  int gmap[256];		/* Grn lut in=8-bit pixel out=screen pixel */
  int bmap[256];		/* Blu lut in=8-bit pixel out=screen pixel */
  int unmap[256];		/* B/W lut in=screen pixel out=8-bit pixel */
  int runmap[256];		/* Red lut in=screen pixel out=8-bit pixel */
  int bunmap[256];		/* Grn lut in=screen pixel out=8-bit pixel */
  int gunmap[256];		/* Blu lut in=screen pixel out=8-bit pixel */
  Boolean world;                /* Full display on */
  int curtool;			/* Current toolbar item */
  int curapp;                   /* Current application */

  int tool_x;			/* Current toolbar x position (at time button
                                   is pushed)  */
  int tool_y;			/* Current toolbar y position (at time button
                                   is pushed)  */
  int tool_sx;			/* Start x for zoom/stretch box */
  int tool_sy;			/* Start y for zoom/stretch box */
  Boolean tool_boxon;		/* Box has been drawn or not */
  int zoom_savex;               /* Save center of zoom position */
  int zoom_savey;               /* Save center of zoom position */

  /* PRESTRETCH VARIABLES */
  double rmin,rmax;		/* Manual red and b/w min/max for loading */
  double gmin,gmax;		/* Manual green min/max for loading */
  double bmin,bmax;		/* Manual blue min/max for loading */
  double rmin_real,rmax_real;   /* Manual red and b/w min/max for loading */
  double gmin_real,gmax_real;   /* Manual green min/max for loading */
  double bmin_real,bmax_real;   /* Manual blue min/max for loading */
  double rmin_reset,rmax_reset;	/* Manual red and b/w min/max for loading */
  double gmin_reset,gmax_reset;	/* Manual green min/max for loading */
  double bmin_reset,bmax_reset;	/* Manual blue min/max for loading */
  Boolean manstr;		/* Load using manual stretch */ 
  double amin[MAX_BANDS];	/* Auto minimum for bands in cube */
  double amax[MAX_BANDS];	/* Auto maximum for bands in cube */
  Boolean avalid[MAX_BANDS];	/* Are amin/amax valid */ 
  Boolean autostr;		/* Load using automatic stretch */
  double prestr_minper;         /* Prestretch minimum percent */
  double prestr_maxper;         /* Prestretch maximum percent */
  UINT1 rlut[65536];		/* Red prestretch lut */
  UINT1 glut[65536];		/* Green prestretch lut */
  UINT1 blut[65536];		/* Blue prestretch lut */
  INT4 pseudolut[32768];	/* Lut for 3-band color on 8-bit device */
  INT4 pseudomap[256][4];	/* rgb triplets for 3-band color */
  INT4 pseudocells;		/* Number of available color cells in cmap */

  /* CURRENT DEFAULTS FOR TOOLBAR FUNCTIONS */
  double zoom_factor;		/* Current zoom factor, default = 2 */
  double roam_percent;		/* Roam screen percentage, default = .75 */
  double stretch_lowper;	/* Low stretch percentage, default = .5 */
  double stretch_hiper;		/* High stretch percentage, default = 99.5 */

  /* POSITION REPORTING VARIABLES */
  Boolean showpos;		/* Report position */

  /* TVDOCTOR VARIABLES */
  Boolean rx_point;		/* In point mode */
  Boolean rx_full_line;		/* In full line mode */
  Boolean rx_se_line;		/* In start/end line mode */
  Boolean rx_rect;		/* Rectangle mode */
  Boolean rx_null;		/* Set to NULL*/
  Boolean rx_lsat;		/* Set to low saturation */
  Boolean rx_hsat;		/* Set to high saturation */
  Boolean rx_dn;		/* Set to rx_dnval */
  FLOAT4 rx_dnval;		/* Set to dn value */
  Boolean rx_all;		/* Edit all bands or only visible bands */
  Boolean rx_readwrite;		/* Is cube opened read/write */
  char *rx_undodn;		/* Undo buffer */
  INT4 *rx_line;		/* Undo line buffer */
  INT4 *rx_samp;		/* Undo samp buffer */ 
  int rx_undopix;		/* Number of pixels in undo buffer */

  /* TVLIST VARIABLES */
  Boolean list_firstime;	/* True = first time a listing has been done */
  Boolean list_roi;		/* True = roi, False = rectangle */
  Boolean list_core;		/* True = List core region, False = List backplane region */
  int list_box_nl;		/* Number of lines in list box */
  int list_box_ns;		/* Number of samples in list box */
  char list_file[256];		/* List save filename */
  Boolean list_all;		/* True = save all bands to list file */
  Widget list_bandLB;
  Widget list_boxLB;
  Widget list_dnTF;
  Widget list_lsLB;
  Widget list_avgLB;
  Widget list_stdLB;
  Widget list_minLB;
  Widget list_maxLB;
  int list_x;
  int list_y;
  int list_band;		/* Band, 0=red/mono, 1=green, 2=blue */
  int list_back;                /* Backplane */
  int list_line;		/* Current center line */		
  int list_samp;		/* Current center samp */
  double list_avg;		/* Current average */
  double list_std;		/* Current standard deviation */
  double list_min;		/* Current minimum */
  double list_max;		/* Current maximum */
  int list_npts;		/* Number of pixels in listing */
  int list_vpts;		/* Number of valid pixels in listing */

  Boolean measure_isis;
  Boolean measure_cart;
  char measure_file[256];

  int reseau_box;
  int reseau_line_width;

  
 /* Dialog box widgets */
  Widget listDB;
  int measure_ctr;              /*Measure tool center 1=on 0=off */

  int msg_response;             /* Response to QviewMessage dialog */
                                /* 0=OK  1=Cancel */
} DSPINFO;

#define V_MAXDSP 6
typedef struct {
  DSPINFO d[V_MAXDSP];		/* Structure containing all display info */
  Boolean used[V_MAXDSP];	/* Indicates if location is used */
  Boolean linked[V_MAXDSP];	/* Indicates display is linked */
  int free;			/* Current free spot used when creating dsps*/
  Boolean cellused[256];	/* Used color cells in default Pseudomap */
  Boolean rcellused[256];	/* Used red cells in default DirectColormap */
  Boolean gcellused[256];	/* Used green cells in default Directmap */
  Boolean bcellused[256];	/* Used blue cells in default DirectColormap */

/* Blink variables */
  Boolean blink_auto;		/* Blink automatic or manual */
  int blink_rate;		/* Automatic blink rate in seconds x 10 */
  int blink_display;		/* Current display being blinked */
  int blink_base;		/* Base display for blinking */
  Boolean blink_cmap;		/* Blink the colormaps too */
  Boolean tvreg;                /* Registration On/Off */
} QVGBLDSP;

/*  Resource Defaults  */
typedef struct {
  int error[18];
  double zoom_factor;            /* Zoom Factor */
  double roam_percent;
  double stretch_loper;
  double stretch_hiper;
  Boolean showpos;              /* Show cursor line/sample & lat/lon */
  char rx_mode[30];
  char rx_value[4];
  float rx_dn;
  Boolean rx_all;
  char list[15];                /* List core or backplane values */
  int list_box_ns;              /* Box size in sample direction */
  int list_box_nl;              /* Box size in line direction */
  char list_file[256];          /* Filename to same listing */
  int display_ns;               /* NS to initially display */
  int display_nl;               /* NL to initially display */
  Boolean blink_auto;           /* Blink automatic or manual */
  int blink_rate;             /* Automatic blink rate in seconds */
  double prestr_lowper;          /* Low end percent for prestretch */
  double prestr_hiper;           /* High end percent for prestretch */
  int prestr_sinc;              /* Sample inc to calculate prestretch */
  int prestr_linc;              /* Line inc to calculate prestretch */
  char filter[60];              /* Filter for file dialog box */
  char path[256];               /* Directory path for file dialog box */
  char font[200];
  unsigned int cursor;
  char measure_mode[13];
} QVGBLDEFAULT;

typedef struct {
  char string[512];		/* String used for messages in QviewMessage */
  Boolean status;		/* 0=Ok button, 1=Cancel Button */
} QVGBLERR;

#define V_WORK_SIZE 131072
typedef struct {
  char *work[4];		/* Work buffers each V_WORK_SIZE bytes*/
  char *big;			/* A big work buffer 1/2 megbyte */
} QVGBLWORK;

typedef struct {
  XColor green;                 /* Green pixel value */
  XColor red;			/* Red pixel value */
  XColor white;			/* White pixel value */
  XColor black;			/* Black pixel value */
  XColor yellow;		/* yellow pixel value */
} QVGBLCOLOR;

#if defined(QV_DECLARE_GLOBALS)
Widget qviewFM;
QVXINFO xinfo;
QVGBLDSP GBLdsp;
QVGBLDEFAULT GBLdefault;
QVGBLERR GBLerr;
QVGBLWORK GBLwork;
QVGBLCOLOR GBLcolor;
#else
extern Widget qviewFM;
extern QVXINFO xinfo;
extern QVGBLDSP GBLdsp;
extern QVGBLDEFAULT GBLdefault;
extern QVGBLERR GBLerr;
extern QVGBLWORK GBLwork;
extern QVGBLCOLOR GBLcolor;
#endif

#define V_NO_TOOL -1
#define V_ZOOM_TOOL 0
#define V_ROAM_TOOL 1
#define V_STRETCH_TOOL 2
#define V_TRACK_TOOL 3
#define V_WORLD_TOOL 4
#define V_LIST_TOOL 5
#define V_DOCTOR_TOOL 6
#define V_LIMBFIT_APP 7
#define V_MEASURE_TOOL 8
#define V_RESEAU_APP 9
#define V_PLOT_APP 10
#define V_BROWSE_APP 11

#define HELP_ON_ITEM    1
#define HELP_ON_QVIEW   2
#define HELP_ON_VERSION 3
#define HELP_ON_HELP    4

#define BYTEDEPTH 4
#define BITMAP_PAD 32

#include "pc2d.p"

#endif
