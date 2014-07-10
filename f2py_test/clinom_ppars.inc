C**************************************************************************
C_TITLE clinom_ppars.inc
C
C_DESC  Common block defining the photometric behavior of the image(s)
C
C_HIST  Aug 28 1989 Randolph Kirk, USGS Flagstaff, Original Version
C**************************************************************************
      COMMON /PPARS1/ NPS,MULPS,MULPSP,PIOPT
      COMMON /PPARS2/ PEX,PEX1,PL2,PL1,FALPHA,TWOGAM
      COMMON /PPARS3/ HAPKE
      COMMON /PPARS4/ PHOFUNC
      COMMON /PPARS5/ PWH,PHG1,PHG2,PHH,PB0,PTHETA,PBH,PCH

      INTEGER*4 NPS		! # of images (1=monoscopic, 2=biscopic)
      INTEGER*4 MULPS	        ! # of operations to evaluate photom funs
      INTEGER*4 MULPSP	        ! # of operation to evaluate pho fun derivs
      REAL*4 PEX		! Minneaert exponents for image(s)
      REAL*4 PEX1		! Minnaert exponents minus 1
      REAL*4 PL2		! Amt of lunar photometric behavior
      REAL*4 PL1		! Amt of Minnaert photometric behavior
      LOGICAL*1 HAPKE	        ! Use hapke fn instead?
      REAL*4 FALPHA(2)		! Henyey-Greenstein f(alpha)
      REAL*4 TWOGAM		! 2*SQRT(1-W) coeff in Hapke approx.
C				  to H-function
      CHARACTER*6 PHOFUNC       ! Photometric function name
      REAL*4 PWH                ! Single scattering albedo of Hapke
      REAL*4 PHG1               ! Asymmetry parameter for Henyey-Greenstein
                                ! alternate phase function
      REAL*4 PHG2               ! Second asymmetry parameter for 2-lobed
                                ! Henyey-Greenstein alternate phase function
      REAL*4 PHH                ! Opposition surge width of Hapke
      REAL*4 PB0                ! Opposition surge strength of Hapke
      REAL*4 PTHETA             ! Small scale surface roughness in degrees of
                                ! Hapke
      REAL*4 PBH                ! First Legendre coefficient of phase
                                ! function of a single particle
      REAL*4 PCH                ! Second Legendre coefficient of phase
                                ! function of a single particle
      INTEGER*4 PIOPT           ! If 0, then use Henyey-Greenstein function
                                ! If 1, then use Legendre function
C**************************************************************************
