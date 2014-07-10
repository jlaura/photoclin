/******************************************************************************/
/* Prototypes for the pho library routines */
/******************************************************************************/

#ifndef PHO_LIB_H
#define PHO_LIB_H

/*#include "isistypes.h"*/
#include "/usgs/isis/inc/isistypes.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/***  get_fortran_common_str  ***************************************************************/
void get_fortran_common_str ( CHAR c_str[], INT4 c_str_len );

/***  pho_fooC  ***************************************************************/
double *pho_fooC
         (double *fooC, double inc,double ema,double phase, double *foo_array);




/***  pho_fooF  ***************************************************************/

extern FLOAT8 pho_foof
        ( FLOAT8 *fooF, FLOAT8 inc,FLOAT8 ema,FLOAT8 phase, FLOAT8 foo_array[]);




#ifdef __cplusplus
}
#endif  /*  __cplusplus  */
#endif


/******************************************************************************/
/*
*_Hist  30 Dec 2000 K Teal THompson - USGS - Flagstaff Original Version
*_End
*******************************************************************************/

