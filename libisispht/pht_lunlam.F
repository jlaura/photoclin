c       ================================================================
        function pht_lunlam(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_lunlam - Lunar (Lommel-Seeliger)-Lambert law photometric model
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle (deg)
        real*8       inc       !i       incidence angle (deg)
        real*8       phase     !i       phase angle (deg)
c
c_desc  Derive model albedo for Lunar (Lommel-Seeliger)-Lambert law.
c       Phase independent and calculated analytically.
c       Lommel-Seeliger law:
c
c       Reflectance=1/(1+cos(E)/cos(I))
c
c       Where: E=the angle between the observer and the slope normal
c              I=the angle between the sun and the slope normal
c
c       From documentation for "shade" program.
c
c_hist  08 Jan 99  Randy Kirk Original Specs & Code
c       01 Mar 99  K Teal Thompson  Put code in separate file, declare
c                  vars, add implicit none.
c       01 Aug 99  KTT - Get rid of one letter variables.
c
c       ================================================================
C NOTE: The syntax of the INCLUDE statement is different in VMS FORTRAN
C       INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
        real*8       munot
        real*8       mu
        real*8       deg2rad
        real*8       pht_lunlam

        parameter (deg2rad=0.01745329251994329576913d0)

        munot=cos(deg2rad*inc)
        mu=cos(deg2rad*ema)

        if ((munot.le.0.d0).or.(mu.le.0.d0)) then
          pht_lunlam =0.d0
        else if (ll.eq.0.d0) then
          pht_lunlam=munot
        else if (ll.eq.1.d0) then
          pht_lunlam=2.d0*munot/(munot+mu)
        else
          pht_lunlam=munot*((1.d0-ll)+2.d0*ll/(munot+mu))
        end if

        return
        end


