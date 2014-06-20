      REAL*4 FUNCTION DDBERR(DELAZIN)
      IMPLICIT NONE
C
C_TITLE	DDBERR - Difference between diagonal B-derivs upon rotation
C
#include "clinom_aepar.inc"
#include "clinom_datum.inc"
#include "clinom_pgeom.inc"
#include "clinom_ppars.inc"
#include "clinom_trig.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	REAL*4		DELAZIN		!I	Angle by which coords
C						will be rotated
C_PARM  REAL*4		DDBERR		!O	Difference between abs
C						values of derivatives of
C						brightness along diagonals
C						in rotated coords.
C
C_DESC	Changes the geometric parameters to be equivalent to rotating
C	coordinates by DELAZIN, evaluates the difference between diagonal
C	brightness derivatives, and restores the original geom. params.
C	Also stores in COMMON the sign of the product of the two derivs.,
C	and an estimate of the proper maximum brightness at which to
C	clip the image for SSIPSF-PI solution.
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C		24-Jun-02 RLK Allow for aspect ratio: Total transformation
C				of parameters is now stretch to unit aspect 
C				followed by rotation by DELAZIN
C
C_PAUSE
C
      REAL*4 COD,SID		! Cosine, sine of rotation angle
      INTEGER*4 INDEX		! Index to input images: 1=numerator or the
C				  only image; 2=denominator if any
      REAL*4 SI1OLD(2),SI2OLD(2) ! Save SI1, SI2
      REAL*4 SE1OLD(2),SE2OLD(2) ! Save SE1, SE2
      REAL*4 DZ10OLD,DZ20OLD	! Save DZ10, DZ20
      INTEGER*4 DATUMTYPOLD	! Save DATUMTYP
      REAL*4 ASPECTOLD		! Save ASPECT
C
      REAL*4 BE			! Brightness returned by photom. fn.
      REAL*4 DB1,DB2		! 1st derivatives of brightness with diagonal
C				  slopes
      REAL*4 HAP,HAM		! Half (ASPECT plus/minus 1)
C				  used to convert inc/ema vectors to ASPECT=1
      REAL*4 HAIP,HAIM		! Half (1/ASPECT plus/minus 1)
C				  used to convert datum derivatives to ASPECT=1
      REAL*4 TEMP		! Temp var used in conversion
C
      HAP=0.5*(ASPECT+1)
      HAM=0.5*(ASPECT-1)
      COD=COS(DELAZIN*DEG_TO_RAD)
      SID=SIN(DELAZIN*DEG_TO_RAD)
      DO INDEX=1,NPS
         SI1OLD(INDEX)=SI1(INDEX)
         SI2OLD(INDEX)=SI2(INDEX)
         SI1(INDEX)= SI1OLD(INDEX)*HAP+SI2OLD(INDEX)*HAM
         SI2(INDEX)= SI1OLD(INDEX)*HAM+SI2OLD(INDEX)*HAP
         TEMP=       SI1(INDEX)*COD+SI2(INDEX)*SID
         SI2(INDEX)=-SI1(INDEX)*SID+SI2(INDEX)*COD
         SI1(INDEX)=TEMP
         SE1OLD(INDEX)=SE1(INDEX)
         SE2OLD(INDEX)=SE2(INDEX)
         SE1(INDEX)= SE1OLD(INDEX)*HAP+SE2OLD(INDEX)*HAM
         SE2(INDEX)= SE1OLD(INDEX)*HAM+SE2OLD(INDEX)*HAP
         TEMP=       SE1(INDEX)*COD+SE2(INDEX)*SID
         SE2(INDEX)=-SE1(INDEX)*SID+SE2(INDEX)*COD
         SE1(INDEX)=TEMP
      ENDDO
      HAIP=0.5*(1.0/ASPECT+1)
      HAIM=0.5*(1.0/ASPECT-1)
      DZ10OLD=DZ10
      DZ20OLD=DZ20
      DZ10= DZ10*HAIP+DZ20*HAIM
      DZ20=-DZ10*HAIM+DZ20*HAIP
      TEMP= DZ10*COD+DZ20*SID
      DZ20=-DZ10*SID+DZ20*COD
      DZ10=TEMP
      DATUMTYPOLD=DATUMTYP
      IF (DATUMTYPOLD.EQ.2) DATUMTYP=1
      ASPECTOLD=ASPECT
      ASPECT=1.0
      CALL PBDER(0,0,0.0,0.0,BE,DB1,DB2,1)
      DDBERR=ABS(DB1)-ABS(DB2)
      DBRAT=SIGN(1.0,DB1*DB2)
      IF (NPS.EQ.1) THEN
         BCLIP0=BCLIP
      ELSE IF (SI1(2)+SI2(2)*DBRAT.NE.0.0) THEN
         BCLIP0=MIN(BCLIP,
     *             BE-2.0*DB1*(CI(2)+SI1(2)*DZ10+SI2(2)*DZ20)/
     *             (SI1(2)+SI2(2)*DBRAT))
      END IF
      DO INDEX=1,NPS
         SI1(INDEX)=SI1OLD(INDEX)
         SI2(INDEX)=SI2OLD(INDEX)
         SE1(INDEX)=SE1OLD(INDEX)
         SE2(INDEX)=SE2OLD(INDEX)
      ENDDO
      DZ10=DZ10OLD
      DZ20=DZ20OLD
      DATUMTYP=DATUMTYPOLD
      ASPECT=ASPECTOLD
      RETURN
      END
