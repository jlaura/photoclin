      SUBROUTINE PBLINEIN1(BBUF1,INS,B,NX,NY,IL,DNATM,IOCT)
      IMPLICIT NONE
C
C_TITLE	PBLINEIN1 - Copy an image line to an array for monoscopic PC
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	INS		!I	Sample dimension of buffer
	REAL*4 	        BBUF1(INS)	!I	Input buffer
	INTEGER*4	IL		!I	Which line this is
	INTEGER*4	NX,NY		!I	Sample, line dimensions of
C						accumulated image
	REAL*4		B(NX,NY)	!B	Array to accum. lines in
	REAL*4		DNATM		!I	Atmospheric contribution
C						to subtract from each DN
	INTEGER*4	IOCT		!I	Which 45 deg "octant"
C						characteristics of the
C						input image lie in; image
C						will be rotated/flopped
C						to put characteristics
C						in octant 1
C
C_DESC	Replaces B(IS,IL) with BBUF1(IS)-DNATM for all IS, if IOCT=1.
C	Otherwise, places the buffer in a rotated and/or flopped 
C	position in B so the characteristic strips in B will have
C	an azimuth between 0 and 45 deg.
C
C_CALLS	None
C
C_HIST	23-Oct-89 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
#include "clinom_specpix.inc"
      INTEGER*4 IS		! Loop counter
      REAL*4 DN
C
      DO IS=1,INS
        IF (BBUF1(IS).GE.VALID_MIN4.AND.BBUF1(IS).LE.VALID_MAX4) THEN
          DN=BBUF1(IS)-DNATM
        ELSE
          DN=NULL4
        ENDIF
        IF (IOCT.EQ.-2) THEN
           B(NX+1-IL,IS)=DN
        ELSE IF (IOCT.EQ.-1) THEN
           B(IS,NY+1-IL)=DN
        ELSE IF (IOCT.EQ.1) THEN
           B(IS,IL)=DN
        ELSE IF (IOCT.EQ.2) THEN
           B(IL,IS)=DN
        END IF
      ENDDO 
      RETURN
      END
