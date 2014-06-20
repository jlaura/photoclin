c       ================================================================
	function pht_lomsel(inc,ema,phase)
c       ================================================================
        implicit none
c_title pht_lomsel  - Lommel-Seeliger photometric model

c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle (deg)
        real*8       inc       !i       incidence angle (deg)
        real*8       phase     !i       phase angle (deg)

c_desc  Derive model albedo for Lommel-Seeliger
c	Straight Lommel-Seeliger law.  no parameters to be passed in
c	common.
 
c_hist  08 Jan 99  Randy Kirk Original Specs & Code
c	01 Mar 99  K Teal Thompson  Put code in separate file, declare
c		   vars, add implicit none.
c       ================================================================
        real*8       munot
        real*8       mu
        real*8       deg2rad
        real*8       pht_lomsel

	parameter (deg2rad=0.01745329251994329576913d0)

	munot=cos(deg2rad*inc)
	mu=cos(deg2rad*ema)

	if ((munot.le.0.d0).or.(mu.le.0.d0)) then
	  pht_lomsel=0.d0
	else
	  pht_lomsel=2.d0*munot/(munot+mu)
	end if

	return
	end

