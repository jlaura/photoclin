      SUBROUTINE BMIX(B,NX,NY)
      IMPLICIT NONE
C
C_TITLE	BMIX - Match image regions off of spherical datum with datum img
C
#include "clinom_aepar.inc"
#include "clinom_datum.inc"
#include "clinom_ops.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		B(NX,NY)	!B	Image array
C
C_DESC	Used to avoid doing PC near the limbs or off the disk for a
C	spherical datum model.  Pixels with a distance from the
C	disk center greater than RMASK2 times the disk radius are
C	set to the local datum brightness.  (For the routine to work,
C	B must be scaled to the photometric function and BMAX defined
C	already.) Pixels inside RMASK1 times the radius are unchanged,
C	and pixels between RMASK1 and RMASK2 have their brightness
C	interpolated between the actual value and the model value
C
C_CALLS	PBDER (CLINOM.OLB)
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C		24-Jun-02 RLK Allow for aspect ratio
C
C_PAUSE
      INTEGER*4 I,J		! Loop counters
      REAL*4 X,Y		! Coordinates relative to sphere center
      REAL*4 X2,Y2		! Squares of X, Y
      REAL*4 R			! Distance from sphere center to pixel
      REAL*4 R2			! Square of R
      REAL*4 RM1		! Distance from sphere center to pixel
C				  center inside which we leave DN alone
      REAL*4 RM2		! Distance from sphere center to pixel
C				  center beyond which we match
      REAL*4 RM12,RM22		! Squares of RM1,RM2
      REAL*4 DRM		! Width of annulus betw RM1, RM2
      REAL*4 BE			! Model brightness of datum in pixel
      REAL*4 DB1,DB2		! Derivatives of model brightness (not
C				  used)
      REAL*4 W			! Weight for interpolating between
C				  actual and datum brightness

      IF (DATUMTYP.NE.2) RETURN	! Applies only to spherical datum
      IF (RMASK2.LE.0.0) RETURN
      IF (RMASK1.GE.RMASK2) THEN
         CALL BMATCH(B,NX,NY)
         RETURN
      END IF
      RM1=R0*RMASK1
      RM12=RM1*RM1
      RM2=R0*RMASK2
      RM22=RM2*RM2
      DRM=RM2-RM1
      DO J=1,NY
         Y=REAL(J)-Y0
         Y2=Y*Y
         DO I=1,NX
C            X=REAL(I)-X0
            X=(REAL(I)-X0)*ASPECT
            X2=X*X
            R2=X2+Y2
            IF (R2.GT.RM12) THEN
               CALL PBDER(I,J,0.0,0.0,BE,DB1,DB2,0)
               IF (R2.GE.RM22) THEN
                  B(I,J)=BE
               ELSE
                  R=SQRT(R2)
                  W=(R-RM1)/DRM
                  B(I,J)=W*BE+(1.0-W)*B(I,J)
               END IF
            END IF
         ENDDO
      ENDDO
      NOPS=NOPS+4*NX*NY+2*NY
      RETURN
      END
