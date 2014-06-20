c	==================================================================
	function lfit_pht_global(par)
c	==================================================================
	implicit none
c_title 
c
c_args  type    variable  i/o      description
	real*8 par			! input:  photometric parameter
	real*8 lfit_pht_global		! output: RMS error of fit
 
c
c_desc  Fits a simple photometric model to the Hapke model by linear least
c	squares fit at a contant given value of the limb-darkening parameter
c	PAR.
c	The RMS error of the fit is returned as the function value; the
c	coefficients of the fit C0 (additive) and C1 (multiplicative) are
c	returned in the common block used to pass in all the stuff that needs
c	to be hidden from R8MNBRAK/R8BRENT.
c
c_hist  23 Nov 99  Randy L Kirk - USGS, Flagstaff; Original Code.
c	 23 Dec 99  K Teal Thompson - Port to Unix; add implicit none.

c_end   ==================================================================
	integer*4 iret
	real*8 arg
	
	integer*4 nlp,nl,ns
	parameter (nlp=51)
	real*8 hapke_img(nlp*2-1,nlp)
c	==================================================================
c	parameters controlling the range of angles over which fit is made
c	==================================================================
	real*8 incmin,incmax	! incidence angle range
	real*8 emamin,emamax	! emission angle range
	real*8 phase		! current phase angle
c	==================================================================
c	parameter controlling which empirical model is fit
c	==================================================================
	integer*4 empflag	! 0=lunar-lambert, 1=minnaert
c	==================================================================
c	parameter controlling whether additive term is used in fit
c	==================================================================
	integer*4 addOffFlag	! 0=no additive, 1=additive term
	real*8 c0,c1		! constant and linear coefficients
				! of linear fit of empirical fn to
				! Hapke @ fixed PAR
	integer*4 nofit		! flag for no fit points found
	common /lfit_pht_global_com/  hapke_img,incmin,incmax,
     *    emamin,emamax,phase,empflag,addOffFlag,c0,c1,nofit,nl,ns

#include "pht_common.inc"
c	include 'pht_common.inc'

	real*8 sum1		! accumulation buffers for least-squares
	real*8 sumx		! fit information: sums of products of
	real*8 sumy		! abcissa and ordinate...
	real*8 sumxx
	real*8 sumxy
	real*8 sumyy
	integer*4 i,j		! loop counters
	real*8 inc,ema		! incidence and emission angles
	real*8 pht_lunlam	! simple photometric function
	real*8 pht_minnaert	! simple photometric function
	real*8 x,y		! here, abcissa and ordinate of fit, i.e.,
				! Hapke model value, simple model value
	real*8 de		! denominator for least squares calcs

	sum1=0.0d0
	sumx=0.0d0
	sumy=0.0d0
	sumxx=0.0d0
	sumxy=0.0d0
	sumyy=0.0d0
	if (empflag.eq.0) then
	   ll=par
	else
	   kay=par
	end if
	bee=1.0d0
	nofit=0

c	==================================================================
c	accumulate statistics
c	==================================================================
	do j=1,nl
	   do i=1,ns
	      call pht_get_angles(nl,ns,j,i,inc,ema,phase,iret)
	      if ((iret.eq.0).and.
     *	         (inc.ge.incmin).and.
     *	         (inc.le.incmax).and.
     *	         (ema.ge.emamin).and.
     *	         (ema.le.emamax)) then
	         if (empflag.eq.0) then
	            x=pht_lunlam(inc,ema,phase)
	         else
	            x=pht_minnaert(inc,ema,phase)
	         end if
	         y=hapke_img(i,j)
	         sum1=sum1+1.0d0
	         sumx=sumx+x
	         sumy=sumy+y
	         sumxx=sumxx+x*x
	         sumxy=sumxy+x*y
	         sumyy=sumyy+y*y
	      end if
	   enddo
	enddo

c	==================================================================
c	check that some points were found so the fit can be done
c	==================================================================
	if ((sum1.lt.1.0d0).or.(sumxx.le.0.0d0).or.
     *  ((addOffFlag.ne.0).and.(sum1*sumxx-sumx*sumx.eq.0.0d0))) then
	   nofit=-1
	   lfit_pht_global = -1.d0
	   return
	end if

c	==================================================================
c	evaluate coefficients and rms error of the linear least-squares fit
c	==================================================================
	if (addOffFlag.eq.0) then
	   c0=0.0d0
	   c1=sumxy/sumxx
	   arg=(sumyy-2.0d0*c1*sumxy+c1*c1*sumxx)/sum1
	   if (arg.gt.0.0d0) then
	      lfit_pht_global=sqrt(arg)
	   else
	      lfit_pht_global=0.0d0
	   end if
	else
	   de=sum1*sumxx-sumx*sumx
	   c0=(sumxx*sumy-sumx*sumxy)/de
	   c1=(sum1*sumxy-sumx*sumy)/de
	   arg=(sumyy+2.0d0*(c0*c1*sumx-c0*sumy-c1*sumxy)
     *        +c0*c0*sum1+c1*c1*sumxx)/sum1
	   if (arg.gt.0.0d0) then
	      lfit_pht_global=sqrt(arg)
	   else
	      lfit_pht_global=0.0d0
	   end if
	end if
	return
	end
