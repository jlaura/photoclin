c       ================================================================
	function pht_topder(inc,ema,phase,ret)
c       ================================================================
        implicit none
c_title pht_topder - topographic derivative of an arbitrary photometric
c	function.
c
c_args  type         variable  i/o      description
        real*8		ema	!i	emission angle (deg)
        real*8		inc	!i	incidence angle (deg)
        real*8		phase	!i	phase angle (deg)
        integer*4	ret	!i	returning error value
				!	(-1) error returned from
				!	my_acos()
c
c_desc  This function sets up coordinates with the nominal surface normal
c	along the Z-axis and the incident illumination in the XZ plane.  Then
c	the angles are calculated and the photometric function is evaluated
c	for four orientations of the normal slightly different from the
c	nominal.  These are combined to yield the gradient of the photometric
c	function indicating the amount of contrast for given topographic
c	slope in the direction to which brightness is most sensitive.
c
c_hist  09 Mar 99 K Teal Thompson  Inplement 08 Jan 99  Randy Kirk Or-
c                 iginal Specs & Code.  Declare vars, add implicit none.
c       26 Jun 2003 Glenn Bennett - modified my_acos function to handle
c                  +1 differently than -1. original code always returned 0;
c                  changed error msgs and added parameter sysEpsilonTolerance
c                  now input >=1 returns 0, input <=-1 returns pi
c                  As per Randy Kirk: Tolerance increased
c                  (from 10**5 to 10**6) * sysEpsilon
c       31 Aug 2005 Janet Barrett - Fixed code so that if input to 
c                  my_acos is >1, then it is rounded down to 1 before
c                  arccos is taken. Also, if input is <-1, then it is
c                  rounded down to -1 before arccos is taken.
c       ================================================================

	external pht_func

        real*8 pht_topder		!the function
        real*8 pht_func		!the user's photometric function
        real*8 my_acos		!my arccosine function which tests
				!outside the machines epsilon limits
	real*8 xi		!xi = sin(inc*deg2rad)
	real*8 zi		!zi = cos(inc*deg2rad)
        real*8 cphi		!cos(phi)
        real*8 phi		!the azimuth from xz plane to emission direction
        real*8 xe		!xe=cphi*sin(ema*deg2rad)
        real*8 ye		!ye=sin(phi*deg2rad)*sin(ema*deg2rad)
        real*8 ze		!ze=cos(ema*deg2rad)
        real*8 epsh		!eps*0.5d0
        real*8 xy		!sin(epsh*deg2rad)
        real*8 z		!cos(epsh*deg2rad)
        real*8 cinc		!cos(inc)
        real*8 cema		!cos(ema)
        real*8 inc1		!acos(cinc1)
        real*8 ema1		!acos(cema1)
        real*8 inc2		!acos(cinc2)
        real*8 ema2		!acos(cema2)
        real*8 inc3		!acos(cinc)
        real*8 ema3		!acos(cema)
        real*8 inc4		!acos(cinc)
        real*8 ema4		!acos(cema)
        real*8 d1		!derivative 1
        real*8 d2		!derivative 2
	real*8 eps		!have fun, change eps
	
        real*8 sysEpsilon	!system epsilon
        real*8 deg2rad
        real*8 rad2deg



	parameter (sysEpsilon=2.2204460492503131E-16)
	parameter (eps=0.04d0,deg2rad=0.01745329251994329576913d0)
 
	rad2deg=1.d0/deg2rad
c	incidence vector; yi is zero
	xi = sin(inc*deg2rad)
	zi = cos(inc*deg2rad)
 
c	phi is the azimuth from xz plane to emission direction; if either inc
c	or ema is zero, it's arbitrary and gets set to zero.  Thus cos(phi),
c	cphi, is set to one.
	if ((inc.eq.0.d0).or.(ema.eq.0.d0)) then
	   cphi=1.d0
	else
	   cphi=(cos(phase*deg2rad)-cos(inc*deg2rad)*cos(ema*deg2rad))/
     .	  (sin(inc*deg2rad)*sin(ema*deg2rad))
	end if


c	now calculate the emission vector
	phi=my_acos(cphi,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.
	xe=cphi*sin(ema*deg2rad)
	ye=sin(phi*deg2rad)*sin(ema*deg2rad)
	ze=cos(ema*deg2rad)
 
c	now evaluate two orthogonal derivatives
	epsh=eps*0.5d0
	xy=sin(epsh*deg2rad)
	z=cos(epsh*deg2rad)
 
	cinc=max(-1.d0,min(xy*xi+z*zi,1.d0))
	inc1=my_acos(cinc,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.
	cema=max(-1.d0,min(xy*xe+z*ze,1.d0))
	ema1=my_acos(cema,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.
 
	cinc=max(-1.d0,min(-xy*xi+z*zi,1.d0))
	inc2=my_acos(cinc,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.

	cema=max(-1.d0,min(-xy*xe+z*ze,1.d0))
	ema2=my_acos(cema,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.
 
	cinc=max(-1.d0,min(z*zi,1.d0))
	inc3=my_acos(cinc,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.

	cema=max(-1.d0,min(xy*ye+z*ze,1.d0))
	ema3=my_acos(cema,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.
 
	cinc=max(-1.d0,min(z*zi,1.d0))
	inc4=my_acos(cinc,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.

	cema=max(-1.d0,min(-xy*ye+z*ze,1.d0))
	ema4=my_acos(cema,ret)/deg2rad
	if (ret.lt.0) then !add negative pht_topder to this return ktt001215
	  pht_topder = -1.d0!value of ret is tested upon return, however
	  return	   !some compilers complain if pht_topder is not as-
	endif		   !signed a value.
 
	d1=(pht_func(inc1,ema1,phase)-pht_func(inc2,ema2,phase))/eps
	d2=(pht_func(inc3,ema3,phase)-pht_func(inc4,ema4,phase))/eps
 
c	finally, combine these two derivatives and return the gradient.
	pht_topder=sqrt(max(1.d-30,d1*d1+d2*d2))
	return
	end

c       ================================================================
	function my_acos(cosAngle,ret)
c       ================================================================
	implicit none

	real*8 my_acos
	real*8 cosAngle
	integer*4 ret

	character*128 string
	real*8 temp
	real*8 cosAngleOriginal

        real*8 sysEpsilon		!system epsilon
        real*8 sysEpsilonTolerance	!system epsilon tolerance

	parameter (sysEpsilon=2.2204460492503131E-16)
	parameter (sysEpsilonTolerance=sysEpsilon * 1.0E+6)


        ret=0

c	return acos(cosAngle) for normal cases

	if (abs(cosAngle).le.1.d0) then
	  my_acos=acos(cosAngle)
	  ret=0
	  return
	endif

	cosAngleOriginal=cosAngle

	cosAngle=sign(1.d0,cosAngle)

	my_acos=acos(cosAngle)
	
	
c	call u_write_msg(3,string)
c	call u_write_msg(3,' ')
c        call u_write_msg(3,'Be advised that the Topographic Model')
c        call u_write_msg(3,
c     .                'Derivative is trying to take the arccosine of:')
c
c	write(string,"('   Angle             = ',f20.17)")
c     .                                                 cosAngleOriginal
c        call u_write_msg(3,string)
	

c	bomb for cosAngle "much" greater than 1.d0.

c	if ((abs(cosAngleOriginal)-1.d0).gt.(sysEpsilonTolerance)) then
c	  temp = (cosAngle*(1.0d0+sysEpsilonTolerance))
c	  write(string,"('sysEpsilonTolerance  = ',f20.17)")temp
c	  call u_write_msg(3,string)
c          call u_write_msg(3,
c     .                'pht_topder: system Epsilon Tolerance exceeded.')
c	  ret = -1.d0
c	  return
c	endif


c	avoid bomb for cosAngle slightly larger than 1.d0.
c	abs(cosAngle-1.d0) should be between zero and sysEpsilon
c	If it's between sysEpsilon and 1000000 * sysEpsilon then let
c	the user know but keep running.

c	if (cosAngle.eq.1.d0) then
c          call u_write_msg(3,'pht_topder has reset the angle to 1.0')
c          call u_write_msg(3,'         and set the arccosine to 0.0')
c	else
c          call u_write_msg(3,'pht_topder has reset the angle to -1')
c          call u_write_msg(3,'         and set the arccosine to pi.')
c	endif

	return
        end


