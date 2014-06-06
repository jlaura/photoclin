#include <stdio.h>
#include <stdlib.h>
#define DECLARE_GLOBALS
#include "pcsi.h"
#include "isismain.h"
#include "isistypes.h"
#include "isismain_c.h"
#include "clinom_ppars.h"

#define PCSI main

int PCSI (int argc,char *argv[])

/*****************************************************************************
* 
*_Title PCSI - Two dimensional photoclinometry (smart interpolation)
*
*_Desc	Performs two-dimensional monoscopic or biscopic photoclinometry
*	using the finite-element method of R. L. Kirk.  Space precludes
*	describing the method in any detail here; the interested user
*	is directed to "III. A Fast Finite-Element Algorithm for Two-
*	dimensional Photoclinometry,"  Ph.D. Thesis (unpubl.), Caltech,
*	pp. 165-258, 1987, by R. L. Kirk.   Additional information is
*	also given in the C_DESC sections of the subroutines below.
*
*	A description of the program given "while standing on one leg"
*	is as follows:  An image (or two registered images of the same
*	region) is input.  The program iteratively estimates the shape
*	of the topographic surface that would come closest (in a least
*	squares sense) to producing such an image (or two images which 
*	have this ratio; arbitrary albedo variations could be present
*	but cancel out in the ratio) under the viewing geometry described 
*	in the image label(s).  The estimated topographic model is
*	then scaled and output as a PICS image.
*
*	The process by which the solution is obtained is doubly iterative:
*	First, the nonlinear equations for the surface that best fits the
*	image(s) are repeatedly linearized and solved for increments to
*	the topography that improve the fit.  Second, each linearized
*	version of the equations is solved by iterative means (direct
*	methods of solution are completely impractical for the large
*	systems of equations engendered by even a modest-sized image).
*	Convergence of the iterative solution is greatly speeded by
*	"multigridding:" working on approximations of the same problem
*	at different resolutions in order to arrive at the different 
*	spatial frequency components of the solution simultaneously.
*	
*	The program parameters divide broadly into three groups:
*	  (1) Input/output file names (may include an initial guess
*	      at the topography, and a log file documenting the iterative
*	      solution process).
*	  (2) Description of surface & atmospheric scattering properties.
*	  (3) Parameters affecting the iteration process.
*
*       File parameters:
*
*	FROM(1), FROM(2) (the latter is optional)---16-bit PICS brightness
*	   image file(s).  Read by DOIO in ISTEP=3, IPASS=1.
*	TO---16-bit PICS file containing digital topographic model registered
*	   pixel-to-pixel to FROM, scaled, and with datum tilt subtracted.
*	   Written by DOIO in ISTEP=3, IPASS=2.
*	LOGFILE (optional)---Text file containing list of parameters
*	   and history of the photoclinometry iteration process.  Opened,
*	   header written to and closed here.  Iteration history written
*	   in subroutines which obtain the logical unit number through
*	   common block /LOG/.
*	ZIN (optional)---32-bit PICS file containing digital topographic
*	   model in "raw" form: elevations at corners of the image pixels,
*	   in units of pixel width, datum subtracted.  Opened/read/
*	   closed with FILEIO in subroutine PCSTO and contents passed
*	   to this routine.
*	ZOUT---32-bit PICS file containing digital topographic model in
*	   "raw" form: elevations at corners of the image pixels, in units 
*	   of pixel width, datum subtracted (suitable for input as ZIN
*	   in a later run of the program).  Opened/updated/closed with
*	   FILEIO in subroutine PCMAIN after each update of the topography.
*
*_Hist	02-Mar-91 Randolph Kirk U.S.G.S. Flagstaff Original Version
*       May 23 2000 Janet Barrett - Ported from VAX to Unix 
*       Mar 19 2003 JB, Added changes needed to make pcsi operate similarly
*                   to pc2d.
*       Jun 11 2004 Kris Becker Reworked how global variables in pcsi.h
*                   were declared to avoid problems with Mac linkage
*
*_End
******************************************************************************/
{
  CHAR errbuf[512];
  INT4 ret;
  INT4 InitPcsi(void);
  INT4 PcsiTopo(void);
/*************************************************************************
* Initialize the program
*************************************************************************/
  (void) u_std_init("PCSI","2004-06-11",
         "Perform two dimensional photoclinometry with smart interpolation");
  (void) u_actime(1,&ret); 
/*************************************************************************
* Get TAE parameters
*************************************************************************/
  C_PPARS1->nps = 1;
  if (InitPcsi()) goto initpcsi_error;
/*************************************************************************
* Do full-up clinometry
*************************************************************************/
  (void) xcmain_pcsi(&cdum[n1],n1+1,nx,ny,dozout,&nsmooth,sordir,&ret);
  if (ret) goto xcmain_pcsi_error;
/*************************************************************************
* Put results in the TO file
*************************************************************************/
  if (PcsiTopo()) goto writetopo_error;
/*************************************************************************
* End of program
*************************************************************************/
  (void) u_free(ibuf);
  (void) u_free(cdum);
  (void) u_actime(2,&ret);
  (void) u_std_exit();
/*************************************************************************
* Handle processing errors
*************************************************************************/
  initpcsi_error:
    (void) sprintf(errbuf,"Error in call to InitPcsi");
    (void) u_error("PCSI-INITPCSI",errbuf,-1,2);
    (void) u_std_exit;

  xcmain_pcsi_error:
    (void) sprintf(errbuf,"Error in call to xcmain_pcsi");
    (void) u_error("PCSI-XCMAINPCSI",errbuf,-2,2);
    (void) u_std_exit;

  writetopo_error:
    (void) sprintf(errbuf,"Error in call to PcsiTopo");
    (void) u_error("PCSI-PCSITOPO",errbuf,-3,2);
    (void) u_std_exit;
    return(-3);
}
