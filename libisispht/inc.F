      SUBROUTINE INC(V,DV,N,R,VRMS,DVRMS,DVMAX,MODE)
      IMPLICIT NONE
C
C_TITLE INC - Increments an array and collects statistics
C
#include "clinom_ops.inc"
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
	INTEGER*4  N		!I      Number of elements in V, DV
	REAL*4     V(N)	        !I/O	Array to be incremented
	REAL*4     DV(N)	!I      Increment to be added
	REAL*4     R	        !I	Multiple of DV to be added to V
	REAL*4     VRMS	        !O	RMS value of incremented V
	REAL*4     DVRMS	!O      RMS value of DV
	REAL*4     DVMAX	!O	Maximum absolute value of DV
	INTEGER*4  MODE		!I	VRMS, DVRMS, DVMAX are only 
C				        calculated if MODE=1
C
C_DESC  Utility routine: increments the array V by a multiple R of 
C       the array DV and if MODE=1 collects statistics.  V, DV may of 
C       course be multi-dimensional in the calling routine as long as 
C       the correct total number of elements is passed.
C
C_HIST  10-AUG-89 Randolph Kirk, USGS Flagstaff, Original Version
C
      INTEGER*4 I
      REAL*4 DVI

      IF (MODE.EQ.1) DVMAX=0.0
      DO I=1,N
         DVI=DV(I)*R
         V(I)=V(I)+DVI
         IF (MODE.EQ.1) DVMAX=MAX(DVMAX,ABS(DVI))
      ENDDO
      IF (MODE.EQ.1) THEN
         CALL RMSCALC(V,N,VRMS)
         CALL RMSCALC(DV,N,DVRMS)
         DVRMS=DVRMS*ABS(R)
         DVMAX=DVMAX*ABS(R)
      END IF
      NOPS=NOPS+2*N
      RETURN
      END
