#include <stdio.h>
#include <string.h>
#include "isistypes.h"
#include "binding.h"


  /* binder to call the Fortran subroutine from the C application */
  FLOAT8 CC_NAME(my_pht_user) ( FLOAT8 inc, FLOAT8 ema, FLOAT8 phase )

  {
    extern FLOAT8 FTN_NAME(my_pht_user) 
      ( FLOAT8 *inc, FLOAT8 *ema, FLOAT8 *phase );

    return ( FTN_NAME(my_pht_user) ( &inc, &ema, &phase ));
  }

