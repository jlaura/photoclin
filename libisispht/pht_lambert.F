c       ================================================================
	function pht_lambert(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_lambert  - preliminary photometric model

c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle (deg)
        real*8       inc       !i       incidence angle (deg)
        real*8       phase     !i       phase angle (deg)

c_desc  derive model albedo for lunar-lambert surface
c	straight lambert law.  no parameters to be passed in common.
 
c_hist  Aug 02 1989  Tammy Becker  U.S.G.S. Flagstaff Original version
c       Aug 17 1994  Kay Edwards  change function to lunar-lambert,
c                    add l.  previous function:
c                    albedo = brightness/munot
c       Oct  5 1994  KE  add phase to args, add phase angle function
c       Nov 10 1994  KE  change constants for phase 5<phase<90
c       Nov 28 1994  KE  change test to include phase > 90
c       Apr 10 1995  ASM  add ianorm user parameter.  output
c                    a,b values normalize data to incidence angle = ianorm
c                    (and phase angle = ianorm).  defaults: ianorm=30 for
c                    Clementine, to match relab (Brown U. reflectance lab)
c       Apr 30 1996  Jim Mathews, USGS, Flagstaff - removed phase angle
c                    function code.
c       Mar 01 1999  K Teal Thompson - gut subroutine and use Randy Kirk
c		     inplementation.  Add implicit none, declare vars.
c		     Old pht_lambert.F saved to pht_lambert_ianorm.F
c       ================================================================
        real*8       munot
        real*8       deg2rad
        real*8       pht_lambert

	parameter (deg2rad=0.01745329251994329576913d0)

	munot=cos(deg2rad*inc)

	if (munot.le.0.d0) then
	  pht_lambert=0.d0
	else
	  pht_lambert=munot
	end if
	return
	end

