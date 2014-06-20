      SUBROUTINE PBSCALE(B,WORK,NX,NY,DNDATUM)
      IMPLICIT NONE
C
C_TITLE	PBSCALE - Scale an image for photoclinometry
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		B(NX*NY)	!B	Image to be normalized
C						(indexed 1-D for speed)
	REAL*4		WORK(NX*NY)	!-	Workspace to estimate mode
	REAL*4		DNDATUM		!I	DN of the datum surface
C						before normalization
C
C_VARS
C
C_DESC	Scales the image array B so it corresponds to the model
C	photometric function to be used in photoclinometry (by dividing
C	by the unscaled DN of the datum surface, then multiplying by
C	the photometric fn of the datum orientation).  DNDATUM may
C	be supplied as an argument, or estimated in this routine as
C	the mode of the data.
C
C_CALLS	SORT (Numerical Recipes)
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
#include "clinom_aepar.inc"
#include "clinom_ppars.inc"
#include "clinom_specpix.inc"
      INTEGER*4 NB		! # of pixels
      INTEGER*4	I		! Loop counter
      REAL*4 SCALE		! Multiplicative scaling for brightness
C
      NB=NX*NY
      SCALE=BNORM/DNDATUM
      DO I=1,NB
         IF (B(I).GE.VALID_MIN4.AND.B(I).LE.VALID_MAX4) THEN
	    IF (NPS.EQ.2.AND.B(I).LT.0.0) THEN
	      B(I)=BCLIP
            ELSE
              B(I)=MAX(0.0,MIN(B(I)*SCALE,BCLIP))
            ENDIF
         ELSE
	    B(I)=NULL4
         ENDIF
C         IF ((NPS.EQ.2).AND.(B(I).LT.0.0)) THEN
C            B(I)=BCLIP				! Numerator shadow
C         ELSE
C            B(I)=MAX(0.0,MIN(B(I)*SCALE,BNULL))! >BNULL-->BNULL
C            IF (B(I).LT.BNULL) B(I)=MIN(B(I),BCLIP)! >BCLIP-->BCLIP
C         END IF
      ENDDO
      RETURN
      END
