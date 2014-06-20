      SUBROUTINE ASEPCALC(AR,AC,NX,NY)
      IMPLICIT NONE
C
C_TITLE	ASEPCALC - Calculate/factor matrices for SSIPSF-PI clinometry
C
#include "clinom_aepar.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		AR(NX,2)	!O	Matrix inverted on rows
	REAL*4		AC(NY,2)	!O	Matrix inverted on columns
C
C_DESC	Constructs and factors the row and column separated matrices for the
C	pseudo-inverse solution to the SSIPSF equations resulting from
C	linearization of the PC/RC equations about the appropriately rotated
C	datum.
C
C_CALLS	S3FACT (CLINOM.OLB)
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      INTEGER*4 I,J		! Loop counters
C
      DO I=1,NX
         AR(I,1)=2.0
         AR(I,2)=-DBRAT
      ENDDO
      DO J=1,NY
         AC(J,1)=2.0
         AC(J,2)= DBRAT
      ENDDO
      CALL S3FACT(AR,NX)
      CALL S3FACT(AC,NY)
      RETURN
      END
