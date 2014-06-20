      SUBROUTINE FINDBMAX
      IMPLICIT NONE
C
C_TITLE	FINDBMAX - Find maximum of photometric fn for clipping
C
C_ARGS	None
C
C_DESC	Finds the maximum value BMAX of the photometric model, for
C	use in clipping the input image.  For all models except
C	Minnaert (-lunar) with k<1 this is straightforward.  The latter
C	case has divergent brightness at the limb and may or may not
C	also have a local maximum in the interior of the disk.  This
C	routine determines whether such a local maximum exists by
C	looking at a table of 1/b at one degree intervals (a "safe"
C	approach to the divergent function) and then doing its usual
C	numerical optimization to pin down the maximum if it exists;
C	if no local maximum exists (or it is less than 1 deg wide or
C	within 1 deg of the limb) the brightness at 1 deg from the
C	limb is returned as the max value.
C
C_CALLS	DPSNEG,PSNEG (CLINOM.OLB)
C	DBRENT (Numerical Recipes)
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_END
C
#include "clinom_aepar.inc"
#include "clinom_ppars.inc"
#include "clinom_psnpar.inc"
      REAL*4 ATHETA,BTHETA,CTHETA ! Bracketing angles for finding maximum
C				    of the photometric function
      REAL*4 DBRENT		! Numerical Recipes minimization rtne
      REAL*4 PSNEG,DPSNEG	! Minus the photometric function vs angle
      EXTERNAL PSNEG,DPSNEG	! in the phase plane, and derivative thereof
      REAL*4 THMIN		! Angle at which PSNEG is minimized
      REAL*4 BINV(-1:90)	! Table of 1/brightness for each
C				  degree from emission direction in the
C				  phase plane
      INTEGER*4 I		! Loop counter: position in BINV
      INTEGER*4 IMIN		! Pointer to local maximum value in BINV

      IF (HAPKE.OR.(PEX.GE.1.0)) THEN! No limb divergence, so solve
         ATHETA=PHASE(1)		! Sub-observer point
         CTHETA=PHASE(1)-90.0		! Bright limb
         BTHETA=(ATHETA+CTHETA)*0.5	! Halfway between
         BMAX=-DBRENT(ATHETA,BTHETA,CTHETA,PSNEG,DPSNEG,
     *    1.03E-3,THMIN)
      ELSE IF ((PEX.LE.0.5).OR.	! Even Minnaert part monotonic
     *         (PHASE(1).GE.ATAN(ABS(PEX-0.5)
     *          /SQRT(ABS(PEX*PEX1))))) THEN
         BMAX=-PSNEG(PHASE(1)-89.0)	! so choose a large upper limit
      ELSE				! Must search for maximum
         DO I=-1,89			! First search 1 deg intervals
            BINV(I)=-1.0/PSNEG(PHASE(1)-REAL(I))
         ENDDO				! in a table...
         BINV(90)=0.0
         IMIN=90
         DO I=89,0,-1
            IF ((BINV(I).LE.BINV(I-1))
     *     .AND.(BINV(I).LE.BINV(I+1))) IMIN=I
         ENDDO	
         IF (IMIN.EQ.90) THEN		! No extremum in table so it
            BMAX=-PSNEG(PHASE(1)-89.0)	! must be monotonic...
         ELSE
            ATHETA=PHASE(1)-REAL(I+1)	! Search within neighborhood
            BTHETA=PHASE(1)-REAL(I)	! discovered in table
            CTHETA=PHASE(1)-REAL(I-1)
            BMAX=-DBRENT(ATHETA,BTHETA,CTHETA,PSNEG,DPSNEG,
     *       1.03E-3,THMIN)
         END IF
      END IF
      RETURN
      END
