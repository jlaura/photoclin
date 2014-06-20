      function r8zbrent(func,x1,x2,tol)
c double precision version of numerical recipes ZBRENT routine
      implicit real*8 (a-h,o-z)
      implicit integer*4 (i-n)
      parameter (itmax=100,eps=3.d-12)
      a=x1
      b=x2
      fa=func(a)
      fb=func(b)
      if(fb*fa.gt.0.d0) pause 'root must be bracketed for r8zbrent.'
      fc=fb
      do 11 iter=1,itmax
        if(fb*fc.gt.0.d0) then
          c=a
          fc=fa
          d=b-a
          e=d
        endif
        if(abs(fc).lt.abs(fb)) then
          a=b
          b=c
          c=a
          fa=fb
          fb=fc
          fc=fa
        endif
        tol1=2.d0*eps*abs(b)+0.5d0*tol
        xm=0.5d0*(c-b)
        if(abs(xm).le.tol1 .or. fb.eq.0.d0)then
          r8zbrent=b
          return
        endif
        if(abs(e).ge.tol1 .and. abs(fa).gt.abs(fb)) then
          s=fb/fa
          if(a.eq.c) then
            p=2.d0*xm*s
            q=1.d0-s
          else
            q=fa/fc
            r=fb/fc
            p=s*(2.d0*xm*q*(q-r)-(b-a)*(r-1.d0))
            q=(q-1.d0)*(r-1.d0)*(s-1.d0)
          endif
          if(p.gt.0.d0) q=-q
          p=abs(p)
          if(2.d0*p .lt. min(3.d0*xm*q-abs(tol1*q),abs(e*q))) then
            e=d
            d=p/q
          else
            d=xm
            e=d
          endif
        else
          d=xm
          e=d
        endif
        a=b
        fa=fb
        if(abs(d) .gt. tol1) then
          b=b+d
        else
          b=b+sign(tol1,xm)
        endif
        fb=func(b)
11    continue
      pause 'r8zbrent exceeding maximum iterations.'
      r8zbrent=b
      return
      end
