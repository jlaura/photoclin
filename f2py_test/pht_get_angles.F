c	==================================================================
	subroutine pht_get_angles(nl,ns,j,i,inc,ema,phase,iret)
c	==================================================================
        implicit none
c_title - pht_get_angles
c
c_args  type    variable          i/o      description
	integer*4 nl,ns,j,i	! inputs:  buffer dimensions, location
	real*8 phase		! input:   phase angle
	real*8 inc,ema		! outputs: incidence, emission angles
	integer*4 iret		! error code: -1 = beyond edge of sphere
				! (not a fatal error, we just want to
				! ignore these parts of the buffer...)

c
c_desc  Calculate the incidence and emission angles at a point in a buffer
c	corresponding to one half of a Gaussian sphere illuminated from the
c	left at phase angle PHASE
c
c_hist  23 Nov 99  Randy L Kirk - USGS, Flagstaff; Original Code.
c	23 Dec 99  K Teal Thompson - Port to Unix; add implicit none.

c_end   ==================================================================

	logical*1 firsttime
	data firsttime /.true./
	real*8 r,r2,s2
	real*8 x,y,z	! refer here to cartesian coords of point on sphere
	real*8 deg2rad
	data deg2rad /0.01745329251994329576913d0/

c	==================================================================
c	Pardon my cheezy error trap.  Probably not needed at all in this
c	==================================================================
c	one-time-use function.
	if (ns.ne.nl*2-1) stop 'Bad buffer dimensions.'

c	==================================================================
c	Precompute radius, radius squared
c	==================================================================
	if (firsttime) then
	   r=dble(nl-1)
	   r2=r*r
	   firsttime=.false.
	end if

c	==================================================================
c	Given buffer dimensions and location in buffer, calculate angles
c	==================================================================
	x=dble(i-nl)
	y=dble(j-1)
	s2=x*x+y*y
	if (s2.ge.r2) then
	   iret=-1
	else
	   iret=0
	   z=sqrt(r2-s2)
	   inc=acos(cos(phase*deg2rad)*z/r
     *        -sin(phase*deg2rad)*x/r)/deg2rad
	   ema=acos(z/r)/deg2rad
	end if
	return
	end


