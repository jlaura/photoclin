#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "isisdylib.h"
#include "u.h"

/*******************************************************************************
Declare defining occurance of external variables.
*******************************************************************************/
typedef FLOAT8  (*PhtFunc)(FLOAT8 p1, FLOAT8 p2, FLOAT8 p3);
static PhtFunc PhtUserFunc = 0;


/*** subroutine ***************************************************************/
void pht_user_init(CHAR *f_str)
/*******************************************************************************
|                         -----------------
|_Title subroutine_name - | pht_user_init |
|                         -----------------
|_binding - pht_user_init in pht_user_bind_c.c
|
|_Args	Type	Variable	I/O	Description
|_Parm  
|_Parm
|_Parm	INT4	error;		O	Return code:
|					 0 - OK
|					-1 - error
|
|_Desc  
|
|_Keys  
|
|_Hist  dd mmm yy  K Teal Thompson - USGS/Flagstaff AZ; Original Version.
|       Feb 23 2004 Kris Becker - Rewrote for Mac Darwin port
|_End
*******************************************************************************/
  {

/******************************************************************************
    dlsym returns pointer to void, so cast to pointer to FLOAT8
    This call to my_pht_user runs through firstime and initializes user_array
******************************************************************************/
    INT4 ret;
    PhtUserFunc = (PhtFunc) u_load_dylib_sym(f_str, "my_pht_user", &ret);
    if ((ret != 0) || (PhtUserFunc == 0)) {
      (void) u_error("PHTUSERINIT-BADUSER",
                      "Unable to find my_pht_user function", -1, 2);
      PhtUserFunc = 0;
    }

    return;
  }



/******* subroutine ***********************************************************/
  FLOAT8 pht_user(FLOAT8 *inc, FLOAT8 *ema, FLOAT8 *phase)
/*******************************************************************************
|                         ------------
|_Title subroutine_name - | pht_user |
|                         ------------
|_binding - pht_user in pht_user_bind_c.c
|
|_Args	Type	Variable	I/O	Description
|_Parm  
|_Parm
|_Parm	INT4	error;		O	Return code:
|					 0 - OK
|					-1 - error
|
|_Desc  
|
|_Keys  
|
|_Hist  dd mmm yy  K Teal Thompson - USGS/Flagstaff AZ; Original Version.
|       Feb 23 2004 Kris Becker - Rewrote for Mac Darwin port
|_End
|******************************************************************************/
{
   if (PhtUserFunc == 0) {
     (void) u_error("PHTUSER-NOFUNC", "No user function present", -2, 2);
     (void) u_std_exit();
   }
    return ((*PhtUserFunc) (*inc, *ema, *phase ));
  }

