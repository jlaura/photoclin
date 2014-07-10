/*****************************************************************
*_TITLE clinom_mem.h 
*
*_DESC  Common block defining the memory constraints on pc2d
*
*_HIST  Jun 01 2001  Randolph Kirk, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_MEM_H
#define CLINOM_MEM_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_mem2_f_common {
  INT4 off1;
  INT4 nlimit;          
} CLINOM_MEM2_F_COMMON;

extern CLINOM_MEM2_F_COMMON     FTN_COMMON(mem2);

static CLINOM_MEM2_F_COMMON *C_MEM2 = &FTN_COMMON(mem2);

typedef struct clinom_mem3_f_common {
  INT4 nmax;          /* Max allocatable memory (size of buffer
                         memory cdum (IDUM)) in longwords */
} CLINOM_MEM3_F_COMMON;

extern CLINOM_MEM3_F_COMMON     FTN_COMMON(mem3);

static CLINOM_MEM3_F_COMMON *C_MEM3 = &FTN_COMMON(mem3);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
