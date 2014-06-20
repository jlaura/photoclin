      SUBROUTINE DESTRIPE(Z,WORK,NX,NY)
      IMPLICIT NONE
C
C_TITLE DESTRIPE - Remove stripes from topography with boxcar filters
C
#include "clinom_aepar.inc"
#include "clinom_ops.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		Z(NX+1,NY+1)	!O	Topographic array
	REAL*4		WORK(NX+1,NY+1)	!-	Work space for filtration
C
C_DESC	"Destripes" the SSIPSF-PI topography by lowpass filtering along the
C	characteristic direction, then highpass filtering this across the
C	characteristic direction and subtracting the result from the original
C	topography.  (This is done in the rotated coordinate system that
C	coincides with the characteristic direction.)
C
C_CALLS	None
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      REAL*8 SUMZ1,SUMZ2,SUMD2	! Double precision place to accumulate
C				  statistics on elevations and slopes
      INTEGER*4 I,J		! Loop counters; position in Z
      INTEGER*4 NFH		! Half-length of lowpass filter
      INTEGER*4 II,JJ		! Loop counters for lowpass filter summation
      REAL*4 SUM		! Sum of points for lowpass filter value
      INTEGER*4 N		! # points inside Z contrib to a lowpass
C				  filter value
      INTEGER*4 IM,IP,JM,JP	! Indices of points adjacent to a given
C				  point in WORK, for highpass filtration
C
C Calculate filter width, based on characteristic horizontal lengthscale
C of topography: (RMS topo)/(RMS slope along characteristics)
      SUMZ1=0.0D0
      SUMZ2=0.0D0
      SUMD2=0.0D0
      IF (DBRAT.GT.0) THEN
         DO J=1,NY+1
            DO I=1,NX
               SUMZ1=SUMZ1+DBLE(Z(I,J))
               SUMZ2=SUMZ2+DBLE(Z(I,J)**2.0)
               SUMD2=SUMD2+DBLE((Z(I+1,J)-Z(I,J))**2.0)
            ENDDO
         ENDDO
         NFH=MAX(2,MIN(NINT(0.5D0*
     * SQRT((SUMZ2-SUMZ1*SUMZ1/REAL((NX+1)*(NY+1)))/SUMD2)
     * ),(NX+1)/4))-1
      ELSE
         DO J=1,NY
            DO I=1,NX+1
               SUMZ1=SUMZ1+DBLE(Z(I,J))
               SUMZ2=SUMZ2+DBLE(Z(I,J)**2.0)
               SUMD2=SUMD2+DBLE((Z(I,J+1)-Z(I,J))**2.0)
            ENDDO
         ENDDO
         NFH=MAX(2,MIN(NINT(0.5D0*
     * SQRT((SUMZ2-SUMZ1*SUMZ1/REAL((NX+1)*(NY+1)))/SUMD2)
     * ),(NY+1)/4))-1
      END IF
C Lowpass filter Z along characteristics & put in WORK
      DO J=1,NY+1
         DO I=1,NX+1
            SUM=0.0
            N=0
            IF (DBRAT.GT.0) THEN
               DO II=MAX(1,I-NFH),MIN(I+NFH,NX+1)
                  SUM=SUM+Z(II,J)
                  N=N+1
               ENDDO
            ELSE
               DO JJ=MAX(1,J-NFH),MIN(J+NFH,NY+1)
                  SUM=SUM+Z(I,JJ)
                  N=N+1
               ENDDO
            END IF
            WORK(I,J)=SUM/REAL(N)
         ENDDO
      ENDDO
C Highpass filter WORK across characteristics & subtract from Z
      DO J=1,NY+1
         JM=J-1
         JP=J+1
         IF (J.EQ.1) JM=J+1
         IF (J.EQ.NY+1) JP=J-1
         DO I=1,NX+1
            IM=I-1
            IP=I+1
            IF (I.EQ.1) IM=I+1
            IF (I.EQ.NX+1) IP=I-1
            IF (DBRAT.GT.0) THEN
               Z(I,J)=Z(I,J)-0.5*WORK(I,J)
     *                      +0.25*(WORK(I,JM)+WORK(I,JP))
            ELSE
               Z(I,J)=Z(I,J)-0.5*WORK(I,J)
     *                      +0.25*(WORK(IM,J)+WORK(IP,J))
            END IF
         ENDDO
      ENDDO
      NOPS=NOPS+(NX+1)*(NY+1)*(13+2*NFH)
      RETURN
      END
