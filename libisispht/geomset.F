      SUBROUTINE GEOMSET(FMAG)
      IMPLICIT NONE
C
C_TITLE	GEOMSET - Scale params for clinometry at magnification FMAG
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	REAL*4		FMAG		!I	Magnification (pixel
C						size in units of full-
C						resolution pixels)
C
C_DESC	Changes the geometric parameters for photoclinometry/radar-
C	clinometry to be appropriate for work at a magnification
C	FMAG relative to the original image, with geometric quantities
C	measured in units of the magnified pixels.
C
C_CALLS	None
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C       May 23 2000 Janet Barrett - Ported from VAX to Unix
C
C_END
C
#include "clinom_datum.inc"
#include "clinom_rgeom.inc"

      Z0F=Z00/FMAG
      Z0SF=Z00S/FMAG
      R0F=R0/FMAG
      R0F2=R0F*R0F
      X0F=(X0-0.5)/FMAG+1.0
      Y0F=(Y0-0.5)/FMAG+1.0
      RMAG=FMAG
      HF=H/FMAG
      HF2=HF*HF
      X0RN=X0R/FMAG+1.0
      X0RE=X0R/FMAG+0.5
      SCENTF=SCENT/FMAG
      THSF=2.0*HF*SCENTF
      RETURN
      END
