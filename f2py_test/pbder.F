      SUBROUTINE PBDER(I,J,DZ1,DZ2,BE,DB1,DB2,MODE)
      IMPLICIT NONE
C
C_TITLE	PBDER - Pixel brightness, derivs from diagonal slopes
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	I		!I	Column of image
	INTEGER*4	J		!I	Row of image
	REAL*4		DZ1,DZ2		!I	Diagonal elevation diffs
C						(in pixel widths) across
C						a pixel
	REAL*4		BE		!O	Pixel brightness, based
C						on photometric function
	REAL*4		DB1,DB2		!O	Derivs of BE wrt DZ1...2
	INTEGER*4	MODE		!I	Highest order of 
C						derivatives that will be
C						calculated
C
C_DESC	Given the diagonal elevation differences across a pixel, calculates
C	the pixel brightness, and possibly the 1st derivatives with respect
C	Integration over the pixel is performed using 1 point Gauss
C	quadrature for the brightness.
C	This routine has no knowledge of the surface photometric properties;
C	it passes the incidence and emission cosines to PS, which returns
C	the photometric function and its derivatives, then applies the
C	chain rule.
C
C_CALLS	DATUM,PS (CLINOM.OLB)
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C		24-Jun-02 RLK Allow for aspect ratio
C
C_END
C
#include "clinom_aepar.inc"
#include "clinom_datum.inc"
#include "clinom_ops.inc"
#include "clinom_pgeom.inc"
#include "clinom_ppars.inc"
      REAL*4 Z01,Z02,Z03,Z04	! Elevations at the corners of the element,
C				  on the datum
      REAL*4 DZ1D,DZ2D		! Diagonal elevation diff incl datum slopes
      REAL*4 ARG		! Square of SQA
      REAL*4 SQA		! Normalization of photmetric vectors
      REAL*4 CINCN,CEMIN	! Inc, emiss cosines for numerator image
      REAL*4 BEN		! Brightness of numerator image
      REAL*4 DBNDI,DBNDE	! Derivatives of BEN with CINCN,CEMIN
      REAL*4 CINCD,CEMID	! Inc, emiss cosines for denominator image
      REAL*4 BED		! Brightness of denominator image
      REAL*4 DBDDI,DBDDE	! Derivatives of BED with CINCD,CEMID
      REAL*4 CIN		! Temporary for calculating DIN1,DIN2
      REAL*4 DIN1,DIN2		! Derivs of CINCN with DZ1,DZ2
      REAL*4 CEN		! Temporary for calculating DEN1,DEN2
      REAL*4 DEN1,DEN2		! Derivs of CEMIN with DZ1,DZ2
      REAL*4 DBN1,DBN2		! Derivs of BEN with DZ1,DZ2
      REAL*4 CID		! Temporary for calculating DID1,DID2
      REAL*4 DID1,DID2		! Derivs of CINCD with DZ1,DZ2
      REAL*4 CED		! Temporary for calculating DED1,DED2
      REAL*4 DED1,DED2		! Derivs of CEMID with DZ1,DZ2
      REAL*4 DBD1,DBD2		! Derivs of BED with DZ1,DZ2
      REAL*4 TERM1,TERM2		! Used in derivatives w/ASPECT
C

      IF (DATUMTYP.EQ.2) THEN
         CALL DATUM(I,J,4,Z01,Z02,Z03,Z04)
         DZ1D=DZ1+Z03-Z01
         DZ2D=DZ2+Z02-Z04
         NOPS=NOPS+6
      ELSE IF (DATUMTYP.EQ.1) THEN
         DZ1D=DZ1+DZ10
         DZ2D=DZ2+DZ20
         NOPS=NOPS+2
      END IF
C      ARG=2.0+DZ1D*DZ1D+DZ2D*DZ2D
      ARG=2.0+(((DZ1D+DZ2D)/ASPECT)**2.0+(DZ1D-DZ2D)**2.0)/2.0
      SQA=SQRT(ARG)
      CINCN=(CI(1)+SI1(1)*DZ1D+SI2(1)*DZ2D)/SQA
      CEMIN=(CE(1)+SE1(1)*DZ1D+SE2(1)*DZ2D)/SQA
      CALL PS(CINCN,CEMIN,BEN,DBNDI,DBNDE,MODE,1)
      IF (NPS.EQ.1) THEN
         BE=BEN
         NOPS=NOPS+25+MULPS
      ELSE
         CINCD=(CI(2)+SI1(2)*DZ1D+SI2(2)*DZ2D)/SQA
         CEMID=(CE(2)+SE1(2)*DZ1D+SE2(2)*DZ2D)/SQA
         CALL PS(CINCD,CEMID,BED,DBDDI,DBDDE,MODE,2)
         IF (BED*BCLIP.LE.BEN) THEN
            BE=BCLIP
            NOPS=NOPS+35+MULPS
         ELSE
            BE=BEN/BED
            NOPS=NOPS+36+MULPS
         END IF
      END IF
      IF (MODE.EQ.0) RETURN
      TERM1=(DZ1D+DZ2D)/(2.0*ASPECT*ASPECT)
      TERM2=(DZ1D-DZ2D)/2.0
      CIN=-CINCN/ARG
C      DIN1=SI1(1)/SQA+CIN*DZ1D
C      DIN2=SI2(1)/SQA+CIN*DZ2D
      DIN1=SI1(1)/SQA+CIN*(TERM1+TERM2)
      DIN2=SI2(1)/SQA+CIN*(TERM1-TERM2)
      CEN=-CEMIN/ARG
C      DEN1=SE1(1)/SQA+CEN*(TERM1+TERM2)
C      DEN2=SE2(1)/SQA+CEN*(TERM1-TERM2)
      DEN1=SE1(1)/SQA+CEN*DZ1D
      DEN2=SE2(1)/SQA+CEN*DZ2D
      DBN1=DBNDI*DIN1+DBNDE*DEN1
      DBN2=DBNDI*DIN2+DBNDE*DEN2
      IF (NPS.EQ.1) THEN
         DB1=DBN1
         DB2=DBN2
         NOPS=NOPS+30+MULPSP
      ELSE
         IF (BED*BCLIP.LE.BEN) THEN
            DB1=0.0
            DB2=0.0
            NOPS=NOPS+30+MULPSP+MULPSP
         ELSE
            CID=-CINCD/ARG
C            DID1=SI1(2)/SQA+CID*DZ1D
C            DID2=SI2(2)/SQA+CID*DZ2D
            DID1=SI1(2)/SQA+CID*(TERM1+TERM2)
            DID2=SI2(2)/SQA+CID*(TERM1-TERM2)
            CED=-CEMID/ARG
C            DED1=SE1(2)/SQA+CED*DZ1D
C            DED2=SE2(2)/SQA+CED*DZ2D
            DED1=SE1(2)/SQA+CED*(TERM1+TERM2)
            DED2=SE2(2)/SQA+CED*(TERM1-TERM2)
            DBD1=DBDDI*DID1+DBDDE*DED1
            DBD2=DBDDI*DID2+DBDDE*DED2
            DB1=(DBN1-BEN*DBD1/BED)/BED
            DB2=(DBN2-BEN*DBD2/BED)/BED
            NOPS=NOPS+62+MULPSP+MULPSP
         END IF
      END IF
      RETURN
      END
