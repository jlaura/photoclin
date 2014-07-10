c       ================================================================
	function r8ei(x) 
c       ================================================================
	implicit none
c_title r8ei - real*8 version of ei(x) in Numerical Recipes Book
c
c_args  type         variable  i/o      description
	real*8		x	!i	value for which ei is calculated
	real*8 		r8ei	!o	exponential integral
c
c_desc  Computes the exponential integral r8ei(x) for x > 0.
c
c_hist  11 Aug 99  K Teal Thompson copy from ei(x) in Numerical
c		   Recipes Book.
c
c_end   ================================================================

	integer*4 maxit	!the maximum number of iterations allowed
	parameter	(maxit=100)
	real*8 eps	!the relative error, or absolute error near the
			!zero of Ei at x = 0:3725
	parameter	(eps=6.d-8)
	real*8 euler	!Euler's constant
	parameter	(euler=.57721566d0)
	real*8 fpmin	!number near the smallest representable
			!oating-point number
	parameter	(fpmin=1.d-30)
	integer*4 k
	real*8 fact,prev,sum,term
c       ================================================================
	if(x.le.0.) pause 'bad argument in r8ei'
	if(x.lt.fpmin)then	!Special case: avoid failure of convergence test
				!because of underflow.
	  r8ei=log(x)+euler
	else if(x.le.-log(eps))then
				!Use power series.
	  sum=0.
	  fact=1.
	  do 11 k=1,maxit
	    fact=fact*x/k
	    term=fact/k
	    sum=sum+term
	    if(term.lt.eps*sum)goto 1
11	  continue
	  pause 'series failed in r8ei'
1	  r8ei=sum+log(x)+euler
	else			!Use asymptotic series.
	  sum=0.		!Start with second term.
	  term=1.
	  do 12 k=1,maxit
	    prev=term
	    term=term*k/x
	    if(term.lt.eps)goto 2
				!Since final sum is greater than one, term itself
				!approximates the relative error.
	    if(term.lt.prev)then
	      sum=sum+term	!Still converging: add new term.
	    else
	      sum=sum-prev	!Diverging: subtract previous term and exit.
	      goto 2
	    endif
12	  continue
2	  r8ei=exp(x)*(1.+sum)/x
	endif
	return
	end
c  (C) Copr. 1986-92 Numerical Recipes Book
