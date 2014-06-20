      SUBROUTINE ADDFIL32(P,S,S2,N,NS,SAMP,ORDER)
      IMPLICIT NONE
C
C_TITLE	ADDFIL32 - Add a line to accumulation buffers for FASTFIL32
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4 	NS		!I	Sample dimension of image
	REAL*4	 	P(NS)		!I	One line of image to be filtered
	REAL*4	 	S(NS)		!B	Work buffer: accumulated
C						sums of pixel values
	REAL*4	 	S2(NS)		!B	Work buffer: accumulated
C						sums of squared pixel values
	INTEGER*4	N(NS)		!B	Work buffer: # of valid
C						pixels contributing to S,S2
	INTEGER*4 	SAMP		!I	Sample dimension of filter
	INTEGER*4	ORDER		!I	Order (highest power) of 
C					  	pixel values accumulated:
C					 	0 -> # of pixels only
C					  	1 -> sum of DN's
C					  	2 -> sum of squares of DN's
C
C_DESC	This is an adaptation of the fast, flexible boxcar filter
C	algorithm of Eric Eliason to operation on an array resident in
C	memory.  Further details may be found in the documentation for
C	the PICS programs FLT8B, FLT16B, FLT32B.  This routine adds the
C	pixel values, # of valid pixels, etc., in one image line to the 
C	accumulation buffers for the main routine FASTFIL32.
C
C_CALLS	
C
C_HIST	 10-AUG-89 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
#include "clinom_specpix.inc"
C      REAL*4 NULL			! Null pixel flag (all bits on)
      REAL*4 DN				! Current pixel value
      INTEGER*4 IS			! Loop counter: current sample

C      NULL='FFFFFFFF'X
      DO IS=1,NS
         DN=P(IS)
C         IF (DN.NE.NULL) THEN
         IF (DN.GE.VALID_MIN4.AND.DN.LE.VALID_MAX4) THEN
            N(IS)=N(IS)+1
            IF (ORDER.GE.1) S(IS)=S(IS)+DN
            IF (ORDER.GE.2) S2(IS)=S2(IS)+DN*DN
         END IF
      ENDDO
      RETURN
      END
