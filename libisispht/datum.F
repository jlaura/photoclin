      SUBROUTINE DATUM(I,J,NPTS,Z01,Z02,Z03,Z04)
      IMPLICIT NONE
C
C_TITLE	DATUM - Calculate datum elevations at corners of pixel
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	I,J		!I	Column, row indices of
C						element (pixel) for
C						which elevation is to
C						be found
	INTEGER*4	NPTS		!I	# of corners at which
C						elevation will be
C						calculated.  If NPTS=1
C						only Z03 will be returned;
C						for any other value, all 4
	REAL*4		Z01,Z02,Z03,Z04	!O	Datum elevations at nodes
C						with indices (I+1,J+1),
C						(I,J+1), (I,J), (I+1,J),
C						respectively
C
C_VARS
C
C_DESC	Calculates the elevations of the datum (reference surface) at
C	the four corners of pixel (I,J).  Definition of the datum is
C	passed in COMMON in the include file.  The datum can be the
C	X-Y plane (i.e. Z=0 everywhere), an inclined plane, or a sphere.
C
C_HIST	05-Mar-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C       Apr 23 1998 Tracie Sucharski, Converted to UNIX/ISIS
C		24-Jun-02 RLK Allow for aspect ratio
C
C_END
C
#include "clinom_aepar.inc"
#include "clinom_datum.inc"
      INTEGER*4 IOLD,JOLD	! Previous pixel for which we evaluated
      DATA IOLD,JOLD / -1, -1 /
      REAL*4 ZX,ZXP,ZY,ZYP	! Terms entering elevations for tilted-
C				  plane datum
      REAL*4 X,XP,Y,YP		! Coordinates of pixel corners relative
C				  to center of spherical datum
      REAL*4 X2,XP2		! Squares of X, XP
      REAL*4 RMY2,RMYP2		! R squared minus squares of Y, YP
      REAL*4 ARG		! Argument of square root

      IF (DATUMTYP.EQ.0) THEN
         Z03=0.0
         IF (NPTS.EQ.1) RETURN
         Z01=0.0
         Z02=0.0
         Z04=0.0
      ELSE IF (DATUMTYP.EQ.1) THEN
         IF (I.NE.IOLD) THEN
            ZX=REAL(I)*DZX0
            ZXP=ZX+DZX0
         END IF
         IF (J.NE.JOLD) THEN
            ZY=Z0F+REAL(J)*DZY0
            ZYP=ZY+DZY0
         END IF
         Z03=ZX +ZY
         IF (NPTS.EQ.1) RETURN
         Z01=ZXP+ZYP
         Z02=ZX +ZYP
         Z04=ZXP+ZY
      ELSE IF (DATUMTYP.EQ.2) THEN
         IF (I.NE.IOLD) THEN
C            X=REAL(I)-X0F
            X=(REAL(I)-X0F)*ASPECT
            X2=X*X
C            XP=X+1.0
            XP=X+ASPECT
            XP2=XP*XP
            IOLD=I
         END IF
         IF (J.NE.JOLD) THEN
            Y=REAL(J)-Y0F
            RMY2=R0F2-Y*Y
            YP=Y+1.0
            RMYP2=R0F2-YP*YP
            JOLD=J
         END IF
         Z03=Z0SF
         ARG=RMY2 -X2
         IF (ARG.GT.0.0) Z03=Z03+SQRT(ARG)
         IF (NPTS.EQ.1) RETURN
         Z01=Z0SF
         ARG=RMYP2-XP2
         IF (ARG.GT.0.0) Z01=Z01+SQRT(ARG)
         Z02=Z0SF
         ARG=RMYP2-X2
         IF (ARG.GT.0.0) Z02=Z02+SQRT(ARG)
         Z04=Z0SF
         ARG=RMY2 -XP2
         IF (ARG.GT.0.0) Z04=Z04+SQRT(ARG)
      END IF

      RETURN
      END
