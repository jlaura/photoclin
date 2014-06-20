      SUBROUTINE RMSCALC(V,N,VRMS)
      IMPLICIT NONE
C
C_TITLE RMSCALC - Find RMS value of an array
C
#include "clinom_ops.inc"
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
	INTEGER*4  N		!I      Number of elements in V
	REAL*4     V(N)		!I      Array whose RMS is to be found
	REAL*4     VRMS		!O	Root Mean Square value of N 
C                                       elements of V.
C
C_DESC  Utility routine: finds the RMS value of a REAL*4 array with 
C       N elements (may be multidimensional in the calling routine).
C
C_HIST  10-AUG-89 Randolph Kirk, USGS Flagstaff, Original Version
C
      INTEGER*4 I
      REAL*4 SUM

      SUM=0.0
      DO I=1,N
         SUM=SUM+V(I)*V(I)
      ENDDO
      VRMS=SQRT(SUM/REAL(N))
      NOPS=NOPS+2*N
      RETURN
      END
