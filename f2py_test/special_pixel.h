/************************************************************************
*_Title SPECIAL_PIXEL.H C definition of special pixel values
*
*_Desc  This C include file defines the preprocessor define values that
*       provide symbolic names for the special pixel values that are
*	hardware specific.  Note that a FORTRAN equivalent .INC include
*	file is supplied for FORTRAN programmers.  Any change that is
*	made to this file MUST also be made to SPECIAL_PIXEL.H!!
*
*_Vars
*/

#ifndef SPECIAL_PIXEL_H
#define SPECIAL_PIXEL_H
#include "isistypes.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/* 1-byte special pixel values  */
#define VALID_MIN1       ((UINT1) 1)
#define NULL1            ((UINT1) 0)
#define LOW_REPR_SAT1    ((UINT1) 0)
#define LOW_INSTR_SAT1   ((UINT1) 0)
#define HIGH_INSTR_SAT1  ((UINT1) 255)
#define HIGH_REPR_SAT1   ((UINT1) 255)
#define VALID_MAX1       ((UINT1) 254)

/* Integer representations of 1-byte values */
#define IVALID_MIN1      ((INT4) 1)
#define INULL1           ((INT4) 0)
#define ILOW_REPR_SAT1   ((INT4) 0)
#define ILOW_INSTR_SAT1  ((INT4) 0)
#define IHIGH_INSTR_SAT1 ((INT4) 255)
#define IHIGH_REPR_SAT1  ((INT4) 255)
#define IVALID_MAX1      ((INT4) 254)

/* 2-byte special pixel values */
#define VALID_MIN2       ((INT2) (-32752))
#define NULL2            ((INT2) (-32768))
#define LOW_REPR_SAT2    ((INT2) (-32767))
#define LOW_INSTR_SAT2   ((INT2) (-32766))
#define HIGH_INSTR_SAT2  ((INT2) (-32765))
#define HIGH_REPR_SAT2   ((INT2) (-32764))
#define VALID_MAX2       ((INT2)   32767 )

/* Integer representations of 2-byte values */
#define IVALID_MIN2      ((INT4) (-32752))
#define INULL2           ((INT4) (-32768))
#define ILOW_REPR_SAT2   ((INT4) (-32767))
#define ILOW_INSTR_SAT2  ((INT4) (-32766))
#define IHIGH_INSTR_SAT2 ((INT4) (-32765))
#define IHIGH_REPR_SAT2  ((INT4) (-32764))
#define IVALID_MAX2      ((INT4)   32767 )


/* Define 4-byte special pixel values for VAX floating point */
#define VAX_VALID_MIN_4                 0xFFEFFFFF
#define VAX_NULL_4                      0xFFFFFFFF
#define VAX_LOW_REPR_SAT_4              0xFFFEFFFF
#define VAX_LOW_INSTR_SAT_4             0xFFFDFFFF
#define VAX_HIGH_INSTR_SAT_4            0xFFFCFFFF
#define VAX_HIGH_REPR_SAT_4             0xFFFBFFFF

/* Define 4-byte special pixel values for IEEE floating point */
#define IEEE_VALID_MIN_4                0xFF7FFFFA
#define IEEE_NULL_4                     0xFF7FFFFB
#define IEEE_LOW_REPR_SAT_4             0xFF7FFFFC
#define IEEE_LOW_INSTR_SAT_4            0xFF7FFFFD
#define IEEE_HIGH_INSTR_SAT_4           0xFF7FFFFE
#define IEEE_HIGH_REPR_SAT_4            0xFF7FFFFF

/***************************************************************************** 
  Set the appropriate values to the special pixel variables
******************************************************************************/
#if (VAX_ARCH == 1)
static const UINT4 valid_min_4      = VAX_VALID_MIN_4;
static const UINT4 null_4           = VAX_NULL_4;
static const UINT4 low_repr_sat_4   = VAX_LOW_REPR_SAT_4;
static const UINT4 low_instr_sat_4  = VAX_LOW_INSTR_SAT_4;
static const UINT4 high_instr_sat_4 = VAX_HIGH_INSTR_SAT_4;
static const UINT4 high_repr_sat_4  = VAX_HIGH_REPR_SAT_4;
#else
static const UINT4 valid_min_4      = IEEE_VALID_MIN_4;
static const UINT4 null_4           = IEEE_NULL_4;
static const UINT4 low_repr_sat_4   = IEEE_LOW_REPR_SAT_4;
static const UINT4 low_instr_sat_4  = IEEE_LOW_INSTR_SAT_4;
static const UINT4 high_instr_sat_4 = IEEE_HIGH_INSTR_SAT_4;
static const UINT4 high_repr_sat_4  = IEEE_HIGH_REPR_SAT_4;
#endif

#define VALID_MIN4      (*((const FLOAT4 *) &valid_min_4))
#define NULL4           (*((const FLOAT4 *) &null_4))
#define LOW_REPR_SAT4   (*((const FLOAT4 *) &low_repr_sat_4))
#define LOW_INSTR_SAT4  (*((const FLOAT4 *) &low_instr_sat_4))
#define HIGH_INSTR_SAT4 (*((const FLOAT4 *) &high_instr_sat_4))
#define HIGH_REPR_SAT4  (*((const FLOAT4 *) &high_repr_sat_4))
#define VALID_MAX4      (const FLOAT4) 1.7E+38


#ifdef __cplusplus
}
#endif  /*  __cplusplus  */
#endif

/*
*_Keys  SPECIAL_PIXEL
*
*_Hist  Aug 06 1989  Kris Becker USGS, Flagstaff Original C Version
*	Sep 01 1990  Kris Becker - Added VALID_MAX1 and VALID_MAX2
*	Sep 04 1990  KJB - Added VALID_MAX4
*	Nov 21 1990  KJB - Added INTEGER (int) versions of 1-byte and 2-byte
*                          special pixel values
*	Jul 12 1991 KJB - Modified to support UNIX system.  Convert
*                         floating point format to IEEE.
*       Jul 27 1993 KJB - Modified 8-bit special pixel values to support
*                         new method for 8-bit data
*       Oct 04 1994 KJB - Clarified history entries
*	Nov 09 1994 Jim Torson - Changed declarations of 4-byte special pixel
*			values from long to unsigned long so that using
*			strict ANSI C compliance will not produce warnings.
*       Aug 22 1995 Kris Becker - Modified to use ISIS typedefs; added
*                                 definitions for VAX and IEEE float values
*       Mar 20 1996 KJB - Added elements so this file can be ingested by C++
*_End
************************************************************************/
