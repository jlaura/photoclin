C**************************************************************************
C_TITLE clinom_aepar.inc
C
C_DESC  Contains parameters needed by ae*calc routines; these
C       parameters apply to both photoclinometry and radarclinometry.
C
C_HIST  Oct 24 1990 Randolph Kirk, USGS Flagstaff, Original Version
C**************************************************************************
      COMMON /AEPAR1/ BNORM,BMAX,BNULL,BCLIP,BCLIP0,ASPECT,DELAZ,DBRAT,
     *CHARAZ
      COMMON /AEPAR2/ FLOP,LOGIMG
      COMMON /AEPAR3/ IOCT

      REAL*4 BNORM		! Normalization of photometric or 
C				  backscatter function: photometric fn at
C				  datum incidence/emission angles, or
C				  backscatter at center incidence angle i
C				  times tan(i) area factor
      REAL*4 BMAX		! Maximum possible value of photometric fn
C				  or backscatter * pixel area factor
      REAL*4 BNULL		! Internal flag value to mark null pixels
C				  (which contrib. only roughness terms to
C				  gradient and Hessian).  Also, for PC,
C				  all image DN's mapping to larger values
C				  than this are flagged by this value
      REAL*4 BCLIP		! All image DN's mapping to larger values
C				  than this (but not to nulls) will be
C				  scaled to this value
      REAL*4 BCLIP0		! Level at which to clip brightness in
C				  biscopic SSIPSF-PI PC to avoid slopes >
C				  those at which the nonlinear brightness
C				  becomes infinite (shadow in denominator)
      REAL*4 ASPECT		! Aspect ratio of pixels in the image
C				  (not ground) coordinate system: 1 for
C				  PC, abs(sin(i0)) for radar, where i0 is
C				  incidence angle to datum.
      LOGICAL*1 FLOP		! Transpose arrays for better performance
C				  of ICCG during injection?
      LOGICAL*1 LOGIMG		! Image is log brightness (for Magellan)
C				  so don't normalize residuals by BNORM
      REAL*4 DELAZ		! Angle to rotate meshes so characteristics
C				  lie on line/sample directions
      REAL*4 DBRAT		! Ratio of diagonal brightness derivatives
C				  in rotated coords (+1 or -1)
      INTEGER*4 IOCT		! Octant containing azimuth of characteristics
C				  -2=-90 to -45 deg, -1=-45 to 0, 1=0 to 45, 2=45 to 90
      REAL*4 CHARAZ             ! Azimuth of characteristics, Cartesian
C**************************************************************************
