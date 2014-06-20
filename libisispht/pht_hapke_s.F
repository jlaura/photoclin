c       ================================================================
	function pht_hapke_s(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_hapke_s - hapke photometric model
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle
        real*8       inc       !i       incidence angle
        real*8       phase     !i       phase angle (deg)
c
c_desc  
c
c_hist  08 Mar 99 K Teal Thompson  Inplement 08 Jan 99  Randy Kirk Or-
c		  iginal Specs & Code.  Declare vars, add implicit none.
c       ================================================================
#include "pht_common.inc"
        real*8 munot
        real*8 mu
        real*8 deg2rad
        real*8 pht_hapke_s
	real*8 u		!dummy argument to hfunc
	real*8 gamma		!dummy argument to hfunc
	real*8 srw
	real*8 sing
	real*8 cosg
	real*8 tang
	real*8 bg
	real*8 pg
	real*8 pg1
	real*8 pg2
        real*8 hgs

	real*8 hfunc

	parameter (deg2rad=.01745329251994329576913d0)

c	Hapke's approx. to Chandra's H function, which I have made
c	a statement function:
        hfunc(u,gamma)=(1.d0+2.d0*u)/(1.d0+2.d0*u*gamma)
 
	munot=cos(deg2rad*inc)
	mu=cos(deg2rad*ema)

        if (inc.gt.90.d0) then     !in shadow, skip the calculations
           pht_hapke_s=0.d0
           return
        end if
 
        srw=sqrt(1.d0-wh)
 
        sing=sin(deg2rad*phase)
        cosg=cos(deg2rad*phase)   !calculate; it's not passed as alpha
        if (cosg.eq.0.d0) then
           tang=1.d10
        else
           tang=sing/cosg
        end if
        if (hh.eq.0.d0) then
           bg=0.d0
        else
           if (phase.le.90.d0) then
              bg=b0/max(1.d-6,1.d0+tang/hh)
           else
              bg=0.d0
           end if
        end if
        if (iopt.eq.0) then !Henyey-Greenstein smooth
	   hgs=hg1*hg1
           pg1=(1.d0-hg2)*(1.d0-hgs)/(1.d0+hgs+2.d0*hg1*cosg)**1.5d0
           pg2=hg2*(1.d0-hgs)/(1.d0+hgs-2.d0*hg1*cosg)**1.5d0
           pg=pg1+pg2
        else !iopt.eq.1 / Legendre smooth
           pg=1.d0+bh*cosg+ch*(1.5d0*cosg**2.0d0-0.5d0)
        end if
 
        munot=cos(deg2rad*inc)
        mu=cos(deg2rad*ema)
 
        pht_hapke_s=wh/4.d0*munot/(munot+mu)*((1.d0+bg)*pg-
     *               1.d0+hfunc(munot,srw)*hfunc(mu,srw))


	return
	end

