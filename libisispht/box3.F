      SUBROUTINE BOX3(Z,WORK,NX,NY)
      IMPLICIT NONE
C
C_TITLE BOX3 - Convolves an array with a 3x3 boxcar
C
#include "clinom_ops.inc"
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
        INTEGER*4  NX,NY	!I	Dimensions of Z
        REAL*4     Z(NX,NY)	!I/O    Array which gets smoothed
        REAL*4     WORK(NX,NY)	!I	Work space
C
C_DESC  Utility routine: convolves the array Z with a 3x3 boxcar (or 
C       as much of the boxcar as lies within the limits of the array 
C       for a given point). Note: WORK also contains the smoothed
C       array on output.
C 
C_HIST  10-AUG-89 Randolph Kirk, USGS Flagstaff, Original Version
C
      INTEGER*4 I,J,II,JJ,IMIN,IMAX,JMIN,JMAX,JM,JP,IM,IP
      REAL*4 ZIJ,WY

C Convolve and hold the result in WORK
      DO J=1,NY
         JM=J-1
         JP=J+1
         JMIN=MAX(1,JM)
         JMAX=MIN(JP,NY)
         WY=1.0/REAL(JMAX-JMIN+1)
         DO I=1,NX
            IM=I-1
            IP=I+1
            IMIN=MAX(1,IM)
            IMAX=MIN(IP,NX)
            ZIJ=0.0
            DO JJ=JMIN,JMAX
               DO II=IMIN,IMAX
                  ZIJ=ZIJ+Z(II,JJ)
               ENDDO
            ENDDO
            WORK(I,J)=ZIJ*WY/REAL(IMAX-IMIN+1)
         ENDDO
      ENDDO
C Copy the result back into Z
C     CALL COPY(WORK,Z,NX*NY)
      CALL U_MOVE4(NX*NY,WORK,Z)
      NOPS=NOPS+11*NX*NY-6*(NX+NY)+4
      RETURN
      END
