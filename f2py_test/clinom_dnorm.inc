C**************************************************************************
C_TITLE clinom_dnorm.inc
C
C_DESC  Common block containing image normalization and pedestal
C       parameters
C
C_HIST  Aug 11 1989 Randolph Kirk, USGS Flagstaff, Original Version
C**************************************************************************
      COMMON /DNORM/ DNNORM,DNATM
      REAL*4 DNNORM		! DN value on datum, once DNATM is subtracted
C				  ('FLAT FIELD' - 'HAZE')
      REAL*4 DNATM		! DN value contributed by atmosphere or
C				  instrument pedestal ('HAZE')
C**************************************************************************
