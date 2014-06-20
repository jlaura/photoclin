      SUBROUTINE AE0CALC(B,DB,AR,AC,NX,NY,BDER)
      IMPLICIT NONE
C
C_TITLE	AE0CALC - Set up equations for SSIPSF-PI clinometry
C
#include "clinom_aepar.inc"
#include "clinom_ops.inc"
#include "clinom_specpix.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
        REAL*4		B(NX,NY)	!I	Brightness image
	REAL*4		DB(NX,NY)	!O	RHS for estimation of
C						topography
	REAL*4		AR(NX,2)	!O	Matrix inverted on rows
	REAL*4		AC(NY,2)	!O	Matrix inverted on columns
	EXTERNAL	BDER		!I	Brightness fn and derivs
C
C_DESC	Constructs the RHS and row and column separated matrices for the
C	pseudo-inverse solution to the SSIPSF equations resulting from
C	linearization of the pc equations about the appropriately rotated
C	datum.
C
C_CALLS	ASEPCALC,BDER (CLINOM.OLB)
C
C_HIST	24-Sep-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      REAL*4 BE			! Brightness of datum
      REAL*4 DB1,DB2		! 1st derivatives of brightness with diagonal
C				  slopes
      INTEGER*4 I,J		! Loop counters
      REAL*4 BIJ		! Pixel brightness (or its logarithm)
C
      J=NY/2
      DO I=1,NX
         CALL BDER(I,J,0.0,0.0,BE,DB1,DB2,1)
         AR(I,1)=BE
         AR(I,2)=DB1
      ENDDO
      DO J=1,NY
         DO I=1,NX
C            IF (B(I,J).EQ.BNULL) THEN
            IF (B(I,J).LT.VALID_MIN4.OR.B(I,J).GT.VALID_MAX4) THEN
               DB(I,J)=0.0
            ELSE
               BIJ=MIN(B(I,J),BCLIP0)
               DB(I,J)=(BIJ-AR(I,1))/AR(I,2)
            END IF
         ENDDO
      ENDDO
      CALL ASEPCALC(AR,AC,NX,NY)
      NOPS=NOPS+2*NX*NY
      RETURN
      END
