c       ================================================================
	function pht_lunlam_emp(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_lunlam_emp  - Empirical Lunar (Lommel-Seeliger)-Lambert law
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle (deg)
        real*8       inc       !i       incidence angle (deg)
        real*8       phase     !i       phase angle (deg)
c
c_desc  Empirical Lunar (Lommel-Seeliger)-Lambert law.
c       Phase dependent and calculated empirically.
c	Limb-darkening LL and phase function are arbitrary polynomials in
c	  phase angle.
c	(May want to extend this to a polynomial with piecewise-linear
c	  opposition effect.)
c
c_hist  08 Jan 99  Randy Kirk Original Specs & Code
c       01 Mar 99  K Teal Thompson  Put code in separate file, declare
c                  vars, add implicit none.
c	29 Jul 99  KTT - Copy pht_lunlam_emp to pht_lunlam_pol and rewrite
c		   pht_lunlam_emp to use data points rather than poly-
c		   nomials.
c	01 Aug 99  KTT - Get rid of one letter variables.
c       ================================================================
#include "pht_common.inc"
        real*8       munot
        real*8       mu
        real*8       oldphase
        real*8       deg2rad
        real*8       pht_lunlam_emp

	parameter (deg2rad=0.01745329251994329576913d0)
	data oldphase /9999.d0/

	munot=cos(deg2rad*inc)
	mu=cos(deg2rad*ema)

	if (phase.ne.oldphase) then
	   call r8splint(llphase,llval,ll2,numllpoints,phase,ll)
	   if (numbeepoints.eq.0)then
	     bee=1.d0
	   else
	     call r8splint(bphase,bval,bee2,numbeepoints,phase,bee)
	   endif
	   oldphase=phase
	end if

	if ((munot.le.0.d0).or.(mu.le.0.d0)) then
	   pht_lunlam_emp=0.d0
	else if (ll.eq.0.d0) then
	   pht_lunlam_emp=bee*munot
	else if (ll.eq.1.d0) then
	   pht_lunlam_emp=bee*2.d0*munot/(munot+mu)
	else
	   pht_lunlam_emp=bee*munot*((1.d0-ll)+2.d0*ll/(munot+mu))
	end if

	return
	end
