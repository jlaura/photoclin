
C	**********************************************************************
C	MORE ATMOSPHERE FUNCTIONS
C	I have modified the inner integrand, InrFunc2bint, to evaluate any
C	of 4 different integrands, based on the value of AtmSwitch.  One
C	of these is for Teal's pht_get_ah_table, which has been modified to set
C	the switch to zero.  The other 3 are used in get_hahg_tables, a new
C	routine that sets up 3 integrals for the Hapke atmosphere routines.


c	================================================================
        subroutine pht_get_ah_table  ()
c	================================================================
	implicit none
c_title pht_get_ah_table - perform integrals to get hemispheric, bi-
c	hemispheric albedo; for testing ouput a table of values.
c
c_args  find key variables in pht_common.inc
c
c_desc  This subroutine integrates the photometric fn times mu over
c	mu = 0 to 1 and then over phi = 0 to 2pi to calculate the
c	hemispheric reflectance Ah needed for the photometric model
c	with atmosphere.  Trapezoid rule is applied to table of Ah
c	to obtain bihemispheric albedo Ab.  The parameter atmSwitch
c	(passed in COMMON) is set to 0 to integrate Ah.
c 
c	Nota bene, I don't think the
c	analytic or numerical expressions will be happy at 90 degrees
c	incidence, so evaluate out to 89 and we'll extrapolate the last
c	degree later.  This routine sets one up to evaluate AH by spline
c	interpolation of the table.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original specs
c	15 Jan 1999 K Teal Thompson  Original code;
c	12 Aug 1999 K T Thompson - Declare reals as real*8.
c	29 Dec 2000 RLK - modified /hide_inc/ so phi gets passed, etc.
c				moved factors to outside integration
c       30 May 2006 RLK - added code to report the directional hemispheric
c                   albedo that relates to thermal balance for simulating
c                   Themis images
c_pause	==================================================================
C      INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"

c	Teal has made all reals real*8 down to the lowest numerical routine.

	real*8		pi
	parameter	(pi=3.1415926535897932384626433832795d0)
	real*8		deg2rad
	parameter	(deg2rad=0.01745329251994329576913d0)

c	declare the function-to-be-integrated here as external for passing
	real*8		outrFunc2bint
	external	outrFunc2bint
C This common block is used to pass needed parameters into the innermost
C integrand function from both the inner integral/outer integrand routine
C and the outermost routine
        common /hide_inc/ inc_temp,phi_temp,munot,sini,cosphi
	real*8		inc_temp	!incidence angle
	real*8		phi_temp	!azimuth
	real*8		munot		!cosine of incidence
	real*8		sini		!sine of incidence
	real*8		cosphi		!cosine of azimuth


c	real*8		incIncr	!incidence angle increment
c	integer*4	ninc		!in the ah_common in pht_common.inc
c	parameter	(ninc=91)	!in the ah_common in pht_common.inc
	real*8 		incmin,incmax,deltaInc ! limits of table, increment
	data		incmin,incmax /0.0d0,90.0d0/
	integer*4	inccnt	!do loop incidence angle count,1:ninc
	real*8		fun_temp	!temporary variable for integral
	real*8		factor		!factor for integration: 1 except at ends
					!of interval where it's 1/2
	real*8		yp1,yp2


        deltaInc=(incmax-incmin)/dble(ninc-1)
        Ab=0.0d0
        write(*,*)'rho = ',phtrho 
        do inccnt=1,ninc
           inc_temp=incmin+deltaInc*dble(inccnt-1)
           incTable(inccnt)=inc_temp
           munot=cos(deg2rad*inc_temp)
           sini=sin(deg2rad*inc_temp)

           if (inc_temp.eq.90.0d0) then
              AhTable(inccnt)=0.0d0
           else if (char_pht_function(1:6).eq.'LAMBER') then
              AhTable(inccnt)=1.0d0
           else if (char_pht_function(1:6).eq.'LOMSEL') then
              AhTable(inccnt)=2.0d0*log((1.0d0+munot)/munot)
           else if (char_pht_function(1:6).eq.'MINNAE') then
              AhTable(inccnt)=(munot**kay)/kay
           else if (char_pht_function(1:6).eq.'LUNLAM') then
              Ahtable(inccnt)=
     .         2.0d0*ll*log((1.0d0+munot)/munot)+(1.0d0-ll)
           else
c             numerically integrate the other photometric models
c             outer integral is over phi from 0 to pi rad = 180 deg
              atmSwitch=0
              call qromb_phi(outrFunc2bint,0.0d0,180.0d0,fun_temp)
c             the correct normalization with phi in degrees is:
              AhTable(inccnt)=fun_temp/(90.0d0*munot)
           endif

c          Let's get our estimate of Ab by integrating (summing)
c          A (i)sinicosi over our A  table
c           H                      H
           if ((inccnt.eq.1).or.(inccnt.eq.ninc)) then
              factor=0.5d0 !first,last boundary for partitions
           else
              factor=1.0d0 !other partition boundaries
           end if
           Ab=Ab+AhTable(inccnt)*munot*sini*factor

          if (mod(inccnt,10).eq.1)
     .    write(*,*)'Angle   ','Hem. Alb value   ','Hem. Alb * rho'
          write(*,*)inc_temp,'    ', AhTable(inc_temp),'   ',
     .         AhTable(inc_temp)*phtrho

        enddo
        factor=2.0d0*deltaInc*deg2rad
        Ab=Ab*factor			!normalize by all the fixed factors

c	write(*,*)'Bihemispheric albedo: ',Ab

c       Set up numerical recipes for future spline interp.
        yp1=1.d30
        yp2=1.d30
        call r8spline(incTable,AhTable,ninc,yp1,yp2,AhTable2)

        return
        end


c	================================================================
        subroutine get_hahg_tables  ()
c	================================================================
	implicit none
c_title get_hahg_tables - perform integrals for Hapke/Henyey-Greenstein
c	atmosphere correction; for testing ouput a table of values.
c
c_args  find key variables in pht_common.inc
c
c_desc  This subroutine integrates functions involving the single
c	particle phase function (assumed Henyey-Greenstein) over
c	mu = 0 to 1 and then over phi = 0 to 2pi to calculate the
c	corrections needed for the anisotropic photometric model
c	with atmosphere.  Trapezoid rule is applied to table of Ah
c	to obtain bihemispheric albedo Ab.  The parameter atmSwitch
c	is set to 1, 2, 3 to evaluate the 3 required integrals.
c 
c	Nota bene, I don't think the
c	analytic or numerical expressions will be happy at 90 degrees
c	incidence, so evaluate out to 89 and we'll extrapolate the last
c	degree later.  This routine sets one up to evaluate AH by spline
c	interpolation of the table.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original specs
c	15 Jan 1999 K Teal Thompson  Original code;
c	12 Aug 1999 K T Thompson - Declare reals as real*8.
c	07 Jul 2000 R. L. Kirk - Modify get_ah_table to get other integrals
c	29 Dec 2000 RLK - modified /hide_inc/ so phi gets passed, etc.
c				moved factors to outside integration
c_pause	==================================================================
C      INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"

c	Teal has made all reals real*8 down to the lowest numerical routine.

	real*8		pi
	parameter	(pi=3.1415926535897932384626433832795d0)
	real*8		deg2rad
	parameter	(deg2rad=0.01745329251994329576913d0)

c	declare the function-to-be-integrated here as external for passing
	real*8		outrFunc2bint
	external	outrFunc2bint
C This common block is used to pass needed parameters into the innermost
C integrand function from both the inner integral/outer integrand routine
C and the outermost routine
        common /hide_inc/ inc_temp,phi_temp,munot,sini,cosphi
	real*8		inc_temp	!incidence angle
	real*8		phi_temp	!azimuth
	real*8		munot		!cosine of incidence
	real*8		sini		!sine of incidence
	real*8		cosphi		!cosine of azimuth

c	integer*4	ninc		!in the ah_common in pht_common.inc
c	parameter	(ninc=91)	!in the ah_common in pht_common.inc
	real*8 		incmin,incmax,deltaInc ! limits of table, increment
	data		incmin,incmax /0.0d0,90.0d0/
	integer*4	inccnt	!do loop incidence angle count,1:ninc
	real*8		fun_temp	!temporary variable for integral
	real*8		hahgsb_temp	!save increment to hahgsb
	real*8		factor		!factor for integration: 1 except at ends
					!of interval where it's 1/2
	real*8		yp1,yp2


        deltaInc=(incmax-incmin)/dble(ninc-1)
        hahgsb=0.0d0
        do inccnt=1,ninc
           inc_temp=incmin+deltaInc*dble(inccnt-1)
           incTable(inccnt)=inc_temp
           munot=cos(deg2rad*inc_temp)
           sini=sin(deg2rad*inc_temp)

           atmSwitch=1
           call qromb_phi(outrFunc2bint,0.0d0,180.0d0,fun_temp)
           hahgtTable(inccnt)=fun_temp*wha/(360.0d0)
           atmSwitch=2
           call qromb_phi(outrFunc2bint,0.0d0,180.0d0,fun_temp)
	   hahgsb_temp=fun_temp*wha/(360.0d0)
           if ((inccnt.eq.1).or.(inccnt.eq.ninc)) then
              factor=0.5d0 !first,last boundary for partitions
           else
              factor=1.0d0 !other partition boundaries
           end if
           hahgsb=hahgsb+sini*factor*hahgsb_temp
           if (inc_temp.eq.0.0d0) then
              hahgt0Table(inccnt)=0.0d0
           else
              atmSwitch=3
              call qromb_phi(outrFunc2bint,0.0d0,180.0d0,fun_temp)
              hahgt0Table(inccnt)=fun_temp*wha*munot/(360.0d0*sini)
           end if

c          if (mod(inccnt,10).eq.1)
c     .      write(*,*)'Angle   ','Trans value',
c     .      'Refl value','Trans0 value'
c            write(*,*)inc_temp,'    ',
c     .      hahgtTable(inc_temp),hahgsb_temp,hahgt0Table(inc_temp)

        enddo
        factor=2.0d0*deltaInc*deg2rad
        hahgsb=hahgsb*factor		!normalize by all the fixed factors
						!after the loop, it's faster

c	write(*,*)'Correction to s-bar:  ',hahgsb

c       Set up numerical recipes for future spline interp.
        yp1=1.d30
        yp2=1.d30
        call r8spline(incTable,hahgtTable,ninc,yp1,yp2,hahgtTable2)
        yp1=1.d30
        yp2=1.d30
        call r8spline(incTable,hahgt0Table,ninc,yp1,yp2,hahgt0Table2)

        return
        end



c	==========================================================
	function outrFunc2bint(phi)
c	==========================================================
	implicit none
c_title - outrFunc2bint
c
c_desc  This function is the integral over mu at specified phi
c_args  type    variable  i/o      description
	real*8	phi	  !i	   angle at which we are integrating
c
c_hist  15 Mar 99  Randy Kirk Original code;
c	12 Aug 99  K T Thompson - Declare reals as real*8.
c	29 Dec 2000 RLK - modified /hide_inc/ so phi gets passed, etc.
c
c_pause   ==================================================================

	real*8		deg2rad
	parameter	(deg2rad=0.01745329251994329576913d0)

c	declare the function-to-be-integrated here as external for passing
	real*8		inrFunc2bint
	real*8		outrFunc2bint
	external 	inrFunc2bint
C This common block is used to pass needed parameters into the innermost
C integrand function from both the inner integral/outer integrand routine
C and the outermost routine
        common /hide_inc/ inc_temp,phi_temp,munot,sini,cosphi
	real*8		inc_temp	!incidence angle
	real*8		phi_temp	!azimuth
	real*8		munot		!cosine of incidence
	real*8		sini		!sine of incidence
	real*8		cosphi		!cosine of azimuth

	real*8		result

        phi_temp=phi
        cosphi=cos(phi*deg2rad)
	call qromb_mu(inrFunc2bint,1.d-6,1.0d0,result)	!integral over mu
				 			!from 0 to 1
	outrFunc2bint=result

	return
	end



c	==========================================================
	function inrFunc2bint(mu)
c	==========================================================
	implicit none
c_title - InrFunc2bint
c
c_desc  this function is the inner integrand with all its 
c	parameters except cos(ema) hidden. For atmSwitch=0 this
c	integrand is mu times the photometric angle, giving the
c	hemispheric albedo for the outer integral.  atmSwitch of
c	1, 2, 3, give the 3 integrals over the atmospheric single-
c	particle phase function used in the Hapke/Henyey-Greenstein
c	atmospheric model.
c_args  type    variable  i/o      description
        real*8  mu        !i       cos angle at which we are integrating
c
c_hist  15 Mar 99  Randy Kirk Original code;
c	12 Aug 99  K T Thompson - Declare reals as real*8.
c	07 Jul 2000 R. L. Kirk - Add other integrals besides Ah
c	29 Dec 2000 RLK - modified /hide_inc/ so phi gets passed, etc.
c				moved factors to outside integration
cc_pause   ==================================================================
C      INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"

	real*8		deg2rad
	parameter	(deg2rad=0.01745329251994329576913d0)

	real*8		inrFunc2bint
C This common block is used to pass needed parameters into the innermost
C integrand function from both the inner integral/outer integrand routine
C and the outermost routine
        common /hide_inc/ inc,phi,munot,sini,cosphi
	real*8		inc		!incidence angle
	real*8		phi		!azimuth
	real*8		munot		!cosine of incidence
	real*8		sini		!sine of incidence
	real*8		cosphi		!cosine of azimuth

	real*8		ema		!pass in mu, get emission angle
	real*8		sine		!sin(ema)
	real*8		ALPHA
	real*8		phase		!angle between sun and camera
	real*8		phasefn		!Henyey-Greenstein phase fn
	real*8		xx		!temp var to calculate emunot, emu
	real*8		emunot		!exp(-tau/munot)
	real*8		emu		!exp(-tau/mu)
	real*8		tfac		!factor that occurs in the
					!integrals for transmission
	real*8		pht_func	!calls the user's photometric function


c	calculate the phase angle
c	also calculate any of the other redundant parameters
	ema=acos(mu)/deg2rad
	sine=sin(ema*deg2rad)
	if ((atmSwitch.eq.0).or.(atmSwitch.eq.2)) then	! Reflection
	   ALPHA=sini*sine*cosphi+munot*mu		! phase <= 90
	else						! Transmission
	   ALPHA=sini*sine*cosphi-munot*mu		! phase >= 90
	end if
	phase=acos(ALPHA)/deg2rad
c	now evaluate the integrand; all needed parameters
c	have been hidden separately and passed to it in COMMON.
	if (atmSwitch.eq.0) then
c	  integrand for hemispheric albedo
	  inrFunc2bint=mu*pht_func(inc,ema,phase)
	else 
	  phasefn=(1.d0-hga*hga)/
     .          (1.d0+2.d0*hga*alpha+hga*hga)**1.5d0

	  xx=-tau/max(munot,1.d-30)
	  if (xx.lt.-69.d0) then
	     emunot=0.d0
	  else if (xx.gt.69.d0) then
	     emunot=1.d30
	  else
	     emunot=exp(xx)
	  end if

	  xx=-tau/max(mu,1.d-30)
	  if (xx.lt.-69.d0) then
	     emu=0.d0
	  else if (xx.gt.69.d0) then
	     emu=1.d30
	  else
	     emu=exp(xx)
	  end if

	  if (mu.eq.munot) then
	    tfac=tau*emunot/(munot*munot)
	  else
	    tfac=(emunot-emu)/(munot-mu)
	  end if

	  if (atmSwitch.eq.1) then
	    inrFunc2bint=mu*(phasefn-1.0d0)*tfac
	  else if (atmSwitch.eq.2) then
	    inrFunc2bint=munot*mu*(phasefn-1.0d0)*
     .                (1.0d0-emunot*emu)/(munot+mu)
	  else if (atmSwitch.eq.3) then
	    inrFunc2bint=-sine*cosphi*(phasefn-1.0d0)*tfac
	  else
	    pause 'Bad value of atmSwitch in inrFunc2bint.'
	  end if
	end if

	return
	end


c	EXTERNAL Statement
c	The EXTERNAL statement lets you use the names of external procedures as
c	arguments to other subprograms.
c	see pg 4-22 & 4-25 of DEC Fortran


c	================================================================
	subroutine qromb_phi(func,a,b,ss)
c	================================================================
	implicit none
c_title qromb_phi - copied from qromb in Numerical Recipes Book.
c
c_args  type         variable  i/o      description
	real*8		a,b		!limits of integration/
	real*8		func		!function to be int.
	real*8		ss		!integral of func
c
c_desc  Returns as ss the integral of the function func from a to b.
c       Integration is performed by Romberg's method of order 2K,
c	where, e.g., K=2 is simpson's rule.  Numerical Recipes in
c	Fortran:  The Art of Scientific Computing Second Edition;
c	Press, William H.; 1992; pg.134.
c	uses polint,trapzd_phi
c_hist  11 Aug 99  K Teal Thompson copy from qromb(x) in Numerical
c                  Recipes Book.  Declare reals as real*8.
c	29 Dec 00  Randolph L. Kirk - add absolute error tolerance
c
c_end   ================================================================
	external func

	real*8	  eps				!fractional accuracy desired
	parameter (eps=1.d-4)			!23Dec00 Change back from
						! 1.d-3 and add absolute
						! test also
	real*8    epsa				! Absolute accuracy desired
	parameter (epsa=1.d-6)			!conservative for r*8
	integer*4 stepcntmax,stepcntmaxp	!limits the total number of
	parameter (stepcntmax=20)		!steps
	parameter (stepcntmaxp=stepcntmax+1)
	integer*4 numPoints,numPointsMin	!number of points used in
	parameter (numPoints=5)			!the extrapolation
	parameter (numPointsMin=numPoints-1)
	integer*4 stepcnt
	real*8 dss,h(stepcntmaxp),s(stepcntmaxp)!These store the successive
						!trapezoidal approximations
						!and their relative stepsizes.

c       call u_write_msg(3,'start qromb_phi.')

	h(1)=1.d0
	do 11 stepcnt=1,stepcntmax
	  call trapzd_phi(func,a,b,s(stepcnt),stepcnt)
	  if (stepcnt.ge.numPoints) then
	    call polint(h(stepcnt-numPointsMin),s(stepcnt-numPointsMin),
     .	    numPoints,0.d0,ss,dss)
	    if (abs(dss).le.eps*abs(ss)) return
	    if (abs(dss).le.epsa) return
	  endif
	  s(stepcnt+1)=s(stepcnt)
	  h(stepcnt+1)=0.25d0*h(stepcnt)
11	continue
c	pause 'too many steps in qromb_phi'

c       call u_write_msg(3,'end qromb_phi.')

	end
c       (C) Copr. 1986-92 Numerical Recipes Book.

c	================================================================
	subroutine qromb_mu(func,a,b,ss)
c	================================================================
	implicit none
c_title qromb_phi - copied from qromb in Numerical Recipes Book.
c
c_args  type         variable  i/o      description
	real*8		a,b		!limits of integration/
	real*8		func		!function to be int.
	real*8		ss		!integral of func
c
c_desc  Returns as ss the integral of the function func from a to b.
c	Integration is performed by Romberg's method of order 2K,
c	where, e.g., K=2 is Simpson's rule.
c	Numerical Recipes in Fortran:  The Art of Scientific Computing
c	Second Edition; Press, William H.; 1992; pg.134.
c_hist  11 Aug 99  K Teal Thompson copy from qromb(x) in Numerical
c                  Recipes Book.  Declare reals as real*8.
c	29 Dec 00  Randolph L. Kirk - add absolute error tolerance
c
c_end   ================================================================

	external func

	real*8	  eps				!fractional accuracy desired
	parameter (eps=1.d-4)			!23Dec00 Change back from
						! 1.d-3 and add absolute
						! test also
	real*8    epsa				! Absolute accuracy desired
	parameter (epsa=1.d-6)			!conservative for r*8
	integer*4 stepcntmax,stepcntmaxp	!limits the total number of
	parameter (stepcntmax=20)		!steps
	parameter (stepcntmaxp=stepcntmax+1)
	integer*4 numPoints,numPointsMin	!number of points used in
	parameter (numPoints=5)			!the extrapolation
	parameter (numPointsMin=numPoints-1)
	integer*4 stepcnt
	real*8 dss,h(stepcntmaxp),s(stepcntmaxp)!These store the successive
						!trapezoidal approximations
						!and their relative stepsizes.
c	uses polint,trapzd_mu
	h(1)=1.d0
	do 11 stepcnt=1,stepcntmax
c	  stempcnt will determine number of trapezoidal partitions of area
c	  under curve for "integration".
	  call trapzd_mu(func,a,b,s(stepcnt),stepcnt)
	  if (stepcnt.ge.numPoints) then
	    call polint(h(stepcnt-numPointsMin),s(stepcnt-numPointsMin),
     .	    numPoints,0.d0,ss,dss)
c	    we work only until our necessary accuracy is achieved.
	    if (abs(dss).le.eps*abs(ss)) return
	    if (abs(dss).le.epsa) return
	  endif
	  s(stepcnt+1)=s(stepcnt)
	  h(stepcnt+1)=0.25d0*h(stepcnt)
c	  This is a key step:  the factor is 0.25d0 even though
c	  the stepsize is decreased by 0.5d0.  This makes the
c	  extraplolation a polynomial in h-squared as allowed
c	  by the equation 4.2.1 pg.132, not just a polynomial in h.

11      continue
c	pause 'too many steps in qromb_mu'
	end

c       (C) Copr. 1986-92 Numerical Recipes Book.

c	================================================================
	subroutine trapzd_phi(func,a,b,s,n)
c	================================================================
	implicit none

c_title trapzd_phi - copied from trapzd in Numerical Recipes Book
c
c_args  type         variable  i/o      description
	real*8		func	!i	function to be integrated
	real*8		a	!i	lower limit of phi
	real*8		b	!i	upper limit of phi
	real*8		s	!o	integral (estimated)
	integer*4 	n	!i	number of partitions
c
c_desc  This routine computes the nth stage of refinement of an extended
c	trapezoidal rule.  func is the input as the name of the function
c	to be integrated between limits a and b, also input.  When called
c	with n=1, the routine returns as s the crudest estimate of :
c		_
c	     / b
c	     |
c	     |  f(x)dx
c	    _/ a
c
c	subsequent calls with n=2,3,... (in that sequential order) will
c	improve the accuracy of s by adding 2 to the n-2 additional in-
c	terior points.  s should not be modified between sequential calls.
c	Numerical Recipes in Fortran:  The Art of Scientific Computing
c	Second Edition; Press, William H.; 1992; pg.131.
c_hist  11 Aug 99  K Teal Thompson copy from trapzd(x) in Numerical
c                  Recipes Book.  Declare reals as real*8.
c
c_end   ================================================================

	external func
	real*8	begin
	real*8	end
	integer*4 it,j
	real*8 del,sum,tnm,x

	if (n.eq.1) then
          begin=func(a)
          end  =func(b)
	  s=0.5d0*(b-a)*(begin+end)
	else
	  it=2**(n-2)
	  tnm=it
	  del=(b-a)/tnm   !This is the spacing of the points to be added.
	  x=a+0.5d0*del
	  sum=0.d0
	  do 11 j=1,it
	    sum=sum+func(x)
	    x=x+del
11	continue
	  s=0.5d0*(s+(b-a)*sum/tnm) !This replaces s with its refined value.
	endif

c       call u_write_msg(3,'end trapzd_phi.')

	return
	end
c       (C) Copr. 1986-92 Numerical Recipes Book.


c	================================================================
	subroutine trapzd_mu(func,a,b,s,n)
c	================================================================
	implicit none
 
c_title trapzd_mu - copied from trapzd in Numerical Recipes Book
c
c_args  type         variable  i/o      description
	real*8		func	!i	function to be integrated
	real*8		a	!i	lower limit of mu
	real*8		b	!i	upper limit of mu
	real*8		s	!o	integral (estimated)
	integer*4 	n	!i	number of partitions
c
c_desc  This routine computes the nth stage of refinement of an
c	extended trapezoidal rule.  func is the input as the name of the
c	function to be integrated between limits a and b, also input.  When
c	called with n=1, the routine returns as s the crudest estimate of :
c	       _
c	      / b
c	      |
c	      |  f(x)dx
c	     _/ a
c 
c	subsequent calls with n=2,3,... (in that sequential order) will improve
c	the accuracy of s by adding 2 to the n-2 additional interior points.  s
c	should not be modified between sequential calls.
c	Numerical Recipes in Fortran:  The Art of Scientific Computing
c	Second Edition; Press, William H.; 1992; pg.131.
c_hist  11 Aug 99  K Teal Thompson copy from trapzd() in Numerical
c                  Recipes Book.  Declare reals as real*8.
c
c_end   ================================================================
 
        real*8 begin,end
	external func
	integer*4 it,j
	real*8 del,sum,tnm,x

c       call u_write_msg(3,'start trapzd_mu.')

	if (n.eq.1) then
          begin=func(a)
          end  =func(b)
          s=0.5d0*(b-a)*(begin+end)
	else
	  it=2**(n-2)
	  tnm=it
	  del=(b-a)/tnm
	  x=a+0.5d0*del
	  sum=0.d0
	  do 11 j=1,it
	    sum=sum+func(x)
	    x=x+del
11	continue
	  s=0.5d0*(s+(b-a)*sum/tnm)
	endif

c       call u_write_msg(3,'end trapzd_mu.')

	return
	end
c       (C) Copr. 1986-92 Numerical Recipes Book.
c	================================================================
	subroutine polint(xa,ya,n,x,y,dy)
c	================================================================
	implicit none
c_title polint - copy from polint() in Numerical Recipes Book
c
c_args  type         variable  i/o      description
	integer*4	n	!i
	real*8		xa(n)	!i
	real*8		ya(n)	!i
	real*8		x	!i
	real*8		y	!o
	real*8		dy	!o	error estimate
c
c_desc  Given arrays xa and ya, each of length n, and given a value x,
c	this routine returns a value y, and an error estimate dy. If
c	P(x) is the polynomial of degree N-1 such that P(xa ) = ya ;
c							   i      i
c	i = 1, . . . , n, then the returned value y = P(x).
c_hist  11 Aug 99  K Teal Thompson copy from polint() in Numerical
c                  Recipes Book.  Declare reals as real*8.
c_end   ================================================================
	integer*4 nmax
	parameter (nmax=10)	!Largest anticipated value of n.
	integer*4 i,m,ns
	real*8 den,dif,dift,ho,hp,w,c(nmax),d(nmax)
	ns=1
	dif=abs(x-xa(1))
	do 11 i=1,n		!Here we find the index ns of the closest
				!table entry,
	  dift=abs(x-xa(i))
	  if (dift.lt.dif) then
	    ns=i
	    dif=dift
	  endif
	  c(i)=ya(i)		!and initialize the tableau of c's and d's.
	  d(i)=ya(i)
11	continue
	y=ya(ns)		!This is the initial approximation to y.
	ns=ns-1
	do 13 m=1,n-1		!For each column of the tableau,
				!we loop over the current c's and d's
	  do 12 i=1,n-m		!and update them.
	    ho=xa(i)-x
	    hp=xa(i+m)-x
	    w=c(i+1)-d(i)
	    den=ho-hp
	    if(den.eq.0.)pause 'failure in polint'
	    den=w/den
	    d(i)=hp*den
	    c(i)=ho*den
12	  continue
	  if (2*ns.lt.n-m)then	!After each column in the tableau is
				!completed, we decide which correction,
				!c or d, we want to add to our accu-
	    dy=c(ns+1)		!mulating value of y, i.e., which path
				!to take through the tableau|forking up
	  else			!or down. We do this in such a way as to
				!take the most "straight line" route
	    dy=d(ns)		!through the tableau to its apex, updat-
				!ing ns accordingly to keep track of
	    ns=ns-1		!where we are. This route keeps the par-
				!tial approximations centered (insofar
	  endif			!as possible) on the target x. The last
	  y=y+dy		!dy added is thus the error indication.
13	continue
	return
	end
c       (C) Copr. 1986-92 Numerical Recipes Book

c	================================================================
	subroutine r8spline(x,y,n,yp1,ypn,y2)
c	================================================================
        implicit none
c_title r8spline - real*8 version of spline in Numerical Recipes
c                  Book
c
c_args  type         variable  i/o      description
	real*8		yp1	!i
	real*8		ypn	!i
	integer*4	n	!i
	real*8		x(n)	!i
	real*8		y(n)	!i
	real*8		y2(n)	!o
c
c_desc  Given arrays x(1:n) and y(1:n) containing a tabulated function, i.e.,
c	y  = f(x ), with x  <  x  < .. < x , and given values yp1 and ypn for
c	 i	i         1     2         n
c
c	the first derivative of the interpolating function at points 1 and n,
c	respectively, this routine returns an array y2(1:n) of length n which
c	contains the second derivatives of the interpolating function at the
c                                                                 30
c	tabulated points x .  If yp1 and/or ypn are equal to 1 x 10   or lar-
c                         i
c	ger, the routine is signaled to set the corresponding boundary condi-
c	tion for a natural spline, with zero second derivative on that bound-
c	ary.
c
c_hist  11 Aug 99  K Teal Thompson copy from spline() in Numerical
c                  Recipes Book.
c_end   ================================================================

	integer*4 nmax		!largest anticipated value of n.
	parameter (nmax=500)
	integer*4 i,k
	real*8 p,qn,sig,un,u(nmax)
	if (yp1.gt..99e30) then	!The lower boundary condition is set either to be
	  y2(1)=0.d0		!"natural"
	  u(1)=0.d0
	else			!or else to have a specified first derivative.
	  y2(1)=-0.5d0
	  u(1)=(3.d0/(x(2)-x(1)))*((y(2)-y(1))/(x(2)-x(1))-yp1)
	endif
	do 11 i=2,n-1		!This is the decomposition loop of the tri-
				!diagonal algorithm.  y2 and u are used for
				!temporary storage of the decomposed factors.
	  sig=(x(i)-x(i-1))/(x(i+1)-x(i-1))
	  p=sig*y2(i-1)+2.d0
	  y2(i)=(sig-1.d0)/p
	  u(i)=(6.d0*((y(i+1)-y(i))/(x(i+
     *	    1)-x(i))-(y(i)-y(i-1))/(x(i)-x(i-1)))/(x(i+1)-x(i-1))-sig*
     *	    u(i-1))/p
11	continue
	if (ypn.gt..99e30) then	!The lower boundary condition is set either to be
	  qn=0.d0		!"natural"
	  un=0.d0
	else			!or else to have a specified first derivative.
	  qn=0.5d0
	  un=(3.d0/(x(n)-x(n-1)))*(ypn-(y(n)-y(n-1))/(x(n)-x(n-1)))
	endif
	y2(n)=(un-qn*u(n-1))/(qn*y2(n-1)+1.d0)
	do 12 k=n-1,1,-1	!This is the back-substitution loop of the tridi-
	  y2(k)=y2(k)*y2(k+1)+u(k)	!agonal algorithm.
12	continue
	return
	end
C  (C) Copr. 1986-92 Numerical Recipes Book

