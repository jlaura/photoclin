#include <stdio.h>
#include "binding.h"


void FTN_NAME (pht_user_init) ( CHAR *objectString)

  {

    extern void CC_NAME (pht_user_init)
                                   ( CHAR *objectString );


    CC_NAME (pht_user_init)  ( objectString );
    return;

  }

/*******************************************************************************
*_Binding pht_user_init - User's own photometric model initialization binder
*		to call C-language subroutine from Fortran application
*
*
*
*_args  type	variable	i/o	description
*       CHAR*	phase		!i	phase angle in degrees
*
*_desc  The Fortran to C binder to pht_user_init that allows Fortran
*	"photomet" to call u_get_string_parm to load the user's object
*	module name and pass it into a global C string variable.
*
*_hist  05 Feb 2001 K Teal Thompson - Flagstaff Original Version
*******************************************************************************/





FLOAT8 FTN_NAME (pht_user) ( FLOAT8 *inc, FLOAT8 *ema, FLOAT8 *phase )

  {

    extern FLOAT8 CC_NAME (pht_user)
                                   ( FLOAT8 *inc, FLOAT8 *ema, FLOAT8 *phase );

    return ( (CC_NAME (pht_user)  ( inc, ema, phase )) );

  }

/*******************************************************************************
*_Binding pht_user - User's own photometric model binder to call
*		C-language subroutine from Fortran application
*
*_args  type         variable  i/o      description
*       real*8       ema       !i       emission angle in degrees
*       real*8       inc       !i       incidence angle in degrees
*       real*8       phase     !i       phase angle in degrees
*
*_desc  The Fortran to C binder that allows Fortran "photomet" to call
*	C-language pht_user which will dymamically load user's subroutine
*	object for the photometric function the user wants.
*
*_hist  18 Jan 2001 K Teal Thompson - Flagstaff Original Version
*******************************************************************************/


