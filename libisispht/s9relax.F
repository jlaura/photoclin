      SUBROUTINE S9RELAX(B,Z,DZ,WORK,A,NX,NY,W,BRMS,ZRMS,DZRMS,DZMAX,
     *IOCT)
      IMPLICIT NONE
C
C_TITLE S9RELAX - Relaxation driver routine
C
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
	INTEGER*4  NX,NY	!I	Dimensions of mesh for Z, B
	REAL*4     B(NX,NY)	!I/O	Righthand side (gets updated)
	REAL*4     Z(NX,NY)	!I/O	Solution (gets updated)
	REAL*4     DZ(NX,NY)	!O	Increment to solution
	REAL*4     WORK(NX,NY)	!I	Work space for smoothing
	REAL*4     A(5,NX,NY)	!I	Matrix, in compressed form
	REAL*4     W		!I	Controls type of iteration:
C					W=0 gives Jacobi iteration
C					W<0 gives smoothed Jacobi iteration
C					0<W<1 is underrelaxation 
C					W=1 is Gauss Seidel iteration
C					1<W<2 is overrelaxation (SOR)
C					2=<W is permitted but it won't work
	REAL*4     BRMS		!O	RMS value of updated righthand side
	REAL*4     ZRMS		!O	RMS value of solution
	REAL*4     DZRMS	!O	RMS value of increment
	REAL*4     DZMAX	!O	Maximum absolute value of increment
	INTEGER*4  IOCT	!I	Controls SOR sweep direction
C					-2 = -line +samp (inner, outer loops)
C					-1 = +samp -line
C					+1 = +samp +line
C					+2 = +line + samp
C                               The IOCT=+1 sweep direction is the old 
C                               default, and is used for all other values of
C                               IOCT that may be supplied in error.
C
C_DESC  Relaxation driver routine: takes one complete step of SOR or Jacobi 
C       iteration. Note: WORK is used only for Jacobi iteration with smoothing. 
C       If 0=<W it can be a dummy array rather than one of full size.
C
C_HIST  10-AUG-89 Randolph Kirk, USGS Flagstaff, Original Version
C       21 Jul 2002 RLK Added IOCT parameter controlling SOR sweep direction
C
      INTEGER*4 NZ

      NZ=NX*NY
C Solve for increment to solution
C      CALL COPY(B,DZ,NZ)	! Use if R2R is not available
      CALL U_MOVE4(NZ,B,DZ)
      IF (W.LE.0.0) THEN
         CALL S9JACOBI(A,DZ,NX,NY)
      ELSE
         CALL S9SOR(A,DZ,NX,NY,W,IOCT)
      END IF
C Increment solution and decrement righthand side
      IF (W.LT.0.0) THEN 
         CALL U_MOVE4(NZ,DZ,WORK)
         CALL BOX3(WORK,DZ,NX,NY)
         CALL INC(Z,DZ,NZ,1.0,ZRMS,DZRMS,DZMAX,1)
         CALL S9RHSDEC(B,A,DZ,NX,NY,BRMS)
cC Smoothed Jacobi case is tricky; must save old Z and use
cC (new Z - old Z) rather than DZ to decrement B
cC         CALL COPY(Z,WORK,NZ)
c         CALL R2R(Z,WORK,NZ)
c         CALL INC(Z,DZ,NZ,1.0,ZRMS,DZRMS,DZMAX,1)
cC         CALL COPY(WORK,DZ,NZ)
c         CALL R2R(WORK,DZ,NZ)
c         CALL BOX3(Z,WORK,NX,NY)
c         CALL INC(WORK,DZ,NZ,-1.0,DUM1,DUM2,DUM3,0)
c         CALL S9RHSDEC(B,A,WORK,NX,NY,BRMS)
      ELSE
C Unsmoothed case is much simpler
         CALL INC(Z,DZ,NZ,1.0,ZRMS,DZRMS,DZMAX,1)
         CALL S9RHSDEC(B,A,DZ,NX,NY,BRMS)
      END IF
      RETURN
      END
