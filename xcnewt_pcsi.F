      SUBROUTINE XCNEWT_PCSI(B,Z,E,DZ,DDZ,A,DL,WK,ABAND,ALOW,NX,NY,
     * NI,LMAG,AEKNOWN,ERMS,DBRMS,ZRMS,METHOD,BDER,ERRVAL)
      IMPLICIT NONE
C
C_TITLE	XCNEWT_PCSI - Take one Newton-Raphson step for clinometry
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		B(NX,NY)	!I	Brightness image
	REAL*4		Z(NX+1,NY+1)	!B	Topographic array
	REAL*4		E(NX+1,NY+1)	!B	Gradient vector (RHS
C						of the linearized equations 
C						for increments to Z)
	REAL*4		DZ(NX+1,NY+1)	!-	Increment to DZ for one
C						Newton-Raphson linearization
	REAL*4		DDZ(NX+1,NY+1)	!-	Increment to DDZ for one
C						SOR or CGM iteration
	REAL*4		A(5,NX+1,NY+1)	!B	Matrix for the linearized
C						equations for increments to Z
	REAL*4		DL(5,NX+1,NY+1)	!-	Incomplete Cholesky factor-
C						ization of A (used to speed
C						CGM iteration)
	REAL*4		WK(NX+1,NY+1)	!-	Work space for CGM iteration
	REAL*4		ABAND(NX+1,NY+1,2*NX+5)	! Matrix for direct factorizn
	REAL*4		ALOW(NX+1,NY+1,NX+2)	! Lower band for direct 
	INTEGER*4	NI		!B	# of this Newton-Raphson step
	INTEGER*4	LMAG		!I	Log base 2 mesh spacing at 
C						which this step is being 
C						taken, relative to full
C						image resolution
	LOGICAL*1	AEKNOWN		!B	Have A, E already been com-
C						puted for this Z?
	REAL*4		ERMS		!B	RMS value of the residual E
	REAL*4		DBRMS		!B	RMS value of the error in
C						the model brightnesses
	REAL*4		ZRMS		!B	RMS value of the elevations
	CHARACTER	METHOD*(*)	!I	Iterative method to use in
C						solving the linearized eqns:
C						CGM=conjugate gradient, or
C						SOR=successive over-relaxation
C						DIR=direct factorization
	EXTERNAL	BDER		!I	Brightness fn and derivs
	INTEGER*4	ERRVAL		!O	 0=no error
C						-1=SOR/CGM step diverged 
C						   "slightly"
C						+1=fatal divergence
C
C_VARS
#include "clinom_aepar.inc"
#include "clinom_ipars.inc"
#include "clinom_log.inc"
#include "clinom_ops.inc"
C
C_DESC	Takes one Newton-Raphson step in finding the solution to
C	the nonlinear clinometry equations:  linearizes the
C	equations about the current topographic estimate and performs
C	a number of successive over-relaxation (SOR) or conjugate gradient 
C	(CGM) iterations to approximate the solution to the linearized 
C	equations.
C
C	Iteration for the solution to the linearized equations may 
C	sometimes fail to converge if the topography is too rough or the 
C	image is too noisy.  When it detects divergence, this routine
C	uses the solution before the divergent step and sets an error
C	flag so that the calling routine can take action (such as
C	smoothing the solution).  Additional protection against divergence 
C	can be obtained by optional "despiking" (a form of nonlinear 
C	filtration in which only values that deviate from the local average 
C	by more than a threshold amount are changed, being set to the 
C	local average) of the topographic increments before 
C	they are used.  Convergence can also sometimes be improved by 
C	setting the parameter CONTRAST to a value less than 1.  Increments 
C	to the topography are then calculated as if the current estimate 
C	came closer by a factor of CONTRAST to reproducing the observed 
C	image than it in fact does, possibly preventing "overshooting" 
C	of the true solution by the estimate.
C
C_CALLS	AE1CALC (CLINOM.OLB)
C	INC,{S9CGM,S9ICHOL--optional},S9RELAX,S9RHSDEC,S9BAND (S9FAST)
C	FASTFIL32, NULLCOUNT32 (FASTFIL)
C	ERRMES (PICS)
C
C_HIST	02-Mar-91 Randolph Kirk U.S.G.S. Flagstaff Original Version
C		?? Janet Barrett Ported to ISIS/UNIX
C		21-Jul-02 RLK Pass IOCT to control SOR/ICCG sweep dir
C		26-Jul-02 RLK Add variable penalty number
C				Add code for direct factorization
C
C_PAUSE
C
      LOGICAL*1 DOCGM		! Use conjugate gradient method?
      LOGICAL*1 DODIR		! Use direct factorization?
      REAL*4 FMAG		! Mesh spacing
      REAL*4 WFAC		! Factor for increasing SOR weight W
C				  asymptotically to WMAX
      INTEGER*4 NB		! Number of elements in B
      INTEGER*4 NX1,NY1		! Sample, line dimensions of Z
      INTEGER*4 NZ		! Number of nodes in Z
      REAL*4 FOPS		! Millions of floating ops so far
      REAL*4 W			! Weight for SOR
      REAL*4 WRAT		! Ratio of two successive values of W
      INTEGER*4 IBAD		! # of bad pivots encountered in incomplete
C				  Cholesky decomposition
      INTEGER*4 NII		! # of this SOR/CGM step
      REAL*4 DDZRMS		! RMS of SOR/CGM increment to DZ
      REAL*4 WOLD		! Previous value of W
      REAL*4 DDZRMSOLD		! Previous value of DDZRMS
      REAL*4 ERMSL		! RMS residual after this SOR/CGM step
      REAL*4 DZRMS		! RMS of Newton-Raphson increment to Z
      REAL*4 DZMAX              ! Maximum value of DZ over all nodes
      REAL*4 DDZMAX		! Maximum value of DDZ over all nodes
      REAL*4 DUM1,DUM2,DUM3	! Junk
      REAL*4 DDZRAT		! Fraction of DZ from this DDZ
      REAL*4 XFRC		! Fraction of non-null nodes in box
C				  required for despike filter to work
      REAL*4 XMIN		! As XFRC, but *number* of non-nulls
      REAL*4 NORM,XADB,XMLT	! Filter parameters not used in despiking
      REAL*4 TOL		! Absolute deviation of node value from
C				  box average that will result in despiking
C				  (c.f. STD, which is deviation in sigmas)
      INTEGER*4 RET		! Error flag for FASTFIL32
      INTEGER*4 NNULL		! # of null-flagged nodes left in DZ
      REAL*4 R			! Line search parameter (not used)
      CHARACTER ERRKEY*17       ! Keyword value for error message
      CHARACTER ERRMSG*80       ! Error message
C
      ERRVAL=0
      DOCGM=(METHOD(1:1).EQ.'C')
      DODIR=(METHOD(1:1).EQ.'D')
      FMAG=REAL(2**LMAG)
      IF (WMAX.GT.1) WFAC=0.25*(1.0-(2.0/WMAX-1.0)**2.0)
      NB=NX*NY
      NX1=NX+1
      NY1=NY+1
      NZ=NX1*NY1
      IF (.NOT.AEKNOWN) THEN	!Calculate matrix, RHS if not yet known
         CALL AE1CALC(B,Z,E,A,NX,NY,ERMS,DBRMS,ZRMS,1,BDER)
         AEKNOWN=.TRUE.
         FOPS=NOPS*1.0E-6
      END IF
      IF ((.NOT.AEKNOWN).OR.(NI.EQ.0)) THEN
         IF (LOGIT) WRITE (IULOG,
     *    '(/,I3,1X,F9.4,1X,I2,2X,F8.5,1X,F17.8,1X,F15.8,1X,F13.6)',
     *    ERR=3000) NI,FOPS,INT(FMAG),CONTRAST,ERMS,DBRMS,
     *    ZRMS*FMAG/ASPECT
 3000    CONTINUE
      END IF
      NI=NI+1
      DZRMS=0.0
      DDZRMS=0.0
      IF (DODIR) THEN
C DO DIRECT (BUT BANDED) LU-DECOMPOSITION SOLUTION OF TANGENT EQUATION
         CALL S9BAND(E,DZ,DDZ,A,ABAND,ALOW,NX1,NY1,DZRMS)
      ELSE
         W=1.0
         WRAT=1.0
         IF (WMAX.LT.1.0) W=WMAX
         IF (DOCGM) CALL S9ICHOL(A,DL,NX1,NY1,IBAD,IOCT)
         NII=0
         DDZRMS=0.0
         CALL U_FILL4(0.0,NZ,DZ)
C Start loop for SOR/CGM solution to tangent equation
   10    CONTINUE
            NII=NII+1
            WOLD=W
            IF ((NII.GT.1).AND.(WMAX.GT.1.0)) W=1.0/(1.0-WFAC*W)
            DDZRMSOLD=DDZRMS
            IF (DOCGM) THEN
               CALL S9CGM(E,DZ,DDZ,A,DL,WK,NX1,NY1,
     *         ERMSL,DZRMS,DDZRMS,DDZMAX,1,NII,IOCT)
            ELSE
               CALL S9RELAX(E,DZ,DDZ,WK,A,NX1,NY1,W,
     *         ERMSL,DZRMS,DDZRMS,DDZMAX,IOCT)
            END IF
            IF (ITMAX.EQ.1) GO TO 20
            IF (NII.LE.1) GO TO 10
            IF (WMAX.GT.1.0) WRAT=(W/WOLD)**2.0
C Test for divergence
            IF ((.NOT.DOCGM).AND.
     *       ((DDZRMS/(1.3*WRAT).GT.DDZRMSOLD).OR.
     *       (DDZMAX.GT.DDZRMS*DIVTOL))) THEN
C Flag divergence with ERRVAL, use all but the last SOR/CGM step,
C and return.
               ERRVAL=-1
               CALL INC(DZ,DDZ,NZ,-1.0,DUM1,DUM2,DUM3,0)
               CALL INC(DDZ,DDZ,NZ,-2.0,DUM1,DUM2,DUM3,0)
               CALL S9RHSDEC(E,A,DDZ,NX1,NY1,ERMSL)
               DDZRMS=DDZRMSOLD
               GO TO 20
            END IF
            DDZRAT=DDZRMS/(DZRMS*DDZTOL)
         IF ((NII.LT.ITMAX).AND.((DDZRAT.GT.2.0).OR.
     *    ((NI.GT.1).AND.(DDZRAT.GT.1.0)))) GO TO 10
   20    CONTINUE
      END IF
C DESPIKE THE INCREMENT TO Z
      IF ((NDS(1).GT.1).AND.(NDS(2).GT.1).AND.
     *           (STD(1).GT.0.0).AND.(STD(2).GT.0.0)) THEN
         XFRC=0.0
         XMIN=REAL(NDS(1))
         TOL=10000.0
         CALL FASTFIL32(DZ,DDZ,E(1,1),E(1,2),E(1,3),NY1,NX1,
     *   NDS(1),NDS(1),XFRC,XMIN,NORM,XADB,XMLT,TOL,STD(1),
     *   'STDZ',RET)
         IF (RET.NE.0) GO TO 9001
         CALL U_MOVE4(NZ,DDZ,DZ)
         CALL NULLCOUNT32(DZ,NZ,NNULL)
         DO WHILE (NNULL.GT.0)
            XFRC=0.0
            XMIN=REAL(NDS(2))
            CALL FASTFIL32(DZ,DDZ,E(1,1),E(1,2),E(1,3),NY1,NX1,
     *      NDS(2),NDS(2),XFRC,XMIN,NORM,XADB,XMLT,TOL,STD(2),
     *      'STD',RET)
            IF (RET.NE.0) GO TO 9001
            CALL U_MOVE4(NZ,DDZ,DZ)
            CALL NULLCOUNT32(DZ,NZ,NNULL)
         ENDDO
C A LINE-SEARCH FOR OPTIMAL R COULD BE DONE HERE
         R=1.0
         CALL INC(Z,DZ,NZ,R,ZRMS,DZRMS,DZMAX,1)! RECALCULATE DZRMS
      ELSE
C A LINE-SEARCH FOR OPTIMAL R COULD BE DONE HERE
         R=1.0
         CALL INC(Z,DZ,NZ,R,DUM1,DUM2,DUM3,0)  ! DZRMS UNCHANGED
      END IF
C Update penalty number for next iteration
      AL=ALASYM+(AL-ALASYM)*ALRAT
C UPDATE HESSIAN MATRIX AND GRADIENT VECTOR, AS WELL AS STATISTICS
      CALL AE1CALC(B,Z,E,A,NX,NY,ERMS,DBRMS,ZRMS,1,BDER)
      FOPS=NOPS*1.0E-6
      IF (LOGIT) WRITE (IULOG,
     * '(I3,1X,F9.4,1X,I2,2X,F8.5,1X,F17.8,1X,F15.8,1X,F13.6)',
     * ERR=3010) NI,FOPS,INT(FMAG),CONTRAST,ERMS,DBRMS,
     * ZRMS*FMAG/ASPECT
 3010 CONTINUE
      RETURN
C
C Handle processing errors
C
9001  CONTINUE
      ERRKEY = 'XCNEWT-FASTFIL32'
      ERRMSG = 'Error in FASTFIL32'
      ERRVAL = -1
      GOTO 9999
C
C Write an error message
C
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,1)
      RETURN
      END
