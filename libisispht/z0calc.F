      SUBROUTINE Z0CALC(DB,Z,NX,NY)
      IMPLICIT NONE
C
C_TITLE	Z0CALC - Complete SSIPSF-PI solution for topography
C
#include "clinom_aepar.inc"
#include "clinom_ops.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		Z(NX+1,NY+1)	!O	Topographic array
	REAL*4		DB(NX,NY)	!I	Scaled deviation of B
C						from datum value, operated
C						on by row & column matrices
C
C_DESC	Completes the least-squares solution for the first approximation
C	to the altitude Z from the intermediate quantity in DB by 
C	multiplication by AT (which has only +/-1's in this case).
C
C_CALLS	None
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_END
C
      INTEGER*4 I,J		! Loop counters
      INTEGER*4 NX1,NY1		! Dimensions of Z
      REAL*4 ZIJ		! One element of Z
C
      NX1=NX+1
      NY1=NY+1
      DO J=1,NY1
         DO I=1,NX1
            ZIJ=0.0
            IF ((I.NE.NX1).AND.(J.NE.NY1)) ZIJ=ZIJ+DB(I  ,J  )
            IF ((I.NE.1  ).AND.(J.NE.NY1)) ZIJ=ZIJ-DB(I-1,J  )*DBRAT
            IF ((I.NE.1  ).AND.(J.NE.1  )) ZIJ=ZIJ-DB(I-1,J-1)
            IF ((I.NE.NX1).AND.(J.NE.1  )) ZIJ=ZIJ+DB(I  ,J-1)*DBRAT
            Z(I,J)=ZIJ
         ENDDO
      ENDDO
      NOPS=NOPS+6*NX*NY+4*(NX+NY)+9
      RETURN
      END
