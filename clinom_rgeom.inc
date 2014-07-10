C**************************************************************************
C_TITLE clinom_rgeom.inc
C
C_DESC  Common block defining the imaging geometry for the radar scene
C
C_HIST  Aug 28 1989 Randolph Kirk, USGS Flagstaff, Original Version
C**************************************************************************
      COMMON /RGEOM/ H,X0R,SCENT,HF,HF2,X0RN,X0RE,SCENTF,THSF

      REAL*4 H			! Elevation of radar track above datum 
C				  plane in units of pixels on finest mesh
      REAL*4 X0R		! Horizontal distance (on datum plane) from
C				  sub-radar track to far edge of image in
C				  units of pixels on finest mesh
      REAL*4 SCENT		! Slant range to center of image, in units
C				  of pixels on finest mesh
      REAL*4 HF			! As H, but in units of pixels on current
C				  mesh
      REAL*4 HF2		! Square of HF
      REAL*4 X0RN		! Coeff giving horizontal distance from
C				  sub-radar track to *node* I in units of
C				  pixels on current mesh: Xnode=X0RN - I
      REAL*4 X0RE		! As X0RN but for center of element I
      REAL*4 SCENTF		! As SCENT but in units of current pixels
      REAL*4 THSF		! 2*HF*SCENTF; THSF/X is twice the diff-
C				  erence in arc length (datum angle times 
C				  SCENTF) on the datum across a pixel in
C				  the range direction.
C**************************************************************************
