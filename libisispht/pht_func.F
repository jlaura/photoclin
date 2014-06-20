c       ================================================================
        function pht_func(inc,ema,phase)
c       ================================================================
	implicit none
 
c_title pht_function - get the photometric function
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle
        real*8       inc       !i       incidence angle
        real*8       phase     !i       phase angle
c
c_desc  Call the user's requested pht_func.  This subroutine is called
c	in get_ah_table.F mod_general.F & topder.F.
c
c_hist  19 Mar 99  K Teal Thompson - U.S.G.S. Flagstaff original version
c       13 Feb 03  Janet Barrett - Fixed a problem in which the subroutine
c                  was incorrectly comparing char_pht_function with the
c                  character string 'HAPKES' for doing smooth Hapke. The
c                  char_pht_function is now being compared with the 
c                  character strings 'HAPH_S' and 'HAPL_S' to determine if
c                  smooth Hapke should be performed.
c
c_end
#include "pht_common.inc"

        real*8     pht_func	!the function returned

        real*8     pht_lambert
        real*8     pht_lunlam_emp
        real*8     pht_lunlam_pol
        real*8     pht_hapke
        real*8     pht_lomsel
        real*8     pht_minnaert
        real*8     pht_minnaert_emp
        real*8     pht_minnaert_pol
        real*8     pht_hapke_s
        real*8     pht_lunlam
        real*8     pht_user
 
        if (char_pht_function.eq.'LUNLAM') 
     .    pht_func=pht_lunlam(inc,ema,phase)
        if (char_pht_function.eq.'LL_POL') 
     .    pht_func=pht_lunlam_pol(inc,ema,phase)
        if (char_pht_function.eq.'LL_EMP') 
     .    pht_func=pht_lunlam_emp(inc,ema,phase)
        if (char_pht_function.eq.'MIN') 
     .    pht_func=pht_minnaert(inc,ema,phase)
        if (char_pht_function.eq.'MN_POL') 
     .    pht_func=pht_minnaert_pol(inc,ema,phase)
        if (char_pht_function.eq.'MN_EMP') 
     .    pht_func=pht_minnaert_emp(inc,ema,phase)
        if (char_pht_function.eq.'HAPHEN')
     .    pht_func=pht_hapke(inc,ema,phase)
        if (char_pht_function.eq.'HAPLEG')
     .    pht_func=pht_hapke(inc,ema,phase)
        if (char_pht_function.eq.'HAPH_S')
     .    pht_func=pht_hapke_s(inc,ema,phase)
        if (char_pht_function.eq.'HAPL_S')
     .    pht_func=pht_hapke_s(inc,ema,phase)
        if (char_pht_function.eq.'LAMBER')
     .    pht_func=pht_lambert(inc,ema,phase)
        if (char_pht_function.eq.'LOMSEL')
     .    pht_func=pht_lomsel(inc,ema,phase)
	if (char_pht_function.eq.'USER')
     .    pht_func=pht_user(inc,ema,phase)

 
        return
        end
 
