      SUBROUTINE TRANSP(ARRAY,WORK,NX,NY)
      IMPLICIT NONE
C
C_TITLE	TRANSP - Transpose an array (need not be square)
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
C						(before transposition)
	REAL*4		ARRAY(NX,NY)	!B	Array to be transposed,
C						contains transpose on output
	REAL*4		WORK(NY,NX)	!O	Work space; also
C						contains transpose on output
C
C_DESC	Transposes an array (swaps the line and sample directions).
C	The result is returned BOTH the original storage location
C	and the work space WORK, and although the subroutine works 
C	just fine if the array is not square, the calling routine 
C	will be very confused by the ARRAY in this case because it 
C	will still have the original dimensions declared.  This routine 
C	is mainly intended to be called from routines that pass on the 
C	array rather than dealing with it themselves.  In such situations, 
C	the dimensions should be passed in inverted order after calling 
C	TRANSP.
C
C_CALLS	R2R (PICS)
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      INTEGER*4 I,J		! Loop counters
C
      DO J=1,NY
         DO I=1,NX
            WORK(J,I)=ARRAY(I,J)
         ENDDO
      ENDDO
      CALL U_MOVE4(NX*NY,WORK,ARRAY)
      RETURN
      END
