c	================================================================
	subroutine pht_get_hapke_img(hapke_img,nl,ns,phase,emamax,
     .  doatm,iatm)
c	================================================================
        implicit none
c_title - pht_get_hapke_img
c
c_args  type    variable  i/o      description
	integer*4 nl
	integer*4 ns
	real*8 hapke_img(ns,nl)		! array for hapke image of sphere
	real*8	  phase			! current phase angle
	real*8	  emamax		! max emission angle needed
	logical*1 doatm			! include atmospheric effects?
	integer*4 iatm			! type of atmospheric scattering
				        ! 1 = 1st order isotropic
				        ! 2 = 2nd order isotropic
				        ! 3 = 1st order anisotropic
				        ! 4 = 2nd order anisotropic
				        ! 5 = 1st order Henyey-Greenstein
				        ! 6 = 2nd order Henyey-Greenstein
c
c_desc  Fills the buffer HAPKE_IMG with the Hapke-model shaded image of
c	a sphere with illumination from the left at the given PHASE.
c
c_hist  23 Nov 99  Randy L Kirk - USGS, Flagstaff; Original Code.
c	23 Dec 99  K Teal Thompson - Port to Unix; add implicit none.
c       24 Sep 02  Janet Barrett - Added support for 1st and 2nd order
c                  Henyey-Greenstein

c_end   ==================================================================
	integer*4 j,i
	real*8 pht_hapke
	real*8 inc,ema
	integer*4 iret		! error code: -1 = beyond edge of sphere
				! (not a fatal error, we just want to
				! ignore these parts of the buffer...)
	real*8 munot
	real*8 ahi
	real*8 pstd,trans,trans0,sbar
	real*8 deg2rad
	data deg2rad /0.01745329251994329576913d0/
#include "pht_common.inc"
c     INCLUDE 'pht_common.inc'

c	==================================================================
c	Pardon my cheezy error trap.  Probably not needed at all in this
c	one-time-use function.
c	==================================================================
	if (ns.ne.nl*2-1) stop 'Bad buffer dimensions.'

	do j=1,nl
	  do i=1,ns
	    call pht_get_angles(nl,ns,j,i,inc,ema,phase,iret)
	    if ((iret.eq.0).and.(ema.le.emamax)) then
	      hapke_img(i,j)=pht_hapke(inc,ema,phase)
	      if (doatm) then
	        if (iatm.eq.1) then
	          call atm_iso1(inc,ema,phase,pstd,trans,trans0,sbar)
	        else if (iatm.eq.2) then
	          call atm_iso2(inc,ema,phase,pstd,trans,trans0,sbar)
	        else if (iatm.eq.3) then
	          call atm_an11(inc,ema,phase,pstd,trans,trans0,sbar)
	        else if (iatm.eq.4) then
	          call atm_an12(inc,ema,phase,pstd,trans,trans0,sbar)
	        else if (iatm.eq.5) then
	          call atm_hhg1(inc,ema,phase,pstd,trans,trans0,sbar)
                else
	          call atm_hhg2(inc,ema,phase,pstd,trans,trans0,sbar)
	        end if
	        call r8splint(inctable,ahtable,ahtable2,ninc,inc,ahi)
	        munot=cos(deg2rad*inc)
	        hapke_img(i,j)=pstd+trans*munot*ahi/(1.0d0-ab*sbar)
     *                         +trans0*(hapke_img(i,j)-ahi*munot)
	      end if
	    end if
	  enddo
	enddo
	return
	end

