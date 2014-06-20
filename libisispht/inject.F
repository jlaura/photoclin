      SUBROUTINE INJECT(LMAG,FAST,BBIG,ZBIG,EBIG,NXBIG,NYBIG,
     * B,Z,E,AR,AC,BE,DB,A,DL,WK,NX,NY,ERMS,DBRMS,ZRMS,TAURMS,
     * BDER,ERRVAL)
      IMPLICIT NONE
C
C_TITLE	INJECT - Inject solution and righthand side to mesh of 1/2 res
C
#include "clinom_aepar.inc"
#include "clinom_datum.inc"
#include "clinom_ipars.inc"
#include "clinom_ops.inc"
#include "clinom_pgeom.inc"
#include "clinom_ppars.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	LMAG		!I	Log base two of new 
C						resolution at which to work
	LOGICAL*1	FAST		!I	Do fast injection of image?
	INTEGER*4	NXBIG,NYBIG	!I	Sample, line dimensions on
C						fine mesh
	REAL*4		BBIG(NXBIG,NYBIG)!I	Brightness image on fine mesh
	REAL*4		ZBIG(NXBIG+1,NYBIG+1)!I	Topographic array on fine mesh
	REAL*4		EBIG(NXBIG+1,NYBIG+1)!I	Gradient vector on fine mesh
	INTEGER*4	NX,NY		!I	Sample, line dimensions on
C						coarse mesh (1/2 as big)
	REAL*4		B(NX,NY)	!O	Brightness image on coarse
C						mesh: constructed so that
C						E values will be close to 
C						alternate line and sample
C						values of EBIG
	REAL*4		Z(NX+1,NY+1)	!O	Topographic array on coarse 
C						mesh:  alternate lines and 
C						samples of ZBIG
	REAL*4		E(NX+1,NY+1)	!O	Gradient vector on coarse
C						mesh (also internal scratch)
	REAL*4		AR(2,NX)	!-	Matrix to be inverted on rows
	REAL*4		AC(2,NY)	!-	Matrix to be inverted on cols
	REAL*4		BE(NX,NY)	!-	RHS of equations for B
	REAL*4		DB(NX,NY)	!-	Basis vector for conjugate
C				  		gradient method
	REAL*4		A(5,NX+1,NY+1)	!O	Matrix for photoclinometry
C						eqns on coarse mesh (also
C						internal scratch)
	REAL*4		DL(5,NX,NY)	!-	Incomplete Cholesky 
C						decomposition of A
	REAL*4		WK(5,NX,NY)	!-	Work vector for conjugate
C				  		gradient method
	REAL*4		ERMS		!O	RMS value of E
	REAL*4		DBRMS		!O	RMS value of difference betw
C						actual, model brightness
	REAL*4		ZRMS		!O	RMS value of Z
	REAL*4		TAURMS		!O	RMS difference of E and
C						collocated samples of EBIG;
C						a measure of truncation error
	EXTERNAL	BDER		!I	Brightness fn and derivs
	INTEGER*4	ERRVAL		!	+ve=error state
C						  0=no error
C
C_DESC	"Injects" or calculates from the fine-mesh quantities the arrays
C	necessary to work on "the same" clinometry program on a
C	coarse mesh with half the resolution.
C	 (1) Z is injected, i.e., its values are the values of the
C	     coarse mesh at corresponding locations (every other
C	     line and sample.  In practice, Z values are divided
C	     by 2 so that they are expressed in units of pixel
C	     widths on their mesh, and the same photometric fn
C	     routine can be used.
C	 (2) B is defined IMPLICITLY by the requirement that the
C	     gradient vector which forms the RHS of the photoclino-
C	     metry equation be injected in the same way as described
C	     for B above.
C	 (2') Modified program to allow choice of "fast" injection
C	     of B by simply averaging pixels 2x2 if desired.
C	We could simply inject to get E from EBIG, but we would not
C	be able to update E when Z changes without knowing B.
C	To get B from E, we must invert a large sparse matrix---in
C	fact, a matrix with the same sparseness pattern as that
C	in the linearized photoclinometry problem itself!  This is
C	attempted as follows:
C	  (1) Get a first guess at B by solving the SSIPSF set of
C	      equations which would obtain if the topography did
C	      not deviate from the datum.  This may require rotating
C	      coordinates as for SSIPSF-PI photoclinometry.  Note
C	      the solution in this case is by least-squares pseudo-
C	      inverse, since E overdetermines B.
C	  (2) Iteratively solve the equation using the matrix derived
C	      from the true topography
C	       (a) Attempt incomplete Cholesky decomposition of the
C	           matrix.
C	       (b) If this succeeds, do conjugate gradient iteration
C	           for B, using the incomplete Cholesky factorization.
C		   If this succeeds it will likely be EXTREMELY fast
C		   to converge.
C	       (c) If the factorization fails, OR if CGM iteration
C		   does not work, solve the equations by successive
C	           over-relaxation (SOR) iteration.  This is likely
C	           to be slow.
C	       (d) If SOR is too slow, give up
C	In general, the smoother the topography, the quicker things
C	work.
C	Final note:  In actuality, we determine from E the difference
C	between B and the brightnesses computed from Z.  The latter
C	are computed and added to get the "observed" B as a last step.
C
C_CALLS	AE1APO,AE1CALC,AE4CALC,BDER,GEOMSET,INJ0,TRANSP (CLINOM.OLB)
C	S9CGM,S9ICHOL,S9RELAX,S9RHSDEC (S9FAST)
C	ERRMES,R2R (PICS)
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C	21-Jul-02 RLK Add IOCT to S9RELAX call to control SOR/ICCG sweep dir
C       19-Jan-03 RLK Fudge IOCT to give correct swap direction for ICCG
C	03-Apr-06 RLK Add fast option: 2x2 averaging of B
C			Also, eliminated old commented-out code
C
C_PAUSE
C
#include "clinom_specpix.inc"
      REAL*4 PIVTOL		! Fraction of bad pivots tolerated
      PARAMETER (PIVTOL=1.0E-3)	! in S9ICHOL
      REAL*4 IDIVTOL		! Abandon conjugate gradient method
      PARAMETER(IDIVTOL=0.8)	! if residual decreased by no more
C				  than this fraction after NDIV steps
      INTEGER*4 NXBIG1,NYBIG1	! Dimensions of ZBIG
      INTEGER*4 NX1,NY1		! Dimensions of Z
      INTEGER*4 NB		! # of brightness values (elements)
      INTEGER*4 NZ		! # of topography values (nodes)
      REAL*4 WSAVE		! Save value of a priori weight WEIGHT0
C				  which is temporarily turned off
      REAL*4 BERMS0		! Initial RMS residual in seeking B
      REAL*4 BERMS		! Current RMS residual in seeking B
      REAL*4 TMP		! Temporary, for swapping geometric qtys
      INTEGER*4 ITMP		! Temporary, for swapping image dimensions
      INTEGER*4 INDEX		! Index to 1 or 2 photometric functions
      INTEGER*4 IOCTI           ! Value of IOCT that gives best sweep
C                                 direction for ICCG injection, not same
C                                 as for SOR
      INTEGER*4 IBAD		! # of bad pivots encountered in doing
C				  incomplete Cholesky decomposition of A
      INTEGER*4 MODE		! 1=use DL in conjugate gradient method
C				  0=ignore DL and use SOR method
      REAL*4 BETOL		! Final/initial RMS residual in equations
C				  for B that we attempt to reach
      INTEGER*4 IITMAX		! Max # of conjugate gradient steps done
      INTEGER*4 NDIV		! Test for conjugate gradient divergence
C				  after this many steps
      INTEGER*4 NI		! # of conjugate gradient/SOR steps so far
      REAL*4 W			! "Over"relaxation weight for SOR
      REAL*4 BERMSMIN		! Smallest RMS residual achieved
      INTEGER*4 INIT		! Flag to reinitialize conjugate gradients
      REAL*4 BRMS		! RMS value of B after an iteration step
      REAL*4 DBMAX		! Max local change in B
      INTEGER*4 I,J		! Loop counters (indices in B, E)
      INTEGER*4 IBIG,IBIGP	! 2I-1, 2I (indices in BBIG, EBIG)
      INTEGER*4 JBIG,JBIGP	! 2J-1, 2J (indices in BBIG, EBIG)
      REAL*4 DZ1,DZ2		! Diagonal elevation differences across
C				  the pixel
      REAL*4 BEST		! Estimated brightness of the pixel
      REAL*4 DB1,DB2		! 1st derivatives of brightness with diagonal
C				  slopes
      REAL*4 DE			! Local error in injecting gradient vector
      REAL*8 SUM		! Double precision place to accumulate the
C				  statistic TAURMS
      CHARACTER ERRKEY*17       ! Keyword value for error message
      CHARACTER ERRMSG*80       ! Error message
C
      ERRVAL=0
      NXBIG1=NXBIG+1
      NYBIG1=NYBIG+1
      NB=NX*NY
      NX1=NX+1
      NY1=NY+1
      NZ=NX1*NY1
C Explicitly inject Z and temporarily remove any a priori topographic
C constraints from E
      CALL INJ0(ZBIG,NXBIG1,NYBIG1,Z,NX1,NY1,0.5)
      WSAVE=WEIGHT0
      IF (WEIGHT0.NE.0.0) THEN
         WEIGHT0=-WEIGHT0
         CALL AE1APO(ZBIG,EBIG,A,NXBIG,NYBIG,0)
         WEIGHT0=0.0
      END IF
      CALL GEOMSET(REAL(2**LMAG))
      IF (FAST) THEN
C Do "fast" injection by 2x2 averaging of image B
         DO J=1,NY
            JBIGP=J+J
            JBIG=JBIGP-1
            DO I=1,NX
               IBIGP=I+I
               IBIG=IBIGP-1
               IF ((BBIG(IBIG,JBIG).LT.VALID_MIN4.OR.
     *              BBIG(IBIG,JBIG).GT.VALID_MAX4).OR.
     *             (BBIG(IBIGP,JBIG).LT.VALID_MIN4.OR.
     *              BBIG(IBIGP,JBIG).GT.VALID_MAX4).OR.
     *             (BBIG(IBIG,JBIGP).LT.VALID_MIN4.OR.
     *              BBIG(IBIG,JBIGP).GT.VALID_MAX4).OR.
     *             (BBIG(IBIGP,JBIGP).LT.VALID_MIN4.OR.
     *              BBIG(IBIGP,JBIGP).GT.VALID_MAX4)) THEN
                  B(I,J)=NULL4
               ELSE
                  B(I,J)=(BBIG(IBIG ,JBIG )+BBIG(IBIGP,JBIG )
     *                   +BBIG(IBIG ,JBIGP)+BBIG(IBIGP,JBIGP))*0.25
               END IF
            ENDDO
         ENDDO
      ELSE
C Do original "slow" injection by solving implicitly for B
C Start by explicitly injecting E
         CALL INJ0(EBIG,NXBIG1,NYBIG1,E,NX1,NY1,1.0)
         CALL U_FILL4(0.0,NB,B)
C Code to flop Z, E is no longer needed because we control ICCG sweep
C direction with IOCT rather than transposing image and DEM.
C Construct righthand side and matrix
         CALL AE4CALC(Z,E,BE,A,NX,NY,BERMS0,BDER)
         BERMS=BERMS0
C Attempt incomplete Cholesky decomposition of the matrix.
         IF (IOCT.EQ.-2) THEN
            IOCTI = -1
         ELSEIF (IOCT.EQ.-1) THEN
            IOCTI = -2
         ELSEIF (IOCT.EQ.2) THEN
            IOCTI = 1
         ELSE
            IOCTI = 2
         ENDIF
         CALL S9ICHOL(A,DL,NX,NY,IBAD,IOCTI)
C         PRINT*,'IBAD = ',IBAD
         IF (FLOAT(IBAD).LT.FLOAT(NB)*PIVTOL) THEN
C IC decomposition succeeded.  Prepare to do CGM using the decomposition.
C Expect to attain a very accurate solution in reasonable time.
            MODE=1
C            PRINT*,'USING DECOMPOSITION.'
C            BETOL=1.0E-6
C            BETOL=3.0E-3
            BETOL=1.0E-4
         ELSE
C IC decomposition failed.  Prepare to do SOR and don't ask for such
C high accuracy, as the method is slow.  
            MODE=0
C            PRINT*,'ABANDONING DECOMPOSITION.'
            BETOL=3.0E-3
         END IF
         IITMAX=MIN(NX,NY)*8
         NDIV=MIN(IITMAX-1,MAX(10,IITMAX/128))
C Loop over CGM/SOR steps
   10    CONTINUE
C         TYPE *
         NI=0
         W=1.0
         BERMSMIN=BERMS
         DO WHILE ((NI.LE.1).OR.
     *    ((NI.LT.IITMAX).AND.(BERMS.GT.BERMS0*BETOL)))
            NI=NI+1
            INIT=NI
            IF (MODE.EQ.1) THEN
               CALL S9CGM(BE,B,DB,A,DL,WK,NX,NY,
     *         BERMS,BRMS,DBRMS,DBMAX,MODE,INIT,IOCTI)
            ELSE
               CALL S9RELAX(BE,B,DB,WK,A,NX,NY,
     *         W,BERMS,BRMS,DBRMS,DBMAX,IOCT)		!WK a dummy here
            END IF
C            PRINT 1010,NI,DBRMS,BERMS/BERMS0
 1010       FORMAT('+ITERATION ',I4,':  DBRMS=',1PG14.7,', BERMS=',
     *      G14.7)
            BERMSMIN=MIN(BERMSMIN,BERMS)
            IF ((MODE.EQ.0).AND.(BERMS.GT.BERMSMIN*2.0)) GO TO 9001
C SOR failed to converge in reasonable time.  Things look hopeless.
            IF ((MODE.EQ.1).AND.(NI.GT.NDIV).AND.
     *       (BERMS.GT.BERMS0*IDIVTOL)) THEN
C CGM diverged.  Try SOR instead.
               MODE=0
C               PRINT*,'ABANDONING DECOMPOSITION AND STARTING OVER.'
               BETOL=3.0E-3
               GO TO 10
            END IF
         ENDDO
         IF (MODE.EQ.1) THEN
         END IF
C Above calculation gave the ERROR in model brightness at coarse
C resolution.  Add in the model to get the "observed" brightness.
         DO J=1,NY
            JBIGP=J+J
            JBIG=JBIGP-1
            DO I=1,NX
               IBIGP=I+I
               IBIG=IBIGP-1
               IF ((BBIG(IBIG,JBIG).LT.VALID_MIN4.OR.
     *              BBIG(IBIG,JBIG).GT.VALID_MAX4).OR.
     *             (BBIG(IBIGP,JBIG).LT.VALID_MIN4.OR.
     *              BBIG(IBIGP,JBIG).GT.VALID_MAX4).OR.
     *             (BBIG(IBIG,JBIGP).LT.VALID_MIN4.OR.
     *              BBIG(IBIG,JBIGP).GT.VALID_MAX4).OR.
     *             (BBIG(IBIGP,JBIGP).LT.VALID_MIN4.OR.
     *              BBIG(IBIGP,JBIGP).GT.VALID_MAX4)) THEN
                  B(I,J)=NULL4
               ELSE
                  DZ1=Z(I  ,J  )-Z(I+1,J+1)
                  DZ2=Z(I  ,J+1)-Z(I+1,J  )
                  CALL BDER(I,J,DZ1,DZ2,BEST,DB1,DB2,0)
                  IF (BEST.LE.0.0) THEN
                     B(I,J)=0.0
                  ELSE
                     B(I,J)=B(I,J)/CONTRAST+BEST
                  END IF
               END IF
            ENDDO
         ENDDO
      END IF
C Done calculating B, by whichever means
C Calculate the gradient vector, Hessian matrix, and statistics
C If there are a priori constraints we first calculate the gradient
C vector without them to get the truncation error estimate
      IF (WSAVE.EQ.0.0) THEN
         MODE=1
      ELSE
         MODE=0
      END IF
      CALL AE1CALC(B,Z,E,A,NX,NY,ERMS,DBRMS,ZRMS,MODE,BDER)
C Compare the gradient vector with the high resolution one to get
C an estimate of truncation error
      SUM=0.0D0
      DO J=1,NY+1
         JBIG=J+J-1
         DO I=1,NX+1
            IBIG=I+I-1
            DE=E(I,J)-EBIG(IBIG,JBIG)
            SUM=SUM+DBLE(DE)*DBLE(DE)
         ENDDO
      ENDDO
C Now calculate A and E including the a priori constraints, if any
      IF (WSAVE.NE.0.0) THEN
         WEIGHT0=WSAVE
         CALL AE1CALC(B,Z,E,A,NX,NY,ERMS,DBRMS,ZRMS,1,BDER)
      END IF
      TAURMS=SQRT(SNGL(SUM)/REAL(NZ))/CONTRAST
      IF (.NOT.LOGIMG) TAURMS=TAURMS/(BNORM*BNORM)
      NOPS=NOPS+4*NB+3*NZ
      RETURN
C
C Handle processing errors
C
9001  CONTINUE
      ERRKEY = 'INJECT-SORDIV'
      ERRMSG = 'SOR iteration diverged'
      ERRVAL = -1
      GOTO 9999
C
C Write an error message
C
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,1)
      RETURN
      END
