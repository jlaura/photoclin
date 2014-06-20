

c	================================================================
	subroutine atm_iso1(inc,ema,phase,pstd,trans,trans0,sbar)
c	================================================================
        implicit none
c_title atm_iso1 - Isotropic atmos scattering in the 1st approximation
c
c_args  type  variable  i/o      description
 
	real*8  inc       !I       incidence angle
	real*8  ema       !i       emission angle
	real*8  phase     !i       phase angle
	real*8  pstd      !o       pure atmospheric-scattering term
	real*8  trans     !o       transmission of surface reflected
                        !        light through the atmosphere overall
	real*8  trans0    !o       transmission of surface reflected
                        !        light through the atmosphere with
                        !        no scatterings in the atmosphere
	real*8  sbar      !o       illumination of the ground by the sky
c
c_desc  Isotropic atmospheric scattering in the first approximation
c	The model for scattering for a general, non-Lambertian surface with
c	an atmosphere looks like this:
c
c       P = Pstd + trans*(rho*Ah*munot)/(1.d0-rho*Ab*sbar)
c     +     trans0*rho*(Psurf-Ah*munot)
c
c	where P is the overall photometric function (the model of the data),
c	PSTD is the pure atmospheric-scattering term, PSURF is the surface
c	photometric function, AH*MUNOT is a Lambertian approximation to this
c	with hemispheric albedo AH, TRANS and TRANS0 quantify transmission
c	of surface reflected light through the atmosphere overall and with
c	no scatterings in the atmosphere, and finally SBAR quantifies the
c	illumination of the ground by the sky.  RHO is the ratio of the sur-
c	face albedo to the albedo assumed in the functional form of PSURF.
c
c_hist  21 Dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       12 Mar 1999 K Teal Thompson  Port to Unix/ISIS; declare vars;
c	add implicit none.
c_end   ================================================================
C	     INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
c	Must access common blocks ATM_TABLES, APARS

	real*8 tauold
	real*8 whaold
	real*8 r8expint
	real*8 e2
	real*8 e3
	real*8 e4
	real*8 e5
	real*8 x0
	real*8 wha2
	real*8 munot
	real*8 mu
	real*8 xx
	real*8 emunot
	real*8 emu
	real*8 x
	real*8 xmunot
	real*8 y
	real*8 ymunot
	real*8 xmu
	real*8 ymu
	real*8 gmunot
	real*8 gmu
	real*8 y0
	real*8 alpha0
	real*8 delta
	real*8 fixcon
	real*8 beta0
	real*8 alpha1
	real*8 beta1
	real*8 alpha2
	real*8 beta2
	real*8 hpsq1
	real*8 munotp
c	real*8 hspq1
	real*8 mup
	real*8 fix
	real*8 gammax
	real*8 gammay
	real*8 deg2rad
	parameter (deg2rad=0.01745329251994329576913d0)
	data tauold,whaold /-1.d0,1.d30/


	if (tau.eq.0.d0) then
	   pstd=0.d0
	   trans=1.d0
	   trans0=1.d0
	   sbar=0.d0
	   return
	end if
	if ((tau.ne.tauold).or.(wha.ne.whaold)) then
c	preparation includes exponential integrals e sub 2 through 4
	   wha2=0.5d0*wha
	   e2=r8expint(2,tau)
	   e3=r8expint(3,tau)
	   e4=r8expint(4,tau)
c	zeroth moments of (uncorrected) x and y times characteristic fn
	   x0=wha2
	   y0=wha2*e2
c	higher-order correction term for x and y
	   delta=(1.d0-(x0+y0)-(1.d0-wha)/(1.d0-(x0-y0)))
     .	   /(wha*(0.5d0-e3))
c	moments of (corrected) x and y 
	   alpha0=1.d0+delta*(0.5d0-e3)
	   alpha1=0.5d0+delta*(0.3333333333333333333333d0-e4)
	   beta0=e2+delta*(0.5d0-e3)
	   beta1=e3+delta*(0.3333333333333333333333d0-e4)
c	prepare to find correct mixture of x and y in conservative case
	   if (wha.eq.1.d0) then
	      e5=r8expint(5,tau)
	      alpha2=0.3333333333333333333333d0+delta*(0.25d0-e5)
	      beta2=e4+delta*(0.25d0-e5)
	      fixcon=(beta0*tau-alpha1+beta1)
     .	     /((alpha1+beta1)*tau+2.d0*(alpha2+beta2))
	   end if
c	gamma will be a weighted sum of x and y functions
	   gammax=wha2*beta0
	   gammay=1.d0-wha2*alpha0
c	sbar is total diffuse illumination and comes from moments
	   sbar=1.d0-((2.d0-wha*alpha0)*alpha1+wha*beta0*beta1)
	   tauold=tau
	   whaold=wha
	end if
c	correct the path lengths for planetary curvature
	hpsq1=(1.d0+hnorm)**2.0d0-1.d0
	munot=cos(deg2rad*inc)
	munotp=hnorm/(sqrt(max(1.d-30,hpsq1+munot*munot))-munot)
	munotp=max(munotp,tau/69.0d0)
	mu=cos(deg2rad*ema)
	mup=hnorm/(sqrt(max(1.d-30,hpsq1+mu*mu))-mu)
	mup=max(mup,tau/69.0d0)
c	build the x and y functions of mu0 and mu
	xx=-tau/max(munotp,1.d-30)
	if (xx.lt.-69.d0) then
	   emunot=0.d0
	else if (xx.gt.69.d0) then
	   emunot=1.d30
	else
	   emunot=exp(-tau/munotp)
	end if
	xx=-tau/max(mup,1.d-30)
	if (xx.lt.-69.d0) then
	   emu=0.d0
	else if (xx.gt.69.d0) then
	   emu=1.d30
	else
	   emu=exp(-tau/mup)
	end if
	xmunot=1.d0+delta*munotp*(1.d0-emunot)
	ymunot=emunot+delta*munotp*(1.d0-emunot)
	xmu=1.d0+delta*mup*(1.d0-emu)
	ymu=emu+delta*mup*(1.d0-emu)
c	mix the x and y as required in the conservative case
	if (wha.eq.1.d0) then
	   x=xmunot
	   y=ymunot
	   fix=fixcon*munotp*(x+y)
	   xmunot=x+fix
	   ymunot=y+fix
	   x=xmu
	   y=ymu
	   fix=fixcon*mup*(x+y)
	   xmu=x+fix
	   ymu=y+fix
	end if
c	gamma1 functions come from x and y
	gmunot=gammax*xmunot+gammay*ymunot
	gmu=gammax*xmu+gammay*ymu
c	purely atmos term uses x and y, xmitted surface term uses gammas
	pstd=0.25d0*wha*munotp/(munotp+mup)*(xmunot*xmu-ymunot*ymu)
	trans=gmunot*gmu
c	finally, never-scattered term is given by pure attenuation
	trans0=emunot*emu
	return
	end



c	================================================================
	subroutine atm_iso2(inc,ema,phase,pstd,trans,trans0,sbar)
c	================================================================
	implicit none
c_title atm_iso2 - Isotropic atmos scattering in the 2nd approximation
c
c_args  type  variable  i/o      description
 
	real*8  inc       !i       incidence angle
	real*8  ema       !i       emission angle
	real*8  phase     !i       phase angle
	real*8  pstd      !o       pure atmospheric-scattering term
	real*8  trans     !o       transmission of surface reflected
                        !        light through the atmosphere overall
	real*8  trans0    !o       transmission of surface reflected
                        !        light through the atmosphere with
                        !        no scatterings in the atmosphere
	real*8  sbar      !o       illumination of the ground by the sky
c
c_desc  Isotropic atmospheric scattering in the first approximation
c       The model for scattering for a general, non-Lambertian surface with
c       an atmosphere looks like this:
c 
c       P = Pstd + trans*(rho*Ah*munot)/(1.d0-rho*Ab*sbar)
c     +     trans0*rho*(psurf-Ah*munot)
c
c       where P is the overall photometric function (the model of the data),
c       PSTD is the pure atmospheric-scattering term, PSURF is the surface
c       photometric function, AH*MUNOT is a Lambertian approximation to this
c       with hemispheric albedo AH, TRANS and TRANS0 quantify transmission
c       of surface reflected light through the atmosphere overall and with
c       no scatterings in the atmosphere, and finally SBAR quantifies the
c       illumination of the ground by the sky.  RHO is the ratio of the sur-
c       face albedo to the albedo assumed in the functional form of PSURF.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       12 Mar 1999 K Teal Thompson  Port to Unix/ISIS; declare vars;
c	add implicit none.
c_end   ================================================================
C	     INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
c	Must access common blocks ATM_TABLES, APARS

	real*8 tauold
	real*8 whaold
	real*8 e1
	real*8 e1_2
	real*8 e2
	real*8 e3
	real*8 e4
	real*8 r8expint
	real*8 xx
	real*8 em
	real*8 e
	real*8 g11p
	real*8 g11prime
	real*8 x0
	real*8 wha2
	real*8 g12
	real*8 e5
	real*8 munot
	real*8 mu
	real*8 emunot
	real*8 emu
	real*8 munotp
	real*8 f1munot
	real*8 f1
	real*8 f2
	real*8 f3
	real*8 f4
	real*8 mup
	real*8 x
	real*8 xmunot
	real*8 y
	real*8 ymunot
	real*8 xmu
	real*8 ymu
	real*8 gmunot
	real*8 f1m
	real*8 f2m
	real*8 f3m
	real*8 f4m
	real*8 g12p
	real*8 g13p
	real*8 g14p
	real*8 g13
	real*8 g14
	real*8 delta
	real*8 y0
	real*8 fixcon
	real*8 alpha0
	real*8 alpha1
	real*8 alpha2
	real*8 beta0
	real*8 beta1
	real*8 beta2
	real*8 gammay
	real*8 hpsq1
	real*8 f1mmunot
	real*8 fix
	real*8 gmu
	real*8 gammax
	real*8 f1mmu
	real*8 f1mu
	real*8 r8ei
	real*8 deg2rad
	parameter (deg2rad=0.01745329251994329576913d0)
	real*8 eulgam
	parameter (eulgam=0.5772156d0)
	data tauold,whaold /-1.d0,1.d30/


	if (tau.eq.0.d0) then
	   pstd=0.d0
	   trans=1.d0
	   trans0=1.d0
	   sbar=0.d0
	   return
	end if
	if ((tau.ne.tauold).or.(wha.ne.whaold)) then
c	preparation includes exponential integrals e sub 2 through 4
	   wha2=0.5d0*wha
	   e1=r8expint(1,tau)
	   e1_2=r8expint(1,2.d0*tau)
	   e2=r8expint(2,tau)
	   e3=r8expint(3,tau)
	   e4=r8expint(4,tau)
c	chandra's gmn functions require fm and fn at mu=-1
	   xx=-tau
	   if (xx.lt.-69.d0) then
	      em=0.d0
	   else if (xx.gt.69.d0) then
	      em=1.d30
	   else
	      em=exp(xx)
	   end if
	   f1m=log(2.d0)-em*e1+e1_2
	   f2m=-1.d0*(f1m+em*e2-1.d0)
	   f3m=-1.d0*(f2m+em*e3-0.5d0)
	   g12=(tau*e1*e2+f1m+f2m)*0.5d0
	   g13=(tau*e1*e3+f1m+f3m)*0.3333333333333333333333d0
c	chandra's g'mn functions require g'11 and f at mu=+1
	   xx=tau
	   if (xx.lt.-69.d0) then
	      e=0.d0
	   else if (xx.gt.69.d0) then
	      e=1.d30
	   else
	      e=exp(xx)
	   end if
	   f1=eulgam+log(tau)+e*e1
	   f2=f1+e*e2-1.d0
	   f3=f2+e*e3-0.5d0
	   g11p=g11prime(tau)!evaluate series as function
	   g12p=(tau*(e1-g11p)+em*(f1+f2))*0.25d0
	   g13p=(tau*(0.5d0*e1-g12p)+em*(f1+f3))*0.2d0
c	zeroth moments of (uncorrected) x and y times characteristic fn
	   x0=wha2*(1.d0+wha2*g12)
	   y0=wha2*(e2+wha2*g12p)
c	higher-order correction term for x and y
	   delta=(1.d0-(x0+y0)-(1.d0-wha)/(1.d0-(x0-y0)))
     *	  /(wha*(0.5d0-e3))
c	moments of (corrected) x and y 
	   alpha0=1.d0+wha2*g12+delta*(0.5d0-e3)
	   alpha1=0.5d0+wha2*g13+delta*(0.3333333333333333333333d0-e4)
	   beta0=e2+wha2*g12p+delta*(0.5d0-e3)
	   beta1=e3+wha2*g13p+delta*(0.3333333333333333333333d0-e4)
c	prepare to find correct mixture of x and y in conservative case
	   if (wha.eq.1.d0) then
	      e5=r8expint(5,tau)
	      f4m=-1.d0*(f3m+em*e4-0.3333333333333333333333d0)
	      g14=(tau*e1*e4+f1m+f4m)*0.25d0
	      f4=f3+e*e4-0.3333333333333333333333d0
	      g14p=(tau*(0.5d0*e1-g13p)+em*(f1+f4))*0.1666666d0
	      alpha2=0.3333333333333333333333d0+wha2*g14 +
     .          delta*(0.25d0-e5)
	      beta2=e4+wha2*g14p+delta*(0.25d0-e5)
	      fixcon=(beta0*tau-alpha1+beta1)
     .	     /((alpha1+beta1)*tau+2.d0*(alpha2+beta2))
	   end if
c	gamma will be a weighted sum of x and y functions
	   gammax=wha2*beta0
	   gammay=1.d0-wha2*alpha0
c	sbar is total diffuse illumination and comes from moments
	   sbar=1.d0-((2.d0-wha*alpha0)*alpha1+wha*beta0*beta1)
	   tauold=tau
	   whaold=wha
	end if
c	correct the path lengths for planetary curvature
	hpsq1=(1.d0+hnorm)**2.0d0-1.d0
	munot=cos(deg2rad*inc)
	munotp=hnorm/(sqrt(max(1.d-30,hpsq1+munot*munot))-munot)
	munotp=max(munotp,tau/69.0d0)
	mu=cos(deg2rad*ema)
	mup=hnorm/(sqrt(max(1.d-30,hpsq1+mu*mu))-mu)
	mup=max(mup,tau/69.0d0)
c	build the x and y functions of mu0 and mu
	xx=-tau/max(munotp,1.d-30)
	if (xx.lt.-69.d0) then
	   emunot=0.d0
	else if (xx.gt.69.d0) then
	   emunot=1.d30
	else
	   emunot=exp(-tau/munotp)
	end if
	xx=-tau/max(mup,1.d-30)
	if (xx.lt.-69.d0) then
	   emu=0.d0
	else if (xx.gt.69.d0) then
	   emu=1.d30
	else
	   emu=exp(-tau/mup)
	end if
c	in the second approximation the x and y include the f1 function
	xx=munotp
	if (abs(xx-1.0d0).lt.1.0d-10) then
	   f1munot=f1
	   f1mmunot=xx*(log(1.d0+1.d0/xx)-e1*emunot+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else if (xx.gt.0.d0) then
c	   ei() is from numerical recipes.
	   f1munot=xx*(log(xx/(1.d0-xx))+e1/emunot+
     .              r8ei(tau*(1.d0/xx-1.d0)))
	   f1mmunot=xx*(log(1.d0+1.d0/xx)-e1*emunot+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else
	   pause 'negative munot in f1 function'
	end if
	xx=mup
	if (abs(xx-1.0d0).lt.1.0d-10) then
	   f1mu=f1
	   f1mmu=xx*(log(1.d0+1.d0/xx)-e1*emu+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else if (xx.gt.0.d0) then
c	   ei() is from numerical recipes.
	   f1mu=xx*(log(xx/(1.d0-xx))+e1/emu+
     .              r8ei(tau*(1.d0/xx-1.d0)))
	   f1mmu=xx*(log(1.d0+1.d0/xx)-e1*emu+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else
	   pause 'negative mu in f1 function'
	end if
	xmunot=1.d0+wha2*f1mmunot+delta*munotp*(1.d0-emunot)
	ymunot=emunot*(1.d0+wha2*f1munot)+delta*munotp*(1.d0-emunot)
	xmu=1.d0+wha2*f1mmu+delta*mup*(1.d0-emu)
	ymu=emu*(1.d0+wha2*f1mu)+delta*mup*(1.d0-emu)
c	mix the x and y as required in the conservative case
	if (wha.eq.1.d0) then
	   x=xmunot
	   y=ymunot
	   fix=fixcon*munotp*(x+y)
	   xmunot=x+fix
	   ymunot=y+fix
	   x=xmu
	   y=ymu
	   fix=fixcon*mup*(x+y)
	   xmu=x+fix
	   ymu=y+fix
	end if
c	gamma1 functions come from x and y
	gmunot=gammax*xmunot+gammay*ymunot
	gmu=gammax*xmu+gammay*ymu
c	purely atmos term uses x and y, xmitted surface term uses gammas
	pstd=0.25d0*wha*munotp/(munotp+mup)*(xmunot*xmu-ymunot*ymu)
	trans=gmunot*gmu
c	finally, never-scattered term is given by pure attenuation
	trans0=emunot*emu
	return
	end


c	================================================================
	function g11prime(tau)
c	================================================================
	implicit none

c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       12 Mar 1999 K Teal Thompson  Port to Unix/ISIS; declare vars;
c	add implicit none.
c	================================================================


c	chandra and van de hulst's series approximation for the g'11

c	function needed in second order scattering theory.

	real*8 sum
	real*8 fac
	real*8 tau
	real*8 fi
	real*8 term
	real*8 tol
	real*8 eulgam
	real*8 r8expint
	real*8 e1_2
	real*8 elog
	real*8 pi
	integer*4 icnt
	parameter (pi=3.1415926535897932384626433832795d0)
	parameter (eulgam=0.5772156d0,tol=1.d-6)

	real*8 g11prime

	sum=0.d0
	icnt=1
	fac=-tau
	term=fac
	do while (abs(term).gt.abs(sum)*tol)
	   sum=sum+term
	   icnt=icnt+1
	   fi=float(icnt)
	   fac=fac*(-tau)/fi
	   term=fac/(fi*fi)
	enddo
	elog=log(max(1.d-30,tau))+eulgam
	e1_2=sum+pi*pi/12.d0+0.5d0*elog**2.0d0
	g11prime=2.d0*(r8expint(1,tau)+elog*r8expint(2,tau)-tau*e1_2)
	return
	end !======function=g11prime()==================================



c	================================================================
	subroutine atm_an11(inc,ema,phase,pstd,trans,trans0,sbar)
c	================================================================
	implicit none
c_title atm_an11 - Anisotropic (legendre) atmos scattering in the 1st
c	approximation.
c
c_args  type  variable  i/o      description
 
	real*8  inc       !i       incidence angle
	real*8  ema       !i       emission angle
	real*8  phase     !i       phase angle
	real*8  pstd      !o       pure atmospheric-scattering term
	real*8  trans     !o       transmission of surface reflected
                        !        light through the atmosphere overall
	real*8  trans0    !o       transmission of surface reflected
                        !        light through the atmosphere with
                        !        no scatterings in the atmosphere
	real*8  sbar      !o       illumination of the ground by the sky
c
c_desc  Anisotropic atmospheric scattering with P1 single-particle phase
c	fn, in the first approximation.  This subroutine goes through
c	much of the derivation twice, once for the axisymmetric (m=0)
c	and once for the m=1 parts of scattered light.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code.
c       12 Mar 1999 K Teal Thompson  Port to Unix/ISIS; declare vars;
c	add implicit none
c_end   ================================================================
C	     INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
c	Must access common blocks ATM_TABLES, APARS

	real*8 tauold
	real*8 whaold
	real*8 e2
	real*8 e3
	real*8 e4
	real*8 e5
	real*8 r8expint
	real*8 q02p02
	real*8 q0
	real*8 p0
	real*8 q1
	real*8 wham
	real*8 fac
	real*8 den
	real*8 munot
	real*8 mu
	real*8 xx
	real*8 emunot
	real*8 emu
	real*8 gmunot
	real*8 gmu
	real*8 wha2
	real*8 delta_0
	real*8 x0_0
	real*8 y0_0
	real*8 alpha0_0
	real*8 alpha1_0
	real*8 beta0_0
	real*8 beta1_0
	real*8 q12p12
	real*8 p1
	real*8 hpsq1
	real*8 munotp
c	real*8 hspq1
	real*8 mup
	real*8 sum
	real*8 prod
	real*8 cosazss
	real*8 x0_1
	real*8 y0_1
	real*8 xmunot_0
	real*8 ymunot_0
	real*8 xmu_0
	real*8 ymu_0
	real*8 xmunot_1
	real*8 ymunot_1
	real*8 xmu_1
	real*8 ymu_1
	real*8 delta_1
	real*8 cxx
	real*8 cyy
	real*8 xystuff
	real*8 deg2rad
	parameter (deg2rad=0.01745329251994329576913d0)
	data tauold,whaold /-1.d0,1.d30/


        if (bha.eq.0.d0) bha=1.d-6
        if (tau.eq.0.d0) then
           pstd=0.d0
           trans=1.d0
           trans0=1.d0
           sbar=0.d0
           return
        end if
        if (wha.eq.1.d0)
     * pause 'anisotropic       conservative case not implemented yet.'
        if ((tau.ne.tauold).or.(wha.ne.whaold)) then
c       preparation includes exponential integrals e sub 2 through 5
           wha2=0.5d0*wha
           wham=1.d0-wha
           e2=r8expint(2,tau)
           e3=r8expint(3,tau)
           e4=r8expint(4,tau)
           e5=r8expint(5,tau)
c          first, get the required quantities for the axisymmetric m=0 part
c          zeroth moments of (uncorrected) x and y times characteristic fn
           x0_0=wha2*(1.d0+0.3333333333333333333333d0*bha*wham)
           y0_0=wha2*(e2+bha*wham*e4)
c          higher-order correction term for x and y
           delta_0=(1.d0-(x0_0+y0_0)
     *       -(1.d0-wha*(1.d0+0.3333333333333333333333d0*bha*wham))/
     .       (1.d0-(x0_0-y0_0)))
     *      /(wha*(0.5d0-e3+bha*wham*(0.25d0-e5)))
c          moments of (corrected) x and y
           alpha0_0=1.d0+delta_0*(0.5d0-e3)
           alpha1_0=0.5d0+delta_0*(0.3333333333333333333333d0-e4)
           beta0_0=e2+delta_0*(0.5d0-e3)
           beta1_0=e3+delta_0*(0.3333333333333333333333d0-e4)
c          gamma will be a weighted sum of m=0 x and y functions
c          but unlike before, call the weights q1 and p1, and we also
c	   need additional weights q0 and p0
	   fac=2.d0-wha*alpha0_0
	   den=fac**2.0d0-(wha*beta0_0)**2.0d0
	   q0=bha*wha*wham*(fac*alpha1_0-wha*beta0_0*beta1_0)/den
	   p0=bha*wha*wham*(-fac*beta1_0-wha*beta0_0*alpha1_0)/den
	   q02p02=q0*q0-p0*p0
	   q1=(2.d0*wham*fac)/den
	   p1=(2.d0*wham*wha*beta0_0)/den
	   q12p12=q1*q1-p1*p1
c	   sbar is total diffuse illumination and comes from moments
	   sbar=1.d0-2.d0*(q1*alpha1_0+p1*beta1_0)
c	   we're not done yet!  still have to calculate the m=1 portion
c	   zeroth moments of (uncorrected) x and y times characteristic fn
	   x0_1=0.5d0*wha2*bha*(1.d0-0.3333333333333333333333d0)
	   y0_1=0.5d0*wha2*bha*(e2-e4)
c	   higher-order correction term for x and y
	   delta_1=(1.d0-(x0_1+y0_1)
     *	     -(1.d0-0.3333333333333333333333d0*wha*bha)/
     .       (1.d0-(x0_1-y0_1)))
     *	     /(wha2*bha*((0.5d0-0.25d0)-(e3-e5)))
c	   moments of (corrected) x and y are not needed for m=1, so we're done
	   tauold=tau
	   whaold=wha
	end if
c	correct the path lengths for planetary curvature
	hpsq1=(1.d0+hnorm)**2.0d0-1.d0
	munot=dcos(deg2rad*inc)
	munotp=hnorm/(sqrt(max(1.d-30,hpsq1+munot*munot))-munot)
	munotp=max(munotp,tau/69.0d0)
	mu=dcos(deg2rad*ema)
	mup=hnorm/(sqrt(max(1.d-30,hpsq1+mu*mu))-mu)
	mup=max(mup,tau/69.0d0)
c	build the x and y functions of mu0 and mu
	xx=-tau/max(munotp,1.d-30)
	if (xx.lt.-69.d0) then
	   emunot=0.d0
	else if (xx.gt.69.d0) then
	   emunot=1.d30
	else
	   emunot=dexp(-tau/munotp)
	end if
	xx=-tau/max(mup,1.d-30)
	if (xx.lt.-69.d0) then
	   emu=0.d0
	else if (xx.gt.69.d0) then
	   emu=1.d30
	else
	   emu=dexp(-tau/mup)
	end if
c	first for m=0
	xmunot_0=1.d0+delta_0*munotp*(1.d0-emunot)
	ymunot_0=emunot+delta_0*munotp*(1.d0-emunot)
	xmu_0=1.d0+delta_0*mup*(1.d0-emu)
	ymu_0=emu+delta_0*mup*(1.d0-emu)
c	then for m=1
	xmunot_1=1.d0+delta_1*munotp*(1.d0-emunot)
	ymunot_1=emunot+delta_1*munotp*(1.d0-emunot)
	xmu_1=1.d0+delta_1*mup*(1.d0-emu)
	ymu_1=emu+delta_1*mup*(1.d0-emu)
c	gamma1 functions come from x and y with m=0
	gmunot=p1*xmunot_0+q1*ymunot_0
	gmu=p1*xmu_0+q1*ymu_0
c	purely atmos term uses x and y of both orders and is complex
	sum=munot+mu
	prod=munot*mu
	cxx=1.d0-q0*sum+(q02p02-bha*q12p12)*prod
	cyy=1.d0+q0*sum+(q02p02-bha*q12p12)*prod
	cosazss=dcos(deg2rad*phase)-munot*mu
	xystuff=cxx*xmunot_0*xmu_0-cyy*ymunot_0*ymu_0
     *	 -p0*sum*(xmu_0*ymunot_0+ymu_0*xmunot_0)
     *	 +cosazss*bha*(xmu_1*xmunot_1-ymu_1*ymunot_1)

	pstd=0.25d0*wha*munotp/(munotp+mup)*xystuff
c	xmitted surface term uses gammas from m=0
	trans=gmunot*gmu
c	finally, never-scattered term is given by pure attenuation
	trans0=emunot*emu
	return
	end



c	================================================================
	subroutine atm_an12(inc,ema,phase,pstd,trans,trans0,sbar)
c	================================================================
	implicit none
c_title atm_an11 - Anisotropic (legendre) atmos scattering in the 2nd
c	approximation.
c
c_args  type  variable  i/o      description
 
	real*8  inc       !i       incidence angle
	real*8  ema       !i       emission angle
	real*8  phase     !i       phase angle
	real*8  pstd      !o       pure atmospheric-scattering term
	real*8  trans     !o       transmission of surface reflected
                        !        light through the atmosphere overall
	real*8  trans0    !o       transmission of surface reflected
                        !        light through the atmosphere with
                        !        no scatterings in the atmosphere
	real*8  sbar      !o       illumination of the ground by the sky
c
c_desc  Anisotropic atmospheric scattering with P1 single-particle
c	phase fn, in the second approximation.  This subroutine goes
c	through much of the derivation twice, once for the axisymmetric
c	(m=0) and once for the m=1 parts of scattered light.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code.
c       12 Mar 1999 K Teal Thompson  Port to Unix/ISIS; declare vars;
c	add implicit none
c_end   ================================================================
C	     INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
c	Must access common blocks ATM_TABLES, APARS

	real*8 tauold
	real*8 whaold
	real*8 e1
	real*8 e1_2
	real*8 e2
	real*8 e3
	real*8 e4
	real*8 e5
	real*8 r8expint
	real*8 xx
	real*8 em
	real*8 e
	real*8 g11prime
	real*8 q02p02
	real*8 q0
	real*8 p0
	real*8 q1
	real*8 wham
	real*8 fac
	real*8 den
	real*8 munot
	real*8 mu
	real*8 emunot
	real*8 emu
	real*8 munotp
	real*8 mup
	real*8 gmunot
	real*8 gmu
	real*8 wha2
	real*8 f1m
	real*8 f3m
	real*8 f2m
	real*8 f1
	real*8 f2
	real*8 f3
	real*8 g11p
	real*8 g13p
	real*8 g12p
	real*8 delta_0
	real*8 x0_0
	real*8 y0_0
	real*8 alpha0_0
	real*8 alpha1_0
	real*8 beta0_0
	real*8 beta1_0
	real*8 q12p12
	real*8 p1
	real*8 hpsq1
c	real*8 hspq1
	real*8 f1munot
	real*8 f2munot
	real*8 f3munot
	real*8 f1mmunot
	real*8 f2mmunot
	real*8 f3mmunot
	real*8 f3mu
	real*8 f2mu
	real*8 f1mu
	real*8 f3mmu
	real*8 f2mmu
	real*8 f1mmu
	real*8 sum
	real*8 prod
	real*8 cosazss
	real*8 xystuff
	real*8 f4m
	real*8 g12
	real*8 g13
	real*8 g32
	real*8 g33
	real*8 g32p
	real*8 g34p
	real*8 g33p
	real*8 f4
	real*8 delta_1
	real*8 x0_1
	real*8 y0_1
	real*8 xmunot_0
	real*8 xmu_0
	real*8 ymu_0
	real*8 cxx
c	real*8 q02po2
	real*8 g14
	real*8 g34
	real*8 g14p
	real*8 ymunot_0
	real*8 xmunot_1
	real*8 ymunot_1
	real*8 xmu_1
	real*8 ymu_1
	real*8 cyy
	real*8 r8ei
	real*8 deg2rad
	parameter (deg2rad=0.01745329251994329576913d0)
	real*8 eulgam
	parameter (eulgam=0.5772156d0)
	data tauold,whaold /-1.d0,1.d30/

        if (bha.eq.0.d0) bha=1.d-6
	if (tau.eq.0.d0) then
	   pstd=0.d0
	   trans=1.d0
	   trans0=1.d0
	   sbar=0.d0
	   return
	end if
	if (wha.eq.1.d0) 
     * pause 'anisotropic	conservative case not implemented yet.'
	if ((tau.ne.tauold).or.(wha.ne.whaold)) then
c	preparation includes exponential integrals e sub 2 through 5
	   wha2=0.5d0*wha
	   wham=1.d0-wha
	   e1=r8expint(1,tau)
	   e1_2=r8expint(1,2.d0*tau)
	   e2=r8expint(2,tau)
	   e3=r8expint(3,tau)
	   e4=r8expint(4,tau)
	   e5=r8expint(5,tau)
c	chandra's gmn functions require fm and fn at mu=-1
	   xx=-tau
	   if (xx.lt.-69.d0) then
	      em=0.d0
	   else if (xx.gt.69.d0) then
	      em=1.d30
	   else
	      em=exp(xx)
	   end if
	   f1m=log(2.d0)-em*e1+e1_2
	   f2m=-1.d0*(f1m+em*e2-1.d0)
	   f3m=-1.d0*(f2m+em*e3-0.5d0)
	   f4m=-1.d0*(f3m+em*e4-0.3333333333333333333333d0)
	   g12=(tau*e1*e2+f1m+f2m)*0.5d0
	   g13=(tau*e1*e3+f1m+f3m)*0.3333333333333333333333d0
	   g14=(tau*e1*e4+f1m+f4m)*0.25d0
	   g32=(tau*e3*e2+f3m+f2m)*0.25d0
	   g33=(tau*e3*e3+f3m+f3m)*0.2d0
	   g34=(tau*e3*e4+f3m*f4m)*0.1666666666666d0
c	chandra's g'mn functions require g'11 and f at mu=+1
	   xx=tau
	   if (xx.lt.-69.d0) then
	      e=0.d0
	   else if (xx.gt.69.d0) then
	      e=1.d30
	   else
	      e=exp(xx)
	   end if
	   f1=eulgam+log(tau)+e*e1
	   f2=f1+e*e2-1.d0
	   f3=f2+e*e3-0.5d0
	   f4=f3+e*e4-0.3333333333333333333333d0
	   g11p=g11prime(tau)!evaluate series as function
	   g12p=(tau*(e1-g11p)+em*(f1+f2))*0.25d0
	   g13p=(tau*(0.5d0*e1-g12p)+em*(f1+f3))*0.2d0
	   g14p=(tau*(0.3333333333333333333333d0*e1-g13p)+
     .          em*(f1+f4))*0.1666666666666d0
	   g32p=(tau*(e1-g13p)+em*(f3+f2))*0.1666666666666d0
	   g33p=(tau*(0.5d0*e1-g32p)+em*(f3+f3))*0.142857d0
	   g34p=(tau*(0.3333333333333333333333d0*e1-g33p)+
     .		em*(f3+f4))*0.125d0
c	first, get the required quantities for the axisymmetric m=0 part
c	zeroth moments of (uncorrected) x and y times characteristic fn
	   x0_0=wha2*(1.d0+0.3333333333333333333333d0*
     .       bha*wham+wha2*(g12+bha*wham*(g14+g32)
     *	     +bha**2.0d0*wham**2.0d0*g34))
	   y0_0=wha2*(e2+bha*wham*e4+wha2*(g12p+bha*wham*(g14p+g32p)
     *	     +bha**2.0d0*wham**2.0d0*g34p))
c	higher-order correction term for x and y
	   delta_0=(1.d0-(x0_0+y0_0)
     *	     -(1.d0-wha*(1.d0+0.3333333333333333333333d0*
     .       bha*wham))/(1.d0-(x0_0-y0_0)))
     *	     /(wha*(0.5d0-e3+bha*wham*(0.25d0-e5)))
c	moments of (corrected) x and y 
	   alpha0_0=1.d0+wha2*(g12+bha*wham*g32)+delta_0*(0.5d0-e3)
	   alpha1_0=0.5d0+wha2*(g13+bha*wham*g33)+delta_0*
     .       (0.3333333333333333333333d0-e4)
	   beta0_0=e2+wha2*(g12p+bha*wham*g32p)+delta_0*(0.5d0-e3)
	   beta1_0=e3+wha2*(g13p+bha*wham*g33p)+delta_0*
     .       (0.3333333333333333333333d0-e4)
c	gamma will be a weighted sum of m=0 x and y functions
c	but unlike before, call the weights q1 and p1, and we also
c	need additional weights q0 and p0
	   fac=2.d0-wha*alpha0_0
	   den=fac**2.0d0-(wha*beta0_0)**2.0d0
	   q0=bha*wha*wham*(fac*alpha1_0-wha*beta0_0*beta1_0)/den
	   p0=bha*wha*wham*(-fac*beta1_0-wha*beta0_0*alpha1_0)/den
	   q02p02=q0*q0-p0*p0
	   q1=(2.d0*wham*fac)/den
	   p1=(2.d0*wham*wha*beta0_0)/den
	   q12p12=q1*q1-p1*p1
c	sbar is total diffuse illumination and comes from moments
	   sbar=1.d0-2.d0*(q1*alpha1_0+p1*beta1_0)
c	we're not done yet!  still have to calculate the m=1 portion
c	zeroth moments of (uncorrected) x and y times characteristic fn
	   x0_1=0.5d0*wha2*bha*(1.d0-0.3333333333333333333333d0+
     .        0.5d0*wha2*bha
     *	      *(g12-(g14+g32)+g34))
	   y0_1=0.5d0*wha2*bha*(e2-e4+0.5d0*wha2*bha
     *	      *(g12p-(g14p+g32p)+g34p))
c	higher-order correction term for x and y
	   delta_1=(1.d0-(x0_1+y0_1)
     *	     -(1.d0-0.3333333333333333333333d0*wha*bha)/
     .       (1.d0-(x0_1-y0_1)))
     *	     /(wha2*bha*((0.5d0-0.25d0)-(e3-e5)))
c	moments of (corrected) x and y are not needed for m=1, so we're done
	   tauold=tau
	   whaold=wha
	end if
c	correct the path lengths for planetary curvature
	hpsq1=(1.d0+hnorm)**2.0d0-1.d0
	munot=cos(deg2rad*inc)
	munotp=hnorm/(sqrt(max(1.d-30,hpsq1+munot*munot))-munot)
	munotp=max(munotp,tau/69.0d0)
	mu=cos(deg2rad*ema)
	mup=hnorm/(sqrt(max(1.d-30,hpsq1+mu*mu))-mu)
	mup=max(mup,tau/69.d0)
c	build the x and y functions of mu0 and mu
	xx=-tau/max(munotp,1.d-30)
	if (xx.lt.-69.d0) then
	   emunot=0.d0
	else if (xx.gt.69.d0) then
	   emunot=1.d30
	else
	   emunot=exp(-tau/munotp)
	end if
	xx=-tau/max(mup,1.d-30)
	if (xx.lt.-69.d0) then
	   emu=0.d0
	else if (xx.gt.69.d0) then
	   emu=1.d30
	else
	   emu=exp(-tau/mup)
	end if
c	in the second approximation the x and y include the f1, f3 functions
	xx=munotp
	if (abs(xx-1.0d0).lt.1.0d-10) then
           f1munot=f1
           f1mmunot=xx*(log(1.d0+1.d0/xx)-e1*emunot+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
        else if (xx.gt.0.d0) then
c          ei() is from numerical recipes.
           f1munot=xx*(log(xx/(1.d0-xx))+e1/emunot+
     .              r8ei(tau*(1.d0/xx-1.d0)))
           f1mmunot=xx*(log(1.d0+1.d0/xx)-e1*emunot+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else
	   pause 'negative munot in f1 function'
	end if
	f2munot=munotp*(f1munot+e2/emunot-1)
	f2mmunot=-munotp*(f1mmunot+e2*emunot-1)
	f3munot=munotp*(f2munot+e3/emunot-0.5d0)
	f3mmunot=-munotp*(f2mmunot+e3*emunot-0.5d0)
	xx=mup
        if (abs(xx-1.0d0).lt.1.0d-10) then
           f1mu=f1
           f1mmu=xx*(log(1.d0+1.d0/xx)-e1*emu+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
        else if (xx.gt.0.d0) then
c          ei() is from numerical recipes.
           f1mu=xx*(log(xx/(1.d0-xx))+e1/emu+
     .              r8ei(tau*(1.d0/xx-1.d0)))
           f1mmu=xx*(log(1.d0+1.d0/xx)-e1*emu+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else
	   pause 'negative mu in f1 function'
	end if
	f2mu=mup*(f1mu+e2/emu-1)
	f2mmu=-mup*(f1mmu+e2*emu-1)
	f3mu=mup*(f2mu+e3/emu-0.5d0)
	f3mmu=-mup*(f2mmu+e3*emu-0.5d0)
c	first for m=0
	xmunot_0=1.d0+wha2*(f1mmunot+bha*wham*f3mmunot)
     *	 +delta_0*munotp*(1.d0-emunot)
	ymunot_0=emunot*(1.d0+wha2*(f1munot+bha*wham*f3munot))
     *	 +delta_0*munotp*(1.d0-emunot)
	xmu_0=1.d0+wha2*(f1mmu+bha*wham*f3mmu)
     *     +delta_0*mup*(1.d0-emu)
	ymu_0=emu*(1.d0+wha2*(f1mu+bha*wham*f3mu))
     *     +delta_0*mup*(1.d0-emu)
c	then for m=1
	xmunot_1=1.d0+0.5d0*wha2*bha*(f1mmunot-f3mmunot)
     *	  +delta_1*munotp*(1.d0-emunot)
	ymunot_1=emunot*(1.d0+0.5d0*wha2*bha*(f1munot-f3munot))
     *	  +delta_1*munotp*(1.d0-emunot)
	xmu_1=1.d0+0.5d0*wha2*bha*(f1mmu-f3mmu)
     *     +delta_1*mup*(1.d0-emu)
	ymu_1=emu*(1.d0+0.5d0*wha2*bha*(f1mu-f3mu))
     *     +delta_1*mup*(1.d0-emu)
c	gamma1 functions come from x and y with m=0
	gmunot=p1*xmunot_0+q1*ymunot_0
	gmu=p1*xmu_0+q1*ymu_0
c	purely atmos term uses x and y of both orders and is complex
	sum=munot+mu
	prod=munot*mu
	cxx=1.d0-q0*sum+(q02p02-bha*q12p12)*prod
	cyy=1.d0+q0*sum+(q02p02-bha*q12p12)*prod
	cosazss=cos(deg2rad*phase)-munot*mu
	xystuff=cxx*xmunot_0*xmu_0-cyy*ymunot_0*ymu_0
     *	 -p0*sum*(xmu_0*ymunot_0+ymu_0*xmunot_0)
     *	 +cosazss*bha*(xmu_1*xmunot_1-ymu_1*ymunot_1)
	pstd=0.25d0*wha*munotp/(munotp+mup)*xystuff
c	xmitted surface term uses gammas from m=0
	trans=gmunot*gmu
c	finally, never-scattered term is given by pure attenuation
	trans0=emunot*emu
	return
	end



c	================================================================
	subroutine atm_hhg1(inc,ema,phase,pstd,trans,trans0,sbar)
c	================================================================
        implicit none
c_title atm_hhg1 - Henyey-Greenstein atmos scattering in the 1st approximation
c
c_args  type  variable  i/o      description
 
	real*8  inc       !I       incidence angle
	real*8  ema       !i       emission angle
	real*8  phase     !i       phase angle
	real*8  pstd      !o       pure atmospheric-scattering term
	real*8  trans     !o       transmission of surface reflected
                        !        light through the atmosphere overall
	real*8  trans0    !o       transmission of surface reflected
                        !        light through the atmosphere with
                        !        no scatterings in the atmosphere
	real*8  sbar      !o       illumination of the ground by the sky
c
c_desc  Isotropic atmospheric scattering in the first approximation, 
c	with corrections to the singly-scattered terms (in the spirit of
c	Hapke's photometric function for surfaces) for a strongly
c	anisotropic single-particle phase function.  The particular
c	phase function implemented is a single-term Henyey-Greenstein.
c	The model for scattering for a general, non-Lambertian surface with
c	an atmosphere looks like this:
c
c       P = Pstd + trans*(rho*Ah*munot)/(1.d0-rho*Ab*sbar)
c     +     trans0*rho*(Psurf-Ah*munot)
c
c	where P is the overall photometric function (the model of the data),
c	PSTD is the pure atmospheric-scattering term, PSURF is the surface
c	photometric function, AH*MUNOT is a Lambertian approximation to this
c	with hemispheric albedo AH, TRANS and TRANS0 quantify transmission
c	of surface reflected light through the atmosphere overall and with
c	no scatterings in the atmosphere, and finally SBAR quantifies the
c	illumination of the ground by the sky.  RHO is the ratio of the sur-
c	face albedo to the albedo assumed in the functional form of PSURF.
c
c_hist  07 Jul 2000 Randy Kirk, USGS, Flagstaff - Original code
c       18 Dec 2000 K Teal Thompson  Port to Unix/ISIS.
c_end   ================================================================
C	     INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
c	Must access common blocks ATM_TABLES, APARS

	real*8 tauold
	real*8 whaold
	real*8 r8expint
	real*8 e2
	real*8 e3
	real*8 e4
	real*8 e5
	real*8 x0
	real*8 wha2
	real*8 munot
	real*8 mu
	real*8 xx
	real*8 emunot
	real*8 emu
	real*8 x
	real*8 xmunot
	real*8 y
	real*8 ymunot
	real*8 xmu
	real*8 ymu
	real*8 gmunot
	real*8 gmu
	real*8 y0
	real*8 alpha0
	real*8 delta
	real*8 fixcon
	real*8 beta0
	real*8 alpha1
	real*8 beta1
	real*8 alpha2
	real*8 beta2
	real*8 hpsq1
	real*8 munotp
c	real*8 hspq1
	real*8 mup
	real*8 fix
	real*8 gammax
	real*8 gammay
	real*8 hahgt
	real*8 hahgt0
	real*8 phasefn
	real*8 deg2rad
	parameter (deg2rad=0.01745329251994329576913d0)
	data tauold,whaold /-1.d0,1.d30/


	if (tau.eq.0.d0) then
	   pstd=0.d0
	   trans=1.d0
	   trans0=1.d0
	   sbar=0.d0
	   return
	end if
	if ((tau.ne.tauold).or.(wha.ne.whaold)) then
c	preparation includes exponential integrals e sub 2 through 4
	   wha2=0.5d0*wha
	   e2=r8expint(2,tau)
	   e3=r8expint(3,tau)
	   e4=r8expint(4,tau)
c	zeroth moments of (uncorrected) x and y times characteristic fn
	   x0=wha2
	   y0=wha2*e2
c	higher-order correction term for x and y
	   delta=(1.d0-(x0+y0)-(1.d0-wha)/(1.d0-(x0-y0)))
     .	   /(wha*(0.5d0-e3))
c	moments of (corrected) x and y 
	   alpha0=1.d0+delta*(0.5d0-e3)
	   alpha1=0.5d0+delta*(0.3333333333333333333333d0-e4)
	   beta0=e2+delta*(0.5d0-e3)
	   beta1=e3+delta*(0.3333333333333333333333d0-e4)
c	prepare to find correct mixture of x and y in conservative case
	   if (wha.eq.1.d0) then
	      e5=r8expint(5,tau)
	      alpha2=0.3333333333333333333333d0+delta*(0.25d0-e5)
	      beta2=e4+delta*(0.25d0-e5)
	      fixcon=(beta0*tau-alpha1+beta1)
     .	     /((alpha1+beta1)*tau+2.d0*(alpha2+beta2))
	   end if
c	gamma will be a weighted sum of x and y functions
	   gammax=wha2*beta0
	   gammay=1.d0-wha2*alpha0
c	sbar is total diffuse illumination
c	isotropic part comes from moments, correction is numerical integral
	   call get_hahg_Tables()
	   sbar=1.d0-((2.d0-wha*alpha0)*alpha1+wha*beta0*beta1)+hahgsB
	   tauold=tau
	   whaold=wha
	end if
c	correct the path lengths for planetary curvature
	hpsq1=(1.d0+hnorm)**2.0d0-1.d0
	munot=cos(deg2rad*inc)
	munotp=hnorm/(sqrt(max(1.d-30,hpsq1+munot*munot))-munot)
	munotp=max(munotp,tau/69.0d0)
	mu=cos(deg2rad*ema)
	mup=hnorm/(sqrt(max(1.d-30,hpsq1+mu*mu))-mu)
	mup=max(mup,tau/69.0d0)
c	build the x and y functions of mu0 and mu
	xx=-tau/max(munotp,1.d-30)
	if (xx.lt.-69.d0) then
	   emunot=0.d0
	else if (xx.gt.69.d0) then
	   emunot=1.d30
	else
	   emunot=exp(-tau/munotp)
	end if
	xx=-tau/max(mup,1.d-30)
	if (xx.lt.-69.d0) then
	   emu=0.d0
	else if (xx.gt.69.d0) then
	   emu=1.d30
	else
	   emu=exp(-tau/mup)
	end if
	xmunot=1.d0+delta*munotp*(1.d0-emunot)
	ymunot=emunot+delta*munotp*(1.d0-emunot)
	xmu=1.d0+delta*mup*(1.d0-emu)
	ymu=emu+delta*mup*(1.d0-emu)
c	mix the x and y as required in the conservative case
	if (wha.eq.1.d0) then
	   x=xmunot
	   y=ymunot
	   fix=fixcon*munotp*(x+y)
	   xmunot=x+fix
	   ymunot=y+fix
	   x=xmu
	   y=ymu
	   fix=fixcon*mup*(x+y)
	   xmu=x+fix
	   ymu=y+fix
	end if

c	gamma1 functions come from x and y, with a correction for
c	highly forward-scattered light as tabulated in hahgtTable
	call r8splint(incTable,hahgtTable,hahgtTable2,ninc,inc,hahgt)
	gmunot=gammax*xmunot+gammay*ymunot+hahgt
	call r8splint(incTable,hahgtTable,hahgtTable2,ninc,ema,hahgt)
	gmu=gammax*xmu+gammay*ymu+hahgt
c	purely atmos term uses x and y (plus single-particle phase
c	function correction)
	phasefn=(1.d0-hga*hga)/
     .        (1.d0+2.d0*hga*cos(deg2rad*phase)+hga*hga)**1.5d0
	pstd=0.25d0*wha*munotp/(munotp+mup)*(
     .     (xmunot*xmu-ymunot*ymu)+(phasefn-1.d0)*(1.d0-emu*emunot))
c xmitted surface term uses gammas
	trans=gmunot*gmu
c	finally, never-scattered term is given by pure attenuation, with
c 	a correction for highly forward-scattered light (on the way down
c	but not on the way up) as tabulated in hahgt0Table
	call r8splint(incTable,hahgt0Table,hahgt0Table2,ninc,inc,hahgt0)
	trans0=(emunot+ hahgt0)*emu
	return
	end



c	================================================================
	subroutine atm_hhg2(inc,ema,phase,pstd,trans,trans0,sbar)
c	================================================================
	implicit none
c_title atm_hhg2 - Henyey-Greenstein atmos scattering 2nd approx for shadows
c
c_args  type    variable  i/o      description
 
	real*8  inc       !i       incidence angle
	real*8  ema       !i       emission angle
	real*8  phase     !i       phase angle
	real*8  pstd	  !o       pure atmospheric-scattering term
	real*8  trans     !o       transmission of surface reflected
                          !        light through the atmosphere overall
	real*8  trans0    !o       transmission of surface reflected
                          !        light through the atmosphere with
                          !        no scatterings in the atmosphere
	real*8  sbar      !o       illumination of the ground by the sky
c
c_desc  Isotropic atmospheric scattering in the second approximation,
c	with corrections to the singly-scattered terms (in the spirit of
c	Hapke's photometric function for surfaces) for a strongly
c	anisotropic single-particle phase function.  The particular
c	phase function implemented is a single-term Henyey-Greenstein.
c	The model for scattering for a general, non-Lambertian surface with
c	an atmosphere looks like this:
c 
c       P = Pstd + trans*(rho*Ah*munot)/(1.d0-rho*Ab*sbar)
c     +     trans0*rho*(psurf-Ah*munot)
c
c       where P is the overall photometric function (the model of the data),
c       PSTD is the pure atmospheric-scattering term, PSURF is the surface
c       photometric function, AH*MUNOT is a Lambertian approximation to this
c       with hemispheric albedo AH, TRANS and TRANS0 quantify transmission
c       of surface reflected light through the atmosphere overall and with
c       no scatterings in the atmosphere, and finally SBAR quantifies the
c       illumination of the ground by the sky.  RHO is the ratio of the sur-
c       face albedo to the albedo assumed in the functional form of PSURF.
c
c_hist  07 Jul 2000 Randy Kirk, USGS, Flagstaff - Original code
c       18 Dec 2000 K Teal Thompson  Port to Unix/ISIS.
c_end   ================================================================
C	     INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
c	Must access common blocks ATM_TABLES, APARS

	real*8 tauold
	real*8 whaold
	real*8 e1
	real*8 e1_2
	real*8 e2
	real*8 e3
	real*8 e4
	real*8 r8expint
	real*8 xx
	real*8 em
	real*8 e
	real*8 g11p
	real*8 g11prime
	real*8 x0
	real*8 wha2
	real*8 g12
	real*8 e5
	real*8 munot
	real*8 mu
	real*8 emunot
	real*8 emu
	real*8 munotp
	real*8 f1munot
	real*8 f1
	real*8 f2
	real*8 f3
	real*8 f4
	real*8 mup
	real*8 x
	real*8 xmunot
	real*8 y
	real*8 ymunot
	real*8 xmu
	real*8 ymu
	real*8 gmunot
	real*8 f1m
	real*8 f2m
	real*8 f3m
	real*8 f4m
	real*8 g12p
	real*8 g13p
	real*8 g14p
	real*8 g13
	real*8 g14
	real*8 delta
	real*8 y0
	real*8 fixcon
	real*8 alpha0
	real*8 alpha1
	real*8 alpha2
	real*8 beta0
	real*8 beta1
	real*8 beta2
	real*8 gammay
	real*8 hpsq1
	real*8 f1mmunot
	real*8 fix
	real*8 gmu
	real*8 gammax
	real*8 f1mmu
	real*8 f1mu
	real*8 r8ei
	real*8 hahgt
	real*8 hahgt0
	real*8 phasefn
	real*8 deg2rad
	parameter (deg2rad=0.01745329251994329576913d0)
	real*8 eulgam
	parameter (eulgam=0.5772156d0)
	data tauold,whaold /-1.d0,1.d30/


	if (tau.eq.0.d0) then
	   pstd=0.d0
	   trans=1.d0
	   trans0=1.d0
	   sbar=0.d0
	   return
	end if
	if ((tau.ne.tauold).or.(wha.ne.whaold)) then
c	preparation includes exponential integrals e sub 2 through 4
	   wha2=0.5d0*wha
	   e1=r8expint(1,tau)
	   e1_2=r8expint(1,2.d0*tau)
	   e2=r8expint(2,tau)
	   e3=r8expint(3,tau)
	   e4=r8expint(4,tau)
c	chandra's gmn functions require fm and fn at mu=-1
	   xx=-tau
	   if (xx.lt.-69.d0) then
	      em=0.d0
	   else if (xx.gt.69.d0) then
	      em=1.d30
	   else
	      em=exp(xx)
	   end if
	   f1m=log(2.d0)-em*e1+e1_2
	   f2m=-1.d0*(f1m+em*e2-1.d0)
	   f3m=-1.d0*(f2m+em*e3-0.5d0)
	   g12=(tau*e1*e2+f1m+f2m)*0.5d0
	   g13=(tau*e1*e3+f1m+f3m)*0.3333333333333333333333d0
c	chandra's g'mn functions require g'11 and f at mu=+1
	   xx=tau
	   if (xx.lt.-69.d0) then
	      e=0.d0
	   else if (xx.gt.69.d0) then
	      e=1.d30
	   else
	      e=exp(xx)
	   end if
	   f1=eulgam+log(tau)+e*e1
	   f2=f1+e*e2-1.d0
	   f3=f2+e*e3-0.5d0
	   g11p=g11prime(tau)!evaluate series as function
	   g12p=(tau*(e1-g11p)+em*(f1+f2))*0.25d0
	   g13p=(tau*(0.5d0*e1-g12p)+em*(f1+f3))*0.2d0
c	zeroth moments of (uncorrected) x and y times characteristic fn
	   x0=wha2*(1.d0+wha2*g12)
	   y0=wha2*(e2+wha2*g12p)
c	higher-order correction term for x and y
	   delta=(1.d0-(x0+y0)-(1.d0-wha)/(1.d0-(x0-y0)))
     *	  /(wha*(0.5d0-e3))
c	moments of (corrected) x and y 
	   alpha0=1.d0+wha2*g12+delta*(0.5d0-e3)
	   alpha1=0.5d0+wha2*g13+delta*(0.3333333333333333333333d0-e4)
	   beta0=e2+wha2*g12p+delta*(0.5d0-e3)
	   beta1=e3+wha2*g13p+delta*(0.3333333333333333333333d0-e4)
c	prepare to find correct mixture of x and y in conservative case
	   if (wha.eq.1.d0) then
	      e5=r8expint(5,tau)
	      f4m=-1.d0*(f3m+em*e4-0.3333333333333333333333d0)
	      g14=(tau*e1*e4+f1m+f4m)*0.25d0
	      f4=f3+e*e4-0.3333333333333333333333d0
	      g14p=(tau*(0.5d0*e1-g13p)+em*(f1+f4))*0.1666666d0
	      alpha2=0.3333333333333333333333d0+wha2*g14 +
     .		delta*(0.25d0-e5)
	      beta2=e4+wha2*g14p+delta*(0.25d0-e5)
	      fixcon=(beta0*tau-alpha1+beta1)
     .	     /((alpha1+beta1)*tau+2.d0*(alpha2+beta2))
	   end if
c	gamma will be a weighted sum of x and y functions
	   gammax=wha2*beta0
	   gammay=1.d0-wha2*alpha0
c	sbar is total diffuse illumination
c	isotropic part comes from moments, correction is numerical integral
	   call get_hahg_tables()
	   sbar=1.d0-((2.d0-wha*alpha0)*alpha1+wha*beta0*beta1)+hahgsB
	   tauold=tau
	   whaold=wha
	end if
c	correct the path lengths for planetary curvature
	hpsq1=(1.d0+hnorm)**2.0d0-1.d0
	munot=cos(deg2rad*inc)
	munotp=hnorm/(sqrt(max(1.d-30,hpsq1+munot*munot))-munot)
	munotp=max(munotp,tau/69.0d0)
	mu=cos(deg2rad*ema)
	mup=hnorm/(sqrt(max(1.d-30,hpsq1+mu*mu))-mu)
	mup=max(mup,tau/69.0d0)
c	build the x and y functions of mu0 and mu
	xx=-tau/max(munotp,1.d-30)
	if (xx.lt.-69.d0) then
	   emunot=0.d0
	else if (xx.gt.69.d0) then
	   emunot=1.d30
	else
	   emunot=exp(-tau/munotp)
	end if
	xx=-tau/max(mup,1.d-30)
	if (xx.lt.-69.d0) then
	   emu=0.d0
	else if (xx.gt.69.d0) then
	   emu=1.d30
	else
	   emu=exp(-tau/mup)
	end if
c	in the second approximation the x and y include the f1 function
	xx=munotp
	if (abs(xx-1.0d0).lt.1.0d-10) then
	   f1munot=f1
	   f1mmunot=xx*(log(1.d0+1.d0/xx)-e1*emunot+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else if (xx.gt.0.d0) then
c	   ei() is from numerical recipes.
	   f1munot=xx*(log(xx/(1.d0-xx))+e1/emunot+
     .              r8ei(tau*(1.d0/xx-1.d0)))
	   f1mmunot=xx*(log(1.d0+1.d0/xx)-e1*emunot+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else
	   pause 'negative munot in f1 function'
	end if
	xx=mup
	if (abs(xx-1.0d0).lt.1.0d-10) then
	   f1mu=f1
	   f1mmu=xx*(log(1.d0+1.d0/xx)-e1*emu+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else if (xx.gt.0.d0) then
c	   ei() is from numerical recipes.
	   f1mu=xx*(log(xx/(1.d0-xx))+e1/emu+
     .              r8ei(tau*(1.d0/xx-1.d0)))
	   f1mmu=xx*(log(1.d0+1.d0/xx)-e1*emu+
     .              r8expint(1,tau*(1.d0+1.d0/xx)))
	else
	   pause 'negative mu in f1 function'
	end if
	xmunot=1.d0+wha2*f1mmunot+delta*munotp*(1.d0-emunot)
	ymunot=emunot*(1.d0+wha2*f1munot)+delta*munotp*(1.d0-emunot)
	xmu=1.d0+wha2*f1mmu+delta*mup*(1.d0-emu)
	ymu=emu*(1.d0+wha2*f1mu)+delta*mup*(1.d0-emu)
c	mix the x and y as required in the conservative case
	if (wha.eq.1.d0) then
	   x=xmunot
	   y=ymunot
	   fix=fixcon*munotp*(x+y)
	   xmunot=x+fix
	   ymunot=y+fix
	   x=xmu
	   y=ymu
	   fix=fixcon*mup*(x+y)
	   xmu=x+fix
	   ymu=y+fix
	end if

c	gamma1 functions come from x and y, with a correction for
c	highly forward-scattered light as tabulated in hahgtTable
	call r8splint(incTable,hahgtTable,hahgtTable2,ninc,inc,hahgt)
	gmunot=gammax*xmunot+gammay*ymunot+hahgt
	call r8splint(incTable,hahgtTable,hahgtTable2,ninc,ema,hahgt)
	gmu=gammax*xmu+gammay*ymu+hahgt
c	purely atmos term uses x and y (plus single-particle phase
c	function correction)
	phasefn=(1.d0-hga*hga)/
     .        (1.d0+2.d0*hga*cos(deg2rad*phase)+hga*hga)**1.5d0
	pstd=0.25d0*wha*munotp/(munotp+mup)*(
     .     (xmunot*xmu-ymunot*ymu)+(phasefn-1.d0)*(1.d0-emu*emunot))
c	xmitted surface term uses gammas
	trans=gmunot*gmu
c	finally, never-scattered term is given by pure attenuation, with
c	a correction for highly forward-scattered light (on the way down
c	but not on the way up) as tabulated in hahgt0Table
	call r8splint(incTable,hahgt0Table,hahgt0Table2,ninc,inc,hahgt0)
	trans0=(emunot+ hahgt0)*emu
	return
	end


