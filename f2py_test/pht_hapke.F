c       ================================================================
        function pht_hapke(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_hapke - hapke photometric model
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle in degrees
        real*8       inc       !i       incidence angle in degrees
        real*8       phase     !i       phase angle in degrees
c
c_desc  Computes normal albedo mult factor w/o opp surge from
C       Hapke input parameters: W,H,BO,HG,THETA.
c       Full-up Hapke's Law with macroscopic roughness.  The photometric
c       function multiplied back in will be modified to take out oppos-
c       ition effect.  This requires saving the actual value of B0 while
c       temporarily setting it to zero in the common block to compute
c       the overall normalization.
c       
c
c_hist  07 Aug 91 Tammy Becker  relinked hapke to new photompr
c       13 Oct 94 Kay Edwards changed test az2.eq.90. to az2.ge.90.
c       10 Apr 95 Trent Hare - changed all real to real*8
c       05 Mar 96 Jim Mathews - changed all local real*8 to real*8
c       16 Feb 97 James M Anderson - changed nonstandard degree trig
c                 to use radians
c       07 Jan 99 K Teal Thompson  change some variable names.
c       11 Jan 99 KTT - Remove mu,munot,and alpha from the argument
c                 list and pass in only ema,inc, and phase.  Remove
c                 lat and lon from argument list because they aren't
c                 used.
c       01 Mar 99 K Teal Thompson  Inplement 08 Jan 99  Randy Kirk Or-
c                 iginal Specs & Code.  Declare vars, add implicit none.
c       18 Nov 99 Randy Kirk - fixed minor typos, implemented return with
c                 smooth Hapke (Theta=0) result before doing rough Hapke
c                 calculations, allow single-particle-phase params = 0
c       ================================================================
C NOTE: The syntax of the INCLUDE statement is different in VMS FORTRAN
C        INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
        real*8 munot
        real*8 mu
        real*8 deg2rad
        real*8 pht_hapke
        real*8 oldtheta
        real*8 u                !dummy argument to hfunc
        real*8 gamma            !dummy argument to hfunc
        real*8 pi
        real*8 tan
        real*8 cost
        real*8 sint
        real*8 cot2t
        real*8 cott
        real*8 tan2t
        real*8 tant
        real*8 sr
        real*8 sing
        real*8 cosg
        real*8 tang
        real*8 bg
        real*8 pg
        real*8 pg1
        real*8 pg2
        real*8 sini
        real*8 coti
        real*8 ecoti
        real*8 ecot2i
        real*8 sine
        real*8 cote
        real*8 cosei
        real*8 sinei
        real*8 caz
        real*8 az
        real*8 az2
        real*8 faz
        real*8 tanaz2
        real*8 ecote
        real*8 ecot2e
        real*8 q
        real*8 osr
        real*8 u0p0
        real*8 sin2a2
        real*8 rr1
        real*8 rr2
        real*8 hgs
        real*8 cot2i
        real*8 cot2e
        real*8 up0
        real*8 api
        real*8 s2ei
        real*8 s2ee
        real*8 ecei
        real*8 ecee
        real*8 u0p
        real*8 up

        real*8 hfunc

        data oldtheta /-999.9d0/        ! Hopefully impossible value

        parameter (deg2rad=0.01745329251994329576913d0)
        parameter (pi=3.1415926535897932384626433832795d0)

c       Hapke's approx. to Chandra's H function, which I have made
c       a statement function:
        hfunc(u,gamma)=(1.0d0+2.0d0*u)/(1.0d0+2.0d0*u*gamma)
 
        munot=cos(deg2rad*inc)
        mu=cos(deg2rad*ema)

        if (theta.ne.oldtheta) then
           cost=cos(deg2rad*theta)
           sint=sin(deg2rad*theta)
           cott=cost/max(1.0d-10,sint)   !theta could be 0 so check
           cot2t=cott*cott
           tant=sint/cost                 !no check for theta=90 error
           tan2t=tant*tant
           sr=sqrt(1.0d0+pi*tan2t)
           osr=1.0d0/sr
           oldtheta=theta
        end if
   
        if (inc.ge.90.0d0) then     !in shadow, skip the calculations
           pht_hapke=0.0d0
           return
        end if
 
        gamma=sqrt(1.0d0-wh)
        hgs=hg1*hg1
 
        sing=sin(deg2rad*phase)
        cosg=cos(deg2rad*phase)   !calculate; it's not passed as alpha
        tang=sing/max(1.0d-10,cosg)
        if (hh.eq.0.0d0) then
           bg=0.0d0
        else
           if (phase.le.90.d0) then
              bg=b0/max(1.d0-6,1.d0+tang/hh)
           else
              bg=0.d0
           end if
        end if

        if (iopt.eq.0) then
           pg1=(1.0d0-hg2)*(1.0d0-hgs)/(1.0d0+hgs+2.0d0*hg1*cosg)**1.5d0
           pg2=hg2*(1.0d0-hgs)/(1.0d0+hgs-2.0d0*hg1*cosg)**1.5d0
           pg=pg1+pg2
        else
           pg=1.0d0+bh*cosg+ch*(1.5d0*cosg**2.0d0-0.5d0)
        end if
 
        if (theta.le.0.0d0) then
           pht_hapke=wh/4.0d0*munot/(munot+mu)*((1.0d0+bg)*pg-
     *         1.0d0+hfunc(munot,gamma)*hfunc(mu,gamma))
           return
        endif
 
        sini=sin(deg2rad*inc)
        coti=munot/max(1.0d-10,sini)
        cot2i=coti*coti
        ecoti=exp(min(-cot2t*cot2i/pi,23.d0))      ! upper bound gives 1e10
        ecot2i=exp(min(-2.d0*cott*coti/pi,23.d0))
        u0p0=osr*(munot+sini*tant*ecoti/(2.d0-ecot2i)) !renamed from uopo
 
        sine=sin(deg2rad*ema)
        cote=mu/max(1.d-10,sine)
        cot2e=cote*cote
 
        cosei=mu*munot
        sinei=sine*sini
 
        if (sinei.eq.0.d0) then
           caz=1.d0
           az=0.d0
        else
           caz=(cosg-cosei)/sinei
           if (caz.le.-1.d0) then
              az=180.d0
           else if (caz.gt.1.d0) then
              az=0.d0
           else
              az=acos(caz)/deg2rad
           end if
        end if
        az2=az/2.d0
        if (az2.ge.90.d0) then
           faz=0.d0
        else
           tanaz2=tan(deg2rad*az2)
           faz=exp(min(-2.0d0*tanaz2,23.0d0))
        end if
        sin2a2=sin(deg2rad*az2)**2.0d0
        api=az/180.0d0
 
        ecote=exp(min(-cot2t*cot2e/pi,23.0d0))
        ecot2e=exp(min(-2.0d0*cott*cote/pi,23.0d0))
        up0=osr*(mu+sine*tant*ecote/(2.0d0-ecot2e)) !renamed from upo
 
        if (inc.le.ema) then
           q=osr*munot/u0p0
        else
           q=osr*mu/up0
        end if
 
        if (inc.le.ema) then
           ecei=(2.0d0-ecot2e-api*ecot2i)
           s2ei=sin2a2*ecoti
           u0p=osr*(munot+sini*tant*(caz*ecote+s2ei)/ecei)
           up=osr*(mu+sine*tant*(ecote-s2ei)/ecei)
        else
           ecee=(2.0d0-ecot2i-api*ecot2e)
           s2ee=sin2a2*ecote
           u0p=osr*(munot+sini*tant*(ecoti-s2ee)/ecee)
           up=osr*(mu+sine*tant*(caz*ecoti+s2ee)/ecee)
        end if
 
        rr1=wh/4.0d0*u0p/(u0p+up)*((1.0d0+bg)*pg-
     *      1.0d0+hfunc(u0p,gamma)*hfunc(up,gamma))
        rr2=up*munot/(up0*u0p0*sr*(1.0d0-faz+faz*q))
        pht_hapke=rr1*rr2

        return
        end



