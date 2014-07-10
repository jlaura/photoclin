C**************************************************************************
C_TITLE clinom_specpix.inc
C
C_DESC  Defines special pixel values used by the clinometry software
C
C_HIST  May 10 2001 Janet Barrett, USGS Flagstaff, Original Version
C**************************************************************************
#include "special_pixel.inc"

      INTEGER*4 I4VALMIN                ! Valid minimum Integer*4 value
      INTEGER*4 I4VALMAX                ! Valid maximum Integer*4 value
      INTEGER*4 I4VALNUL                ! Valid null Integer*4 value

      DATA I4VALMIN/-2147483647/
      DATA I4VALMAX/2147483647/
      DATA I4VALNUL/-2147483648/
C**************************************************************************
