c       ================================================================
	function pht_lunlam_pol(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_lunlam_pol  - Empirical Lunar (Lommel-Seeliger)-Lambert law
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle (deg)
        real*8       inc       !i       incidence angle (deg)
        real*8       phase     !i       phase angle (deg)
c
c_desc  Empirical Lunar (Lommel-Seeliger)-Lambert law.
c       Phase dependent and calculated empirically.
c	Limb-darkening L and phase function are arbitrary polynomials in
c	  phase angle.
c	(May want to extend this to a polynomial with piecewise-linear
c	  opposition effect.)
c
c_hist  08 Jan 99  Randy Kirk Original Specs & Code
c       01 Mar 99  K Teal Thompson  Put code in separate file, declare
c                  vars, add implicit none.
c       29 Jul 99  KTT - Copy pht_lunlam_emp to pht_lunlam_pol and rewrite
c                  pht_lunlam_emp.
c       01 Aug 99  KTT - Get rid of one letter variables.
c       ================================================================
#include "pht_common.inc"
	integer*4    i
        real*8       munot
        real*8       mu
        real*8       oldphase
        real*8       deg2rad
        real*8       pht_lunlam_pol

	parameter (deg2rad=0.01745329251994329576913d0)
	data oldphase /9999.d0/

	munot=cos(deg2rad*inc)
	mu=cos(deg2rad*ema)

	if (phase.ne.oldphase) then
	   ll=lcof(numlcoef)
	   do i=numlcoef-1,1,-1
	      ll=lcof(i)+phase*ll
	   enddo
	   bee=bcof(numbcoef)
	   do i=numbcoef-1,1,-1
	      bee=bcof(i)+phase*bee
	   enddo
	   oldphase=phase
	end if

	if ((munot.le.0.d0).or.(mu.le.0.d0)) then
	   pht_lunlam_pol=0.d0
	else if (ll.eq.0.d0) then
	   pht_lunlam_pol=bee*munot
	else if (ll.eq.1.d0) then
	   pht_lunlam_pol=bee*2.d0*munot/(munot+mu)
	else
	   pht_lunlam_pol=bee*munot*((1.d0-ll)+2.d0*ll/(munot+mu))
	end if

	return
	end
