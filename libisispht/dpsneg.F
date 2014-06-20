      REAL*4 FUNCTION DPSNEG(THETA)
      IMPLICIT NONE
C
C_TITLE	DPSNEG - Derivative of PSNEG
C
#include "clinom_psnpar.inc"
#include "clinom_trig.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	REAL*4		THETA		!I	Angle between surf normal
C						(assumed in phase plane)
C						and incidence vector,
C						measured toward emission
C						vector
C_PARM  REAL*4		DPSNEG		!O	THETA derivative of
C						-ve of the brightness
C
C_DESC	Derivative of function PSNEG to be minimized to find the maximum 
C	possible value of the photometric function:  the -ve of the 
C	brightness, for a surface normal vector in the phase plane at angle 
C	THETA.
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
         DPSNEG=0.0
      ELSE
         CINC=COS(THETA*DEG_TO_RAD)
         CEMI=COS(EMI*DEG_TO_RAD)
         CALL PS(CINC,CEMI,BE,DBDI,DBDE,0,1)
         DPSNEG=-(SIGN(DBDI,THETA)-SIGN(DBDE,EMI))
      END IF
      RETURN
      END
