      SUBROUTINE S9CGM(B,Z,DZ,A,DL,WORK,NX,NY,BRMS,ZRMS,DZRMS,DZMAX,
     * MODE,INIT,IOCT)
      IMPLICIT NONE
C
C_TITLE S9CGM - Conjugate gradient driver routine
C
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
	INTEGER*4  NX,NY	!I	Dimensions of mesh for Z, B
	REAL*4     B(NX,NY)	!I/O	Righthand side (gets updated)
	REAL*4     Z(NX,NY)	!I/O	Solution (gets updated)
	REAL*4     DZ(NX,NY)	!O	Successive basis vectors from
C					which solution is constructed
	REAL*4     A(5,NX,NY)	!I	Matrix, in compressed form
	REAL*4     DL(NX,NY)	!I	Approximate factorization of A
C					calculated by S9ICHOL
	REAL*4     WORK(NX,NY)	!O	Matrix times basis vector: [A]{DZ}
	REAL*4     BRMS		!O	RMS value of updated righthand side
	REAL*4     ZRMS		!O	RMS value of solution
	REAL*4     DZRMS	!O	RMS value of increment
	REAL*4     DZMAX	!O	Maximum absolute value of increment
	INTEGER*4  MODE		!I	Factorization DL is used if MODE=1
	INTEGER*4  INIT		!I	Orthogonalization of increments
C					is restarted if INIT=1
	INTEGER*4  IOCT	!I	Controls sweep direction.  The fwd direction is
C					-2 = +samp -line(inner, outer loops)
C					-1 = -line +samp 
C					+1 = +line +samp
C					+2 = +samp +line
C
C_DESC  Conjugate gradient driver routine: takes one complete step of the
C       conjugate gradient method, with or without the incomplete Cholesky
C       approximate factorization.  (Constructs a basis vector orthogonal
C       to those of previous iterations and decides what multiple of it
C       to add to the solution; also decrements the residual).
C
C       In each case, sweep for back substitution is the reverse of that 
C       above. The IOCT=+2 sweep direction is the old default, and is used 
C       for all other values of IOCT that may be supplied in error.  
C
C       Note: DL is used only for the incomplete Cholesky-conjugate gradient 
C       method.  If MODE<>1 it can be a dummy array rather than one of
C       full size.
C
C_HIST  10-AUG-89 Randolph Kirk, USGS Flagstaff, Original Version
C       25-Jul-02 RLK Now direction in which rows are swept to vary, 
C                 based on IOCT
C
      INTEGER*4 NZ
      REAL*4 RN,RNOLD,RNRAT,DUM1,DUM2,DUM3,RD,R,DBRMS,DBMAX

      NZ=NX*NY
      IF (INIT.EQ.1) THEN
         RN=1.0
C         CALL CLEAR(DZ,NZ)
         CALL U_FILL4(0.0,NZ,DZ)
      END IF
C Calculate numerator of amount of forthcoming basis vector to be added
C to the solution.  Also calculate an intermediate result in WORK.
      RNOLD=RN
      CALL S9RN(B,DL,WORK,NX,NY,RN,MODE,IOCT)
C Construct the basis vector from the intermediate result
      RNRAT=RN/RNOLD
      CALL INC(WORK,DZ,NZ,RNRAT,DUM1,DUM2,DUM3,0)
C Calculate the denominator of the amount of basis vector.  Also calculate
C the corresponding change in the residual.
      CALL S9RD(DZ,A,WORK,NX,NY,RD)
C Increment the solution and decrement the residual by the right amount
      R=RN/RD
C      PRINT*,'RN,RNRAT = ',RN,RNRAT
C      PRINT*,'RD,R = ',RD,R
      CALL INC(Z,DZ,NZ,R,ZRMS,DZRMS,DZMAX,1)
      CALL INC(B,WORK,NZ,-R,BRMS,DBRMS,DBMAX,1)
      RETURN
      END
