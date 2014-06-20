c       ================================================================
	function pht_minnaert_emp(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_minnaert_emp  - Minnaert photometric model
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle (deg)
        real*8       inc       !i       incidence angle (deg)
        real*8       phase     !i       phase angle (deg)
c
c_desc  Empirical Minnaert law.
c       Phase dependent and calculated empirically.
c	Limb-darkening kay and phase function are arbitrary polynomials in
c	  phase angle.
c       albedo = brightness*[mu / (mu*munot)**kay)]
c
c_hist  08 Jan 99  Randy Kirk Original Specs & Code
c       01 Mar 99  K Teal Thompson  Put code in separate file, declare
c                  vars, add implicit none.
c       29 Jul 99  KTT - Copy pht_minnaert_emp to pht_minnaert_pol and
c                  rewrite pht_minnaert_emp to use data points rather
c                  than polynomials.
c       01 Aug 99  KTT - Get rid of one letter variables.
c       ================================================================
#include "pht_common.inc"
        real*8       munot
        real*8       mu
        real*8       oldphase
        real*8       deg2rad
        real*8       pht_minnaert_emp

	parameter (deg2rad=0.01745329251994329576913d0)
	data oldphase /9999.d0/

	munot=cos(deg2rad*inc)
	mu=cos(deg2rad*ema)

	if (phase.ne.oldphase) then
           call r8splint(kayphase,kayval,kay2,numkaypoints,phase,kay)
           if (numbeepoints.eq.0)then
             bee=1.d0
           else
             call r8splint(bphase,bval,bee2,numbeepoints,phase,bee)
	   endif
	   oldphase=phase
	end if
 
	if ((munot.le.0.d0).or.(mu.le.0.d0)) then
	   pht_minnaert_emp=0.d0
	else if (kay.eq.1.d0) then
	   pht_minnaert_emp=bee*munot
	else
	   pht_minnaert_emp=bee*munot*(munot*mu)**(kay-1)
	end if


	return
	end

