      SUBROUTINE PS(CINC,CEMI,BE,DBDI,DBDE,MODE,INDEX)
      IMPLICIT NONE
C
C_TITLE	PS - Photometric function and derivatives
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	REAL*4		CINC,CEMI	!I	Cosines of incidence,
C						emission angles
	REAL*4		BE		!O	Pixel brightness
	REAL*4		DBDI,DBDE	!O	Derivs of BE wrt CINC,CEMI
	INTEGER*4	MODE		!I	Highest order of 
C						derivatives that will be
C						calculated
	INTEGER*4	INDEX		!I	Which of 2 photometric
C						fns to use
C
C_DESC	Evaluates the photometric function and possibly its first
C	derivatives, given the photometric cosines.  The photometric
C	function implemented here is either a linear combination of 
C	Minnaert and Lommel-Seeliger (lunar), or Hapke's function
C	with B0, h, and thetabar set to zero.  The limiting cases 
C	of the former (pure Minnaert, pure lunar, Minnaert part=Lambert) 
C	are detected and evaluated by specialized code for maximum 
C	efficiency...this routine gets called a lot.
C
C_CALLS	None
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
#include "clinom_ppars.inc"
#include "clinom_psnpar.inc"
#include "clinom_trig.inc"
#include "pht_common.inc"
      REAL*4 CC			! Sum of cosines of incidence & emiss
      REAL*4 H0D,HD		! Denominators of H-functions
      REAL*4 H0,H		! H-fns (approx) of incidence, emiss
      REAL*4 LOCAL_BH		! Cofactor of lunar factor in Hapke
C				  function
      REAL*4 TMTG		! 2-2*Gamma, used in Hapke deriv.
      REAL*4 H0P,HP		! Derivatives of H0, H
      REAL*4 BEL,BEM		! Lunar, Minnaert contribs to BE
      REAL*4 T			! Temporary

      REAL*8 INC                ! Incidence angle
      REAL*8 EMI                ! Emission angle
      REAL*8 PHASEANG           ! Phase angle
      REAL*8 PHT_FUNC           ! Photometric function
      REAL*8 EPS,EPS2
      REAL*8 INCP
      REAL*8 BEIP               ! Brightness estimate at incidence
C                                 plus epsilon
      REAL*8 BEIM
      REAL*8 INCM
      REAL*8 EMIP
      REAL*8 BEEP
      REAL*8 EMIM
      REAL*8 BEEM
      REAL*8 ANGP
      REAL*8 ANGM

      DATA EPS,EPS2/1.0E-4,2.0E-4/
C
      IF ((CINC.LE.0.0).OR.(CEMI.LE.0.0)) THEN
         BE=0.0
      ELSE IF (PHOFUNC.EQ.'HAPH_S'.OR.
     * PHOFUNC.EQ.'HAPL_S') THEN
         CC=CINC+CEMI
         BEL=CINC/CC
         H0D=1.0+TWOGAM*CINC
         H0=(1.0+2.0*CINC)/H0D
         HD=1.0+TWOGAM*CEMI
         H=(1.0+2.0*CEMI)/HD
         LOCAL_BH=FALPHA(INDEX)+H0*H
         BE=BEL*LOCAL_BH	! NB factor of w/4 omitted from the model
      ELSE IF (PHOFUNC.EQ.'LAMBER'.OR.PHOFUNC.EQ.'LOMSEL'.
     .OR.PHOFUNC(1:3).EQ.'MIN'.OR.PHOFUNC.EQ.'LUNLAM') THEN
         IF (PL2.EQ.0.0) THEN
            IF (PEX.EQ.1.0) THEN
               BE=CINC
            ELSE
               BE=CINC**PEX*CEMI**PEX1
            END IF
         ELSE IF (PL1.EQ.0.0) THEN
            BE=CINC/(CINC+CEMI)
         ELSE
            BEL=CINC/(CINC+CEMI)
            IF (PEX.EQ.1.0) THEN
               BEM=CINC
            ELSE
               BEM=CINC**PEX*CEMI**PEX1
            END IF
            BE=PL2*BEL+PL1*BEM
         END IF
      ELSE
         CHAR_PHT_FUNCTION = PHOFUNC
         WH = PWH
	 HG1 = PHG1
	 HG2 = PHG2
	 HH = PHH
	 B0 = PB0
	 THETA = PTHETA
	 BH = PBH
	 CH = PCH
	 IOPT = PIOPT
	 INC = ACOS(CINC)*RAD_TO_DEG
	 EMI = ACOS(CEMI)*RAD_TO_DEG
	 PHASEANG = PHASE(INDEX)
	 BE = PHT_FUNC(INC,EMI,PHASEANG)
      END IF
      IF (MODE.EQ.0) RETURN
      IF ((CINC.LE.0.0).OR.(CEMI.LE.0.0)) THEN
         DBDI=0.0
         DBDE=0.0
      ELSE IF (PHOFUNC.EQ.'HAPH_S'.OR.
     * PHOFUNC.EQ.'HAPL_S') THEN
         TMTG=2.0-TWOGAM
         H0P=TMTG/(H0D*H0D)
         HP=TMTG/(HD*HD)
         DBDI=(CEMI*LOCAL_BH)/(CC*CC)+BEL*H0P*H
         DBDE=-BE/CC+BEL*H0*HP
      ELSE IF (PHOFUNC.EQ.'LAMBER'.OR.PHOFUNC.EQ.'LOMSEL'.
     .OR.PHOFUNC(1:3).EQ.'MIN'.OR.PHOFUNC.EQ.'LUNLAM') THEN
         IF (PL2.EQ.0.0) THEN
            IF (PEX.EQ.1.0) THEN
               DBDI=1.0
               DBDE=0.0
            ELSE
               DBDI=PEX*BE/CINC
               DBDE=PEX1*BE/CEMI
            END IF
         ELSE IF (PL1.EQ.0.0) THEN
            T=-BE*BE
            DBDI=BE/CINC+T
            DBDE=T/CINC
         ELSE
            T=-BEL*BEL
            DBDI=PL2*(BEL/CINC+T)
            DBDE=PL2*T/CINC
            IF (PEX.EQ.1.0) THEN
               DBDI=DBDI+PL1
            ELSE
               DBDI=DBDI+PL1*PEX*BEM/CINC
               DBDE=DBDE+PL1*PEX1*BEM/CEMI
            END IF
         END IF
      ELSE
         ANGP = CINC + EPS
	 IF (ANGP.GT.1.0) ANGP = 1.0
	 IF (ANGP.LT.-1.0) ANGP = -1.0
C         INCP = ACOS(CINC+EPS)*RAD_TO_DEG
         INCP = ACOS(ANGP)*RAD_TO_DEG
	 BEIP = PHT_FUNC(INCP,EMI,PHASEANG)
	 ANGM = CINC - EPS
	 IF (ANGM.GT.1.0) ANGM = 1.0
	 IF (ANGM.LT.-1.0) ANGM = -1.0
C	 INCM = ACOS(CINC-EPS)*RAD_TO_DEG
	 INCM = ACOS(ANGM)*RAD_TO_DEG
	 BEIM = PHT_FUNC(INCM,EMI,PHASEANG)
C	 DBDI = (BEIP-BEIM)/EPS2
	 DBDI = (BEIP-BEIM)/(ANGP-ANGM)
	 ANGP = CEMI + EPS
	 IF (ANGP.GT.1.0) ANGP = 1.0
	 IF (ANGP.LT.-1.0) ANGP = -1.0
C	 EMIP = ACOS(CEMI+EPS)*RAD_TO_DEG
	 EMIP = ACOS(ANGP)*RAD_TO_DEG
	 BEEP = PHT_FUNC(INC,EMIP,PHASEANG)
	 ANGM = CEMI - EPS
	 IF (ANGM.GT.1.0) ANGM = 1.0
	 IF (ANGM.LT.-1.0) ANGM = -1.0
C	 EMIM = ACOS(CEMI-EPS)*RAD_TO_DEG
	 EMIM = ACOS(ANGM)*RAD_TO_DEG
	 BEEM = PHT_FUNC(INC,EMIM,PHASEANG)
C	 DBDE = (BEEP-BEEM)/EPS2
	 DBDE = (BEEP-BEEM)/(ANGP-ANGM)
      END IF
      RETURN
      END
