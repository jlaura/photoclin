      SUBROUTINE NULLCOUNT32(A,N,NNULL)
      IMPLICIT NONE
C
C_TITLE	NULLCOUNT32 - Count 'null' flags in a REAL*4 array
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	N		!I	# of elements in A
	REAL*4		A(N)		!I	Array to be searched
	INTEGER*4	NNULL		!O	# of nulls found
C
C_DESC	Counts the number of nulls in a REAL*4 array, where a null is
C	defined as "all bits on," the most negative value.
C
C_HIST 11-Aug-89 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_END
#include "clinom_specpix.inc"
      INTEGER*4 I		! Loop counter

      NNULL=0
      DO I=1,N
C         IF (A(I).EQ.'FFFFFFFF'X) NNULL=NNULL+1
         IF (A(I).LT.VALID_MIN4.OR.A(I).GT.VALID_MAX4) NNULL=NNULL+1
      ENDDO
      RETURN
      END
