! -*- f90 -*-
      SUBROUTINE BMATCH(B,NX,NY)
      IMPLICIT NONE
C
C_TITLE	BMATCH - Match image regions off of spherical datum with datum img
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
C	already.)
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
      REAL*4 RM22		! Squared distance from sphere center 
C				  to pixel center beyond which we match
      REAL*4 BE			! Model brightness of datum in pixel
      REAL*4 DB1,DB2		! Derivatives of model brightness (not
C				  used)

      IF (DATUMTYP.NE.2) RETURN	! Applies only to spherical datum
      IF (RMASK2.LE.0.0) RETURN
      RM22=R0*R0*RMASK2*RMASK2
      DO J=1,NY
         Y=REAL(J)-Y0
         Y2=Y*Y
         DO I=1,NX
C            X=REAL(I)-X0
            X=(REAL(I)-X0)*ASPECT
            X2=X*X
            IF (X2+Y2.GT.RM22) THEN
               CALL PBDER(I,J,0.0,0.0,BE,DB1,DB2,0)
               B(I,J)=BE
            END IF
         ENDDO
      ENDDO
      NOPS=NOPS+4*NX*NY+2*NY
      RETURN
      END
