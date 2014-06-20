      SUBROUTINE CHK4INV(DATBUF,NX,NY,INVCNT)
      IMPLICIT NONE
C
C_TITLE	CHK4INV - Determine if there are invalid pixels in an array
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	NX,NY		!I	Dimensions of array
	REAL*4		DATBUF(NX,NY)	!I	Array to check
	INTEGER*4	INVCNT          !O	Number of invalid
C                                               pixels found in array
C
C_DESC	Looks at every pixel in the DATBUF array to see if it falls
C       outside the range of valid pixels.  An invalid pixel is one
C       which is less than VALID_MIN4 or greater than VALID_MAX4
C       (see $ISIS/inc/special_pixel.inc).
C       The number of invalid pixels is kept track of by INVCNT.
C       This subroutine should be called before using SSIPSF-PI as
C       a starting solution because SSIPSF-PI requires that all
C       pixel values be valid.
C
C_HIST	Apr 17 2001 Janet Barrett, USGS, Flagstaff Original Version
C
C_PAUSE
C
#include "clinom_specpix.inc"
      INTEGER*4 I,J

      INVCNT = 0
      DO J = 1,NY
	DO I = 1,NX
	  IF (DATBUF(I,J).LT.VALID_MIN4.OR.DATBUF(I,J).GT.
     *        VALID_MAX4) THEN
            INVCNT = INVCNT + 1 
          ENDIF
        ENDDO
      ENDDO
      RETURN
      END
