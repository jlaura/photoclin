C*************************************************************************
C               -------------------------------------------
C               |    S P E C I A L _ P I X E L . I N C    |
C               -------------------------------------------
C
C_TITLE SPECIAL_PIXEL Definition of special pixel values
C
C Include the C file that will determine architecture information
#include "isisarch.h"
C
C_VARS  TYPE       VARIABLE             DESCRIPTION
C               1-byte special pixel values:
        LOGICAL*1 VALID_MIN1            ! Minimum valid value
        LOGICAL*1 NULL1                 ! Null value
        LOGICAL*1 LOW_REPR_SAT1         ! Low representation saturation
        LOGICAL*1 LOW_INSTR_SAT1        ! Low instrument saturation
        LOGICAL*1 HIGH_INSTR_SAT1       ! High instrument saturation
        LOGICAL*1 HIGH_REPR_SAT1        ! High representation saturation
        LOGICAL*1 VALID_MAX1            ! Maximum valid 1-byte value
#if (LINUX_IX86_ARCH == 1) || (LINUX_PPC_ARCH == 1) || defined(__GNUC__)
        PARAMETER (VALID_MIN1           = X'01')
        PARAMETER (NULL1                = X'00')
        PARAMETER (LOW_REPR_SAT1        = X'00')
        PARAMETER (LOW_INSTR_SAT1       = X'00')
#else
        PARAMETER (VALID_MIN1           = 1)
        PARAMETER (NULL1                = 0)
        PARAMETER (LOW_REPR_SAT1        = 0)
        PARAMETER (LOW_INSTR_SAT1       = 0)
#endif
C  Declare the proper paraemters under the specific operating system
#if (VAX_ARCH == 1) 
        PARAMETER (HIGH_INSTR_SAT1      = 'FF'X)
        PARAMETER (HIGH_REPR_SAT1       = 'FF'X)
        PARAMETER (VALID_MAX1           = 'FE'X)
#else
        PARAMETER (HIGH_INSTR_SAT1      = X'FF')
        PARAMETER (HIGH_REPR_SAT1       = X'FF')
        PARAMETER (VALID_MAX1           = X'FE')
#endif
        INTEGER*4 IVALID_MIN1           ! Minimum valid value
        INTEGER*4 INULL1                ! Null value
        INTEGER*4 ILOW_REPR_SAT1        ! Low representation saturation
        INTEGER*4 ILOW_INSTR_SAT1       ! Low instrument saturation
        INTEGER*4 IHIGH_INSTR_SAT1      ! High instrument saturation
        INTEGER*4 IHIGH_REPR_SAT1       ! High representation saturation
        INTEGER*4 IVALID_MAX1           ! Maximum valid 1-byte value
        PARAMETER (IVALID_MIN1          = 1)
        PARAMETER (INULL1               = 0)
        PARAMETER (ILOW_REPR_SAT1       = 0)
        PARAMETER (ILOW_INSTR_SAT1      = 0)
        PARAMETER (IHIGH_INSTR_SAT1     = 255)
        PARAMETER (IHIGH_REPR_SAT1      = 255)
        PARAMETER (IVALID_MAX1          = 254)

C
C               2-byte special pixel values:
        INTEGER*2 VALID_MIN2            ! Minimum valid value
        INTEGER*2 NULL2                 ! Null value
        INTEGER*2 LOW_REPR_SAT2         ! Low representation saturation
        INTEGER*2 LOW_INSTR_SAT2        ! Low instrument saturation
        INTEGER*2 HIGH_INSTR_SAT2       ! High instrument saturation
        INTEGER*2 HIGH_REPR_SAT2        ! High representation saturation
        INTEGER*2 VALID_MAX2            ! Maximum valid 2-byte value
        PARAMETER (VALID_MIN2           = -32752)
        PARAMETER (NULL2                = -32768)
        PARAMETER (LOW_REPR_SAT2        = -32767)
        PARAMETER (LOW_INSTR_SAT2       = -32766)
        PARAMETER (HIGH_INSTR_SAT2      = -32765)
        PARAMETER (HIGH_REPR_SAT2       = -32764)
        PARAMETER (VALID_MAX2           =  32767)
        INTEGER*4  IVALID_MIN2          ! Minimum valid value
        INTEGER*4  INULL2               ! Null value
        INTEGER*4  ILOW_REPR_SAT2       ! Low representation saturation
        INTEGER*4  ILOW_INSTR_SAT2      ! Low instrument saturation
        INTEGER*4  IHIGH_INSTR_SAT2     ! High instrument saturation
        INTEGER*4  IHIGH_REPR_SAT2      ! High representation saturation
        INTEGER*4  IVALID_MAX2          ! Maximum valid 2-byte value
        PARAMETER (IVALID_MIN2          = -32752)
        PARAMETER (INULL2               = -32768)
        PARAMETER (ILOW_REPR_SAT2       = -32767)
        PARAMETER (ILOW_INSTR_SAT2      = -32766)
        PARAMETER (IHIGH_INSTR_SAT2     = -32765)
        PARAMETER (IHIGH_REPR_SAT2      = -32764)
        PARAMETER (IVALID_MAX2          =  32767)
C
C
C  Declare special pixel types in the proper format for specific
C  platforms

        REAL*4 VAX_VALID_MIN_4           ! VAX Minimum valid value
        REAL*4 VAX_NULL_4                ! VAX Null value
        REAL*4 VAX_LOW_REPR_SAT_4        ! VAX Low representation saturation
        REAL*4 VAX_LOW_INSTR_SAT_4       ! VAX Low instrument saturation
        REAL*4 VAX_HIGH_INSTR_SAT_4      ! VAX High instrument saturation
        REAL*4 VAX_HIGH_REPR_SAT_4       ! VAX High representation saturation
        INTEGER*4 IVAX_VALID_MIN_4       ! VAX Minimum valid value
        INTEGER*4 IVAX_NULL_4            ! VAX Null value
        INTEGER*4 IVAX_LOW_REPR_SAT_4    ! VAX Low representation saturation
        INTEGER*4 IVAX_LOW_INSTR_SAT_4   ! VAX Low instrument saturation
        INTEGER*4 IVAX_HIGH_INSTR_SAT_4  ! VAX High instrument saturation
        INTEGER*4 IVAX_HIGH_REPR_SAT_4   ! VAX High representation saturation
        REAL*4 IEEE_VALID_MIN_4          ! IEEE Minimum valid value
        REAL*4 IEEE_NULL_4               ! IEEE Null value
        REAL*4 IEEE_LOW_REPR_SAT_4       ! IEEE Low representation saturation
        REAL*4 IEEE_LOW_INSTR_SAT_4      ! IEEE Low instrument saturation
        REAL*4 IEEE_HIGH_INSTR_SAT_4     ! IEEE High instrument saturation
        REAL*4 IEEE_HIGH_REPR_SAT_4      ! IEEE High representation saturation
#if (VAX_ARCH == 1)
        PARAMETER (VAX_VALID_MIN_4           = 'FFEFFFFF'X)
        PARAMETER (VAX_NULL_4                = 'FFFFFFFF'X)
        PARAMETER (VAX_LOW_REPR_SAT_4        = 'FFFEFFFF'X)
        PARAMETER (VAX_LOW_INSTR_SAT_4       = 'FFFDFFFF'X)
        PARAMETER (VAX_HIGH_INSTR_SAT_4      = 'FFFCFFFF'X)
        PARAMETER (VAX_HIGH_REPR_SAT_4       = 'FFFBFFFF'X)
        PARAMETER (IVAX_VALID_MIN_4          = 'FFEFFFFF'X)
        PARAMETER (IVAX_NULL_4               = 'FFFFFFFF'X)
        PARAMETER (IVAX_LOW_REPR_SAT_4       = 'FFFEFFFF'X)
        PARAMETER (IVAX_LOW_INSTR_SAT_4      = 'FFFDFFFF'X)
        PARAMETER (IVAX_HIGH_INSTR_SAT_4     = 'FFFCFFFF'X)
        PARAMETER (IVAX_HIGH_REPR_SAT_4      = 'FFFBFFFF'X)
        PARAMETER (IEEE_VALID_MIN_4          = 'FF7FFFFA'X)
        PARAMETER (IEEE_NULL_4               = 'FF7FFFFB'X)
        PARAMETER (IEEE_LOW_REPR_SAT_4       = 'FF7FFFFC'X)
        PARAMETER (IEEE_LOW_INSTR_SAT_4      = 'FF7FFFFD'X)
        PARAMETER (IEEE_HIGH_INSTR_SAT_4     = 'FF7FFFFE'X)
        PARAMETER (IEEE_HIGH_REPR_SAT_4      = 'FF7FFFFF'X)
#else
        PARAMETER (VAX_VALID_MIN_4           = X'FFEFFFFF')
        PARAMETER (VAX_NULL_4                = X'FFFFFFFF')
        PARAMETER (VAX_LOW_REPR_SAT_4        = X'FFFEFFFF')
        PARAMETER (VAX_LOW_INSTR_SAT_4       = X'FFFDFFFF')
        PARAMETER (VAX_HIGH_INSTR_SAT_4      = X'FFFCFFFF')
        PARAMETER (VAX_HIGH_REPR_SAT_4       = X'FFFBFFFF')
        PARAMETER (IVAX_VALID_MIN_4          = X'FFEFFFFF')
        PARAMETER (IVAX_NULL_4               = X'FFFFFFFF')
        PARAMETER (IVAX_LOW_REPR_SAT_4       = X'FFFEFFFF')
        PARAMETER (IVAX_LOW_INSTR_SAT_4      = X'FFFDFFFF')
        PARAMETER (IVAX_HIGH_INSTR_SAT_4     = X'FFFCFFFF')
        PARAMETER (IVAX_HIGH_REPR_SAT_4      = X'FFFBFFFF')
#if (SGI_ARCH == 1)
        PARAMETER (IEEE_VALID_MIN_4          = -3.402822452264808e+38)
        PARAMETER (IEEE_NULL_4               = -3.402822655088904e+38)
        PARAMETER (IEEE_LOW_REPR_SAT_4       = -3.402822857913000e+38)
        PARAMETER (IEEE_LOW_INSTR_SAT_4      = -3.402823060737097e+38)
        PARAMETER (IEEE_HIGH_INSTR_SAT_4     = -3.402823263561193e+38)
        PARAMETER (IEEE_HIGH_REPR_SAT_4      = -3.402823466385289e+38)
#else
        PARAMETER (IEEE_VALID_MIN_4          = X'FF7FFFFA')
        PARAMETER (IEEE_NULL_4               = X'FF7FFFFB')
        PARAMETER (IEEE_LOW_REPR_SAT_4       = X'FF7FFFFC')
        PARAMETER (IEEE_LOW_INSTR_SAT_4      = X'FF7FFFFD')
        PARAMETER (IEEE_HIGH_INSTR_SAT_4     = X'FF7FFFFE')
        PARAMETER (IEEE_HIGH_REPR_SAT_4      = X'FF7FFFFF')
#endif
#endif

C               4-byte special pixel values:
        REAL*4 VALID_MIN4               ! Minimum valid value
        REAL*4 NULL4                    ! Null value
        REAL*4 LOW_REPR_SAT4            ! Low representation saturation
        REAL*4 LOW_INSTR_SAT4           ! Low instrument saturation
        REAL*4 HIGH_INSTR_SAT4          ! High instrument saturation
        REAL*4 HIGH_REPR_SAT4           ! High representation saturation
        REAL*4 VALID_MAX4               ! Maximum valid value
#if (VAX_ARCH == 1)
        PARAMETER (VALID_MIN4           = VAX_VALID_MIN_4)
        PARAMETER (NULL4                = VAX_NULL_4)
        PARAMETER (LOW_REPR_SAT4        = VAX_LOW_REPR_SAT_4)
        PARAMETER (LOW_INSTR_SAT4       = VAX_LOW_INSTR_SAT_4)
        PARAMETER (HIGH_INSTR_SAT4      = VAX_HIGH_INSTR_SAT_4)
        PARAMETER (HIGH_REPR_SAT4       = VAX_HIGH_REPR_SAT_4)
#else
        PARAMETER (VALID_MIN4           = IEEE_VALID_MIN_4)
        PARAMETER (NULL4                = IEEE_NULL_4)
        PARAMETER (LOW_REPR_SAT4        = IEEE_LOW_REPR_SAT_4)
        PARAMETER (LOW_INSTR_SAT4       = IEEE_LOW_INSTR_SAT_4)
        PARAMETER (HIGH_INSTR_SAT4      = IEEE_HIGH_INSTR_SAT_4)
        PARAMETER (HIGH_REPR_SAT4       = IEEE_HIGH_REPR_SAT_4)
#endif
        PARAMETER (VALID_MAX4           = 1.7E38)
C
C_DESC  This INCLUDE file defines PARAMETER values that provide
C       symbolic names for the special pixel values.  Note that
C       a C equivalent .H include file is supplied for C programmers.
C       Any change made to this file MUST also be made to SPECIAL_PIXEL.H!!!
C
C_KEYS  VMS
C
C_HIST  Aug 14 1989 Jim Torson, USGS, Flagstaff - Original Build 2 specs
C       Sep 01 1990 Kris Becker - Added VALID_MAX1 and VALID_MAX2
C       Sep 03 1990 JMT - Added VALID_MAX4
C       Nov 12 1990 JMT - Added INTEGER versions of the 1-byte and 2-byte
C                       special values
C       Jul 17 1991 KJB - Ported to UNIX system.  8-bit data type special
C                         pixel handling.  Converted to IEEE floating point
C                         format.
C       Oct 04 1994 KJB - Fixed bug in 8-bit High saturation values
C       Aug 23 1995 KJB - Added sized types; VAX and IEEE symbols where
C                         appropriate
C       May 14 1996 KJB - Added INTEGER representation of VAX special pixel
C                         values because the SGI barfs on REALs of these
C                         values
C       Sep 09 1998 KJB - Added special floating point values for the SGI
C                         since the FORTRAN compiler freaks out on the
C                         hex assignments of them variables
C       Nov 03 1999 KJB - Added tests for LINUX_PPC_ARCH 
C_END
C**************************************************************************
