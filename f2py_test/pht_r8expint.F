c       ================================================================
	function r8expint(n,x)
c       ================================================================
	implicit none
c_title r8expint - real*8 version of expint(x) by Numerical Recipes Book
c
c_args  type         variable  i/o      description
	integer*4	n
	real*8		x
	real*8		r8expint
c
c_desc  Evaluates the exponential integral En(x).
c	The routine expint allows fast evaluation of En(x) to any
c	accuracy EPS within the reach of your machine's word
c	length for floating-point numbers. The only modification
c	required for increased accuracy is to supply Euler's
c	constant with enough significant digits.
c
c_hist  10 Aug 99  K Teal Thompson copy from ei(x) by Numerical Recipes
c                  Book.
c
c       ================================================================
	integer*4 MAXIT !the maximum allowed number of iterations
	parameter	(maxit=100)
	real*8 EPS	!the desired relative error, not smaller
			!than the machine precision
	parameter	(eps=1.d-7)
	real*8 FPMIN	!number near the smallest representable
			!oating-point number
	parameter	(fpmin=1.d-30)
	real*8 EULER	!Euler's constant
	parameter       (euler=.5772156649d0)
	integer*4 i,ii,nm1
	real*8 a,b,c,d,del,fact,h,psi
c	================================================================
	nm1=n-1
	if(n.lt.0.or.x.lt.0..or.(x.eq.0..and.(n.eq.0.or.n.eq.1)))then
	  pause 'bad arguments in r8expint'
	else if(n.eq.0)then	!Special case
	  r8expint=exp(-x)/x
	else if(x.eq.0.)then	!Another special case
	  r8expint=1./nm1
	else if(x.gt.1.)then	!Lentz's algorithm ( x 5.2).
	  b=x+n
	  c=1./FPMIN
	  d=1./b
	  h=d
	  do 11 i=1,MAXIT
	    a=-i*(nm1+i)
	    b=b+2.
	    d=1./(a*d+b)	!Denominators cannot be zero.
	    c=b+a/c
	    del=c*d
	    h=h*del
	    if(abs(del-1.).lt.EPS)then
	      r8expint=h*exp(-x)
	      return
	    endif
11	  continue
	  pause 'continued fraction failed in r8expint'
	else			!Evaluate series.
	  if(nm1.ne.0)then	!Set first term.
	    r8expint=1./nm1
	  else
	    r8expint=-log(x)-EULER
	  endif
	  fact=1.
	  do 13 i=1,MAXIT
	    fact=-fact*x/i
	    if(i.ne.nm1)then
	      del=-fact/(i-nm1)
	    else
	      psi=-EULER	!Compute (n).
	      do 12 ii=1,nm1
	        psi=psi+1./ii
12	      continue
	      del=fact*(-log(x)+psi)
	    endif
	    r8expint=r8expint+del
	    if(abs(del).lt.abs(r8expint)*EPS) return
13	  continue
	  pause 'series failed in r8expint'
	r8expint=0.0
	endif
	return
	END
C  (C) Copr. 1986-92 Numerical Recipes Book
