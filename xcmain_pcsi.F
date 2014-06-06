      SUBROUTINE XCMAIN_PCSI(IDUM,N10,NX0,NY0,DOZOUT,NSMOOTH,
     *SORDIR,ERRVAL)
      IMPLICIT NONE
#include "clinom_aepar.inc"
#include "clinom_dnorm.inc"
#include "clinom_ipars.inc"
#include "clinom_log.inc"
#include "clinom_mem.inc"
#include "clinom_ops.inc"
#include "clinom_msg.inc"
C
C_TITLE	XCMAIN_PCSI - Driver routine for two-dimensional clinometry
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	IDUM(1)	        !B	Dummy array containing
C						all arrays to be used
	INTEGER*4	N10		!I	Pointer to beginning of
C						IDUM
        INTEGER*4	NX0,NY0		!I	Sample, line dimensions
        LOGICAL*1       DOZOUT          !I      Determines if ZOUT file will
C                                               be created
        INTEGER*4       NSMOOTH         !B      +ve=3x3 boxcar NSMOOTH
C                                                   times before iteration
C                                               -ve=boxcar until residuals
C                                                   stop decreasing; output
C                                                   -ve (# of passes + 1)
C                                               0=no preliminary boxcar
        INTEGER*4       SORDIR          !I      Determines which solution type 
C                                               is used:
C                                               0 - SOR is used
C                                               1 - DIR is used
C                                               2 - CGM is used
	INTEGER*4	ERRVAL		!O	+ve=error state
C						  0=no error
C
C_DESC	Given an image (or ratio of two images) and an initial guess
C	at the topography, iterates to solve the two-dimensional
C	photoclinometry problem of reconstructing the topography from
C	the image.  
C
C	The basis of the iterative method is to make repeated calls to 
C	XCNEWT, which linearizes the clinometry equations and then 
C	iteratively approximates the solution to the linearized equations.  
C	The result from each such call or "Newton-Raphson step" is an 
C	increment to the topographic estimate that hopefully brings it 
C	closer to the true solution.  The increments must be solved for
C	iteratively because the large size of the dataset makes direct
C	methods of solving matrix equations astronomically impractical 
C	in terms of both storage and computation time.
C
C	Iterative methods of solving the linearized equations
C	suffer from the defect that, while they arrive at the
C	high-spatial-frequency part of the solution rapidly, they
C	are extremely slow to converge for low spatial frequencies.
C	In order to improve convergence (in particular, to make the
C	total computational effort proportional to the number of
C	of pixels, rather than the square thereof) the problem is
C	"multigridded."  Lower-frequency components of the full solution
C	are arrived at by attempting to solve the equivalent problem
C	with half the resolution (twice the pixel size or "mesh spacing").
C	Not only are corresponding points half as many pixels apart,
C	yielding convergence in half the number of iterations, but
C	in addition each iteration only takes one fourth the time.
C	The full speedup of the method is realized when a 1/4 resolution
C	mesh is used to speed convergence at 1/2 resolution, a 1/8
C	resolution mesh to speed convergence at 1/4 resolution, and so on.
C
C	Thus, after each Newton-Raphson step, the routine must decide
C	whether to increase resolution (because the coarser problem
C	has converged, at least relative to the state of the finer),
C	to decrease resolution (because little progress is being made),
C	to keep going at the same resolution, or to decide that the
C	current topographic estimate is acceptably close to the true
C	solution and return.
C
C	Iteration for the solution to the linearized equations may 
C	sometimes fail to converge if the topography is too rough or the 
C	image is too noisy.  This routine attempts to restore convergence
C	in such cases by smoothing the topography with a boxcar filter and 
C	then proceeding.  If success is not had after several passes of the 
C	filter, the routine returns with an error flag.  Additional
C	protection against divergence can be obtained by optional 
C	"despiking" (a form of nonlinear filtration where only values that
C	deviate from the local average by more than a threshold amount
C	are changed, being set to the local average) of the topographic 
C	increments increments before they are used.  Convergence can
C	also sometimes be improved by setting the parameter CONTRAST to
C	a value less than 1.  Increments to the topography are then
C	calculated as if the current estimate came closer by a factor
C	of CONTRAST to reproducing the observed image than it in fact
C	does, possibly preventing "overshooting" of the true solution
C	by the estimate.
C
C_FILES	LOGFILE (optional)---Text file containing list of parameters
C	   and history of the photoclinometry iteration process.  Opened
C	   header written to and closed in DOUSER.  Iteration history 
C	   written in this routine; logical unit number passed through
C	   common block /LOG/.
C	ZOUT---32-bit PICS file containing digital topographic model in
C	   "raw" form: elevations at corners of the image pixels, in units 
C	   of pixel width, datum subtracted (suitable for input as ZIN
C	   in a later run of the program).  Opened/updated/closed with
C	   FILEIO after each update of the topography.  
C
C_CALLS	AE1CALC,INJECT,INTERP,XCNEWT,ZCOPY (CLINOM.OLB)
C	FASTFIL32 (FASTFIL)
C	ERRMES,FILEIO,R2R (PICS)
C
C_HIST	02-Mar-91 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_END
C
      INTEGER*4 MAXLMMAX	! Max # of 2-fold mesh coarsenings
      PARAMETER (MAXLMMAX=10)	! dictated by pointer array size
      INTEGER*4 LMMAX		! Max # of 2-fold mesh coarsenings
C				  to be attempted; more would make mesh
C				  too small
      REAL*4 ERMS(0:MAXLMMAX)	! Latest RMS residual on each mesh
      REAL*4 TAURMS(0:MAXLMMAX)	! Truncation error estimate on each mesh
      INTEGER*4 NX(0:MAXLMMAX)	! # of samples in each mesh
      INTEGER*4 NY(0:MAXLMMAX)	! # of lines in each mesh
      INTEGER*4 N1(0:MAXLMMAX)	! Pointer to beginning of B for each mesh
      INTEGER*4 N2(0:MAXLMMAX)	! Pointer to beginning of Z for each mesh
      INTEGER*4 N3(0:MAXLMMAX)	! Pointer to beginning of E for each mesh
      INTEGER*4 LMAG		! # of 2-fold coarsenings to get to this
C				  mesh from full-resolution; mesh #;
C				  log base 2 of mesh spacing
      INTEGER*4 NB,NX1,NY1,NZ
      INTEGER*4 NI		! # of Newton-Raphson steps on this mesh
      INTEGER*4 NBOX		! # of successive attempts to cure 
C				  divergence by smoothing
      LOGICAL*1 AEKNOWN         ! Have matrix and righthand side been
C                                 calculated for this Z yet?
      INTEGER*4 N4,N5,N6,N7,N8	! Pointers to beginnings of dynamically
      INTEGER*4 N9,N8D,N9D      ! allocated arrays DZ,DDZ,A,DL,WK on the
C				  current mesh
      INTEGER*4 RET		! Error flag for various rtnes
      INTEGER*4 LINE,SAMP	! Box size for FASTFIL32
      REAL*4 XFRC,XMIN		! Fraction, # of valid points in box
C				  required by FASTFIL32
      REAL*4 XMLT		! Multiplier for output of FASTFIL32
      REAL*4 NORM,XADB,TOL,STD0	! FASTFIL32 params not used for lowpass
      REAL*4 ERMSOLD		! RMS residual for previous iteration
C				  on this mesh
      REAL*4 DBRMS,ZRMS		! Statistics generated by AE1CALC
      INTEGER*4 ICONS1,ICPUS1,IDIR1,IFLT1 ! Needed for ACTINF
      INTEGER*4 ICONMS1,ICPUMS1,ISWP1
      INTEGER*4 ICONS,ICPUS,IDIR,IFLT ! Needed for ACTINF
      INTEGER*4 ICONMS,ICPUMS,ISWP
      CHARACTER*32 ERRKEY
      CHARACTER*80 ERRMSG
      CHARACTER*3 METHOD
      CHARACTER TYPELONG*17,TYPESHORT*3
      EXTERNAL PBDER
C
      ERRVAL=0
      IF (SORDIR.EQ.0) METHOD='SOR'
      IF (SORDIR.EQ.1) METHOD='DIR'
      IF (SORDIR.EQ.2) METHOD='CGM'
      IF (MAXNR.EQ.0) RETURN
      CALL U_ACTINF(ICONS1,ICONMS1,ICPUS1,ICPUMS1,IDIR1,IFLT1,ISWP1,RET)
      IF (RET.NE.0) GOTO 9004
C
C Set up full resolution arrays and arrays for sizes/pointers to arrays
C and statistics at the full range of resolutions
C
      LINE=3
      SAMP=3
      XMLT=1.0
      NB=NX0*NY0
      N1(0)=N10
      NX(0)=NX0
      NY(0)=NY0
      NX1=NX0+1
      NY1=NY0+1
      NZ=NX1*NY1
      N2(0)=N1(0)+NB
      N3(0)=N2(0)+NZ
      N4=N3(0)+NZ
      N5=N4+NZ
      N6=N5+NZ
      N7=N6+5*NZ
      N8=N7+5*NZ
      N9=N8+NZ
      N8D=N7+NZ*(2*NX1+3)
      N9D=N8D+NZ*(NX1+1)
      CALL U_FILL4(0.0,MAXLMMAX+1,ERMS(0))
      CALL U_FILL4(0.0,MAXLMMAX+1,TAURMS(0))
      LMMAX=0
      DO WHILE ((NX(LMMAX).GE.32).AND.(NY(LMMAX).GE.32).AND.
     *          (LMMAX.LT.DEPTHLIM).AND.(LMMAX.LT.MAXLMMAX))
         LMMAX=LMMAX+1
         NX(LMMAX)=NX(LMMAX-1)/2
         NY(LMMAX)=NY(LMMAX-1)/2
      ENDDO
      IF (IGUESS.EQ.1) THEN
        TYPELONG='ALTITUDES ZEROED.'
	TYPESHORT='FLT'
      ELSEIF (IGUESS.EQ.2) THEN
        TYPELONG='ALTITUDES SCALED.'
	TYPESHORT='STO'
      ELSE
        TYPELONG='SSIPSF-PI DONE.  '
	TYPESHORT='SEP'
      ENDIF
      LMAG=0
      NI=0
      AEKNOWN=.FALSE.
      NBOX=0
      IF (NSMOOTH.GT.0) THEN
C
C If NSMOOTH > 0, try smoothing the starting solution (which may have a lot 
C of high-freq noise) the fixed number NSMOOTH of times
C
         DO NBOX=1,NSMOOTH
            XFRC=0.0
            XMIN=0.0
            CALL FASTFIL32(IDUM(N2(0)),IDUM(N3(0)),IDUM(N4),IDUM(N5),
     *      IDUM(N6),NY1,NX1,LINE,SAMP,XFRC,XMIN,NORM,XADB,XMLT,
     *      TOL,STD0,'LPF',RET)
            IF (RET.NE.0) GOTO 9001
            CALL U_MOVE4(NX1*NY1,IDUM(N3(0)),IDUM(N2(0)))
         ENDDO
         NBOX=0
      ELSE IF (NSMOOTH.LT.0) THEN
C
C If NSMOOTH < 0, try smoothing the starting solution (which may have a lot 
C of high-freq noise) until this no longer reduces the residuals
C
         NSMOOTH=-1
         CALL AE1CALC(IDUM(N1(0)),IDUM(N2(0)),IDUM(N3(0)),IDUM(N6),
     *   NX0,NY0,ERMS(0),DBRMS,ZRMS,0,PBDER)
   10    CONTINUE
            CALL U_MOVE4(NX1*NY1,IDUM(N2(0)),IDUM(N4))
            ERMSOLD=ERMS(0)
            XFRC=0.0
            XMIN=0.0
            CALL FASTFIL32(IDUM(N4),IDUM(N3(0)),IDUM(N5),IDUM(N6),
     *      IDUM(N7),NY1,NX1,LINE,SAMP,XFRC,XMIN,NORM,XADB,XMLT,TOL,
     *      STD0,'LPF',RET)
            IF (RET.NE.0) GOTO 9001
            CALL U_MOVE4(NX1*NY1,IDUM(N3(0)),IDUM(N4))
            CALL AE1CALC(IDUM(N1(0)),IDUM(N4),IDUM(N3(0)),IDUM(N6),
     *      NX0,NY0,ERMS(0),DBRMS,ZRMS,0,PBDER)
            IF (ERMS(0).LT.ERMSOLD) THEN
               CALL U_MOVE4(NX1*NY1,IDUM(N4),IDUM(N2(0)))
               NSMOOTH=NSMOOTH-1
               GO TO 10
            END IF
      END IF
      ERMSOLD=0
      ERMS(0)=0
C
C Loop, taking Newton-Raphson steps and deciding whether to change
C resolution, quit, abort, or whatever
C
   20 CONTINUE
C
C Test if done
C
         CALL U_ACTINF(ICONS,ICONMS,ICPUS,ICPUMS,IDIR,IFLT,ISWP,RET)
         IF ((AEKNOWN.AND.(LMAG.EQ.0).AND.((ERMS(LMAG).LE.ETOL).OR.
     *   (ERMS(LMAG).LE.TAURMS(LMAG)))).OR.((CPULIMIT.GT.0.0).AND.
     *   (REAL(ICPUS-ICPUS1)/60000.0.GT.CPULIMIT).AND.(RET.EQ.0)).OR.
     *   ((LMAG.EQ.0).AND.(NI.GE.MAXNR))) THEN
            RETURN
C
C Test for (mild) divergence of last N-R step
C
         ELSE IF (RET.NE.0) THEN
            IF (NBOX.GT.2) THEN		! Filtering already failed
               GOTO 9002
            ELSE			! Try filtering the solution
               XFRC=0.0
               XMIN=0.0
               CALL FASTFIL32(IDUM(N2(LMAG)),IDUM(N3(LMAG)),IDUM(N4),
     *         IDUM(N5),IDUM(N6),NY(LMAG)+1,NX(LMAG)+1,LINE,SAMP,XFRC,
     *         XMIN,NORM,XADB,XMLT,TOL,STD0,'LPF',RET)
               IF (RET.NE.0) GOTO 9001
               CALL U_MOVE4((NX(LMAG)+1)*(NY(LMAG)+1),IDUM(N3(LMAG)),
     *         IDUM(N2(LMAG)))
               NBOX=NBOX+1
               AEKNOWN=.FALSE.
               RET=0
            END IF
C
C Test for convergence at this resolution (relative to error in solution
C at higher resolution we're trying to improve, or to truncation error).
C If converged, go to next higher resolution.
C
C         ELSE IF ((AEKNOWN).AND.(LMAG.GT.0).AND.
C     *   ((ERMS(LMAG).LE.ERMS(LMAG-1)*BIGTOL).OR.
C     *   (ERMS(LMAG).LE.TAURMS(LMAG)))) THEN
C
C Interpolate the changes in the coarse topography onto the finer mesh
C
C            CALL INTERP(IDUM(N2(LMAG-1)),NX(LMAG-1)+1,NY(LMAG-1)+1,
C     *      IDUM(N2(LMAG)),NX(LMAG)+1,NY(LMAG)+1,2.0)
C            IF (LMAG.EQ.1) THEN
C               IF (TAURMS(2).NE.0.0) THEN
C                  TAURMS(0)=TAURMS(1)*TAURMS(1)/TAURMS(2)
C               ELSE
C                  TAURMS(0)=TAURMS(1)*2.0
C               END IF
C            END IF
C            NI=0
C            NBOX=0
C
C If injection to coarse mesh just failed we go to here to release the
C coarse mesh arrays and reallocate the fine mesh ones.  We have skipped
C tampering with Z, TAURMS, NI, and NBOX on the fine mesh in this case.
C
C   30       CONTINUE
C
C Increase working resolution and recalculate pointers to the arrays
C needed only at the current resolution, starting just after the last
C of the arrays needed at the current resolution and above
C
C            LMAG=LMAG-1
C            AEKNOWN=.FALSE.
C	    NX1=NX(LMAG)+1
C	    NY1=NY(LMAG)+1
C	    NZ=NX1*NY1
C            N4=N3(LMAG)+NZ
C            N5=N4+NZ
C            N6=N5+NZ
C            N7=N6+5*NZ
C            N8=N7+5*NZ
C	    N9=N8+NZ
C	    N8D=N7+NZ*(2*NX1+3)
C	    N9D=N8D+NZ*(NX1+1)
C
C Test for slowness of iteration at this resolution.  If slow, attempt
C to go to next lower resolution.
C
C         ELSE IF ((AEKNOWN).AND.(NI.GT.1).AND.(LMAG.LT.LMMAX).AND.
C     *   (ERMS(LMAG).GT.ERMSOLD*OLDTOL)) THEN
C
C Change working resolution and calculate pointers to all arrays at the
C new resolution, starting after the last array retained from the previous
C working resolution
C
C            LMAG=LMAG+1
C            AEKNOWN=.FALSE.
C	    NB=NX(LMAG)*NY(LMAG)
C	    NX1=NX(LMAG)+1
C	    NY1=NY(LMAG)+1
C	    NZ=NX1*NY1
C            N1(LMAG)=N3(LMAG-1)+(NX(LMAG-1)+1)*(NY(LMAG-1)+1)
C            N2(LMAG)=N1(LMAG)+NB
C            N3(LMAG)=N2(LMAG)+NZ
C            N4=N3(LMAG)+NZ
C            N5=N4+NZ
C            N6=N5+NZ
C            N7=N6+5*NZ
C            N8=N7+5*NZ
C	    N9=N8+NZ
C	    N8D=N7+NZ*(2*NX1+3)
C	    N9D=N8D+NZ*(NX1+1)
C
C Attempt the injection.
C
C            CALL INJECT(LMAG,IDUM(N1(LMAG-1)),IDUM(N2(LMAG-1)),
C     *      IDUM(N3(LMAG-1)),NX(LMAG-1),NY(LMAG-1),IDUM(N1(LMAG)),
C     *      IDUM(N2(LMAG)),IDUM(N3(LMAG)),IDUM(N4),IDUM(N5),IDUM(N4),
C     *      IDUM(N3(LMAG)),IDUM(N6),IDUM(N7),IDUM(N8),NX(LMAG),NY(LMAG),
C     *      ERMS(LMAG),DBRMS,ZRMS,TAURMS(LMAG),PBDER,RET)
C            IF (RET.NE.0) THEN
C
C Injection failed.  Decrease maximum depth of multigrid recursion so
C we don't try this again, then jump to code for return from coarse
C mesh--in order to release coarse-mesh arrays and reallocate fine-mesh
C
C               RET=0
C               LMMAX=LMAG-1
C               GO TO 30
C            ELSE
C
C Injection succeeded.  All that remains is to initialize NI, NBOX
C for the lower resolution
C
C               NI=0
C               NBOX=0
C               AEKNOWN=.TRUE.
C               IF (LOGIT) WRITE (IULOG,*)
C            END IF
         END IF
C
C If we are at full resolution, update the 'backup version' of the
C topography on disk...just in case
C
         IF ((LMAG.EQ.0).AND.(NI.NE.0).AND.DOZOUT) THEN
	    CALL PCSIZOUT()
         END IF
C
C Take one Newton-Raphson step.
C
         ERMSOLD=ERMS(LMAG)
C         CALL XCNEWT_PCSI(IDUM(N1(LMAG)),IDUM(N2(LMAG)),IDUM(N3(LMAG)),
C     *   IDUM(N4),IDUM(N5),IDUM(N6),IDUM(N7),IDUM(N8),IDUM(N8),IDUM(N8),
C     *   NX(LMAG),NY(LMAG),NI,LMAG,AEKNOWN,ERMS(LMAG),DBRMS,ZRMS,
C     *   METHOD,PBDER,RET)
         IF (METHOD.EQ.'CGM') THEN
	   CALL XCNEWT_PCSI(IDUM(N1(LMAG)),IDUM(N2(LMAG)),
     *IDUM(N3(LMAG)),IDUM(N4),IDUM(N5),IDUM(N6),IDUM(N7),
     *IDUM(N8),IDUM(N8),IDUM(N8),NX(LMAG),NY(LMAG),NI,LMAG,
     *AEKNOWN,ERMS(LMAG),DBRMS,ZRMS,'CGM',PBDER,RET)
         ELSEIF (METHOD.EQ.'DIR') THEN
           CALL XCNEWT_PCSI(IDUM(N1(LMAG)),IDUM(N2(LMAG)),
     *IDUM(N3(LMAG)),IDUM(N4),IDUM(N5),IDUM(N6),IDUM(N6),
     *IDUM(N6),IDUM(N7),IDUM(N8D),NX(LMAG),NY(LMAG),NI,LMAG,
     *AEKNOWN,ERMS(LMAG),DBRMS,ZRMS,'DIR',PBDER,RET)
         ELSE
           CALL XCNEWT_PCSI(IDUM(N1(LMAG)),IDUM(N2(LMAG)),
     *IDUM(N3(LMAG)),IDUM(N4),IDUM(N5),IDUM(N6),IDUM(N6),
     *IDUM(N7),IDUM(N7),IDUM(N7),NX(LMAG),NY(LMAG),NI,LMAG,
     *AEKNOWN,ERMS(LMAG),DBRMS,ZRMS,'SOR',PBDER,RET)
	 ENDIF
         IF (RET.EQ.-1) GOTO 9003! Fatal divergence (not currently used)
	 IF (RET.NE.0) GOTO 9005
         NBOX=0	                ! Success: reset to 0 smoothing attempts
      GO TO 20
C************************************************************************
C Handle processing errors
C************************************************************************
9001  ERRKEY = 'XCMAIN-FASTFIL32'
      ERRMSG = 'Error in FASTFIL32 subroutine'
      ERRVAL = -1
      GOTO 9999

9002  ERRKEY = 'XCMAIN-FAILED'
      ERRMSG = 'Divergence not cured by smoothing'
      ERRVAL = -2
      GOTO 9999

9003  ERRKEY = 'XCMAIN-FATAL'
      ERRMSG = 'Fatal divergence'
      ERRVAL = -3
      GOTO 9999

9004  ERRKEY = 'XCMAIN-U_ACTINF'
      ERRMSG = 'Error in call to U_ACTINF'
      ERRVAL = -4
      GOTO 9999

9005  ERRKEY = 'XCMAIN-XCNEWT_PCSI'
      ERRMSG = 'Error in call to XCNEWT_PCSI'
      ERRVAL = -5
      GOTO 9999
C************************************************************************
C Write an error message
C************************************************************************
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,1)
      RETURN
      END
