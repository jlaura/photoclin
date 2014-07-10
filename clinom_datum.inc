C**************************************************************************
C_TITLE clinom_datum.inc
C
C_DESC  Common block defining the datum (tangent plane to planet @ 
C       image center)
C
C_HIST  Mar 12 1990 Randolph Kirk, USGS Flagstatt, Original Version
C**************************************************************************
      COMMON /DATUM1/ Z00,DZX0,DZY0,DZ10,DZ20,Z00S,R0,X0,Y0,
     *               Z0F,Z0SF,R0F,R0F2,X0F,Y0F,RMASK1,RMASK2,RMAG
      COMMON /DATUM2/ DATUMTYP

      INTEGER*4 DATUMTYP	! Type of datum model: 1=plane perp to line
C				! of sight; 2=inclined plane; 3=sphere (but
C				  parameters appropriate to 2 now describe
C				  tangent plane to this sphere)
      REAL*4 Z00		! Constant term added to planar datum, in 
C				  terms of full-res pixels
      REAL*4 DZX0,DZY0		! Sample, line slopes of datum plane
      REAL*4 DZ10,DZ20		! Diagonal slopes of datum plane
      REAL*4 Z00S		! Constant term added to spherical datum
      REAL*4 R0			! Radius of datum sphere in full-res pixels
      REAL*4 X0,Y0		! Center of datum sphere in full-res pixels
      REAL*4 Z0F		! Z00, scaled for operation on coarse mesh
      REAL*4 Z0SF
      REAL*4 R0F		! R0, scaled for operation on coarse mesh
      REAL*4 R0F2		! R0F squared
      REAL*4 X0F,Y0F		! X0,Y0 scaled for operation on coarse mesh
C				  and offset to refer to indices of nodes
C				  rather than elements
      REAL*4 RMASK1,RMASK2	! Pixels further than RMASK2 times the
C				  radius of the spherical datum from
C				  the center of the sphere will be set to
C				  the (local) datum brightness, those
C				  closer than RMASK1 will be left alone,
C				  and those in between will have their
C				  brightness interpolated between the
C				  actual and datum values
      REAL*4 RMAG		! Mesh spacing FMAG, passed in common to
C				  routines that need it
C**************************************************************************
