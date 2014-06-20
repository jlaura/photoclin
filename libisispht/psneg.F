      REAL*4 FUNCTION PSNEG(THETA)
      IMPLICIT NONE
C
C_TITLE	PSNEG - Negative of the photometric fn in the phase plane
C
#include "clinom_psnpar.inc"
#include "clinom_trig.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	REAL*4		THETA		!I	Angle between surf normal
C						(assumed in phase plane)
C						and incidence vector,
C						measured toward emission
C						vector
C_PARM  REAL*4		PSNEG		!O	-ve of the brightness
C
C_DESC	Function to be minimized to find the maximum possible value
C	of the photometric function:  the -ve of the brightness, for
C	a surface normal vector in the phase plane at angle THETA.
C
C_CALLS	PS (CLINOM.OLB)
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      REAL*4 EMI		! Emission angle
      REAL*4 CINC		! Cosine of incidence angle
      REAL*4 CEMI		! Cosine of emission angle
C      REAL*4 PS			! Photometric function routine
      REAL*4 BE			! Brightness returned by photom. fn.
      REAL*4 DBDI		! Deriv of brightness wrt. CINC
      REAL*4 DBDE		! Deriv of brightness wrt. CEMI
C
      EMI=PHASE(1)-THETA
      IF (ABS(EMI).GT.90.0) THEN
         PSNEG=0.0
      ELSE
         CINC=COS(THETA*DEG_TO_RAD)
         CEMI=COS(EMI*DEG_TO_RAD)
         CALL PS(CINC,CEMI,BE,DBDI,DBDE,0,1)
         PSNEG=-BE
      END IF
      RETURN
      END
