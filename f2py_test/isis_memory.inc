/**************************************************************
*_Title isis_memory.inc FORTRAN include file for dynamic memory
*_Args NONE
*_Hist Apr 18 1995 Trent Hare Original Version (Kris Becker)
*      Feb 19 2004 Kris Becker Added support for 64 bit architecture
*                              systems
*
*_End
**************************************************************/

#ifndef ISIS_MEMORY_INC
#define ISIS_MEMORY_INC

#include "isisarch.h"

#if (ISIS_64_BIT == 1)
#define _D_MEMNDX INTEGER*8
#else
#define _D_MEMNDX INTEGER*4
#endif

#endif
