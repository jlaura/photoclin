      SUBROUTINE ZLINEOUT(Z,NX,NY,ZBUF,INS,INL,SCALE,IOCT,COSEMI)
      IMPLICIT NONE
C
C_TITLE	ZLINEOUT - Interpolate/scale 1 line of PC topo to a buffer
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
C						of topo array
	REAL*4		Z(NX+1,NY+1)	!I	Topographic array
        INTEGER*4	INS		!I	Sample dimension of buffer
        INTEGER*4	INL		!I	Line dimension of buffer
	REAL*4 	        ZBUF(INS,INL)	!O	Output buffer
	REAL*4		SCALE		!I	Scaling factor (output
C						DN per pixel width)
	INTEGER*4	IOCT		!I	Which 45 deg "octant"
C						characteristics of the
C						input image lie in; image
C						will be rotated/flopped
C						to put characteristics
C						in octant 1
        REAL*4          COSEMI          !I      Used to scale output file
C                                               from line of sight to
C                                               vertical
C
C_VARS
C
C_DESC	Scales and converts one line of topography from a 2-D real
C	array to a 1-D I*2 buffer.  Bilinear interpolation is used
C	to obtain ZBUF values at locations at the center of squares of
C	four Z-values.  Since the Z values obtained from the 2-D
C	photoclinometry algorithm are for the corners of the input
C	image pixels, the result of this routine is elevations at
C	the centers of one row of pixels.  When successive lines
C	are created by this routine and output, the result is a topo-
C	graphic model registered to the original image.
C	The rotation and/or flopping of the image done on input to
C	put the azimuth of characteristics between 0 and 45 deg is
C	undone at this point.
C
C_CALLS	none
C
C_HIST	17-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
#include "clinom_datum.inc"
#include "clinom_specpix.inc"
      INTEGER*4 IS		! Loop counter: output sample #
      INTEGER*4 IL		! Loop counter: output line #
      INTEGER*4 I,J		! Sample, line in topo array
C
      DO IL = 1,INL
        IF (IOCT.EQ.-2) THEN
           I=NX+1-IL
        ELSE IF (IOCT.EQ.-1) THEN
           J=NY+1-IL
        ELSE IF (IOCT.EQ.1) THEN
           J=IL
        ELSE IF (IOCT.EQ.2) THEN
           I=IL
        END IF
        DO IS=1,INS
           IF (IOCT.EQ.-2) THEN
              J=IS
           ELSE IF (IOCT.EQ.-1) THEN
              I=IS
           ELSE IF (IOCT.EQ.1) THEN
              I=IS
           ELSE IF (IOCT.EQ.2) THEN
              J=IS
           END IF
           ZBUF(IS,IL)=MAX(VALID_MIN4,MIN(
     *      ((Z(I+1,J+1)+Z(I,J+1)+Z(I,J)+Z(I+1,J))
     *      *0.25)*SCALE,VALID_MAX4))
           ZBUF(IS,IL)=ZBUF(IS,IL)*COSEMI
        ENDDO
      ENDDO
      RETURN
      END
