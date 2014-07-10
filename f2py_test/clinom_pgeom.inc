C**************************************************************************
C_TITLE clinom_pgeom.inc
C
C_DESC  Common block defining the imaging geometry for the image(s)
C
C_HIST  Sep 07 1990 Randolph Kirk, USGS Flagstaff, Original Version
C**************************************************************************
      COMMON /PGEOM/ SI1,SI2,CI,SE1,SE2,CE

      REAL*4 SI1(2)		! Coeff of 1st diag difference DZ1 in
C				  cos(incidence) for 1st, 2nd images
      REAL*4 SI2(2)		! Coeff of 2nd diag diff DZ2 in cos(inc)
      REAL*4 CI(2)		! Constant coeff in cos(incidence)
      REAL*4 SE1(2)		! As SI1 but for cos(emission)
      REAL*4 SE2(2)		! As SI2 but for cos(emission)
      REAL*4 CE(2)		! As CI but for cos(emission)
C**************************************************************************
