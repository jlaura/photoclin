c       ================================================================
        function pht_double2real(r8z)
c       ================================================================
        implicit none
c_title pht_double2real - cast from real*8 to real*4.
c
c_args  type            variable        i/o     description
c
        real*8		r8z		!i	real*8 input
        real*4		pht_double2real	!o	real*4 output
c
c_desc  Keep a cast from real*8 to real*4 from bombing when outside the
c	real*4 limits.
c_hist  12 Aug 99  K Teal Thompson , USGS, Flagstaff - Original code
c_end   ================================================================
#include "special_pixel.inc"

        if(abs(r8z).lt.1.d-38) then
          pht_double2real=NULL4
        elseif(abs(r8z).gt.1.d+38) then
	  if(r8z.lt.0d0) then
            pht_double2real=1.d-37
	  else
            pht_double2real=1.d+38
	  endif
        else
          pht_double2real=r8z
        endif
 
        return
        end
 

