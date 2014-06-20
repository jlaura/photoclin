c       ================================================================
        function pht_minnaert(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_minnaert  - Minnaert photometric model
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle (deg)
        real*8       inc       !i       incidence angle (deg)
        real*8       phase     !i       phase angle (deg)
c
c_desc  Derive model albedo using Minnaert equation.
c       Phase independent and calculated analytically.
c       Limb-darkening k is a constant.
c       albedo = brightness*[mu / (mu*munot)**k)]
c       assumptions:
c       1. bidirectional reflectance
c       2. semi-infinite medium
c                                                   k      k-1
c       reflectance (inc,ema,phase)=albedo  *  munot   * mu
c                Minnaert               Minnaert
c
c       where k is the Minnaert index, an empirical constant (called nu in Hapke)
c       If k (nu) = 1, Minnaert's law reduces to Lambert's law.
c       See Theory of Reflectance and Emittance Spectroscopy, 1993;
c       Bruce Hapke; pg. 191-192.
c
c_hist  Aug 02 89  Tammy Becker  U.S.G.S. Flagstaff Original version
c       Apr 10 95  Trent Hare - changed all real to real*8
c       Dec 30 98  K Teal Thompson - update documentation.
c       01 Mar 99  KTT - Inplement 08 Jan 99  Randy Kirk Original Specs
c                  & Code.  Declare vars, add implicit none.
c       01 Aug 99  KTT - Get rid of one letter variables.
c       ================================================================
C NOTE: The syntax of the INCLUDE statement is different in VMS FORTRAN
C       INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"
        real*8       munot
        real*8       mu
        real*8       deg2rad
        real*8       pht_minnaert

        parameter (deg2rad=0.01745329251994329576913d0)

        munot=cos(deg2rad*inc)
        mu=cos(deg2rad*ema)

        if ((munot.le.0.d0).or.(mu.le.0.d0)) then
          pht_minnaert=0.d0
        else if (kay.eq.1.d0) then
          pht_minnaert=munot
        else
          pht_minnaert=munot*(munot*mu)**(kay-1.d0)
        end if

        return
        end


