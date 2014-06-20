c	==================================================================
        function mod_albedo(inc,ema,phase,dn,ret)
c	==================================================================
        implicit none
c_title - mod_albedo - Albedo normalization in absence of atmosphere
c
c_args  type         variable  i/o      description
        real*8		ema	!i	emission angle
        real*8		inc	!i	incidence angle
        real*8		phase	!i	phase angle (deg)
        real*8		dn	!i	input brightness
	integer*4	ret	!o	returning error code
				!	(-1) divide by zero error

        real*8		mod_albedo	!the function
c
c_desc  Consistent dividing out of photometric model at given an-
c	gles and putting it back in at reference incidence but zero
c	phase.  Let the reference incidence default to zero.  For
c	Hapke model only, the photometric function multiplied back
c	in will be modified to take out opposition effect.  This
c	requires saving the actual value of B0 while temporarily
c	setting it to zero in the common block to compute the over-
c	all normalization.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       17 Mar 1999 K Teal Thompson  Add implicit none, declare vars.
c       13 Mar 2003 Janet Barrett, Added hgaref variable.
c_end   ==================================================================
#include "pht_common.inc"
        real*8 b0save
c       real*8 b0
	real*8 emaref
	real*8 phaseref
        real*8 psurfref
        real*8 psurf
c       real*8 thresh
        real*8 psurf1
        real*8 psurf2
        real*8 psurf3
	integer*4 phasecnt
	integer*4 testFlag

        real*8 pht_func


	logical*1 firstime
	data firstime /.true./

	if (firstime.eqv..true.) then
c	  First-time setup:
c	  Calculate normalization at standard conditions
c	  Turn off Hapke opposition effect (must have access to PPARS_HAPKE)
	  b0save=b0
	  b0=0.d0
	  emaref=0.d0
	  phaseref=incref
c	  thresh=100.d0
	  psurfref=pht_func(incref,emaref,phaseref)
	  b0=b0save
c	  end of first-time setup
	  firstime=.false. 
	endif
 
c	code for scaling each pixel
	psurf=pht_func(inc,ema,phase)


	testFlag = 1
cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	if (testFlag.eq.0) then ! this section for testing
cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	call u_write_msg(3,
     .  'psurf1=pht_func(0.d0,phase,phase)')
	call u_write_msg(3,
     .  'psurf2=pht_func(phase,0.d0,phase)')
	call u_write_msg(3,
     .  'psurf3=pht_func(phase/2.d0,phase/2.d0,phase)')
	call u_write_msg(3,
     .  'phase    psurf1     psurf2     psurf3')
	do phasecnt=0,170,10

	phase=dble(phasecnt)

	psurf1=pht_func(0.d0,phase,phase)

	psurf2=pht_func(phase,0.d0,phase)

	psurf3=pht_func(phase/2.d0,phase/2.d0,phase)

	write(*,99)phasecnt,psurf1,psurf2,psurf3
	enddo


        do phasecnt=174,179,5

        phase=dble(phasecnt)

        ema=phase
        inc=0.d0
        psurf1=pht_func(inc,ema,phase)

        ema=0.d0
        inc=phase
        psurf2=pht_func(inc,ema,phase)

        ema=phase/2.d0
        inc=phase/2.d0
        psurf3=pht_func(inc,ema,phase)


        write(*,99)phasecnt,psurf1,psurf2,psurf3
 99	format(i5,'   ',3f11.7)
        enddo
c temp endif, endif above commented out.
c	endif



cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	endif ! if (1.eq.0) testing
cxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx



 10	continue


c	thresh is a parameter limiting how much we amplify the dns
	if (psurfref.gt.psurf*thresh) then
	   mod_albedo=0.d0
	else
	   if (psurf.eq.0) then
	     call u_write_msg(3,
     .       'Divide by zero error.  Mod_albedo.')
	     mod_albedo=0.d0
	     ret = -1
	   else
	     mod_albedo=dn*psurfref/psurf
	   end if
	end if


	return
	end


c	==================================================================
        function mod_topo(inc,ema,phase,dn,ret)
c	==================================================================
        implicit none
c_title Topographic normalization in absence of atmosphere
c
c_args  type         variable  i/o      description
        real*8		ema	!i	emission angle
        real*8		inc	!i	incidence angle
        real*8		phase	!i	phase angle (deg)
        real*8		dn	!i	phase angle (deg)
	integer*4	ret	!o	returning error code
				!	(-1) divide by zero error
				!	(-2) pht_topder error

        real*8		mod_topo	!the function

c_desc  This is done in two passes with a divide filter in between.
c	Pass 1 is simply the albedo normalization without atmosphere.In
c	order that pass 2 knows what reference conditions are used in pass
c	1, we require them to be zero incidence, emission and phase angles.
c	In pass 2 the reference angle parameters in TAE can then be used
c	to specify the final reference conditions.  For the output to be
c	useful we must set the reference incidence angle to a finite value.
c	Otherwise the topographic contrast is normalized to zero.  Pass two
c	takes a special mode as specified below.
c	The PICS version of this mode required input of the NORM parameter
c	(additive constant) used in association with the divide filter.
c	NORM was only used to prevent negative values in PICS so it is not
c	needed in ISIS.  The results of the divide filter will be centered
c	on 1.  However, I now require an input ALBEDO which will be the
c	(uniform) normal albedo with which the output image will be syn-
c	thesized.  The user can obtain a reasonable value for this as the
c	average value of the output of pass 1, or the average over multiple
c	images being processed for mosaicking.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       17 Mar 1999 K Teal Thompson  Add implicit none, declare vars.
c
c_end   ==================================================================
#include "pht_common.inc"
	real*8 b0save
c	real*8 b0
	real*8 emaref
	real*8 phaseref
	real*8 psurf0
	real*8 psurfref
	real*8 pprimeref
	real*8 psurf
	real*8 pprime
	real*8 pht_topder
	real*8 rhobar
c	real*8 thresh


        real*8 pht_func

	logical*1 firstime
	data firstime /.true./

	if (firstime.eqv..true.) then
c	  First-time setup:
	  b0save=b0
	  b0=0.d0
	  psurf0=pht_func(0.d0,0.d0,0.d0)      ! reference of pass 1
	  if (psurf0.eq.0) then
	    call u_write_msg(3,
     .       'Divide by zero error.  mod_topo.')
	    mod_topo=0.d0
	    ret = -1
	  else
	    rhobar=albedo/psurf0
	  end if
	  emaref=0.d0
	  phaseref=incref
c	  thresh=100.d0
          psurfref=pht_func(incref,emaref,phaseref) ! reference of pass 2
          pprimeref=pht_topder(incref,emaref,phaseref,ret)
	  if (ret.lt.0) then
	    mod_topo=0.d0
	    ret=-2
	    return
	  endif
	  b0=b0save
c	  end of first-time setup
	  firstime=.false. 
	endif
 
c	code for scaling each pixel
	psurf=pht_func(inc,ema,phase)
	pprime=pht_topder(inc,ema,phase,ret)
	if (ret.lt.0) then
	  mod_topo=0.d0
	  ret=-2
	  return
	endif
	if (psurf*pprimeref.gt.pprime*thresh) then
	   mod_topo=0.d0
	else
c	  had the following (perhaps because i used a highpass filter)
c		a=rhobar*(psurf*pprimeref)/(psurfref*pprime)
c	  with a divide filter psurfref gets divided out, so we have
c		a=rhobar*(psurf*pprimeref)/(pprime)
c		b=rhobar*psurfref-a
c	        dnout=dn*a+b
	  if (pprime.eq.0) then
	    call u_write_msg(3,
     .       'Divide by zero error.  mod_topo.')
	    mod_topo=0.d0
	    ret = -1
	  else
	    mod_topo=dn*rhobar*(psurf*pprimeref)/pprime
     .		    + rhobar*psurfref
     .              - rhobar*(psurf*pprimeref)/(pprime)
	  end if
	end if
c	I also fixed this in the if statement above.
	return
	end

c	==================================================================
        function mod_albat(inc0,ema0,phase,inc,ema,dn,ret)
c	==================================================================
        implicit none
c_title mod_albat - Albedo normalization with atmosphere
c
c_args  type         variable  i/o      description
        real*8		ema0	!i	emission angle (ellipsoid)
        real*8		inc0	!i	incidence angle (ellipsoid)
        real*8		phase	!i	phase angle (deg) 
	real*8          inc     !i      incidence angle (dem)
	real*8          ema     !i      emission angle (dem)
        real*8		dn	!i	input brightness
	integer*4	ret	!o	returning error code
				!	(-1) divide by zero error

        real*8		mod_albat	!the function

c_desc  
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       17 Mar 1999 K Teal Thompson  Add implicit none, declare vars.
c       10 Jul 2003 RK, Added capability to use photometric angles
c                   from the ellipsoid for the atmosphere model and
c                   use photometric angles from a dem for the 
c                   surface model

c_end   ==================================================================
#include "pht_common.inc"
c	Must access common blocks PPARS_HAPKE, AH_TABLE, APARS
	logical*1 null_negs_flag
	common /null_negs_common/ null_negs_flag
	real*8 b0save
	real*8 emaref
	real*8 phaseref
	real*8 psurfref
	real*8 ahref
	real*8 munotref
	real*8 tausave
	real*8 whasave
	real*8 bhasave
	real*8 hgasave
	real*8 pstdref
	real*8 transref
	real*8 trans0ref
c	real*8 sbarref
	real*8 psurf
	real*8 ahInterp
	real*8 munot
	real*8 deg2rad
	real*8 pstd
	real*8 trans
	real*8 trans0
	real*8 sbar
	real*8 rho
	real*8 dpo
	real*8 q
	real*8 dpm
c	real*8 dnout

	real*8 firsterm
	real*8 secondterm
	real*8 thirdterm
	real*8 fourthterm
	real*8 fifthterm


        real*8 pht_func

	logical*1 firstime
        parameter     (deg2rad=0.01745329251994329576913d0)
	data firstime /.true./
c	data hnorm /0.003d0/,tau/0.12/,wha/0.8d0/,bha/1.d0/


	if (firstime.eqv..true.) then
c	  First-time setup:
c	  Calculate normalization at standard conditions
c	  Turn off Hapke opposition effect
	  b0save=b0
	  b0=0.d0
	  emaref=0.d0
	  phaseref=incref
	  psurfref=pht_func(incref,emaref,phaseref)

	  b0=b0save
c	  get reference hemispheric albedo (b0 doesn't influence it much)
	  call pht_get_ah_table()!AB eval here by integration.

	  call r8splint(incTable,ahTable,ahTable2,ninc,incref,ahref)
c	  call r8splint(incTable,ahTable,ahTable2,ninc,60.d0,AB)!Not needed
	  munotref=cos(deg2rad*incref)

c	  now calculate atmosphere at standard conditions
	  tausave=tau
	  tau=tauref
	  whasave=wha       ! really, maybe we only want to modify tau
	  wha=wharef        ! but this lets us change any of these params
	  bhasave=bha
	  bha=bharef
	  hgasave=hga
	  hga=hgaref
	  call atm_func(incref,emaref,phaseref,pstdref,transref,
     .      trans0ref,sbar)
	  tau=tausave
	  wha=whasave
	  bha=bhasave
	  hga=hgasave
	  firstime=.false. 
c	  end of first-time setup

	endif
 
c	code for scaling each pixel from dn to dnout
	psurf=pht_func(inc,ema,phase)

	call r8splint(incTable,ahTable,ahTable2,ninc,inc0,ahInterp)
	munot=cos(deg2rad*inc0)
	call atm_func(inc0,ema0,phase,pstd,trans,trans0,sbar)

c	with model at actual geometry, calculate rho from dn
	dpo=dn-pstd
	dpm=(psurf-ahInterp*munot)*trans0
	q=ahInterp*munot*trans+AB*sbar*dpo+dpm

	if(dpo.le.0.d0.and.null_negs_flag)then
	  rho=0.d0
	else
	  firsterm=AB*sbar
	  secondterm=dpo*dpm
	  thirdterm=firsterm*secondterm
	  fourthterm=q**2.0d0-4.d0*thirdterm
	  if(fourthterm.lt.0)then
            ret=-1
            call u_write_msg(3,
     .  'ERROR: sqrt of negative - mod_albat')
            mod_albat=0.d0
            return
          else
	    fifthterm=q+sqrt(fourthterm)
	  endif
	  rho=2*dpo/fifthterm
c	  rho=2*dpo/(q+sqrt(q**2.0d0-4.d0*AB*sbar*dpo*dpm))

	endif

c	now use rho and reference geometry to calculate output dnout
c	Teal's note:  mod_albat is a function of dn because rho is a
c	function of dn.
	if ((1.d0-rho*AB*sbar).le.0.d0) then
          ret=-1
          call u_write_msg(3,
     .  'Only divide by zero error - mod_albat')
          mod_albat=0.d0
          return
        else
	  mod_albat=pstdref+rho*(ahref*munotref*transref/
     .    (1.d0-rho*AB*sbar)
     *	  +(psurfref-ahref*munotref)*trans0ref)
        endif


c	These nonlinear transformations can be linearized for application
c	to an entire image by using "representative" values of DN in the
c	denominator of the first step and of rho in the denominator of the
c	second part.

	return
	end
c	==================================================================
        function mod_topat(inc0,ema0,phase,inc,ema,dn,ret)
c	==================================================================
        implicit none
c_title mod_topat - Topographic Normalization with Atmosphere
c
c_args  type         variable  i/o      description
        real*8       ema0      !i       emission angle (ellipsoid)
        real*8       inc0      !i       incidence angle (ellipsoid)
        real*8       phase     !i       phase angle (deg)
        real*8       inc       !i       incidence angle (dem)
        real*8       ema       !i       emission angle (dem)
        real*8       dn        !i	input brightness
	integer*4    ret       !o	returning error code
				!	(-1) divide by zero error
				!	(-2) pht_topder error

        real*8		mod_topat	!the function

c_desc  As in the case without an atmosphere, processing proceeds
c	in three steps, a pass 1 PHOTOM followed by a divide filter to is-
c	olate topography from albedo variations followed by a pass 2 PHOTOM.
c	The first pass is intended to isolate relative albedo from overall
c	photometric shading so that the filter will work as well as possible.
c	The second pass cleans up whatever funny scaling the first pass did
c	and scales the topographic modulation to desired standard conditions;
c	as before these had best not be normal incidence, or the topography
c	will vanish!
c	 
c	Also as in the no-atmosphere case, the albedo mode (with atmosphere
c	this time) is used for the first pass.  The reference geometry for
c	this pass is normal incidence with no atmosphere, i.e.,
c	INC=EMA=PHASE=TAU=0
c	The second pass is going to assume implicitly that these reference
c	values were used the first time, freeing the TAE reference parameters
c	to define the conditions of finite incidence and maybe finite optical
c	depth to which the output will be normalized.
c	 
c	Figuring out the scaling that got applied to the topographic modu-
c	lation in pass 1 turns out to be tricky because of the nonlinearity
c	of the equations and the fact that the original DN (which would be
c	recoverable from the atmosphere model immediately after pass 1) is
c	lost after the divide filter.  As an approximation, I will require
c	the user to input ALBEDO, which is the average DN in the image after
c	pass 1, a measure of the average value of RHO and, given the norm-
c	alization, the average of the normal albedo in the area.
c	 
c	Rather than do the calculation analytically as I did in my hand-
c	written notes, I evaluate the contrast of unit slope numerically 
c	(at given albedo or RHO; the first-order dependence on RHO has been
c	divided out so this is inaccurate to second order same as the other
c	derivation but it is a whole lot easier to follow).  If it turns
c	out to be desirable, we can define two values of ALBEDO for this
c	program, one describing the input (i.e., giving the albedo for this
c	one image, at which slope normalization is calculated) and the other
c	describing the output (the albedo to be used in simulating the out-
c	put image).
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       17 Mar 1999 K Teal Thompson  Add implicit none, declare vars.
c       10 Jul 2003 RK, Added capability to use photometric angles
c                   from the ellipsoid for the atmosphere model and
c                   use photometric angles from a dem for the 
c                   surface model
c
c_end   ==================================================================
#include "pht_common.inc"
c	Must access common blocks PPARS_HAPKE, AH_TABLE, APARS
	real*8 b0save
c	real*8 b0
	real*8 emaref
	real*8 phaseref
	real*8 psurf0
	real*8 psurfref
	real*8 pprimeref
	real*8 ahref
	real*8 munotref
	real*8 tausave
c	real*8 tauref
	real*8 whasave
c	real*8 wharef
	real*8 bhasave
c	real*8 bharef
        real*8 hgasave
	real*8 pstdref
	real*8 transref
	real*8 trans0ref
c	real*8 sbarref
	real*8 psurf
	real*8 pprime
	real*8 pht_topder
	real*8 ahInterp
	real*8 munot
	real*8 deg2rad
	real*8 pstd
	real*8 trans
	real*8 trans0
	real*8 sbar
	real*8 rhotlt
	real*8 dpo
	real*8 q
	real*8 slope
	real*8 pprimeeff
	real*8 rhobar
c	real*8 albedo
	real*8 ptilt
	real*8 dpm
c	real*8 dnout
	real*8 aout
	real*8 bout
	real*8 pflat
	real*8 rhoflat
	real*8 eps
	parameter (eps=0.1d0)       ! Small slope in degrees for derivative
        parameter (deg2rad=0.01745329251994329576913d0)


        real*8 pht_func


	logical*1 firstime
	data firstime /.true./


c	logical*1 firstimetest
c	data firstimetest /.true./



c	data hnorm /0.003d0/,tau/0.3d0/,wha/0.8d0/,bha/1.d0/
c	data albedo /1.d0/

	if (firstime.eqv..true.) then
c	  First-time setup:
c	  Calculate normalization at standard conditions
c	  Turn off Hapke opposition effect
	  b0save=b0
	  b0=0.d0
	  psurf0=pht_func(0.d0,0.d0,0.d0)      ! reference of pass 1
	  if (psurf0.eq.0) then
	    call u_write_msg(3,
     .       'Divide by zero error.  mod_topat.')
	    mod_topat=0.d0
	    ret = -1
	  else
 	    rhobar=albedo/psurf0
	  end if
	  emaref=0.d0
	  phaseref=incref
	  psurfref=pht_func(incref,emaref,phaseref) ! ref of pass 2
	  pprimeref=pht_topder(incref,emaref,phaseref,ret)
	  if (ret.lt.0) then
	    mod_topat=0.d0
	    ret=-2
	    return
	  endif
	  b0=b0save
c	  get reference hemispheric albedo (b0 doesn't influence it much)
	  call pht_get_ah_table() !AB eval here by integration.
	  call r8splint(incTable,ahTable,ahTable2,ninc,incref,ahref)
c	  call r8splint(incTable,ahTable,ahTable2,ninc,60.d0,AB) !don't need
	  munotref=cos(deg2rad*incref)
c	  now calculate atmosphere at standard conditions
	  tausave=tau
	  tau=tauref
	  whasave=wha       ! really, maybe we only want to modify tau
	  wha=wharef
	  bhasave=bha
	  bha=bharef
	  hgasave=hga
	  hga=hgaref
	  call atm_func(incref,emaref,phaseref,
     .      pstdref,transref,trans0ref,sbar)
	  tau=tausave
	  wha=whasave
	  bha=bhasave
	  hga=hgasave
c	  finally, calculate the additive and multiplicative parts of the
c	  output-normalized signal, from the point of view of fixed albedo and
c	  varying topography
	  aout=rhobar*pprimeref*trans0ref
	  bout=pstdref+rhobar*(transref*ahref*munotref/
     *	            (1.d0-rhobar*AB*sbar)
     *	            +trans0ref*(psurfref-ahref*munotref))
c	  end of first-time setup
	  firstime=.false. 
	endif
c	code for scaling each pixel from dn to dnout
	psurf=pht_func(inc,ema,phase)
	pprime=pht_topder(inc,ema,phase,ret)
	if (ret.lt.0) then
	  mod_topat=0.d0
	  ret=-2
	  return
	endif
	call r8splint(incTable,ahTable,ahTable2,ninc,inc0,ahInterp)
	munot=cos(deg2rad*inc0)
	call atm_func(inc0,ema0,phase,pstd,trans,trans0,sbar)

	pflat=pstd+rhobar*(trans*ahInterp*munot/(1.d0-rhobar*AB*sbar)
     *	           +trans0*(psurf-ahInterp*munot))

	ptilt=pflat+rhobar*pprime*trans0*eps

	dpo=ptilt-pstd
	dpm=(psurf-ahInterp*munot)*trans0
	q=ahInterp*munot*trans+AB*sbar*dpo + dpm
	rhotlt=2*dpo/(q+sqrt(q**2.0d0-4.d0*AB*sbar*dpo*dpm))

c	rhoflat added 18 Nov 1999
        dpo=pflat-pstd
	q=ahInterp*munot*trans+AB*sbar*dpo + dpm
	rhoflat=2*dpo/(q+sqrt(q**2.0d0-4.d0*AB*sbar*dpo*dpm))

	pprimeeff=(rhotlt-rhoflat)/(rhoflat*eps)
	slope=(dn-1.d0)/pprimeeff
	mod_topat=aout*slope+bout
	return
	end

c	==================================================================
        function mod_lunar(inc,ema,phase,dn,ret)
c	==================================================================
	implicit real*8 (a-z)
c_title mod_lunar - MOONPR/ALBEDO (and NOALBEDO) mode
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle
        real*8       inc       !i       incidence angle
        real*8       phase     !i       phase angle (deg)
        real*8       dn        !i	phase angle (deg)
	integer*4    ret       !o	returning error code
				!	(-1) divide by zero error

        real*8		mod_lunar	!the function

c_desc  ALBEDO mode really requires special code because of albedo-depen-
c	dent phase function normalization.  NOALBEDO mode in current soft-
c	ware applies the photometric correction with the same parameters
c	appropriate to the Moon, but no phase correction.  The NOALBEDO
c	case can be obtained by using the ordinary albedo normalization
c	code with the right lunar parameter values.  It can also be imple-
c	mented as an early return (or loop exit/go to next pixel) before
c	the phase normalization here if that's desirable.  The benefit would
c	be not having to type the parameters in, but if we have some kind of
c	helper files for parameters then this wouldn't be an issue.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       17 Mar 1999 K Teal Thompson  Add implicit none, declare vars.
c	05 May 1999 KTT Change to implicit real*8 a-z.
c_end   ==================================================================
#include "pht_common.inc"
	logical*1 firstime
	character*8 moonOption
	parameter     (deg2rad=0.01745329251994329576913d0)
	data firstime /.true./
	common /moon_opt_common/d,e,f,g2,h,bsh1,xb1,xb2,xmul,wl,
     .    moonOption
	mod_lunar=0.d0
	if (firstime.eqv..true.) then
	  if (moonOption.eq.'PHOTOM') then
	    ret=-1
	    call u_write_msg(3,
     .  ' ')
	    call u_write_msg(3,
     .  'Error:')
	    call u_write_msg(3,
     .  'Use MOONOPT = ALBEDO or NOALBEDO')
	    call u_write_msg(3,
     .  'with GENMOD = LUNAR.')
	    mod_lunar=0.d0
	    return
	  endif
c	  First-time setup:
c	  Must have PHT_FUNC set to PHT_LUNLAM_EMP before calling this rtne
c	  Use McEwen's polynomial for L, no internal phase function
c	        VALUES FOR MOON DERIVED BY A. MCEWEN, 11/95
	  lcof(1)=-0.019d0   !moon only
	  lcof(2)= 0.00024d0 !
	  lcof(3)=-0.146d-5  !
	  nl=3
	  bcof(1)=1.d0
	  nb=1
c	  override tae inputs and use clem-standard reference state
	  incref=30.d0
	  emaref=0.d0
	  phaseref=30.d0
c	  thresh=100.d0

c	  calculate normalization at standard conditions
c	  don't need to turn off hapke opposition effect this time
	  psurfref=pht_func(incref,emaref,phaseref)

c	  must also have inputs or defaults (see old code) for the
c	  parameters d, wl, e, f, g2, xmul, bsh1, xb1, and xb2.  then
c	  the remaining setup goes as follows.  note that we might find
c	  a more elegant way to handle the repeated calculations of
c	  the functions of the form pg and bshad with error checks for
c	  powers-of-negative and divide-by-zero respectively.  they
c	  could be implemented as statement functions.  the error check
c	  on the bshad is not needed if it's only used for positive
c	  angles, anyway.

	  f1=1.d0-f

c	  (estimate r30 as 0.1 initially)
	  g1=d*0.1d0+e
	  g1sq=g1*g1
	  g2sq=g2*g2
	  c30=cos(30.d0*deg2rad)
	  if (1.d0+g1sq+2.d0*g1*c30.le.0.d0) then
            ret=-1
            call u_write_msg(3,
     .      'Divide by zero error - mod_lunar')
	    mod_lunar=0.d0
            return
          endif

          pg130=f1*(1.d0-g1sq)/(1.d0+g1sq+2.d0*g1*c30)**1.5d0

	  if (1.d0+g2sq+2.d0*g2*c30.le.0.d0) then
            ret=-1
            call u_write_msg(3,
     .      'Divide by zero error - mod_lunar')
	    mod_lunar=0.d0
            return
          endif

	  pg230=f*(1.d0-g2sq)/(1.d0+g2sq+2.d0*g2*c30)**1.5d0
	  if (bsh1.le.0.d0) bsh1=0.d0

	  if (1.d0+tan(30.d0*0.5d0*deg2rad)/h.eq.0.d0) then
            ret=-1
            call u_write_msg(3,
     .      'Divide by zero error - mod_lunar')
	    mod_lunar=0.d0
            return
          endif

	  bshad30=1.d0+bsh1/(1.d0+tan(30.d0*0.5d0*deg2rad)/h)
	  pg30=(pg130+pg230)*bshad30
c	  normalize to buratti function at phase = 2.d0 deg.
	  bc1=xb1+xb2*wl
	  fbc3=1.d0+bc1*2.d0

	  if (fbc3.eq.0.d0) then
            ret=-1
            call u_write_msg(3,
     .      'Divide by zero error - mod_lunar')
	    mod_lunar=0.d0
            return
          endif

	  c3=cos(2.d0*deg2rad)

	  if (1.d0+g2sq+2.d0*g2*c3.le.0.d0) then
            ret=-1
            call u_write_msg(3,
     .      'Divide by zero error - mod_lunar')
	    mod_lunar=0.d0
            return
          endif

	  pg32=f*(1.d0-g2sq)/(1.d0+g2sq+2.d0*g2*c3)**1.5d0

	  if (1.d0+tan(2.d0*0.5d0*deg2rad)/h.eq.0.d0) then
            ret=-1
            call u_write_msg(3,
     .      'Divide by zero error - mod_lunar')
	    mod_lunar=0.d0
            return
          endif

	  bshad3=1.d0+bsh1/(1.d0+tan(2.d0*0.5d0*deg2rad)/h)
c	  initial guess for the answer; after this, use the previous
c	  pixel value instead
	  dnold=0.1d0
c	  end of first-time setup
	  firstime=.false. 
	endif

c	code for scaling each pixel
	psurf=pht_func(inc,ema,phase)
c	thresh is a parameter limiting how much we amplify the dns
	if (psurfref.gt.psurf*thresh) then
	   mod_lunar=0.d0
	else
	   a=psurfref/psurf
	   if (moonOption.eq.'noalbedo'.or.
     .        moonOption.eq.'NOALBEDO') then
	      !moonpr without b additive
	      mod_lunar=dn*a
	   elseif (moonOption.eq.'albedo'.or.
     .        moonOption.eq.'ALBEDO') then
	      ALPHA=cos(phase*deg2rad)!was called cosa
	      if ((1.d0+g2sq+2.d0*ALPHA).le.0.d0) then
	        ret=-1
		call u_write_msg(3,
     .          'Divide by zero error - mod_lunar')
	        mod_lunar=0.d0
	        return
	      endif
	      pg2=f*(1.d0-g2sq)/(1.d0+g2sq+2.d0*ALPHA)**1.5d0
c	      Ohmigod! existing code forgot the unneeded error check on this!
	      if (h.eq.0.) then
		call u_write_msg(3,
     .          'Divide by zero error - mod_lunar')
		ret = -1
	        mod_lunar=0.d0
		return
	      endif
c	      changed b to phase
	      bshad=1.d0+bsh1/(1.d0+tan(phase*0.5d0*deg2rad)/h)
	      r=dn*xmul
c	      as in existing version, guess the answer and iterate
c	      but let's be smart:  guess the answer from the last pixel
c	      and test whether the answer changes instead of doing a fixed
c	      number of iterations
 5000	      continue
	      g1=d*r30old+e
	      g1sq=g1*g1
	      if (1.d0+g1sq+2.d0*g1*ALPHA.le.0.d0) then
                ret=-1
                call u_write_msg(3,
     .          'Divide by zero error - mod_lunar')
		mod_lunar=0.d0
                return
              endif
	      pg1=f1*(1.d0-g1sq)/(1.d0+g1sq+2.d0*g1*ALPHA)**1.5d0
	      pg=(pg1+pg2)*bshad
	      if (phase.lt.2.d0) then
	        fbc=1.d0+bc1*phase
	        if (1.d0+g1sq+2*g1*c3.le.0.d0)  then
                  ret=-1
                  call u_write_msg(3,
     .            'Divide by zero error - mod_lunar')
	          mod_lunar=0.d0
                  return
                endif
	        pg31=f1*(1.d0-g1sq)/(1.d0+g1sq+2*g1*c3)**1.5d0
	        pg3=(pg31+pg32)*bshad3
	        pg=fbc*pg3/fbc3
	      end if
	      if (pg.eq.0.d0)  then
                ret=-1
                call u_write_msg(3,
     .          'Divide by zero error - mod_lunar')
	        mod_lunar=0.d0
                return
              endif
	      r30=r*a*pg30/pg
	      if (abs(r30-r30old).gt.abs(r30old)*0.001d0) then
	        r30old=r30
	        go to 5000
	      end if
	      r30old=r30
	      mod_lunar=r30
	   end if
	end if

	return
	end


c	==================================================================
        function mod_mixed(inc,ema,phase,dn,ret)
c	==================================================================
	implicit real*8 (a-z)
c_title mod_mixed - Mixed albedo/topo normalization without atmosphere.
c
c_args  type         variable  i/o      description
        real*8       ema       !i       emission angle
        real*8       inc       !i       incidence angle
        real*8       phase     !i       phase angle (deg)
        real*8       dn        !i	phase angle (deg)
	integer*4    ret       !o	returning error code
				!	(-1) divide by zero error
				!	(-2) pht_topder error

        real*8	     mod_mixed !internal:the function

c_desc  This mode will do albedo normalization over most of the planet
c	but near the terminator it will normalize topographic contrast to
c	avoid the "seams" we are currently getting with the plain albedo
c	normalization.  The two effects will be joined seamlessly.
c	In addition to the parameters for no-atmosphere albedo normaliza-
c	tion (i.e., the photometric parameters and the choice of angles
c	for normal albedo calculation) this mode needs two more parameters.
c	INCMATCH is the incidence angle at which the RMS contrast from al-
c	bedo matches the RMS contrast from topography.  (Could input a
c	full 3-angle geometry at which the contrasts are equal but since
c	the user is probably going to find this parameter by trial and er-
c	ror it's easier to specify only incidence and use emission=0,
c	phase=incidence for this second reference state.) ALBEDO, the av-
c	erage normal albedo, is also needed.
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       19 Mar 1999 K Teal Thompson  Add implicit none, declare vars.
c	05 May 1999 KTT  Change to implicit real*8 a-z.

c_end   ==================================================================

#include "pht_common.inc"
	logical*1 firstime
	data firstime /.true./
	if (firstime.eqv..true.) then
c	  First-time setup:
c	  Turn off Hapke opposition effect (must have access to PPARS_HAPKE)
c	  Calculate normalization at standard conditions
	  b0save=b0
	  b0=0.d0
	  psurfref=pht_func(incref,emaref,phaseref)
	  pprimeref=pht_topder(incref,emaref,phaseref,ret)
	  if (ret.lt.0) then
	    mod_mixed = 0.d0
	    ret=-2
	    return
	  endif
	  rhobar=albedo/psurfref
c	  thresh=100.d0
c	  Calculate brightness and topo derivative at matchpoint incidence
	  psurfmatch=pht_func(incmatch,0.d0,incmatch)
	  pprimematch=pht_topder(incmatch,0.d0,incmatch,ret)
	  if (ret.lt.0) then
	    mod_mixed = 0.d0
	    ret=-2
	    return
	  endif
c	  Calculate numerator of the stretch coeff. a; if it is very
c	  large or small we haven't chosen a good reference state
	  arg=psurfref**2.0d0
     *      +(psurfmatch*pprimeref/max(1.d-30,pprimematch))**2.0d0
	  if ((arg.lt.1.d-10).or.(arg.gt.1.d10))  ! totally arbitrary lims
     *      pause 'not a good reference state!'
	  anum=sqrt(arg)
	  b0=b0save

	  firstime=.false. 
	endif !end of first-time setup
 
c	code for scaling each pixel
	psurf=pht_func(inc,ema,phase)
	pprime=pht_topder(inc,ema,phase,ret)
	if (ret.lt.0) then
	  mod_mixed = 0.d0
	  ret=-2
	  return
	endif
	arg=psurf**2.0d0
     *    +(psurfmatch*pprime/max(1.d-30,pprimematch))**2.0d0
	aden=sqrt(max(1.d-30,arg))
c	thresh is a parameter limiting how much we amplify the dns
c	shouldn't actually get a large amplification in this mode because of
c	the growing pprime term in the denominator.
c	a=anum/aden
c	b=rhobar*(psurfref-a*psurf)
	if (aden.gt.anum*thresh) then
	   mod_mixed=0.d0
	else
	   mod_mixed = dn * anum/aden +
     .       rhobar*(psurfref-anum/aden*psurf)
	end if

	return
	end


c	==================================================================
        function mod_shade(inc,ema,phase,dn,ret)
c	==================================================================
        implicit none
c_title - mod_albedo - Albedo normalization in absence of atmosphere
c
c_args  type         variable  i/o      description
        real*8		ema	!i	emission angle
        real*8		inc	!i	incidence angle
        real*8		phase	!i	phase angle (deg)
        real*8		dn	!i	input brightness
	integer*4	ret	!o	returning error code
				!	There is no error condition for this routine

        real*8		mod_shade	!the function
c
c_desc  Outputs the surface photometric model at the angles 
c       specified on input.  Used to create a shaded relief
c       simulated image of the ellipsoidal reference surface or
c       of a digital elevation model, whichever is used to
c       calculate the photometric angle for the pixels.
c
c_hist  08 Apr 2003 Randy Kirk, USGS, Flagstaff - Original code
c       15 Jul 2003 RLK Added code to give specified normal albedo
c_end   ==================================================================
#include "pht_common.inc"

        real*8 pht_func
        real*8 b0save
        real*8 emaref
        real*8 phaseref
        real*8 rho
        logical*1 firstime
        data firstime /.true./

        ret=0

	if (firstime.eqv..true.) then
c	  First-time setup:
c	  Turn off Hapke opposition effect
	  b0save=b0
	  b0=0.d0
	  emaref=0.d0
	  phaseref=incref
	  rho=albedo/pht_func(incref,emaref,phaseref)
	  b0=b0save
	  firstime=.false. 
c	  end of first-time setup
	endif


c	code for outputting surface photometric model
	mod_shade =rho*pht_func(inc,ema,phase)

	return
	end


c	==================================================================
        function mod_shdat(inc0,ema0,phase,inc,ema,dn,ret)
c	==================================================================
        implicit none
c_title mod_shdat - Shaded relief simulation with atmosphere
c
c_args  type         variable  i/o      description
        real*8		ema0	!i	emission angle (ellipsoid)
        real*8		inc0	!i	incidence angle (ellipsoid)
        real*8		phase	!i	phase angle (deg)
	real*8          inc     !i      incidence angle (dem)
	real*8          ema     !i      emission angle (dem)
        real*8		dn	!i	input brightness
	integer*4	ret	!o	returning error code
				!	(-1) divide by zero error

        real*8		mod_shdat	!the function

c_desc  
c
c_hist  21 dec 1998 Randy Kirk, USGS, Flagstaff - Original code
c       17 Mar 1999 K Teal Thompson  Add implicit none, declare vars.
c       10 Jul 2003 RLK Modified from mod_albat
c       30 May 2006 RLK Added code to report the directional hemispheric
c                   albedo that relates to thermal balance for simulating
c                   Themis images

c_end   ==================================================================
#include "pht_common.inc"
c	Must access common blocks PPARS_HAPKE, AH_TABLE, APARS
	logical*1 null_negs_flag
	common /null_negs_common/ null_negs_flag
	real*8 b0save
	real*8 emaref
	real*8 phaseref
	real*8 psurfref
	real*8 psurf
	real*8 ahInterp
	real*8 munot
	real*8 deg2rad
	real*8 pstd
	real*8 trans
	real*8 trans0
	real*8 sbar
	real*8 rho



        real*8 pht_func

	logical*1 firstime
        parameter     (deg2rad=0.01745329251994329576913d0)
	data firstime /.true./
c	data hnorm /0.003d0/,tau/0.12/,wha/0.8d0/,bha/1.d0/


	if (firstime.eqv..true.) then
c	  First-time setup:
c	  Calculate normalization at standard conditions
c	  Turn off Hapke opposition effect
	  b0save=b0
	  b0=0.d0
	  emaref=0.d0
	  phaseref=incref
	  psurfref=pht_func(incref,emaref,phaseref)

C	  Solve for relative albedo factor rho by requiring the I/F
C	  at reference conditions to be equal to user-specified albedo.
C	  This I/F is the normal albedo, if incref=0.0 as appropriate
C	  for this mode
	  rho=albedo/psurfref
          phtrho = rho

	  b0=b0save
c	  get reference hemispheric albedo (b0 doesn't influence it much)
	  call pht_get_ah_table()!AB eval here by integration.

	  firstime=.false. 
c	  end of first-time setup

	endif
 
c	code for scaling each pixel from dn to dnout
	psurf=pht_func(inc,ema,phase)

	call r8splint(incTable,ahTable,ahTable2,ninc,inc0,ahInterp)
	munot=cos(deg2rad*inc0)
	call atm_func(inc0,ema0,phase,pstd,trans,trans0,sbar)


c	Use rho and actual geometry to calculate output dnout
c	This is not a function of dn because we are simulating
c	an image by shaded relief, not normalizing an image.
	  mod_shdat=pstd+rho*(ahInterp*munot*trans/
     .    (1.d0-rho*AB*sbar)
     *	  +(psurf-ahInterp*munot)*trans0)


	return
	end


C	**********************************************************************
C	ATMOSPHERE FUNCTIONS
C	These are Teal's atmosphere functions.  ATM_FUNC	is modified to 
C	include the two new modes, which are also provided.  (The old modes
C	must also be provided since I took out the declaration of common /apars/
C	and put it into the include file.  Function g11prime did not change.)

c       ==================================================================
        subroutine atm_func(inc,ema,phase,pstd,trans,trans0,sbar)
c       ==================================================================
        implicit none
c_title atm_func - call the user requested atmosphere subroutine.
c
c_args  type            variable     i/o     description

	real*8		inc		!I	incidence angle
	real*8		ema		!I	emission angle
	real*8		phase		!I	phase angle
	real*8		pstd		!o	pure atmospheric-scattering term
	real*8		trans		!o	transmission of surface
					!	reflected light through the
					!	atmosphere overall
	real*8		trans0		!o	transmission of surface
					!	reflected light through the
					!	atmosphere with no scatterings
					!	in the atmosphere
	real*8		sbar		!o	illumination of the ground by
					!	the sky

c_desc  Convert user's input atmosphere model string to a subroutine.
c	Called in mod_general.F.
c
c_hist  19 Mar 99  K Teal Thompson  Original code;
c       07 Jul 00  R. L. Kirk  Added Hapke/Henyey-Greenstein atmos models

c_end   ==================================================================
C	     INCLUDE 'PHO_COMMON.INC'
#include "pht_common.inc"


        if (atmosmodel.eq.'I1')
     .    call atm_iso1(inc,ema,phase,pstd,trans,trans0,sbar)
        if (atmosmodel.eq.'I2')
     .    call atm_iso2(inc,ema,phase,pstd,trans,trans0,sbar)
        if (atmosmodel.eq.'A1')
     .    call atm_an11(inc,ema,phase,pstd,trans,trans0,sbar)
        if (atmosmodel.eq.'A2')
     .    call atm_an12(inc,ema,phase,pstd,trans,trans0,sbar)
        if (atmosmodel.eq.'H1')
     .    call atm_hhg1(inc,ema,phase,pstd,trans,trans0,sbar)
        if (atmosmodel.eq.'H2')
     .    call atm_hhg2(inc,ema,phase,pstd,trans,trans0,sbar)

        return
        end




c       ==========================================================
        subroutine r8splint(xa,ya,y2a,n,x,y)
c       ==========================================================
	implicit none
c_title r8splint - real*8 version of splint in Numerical Recipes
c		   Book
c
c_args  type         variable  i/o      description
        integer*4	n	!i
	real*8		xa(n)	!i
	real*8		ya(n)	!i
	real*8		y2a(n)	!i
        real*8		x	!i
	real*8		y	!o
c
c_desc  It is important to understand that the program spline is
c	called only once to process an entire tabulated function
c	in arrays xi and yi . Once this has been done, values of
c	the interpolated function for any value of x are obtained
c	by calls (as many as desired) to a separate routine splint
c	(for "spline interpolation"):
c	      ---    ---
c	Given the arrays xa(1:n) and ya(1:n) of length n, which
c	tabulate a function (with the xai 's in order), and given
c	the array y2a(1:n), which is the output from spline above,
c	and given a value of x, this routine returns a cubic-spline
c	interpolated value y.
c
c_hist  11 Aug 99  K Teal Thompson copy from splint(x) in Numerical
c                  Recipes Book.
c       ================================================================


        integer*4 k,khi,klo
        real*8 a,b,h

        klo=1
c	We will find the right place in the table by means of bisec-
c	tion.  This is optimal if sequential calls to this routine
c	are at random values of x. If sequential calls are in order,
c	and closely spaced, one would do better to store previous
c	values of klo and khi and test if they remain appropriate on
c	the next call.

        khi=n
 1      if (khi-klo.gt.1) then
          k=(khi+klo)/2
          if(xa(k).gt.x)then
            khi=k
          else
            klo=k
          endif
        goto 1
        endif
c	klo and khi now bracket the input value of x.

        h=xa(khi)-xa(klo)
        if (h.eq.0.0d0) pause 'bad xa input in r8splint'
        a=(xa(khi)-x)/h !Cubic spline polynomial is now evaluated.
        b=(x-xa(klo))/h
        y=a*ya(klo)+b*ya(khi)+((a**3.0d0-a)*y2a(klo)+(b**3.0d0-b)*
     .    y2a(khi))*(h**2.0d0)/6.d0
        return
        end
c  (c) copr. 1986-92 Numerical Recipes Book
