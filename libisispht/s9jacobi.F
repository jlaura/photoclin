      SUBROUTINE S9JACOBI(A,DZ,NX,NY)
      IMPLICIT NONE
C
C_TITLE S9JACOBI - Relaxation internal routine
C
#include "clinom_ops.inc"
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
	INTEGER*4  NX,NY	!I	Dimensions of mesh
	REAL*4     A(5,NX,NY)	!I	Matrix, in compressed form
	REAL*4     DZ(NX,NY)	!I/O	Input - Righthand side
C                                       Output - Increment to solution
C
C_DESC  Relaxation internal routine: calculates the increment to the 
C       solution for Jacobi iteration using only the diagonal elements 
C       of A.
C
C_HIST  10-AUG-89 Randolph Kirk, USGS Flagstaff, Original Version
C
      INTEGER*4 I,J

      DO J=1,NY
         DO I=1,NX
            IF (A(1,I,J).EQ.0.0) THEN
               DZ(I,J)=0.0
            ELSE
               DZ(I,J)=DZ(I,J)/A(1,I,J)
            END IF
         ENDDO
      ENDDO
      NOPS=NOPS+NX*NY
      RETURN
      END
