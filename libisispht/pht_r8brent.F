      function r8brent(ax,bx,cx,f,tol,xmin)
C Brent's method 1-D minimization routine.
C Double precision version of BRENT.  Numerical Recipes, p. 284.
      implicit real*8 (a-h,o-z)
      parameter (itmax=100,cgold=0.3819660d0,zeps=1.0d-15)
      a=min(ax,cx)
      b=max(ax,cx)
      v=bx
      w=v
      x=v
      e=0.d0
      fx=f(x)
      fv=fx
      fw=fx
      do iter=1,itmax
         xm=0.5d0*(a+b)
         tol1=tol*abs(x)+zeps
         tol2=2.d0*tol1
         if (abs(x-xm).le.tol2-0.5d0*(b-a)) goto 3
         if (abs(e).gt.tol1) then
            r=(x-w)*(fx-fv)
            q=(x-v)*(fx-fw)
            p=(x-v)*q-(x-w)*r
            q=2.d0*(q-r)
            if (q.gt.0.d0) p=-p
            q=abs(q)
            etemp=e
            e=d
            if ((abs(p).ge.abs(0.5d0*q*etemp)).or.(p.le.q*(a-x))
     *          .or.(p.ge.q*(b-x))) goto 1
            d=p/q
            u=x+d
            if ((u-a.lt.tol2).or.(b-u.lt.tol2)) d=sign(tol1,xm-x)
            goto 2
         endif
   1     if (x.ge.xm) then
            e=a-x
         else
            e=b-x
         endif
         d=cgold*e
   2     if (abs(d).ge.tol1) then
            u=x+d
         else
            u=x+sign(tol1,d)
         endif
         fu=f(u)
         if (fu.le.fx) then
            if (u.ge.x) then
               a=x
            else
               b=x
            endif
            v=w
            fv=fw
            w=x
            fw=fx
            x=u
            fx=fu
         else
            if (u.lt.x) then
               a=u
            else
               b=u
            endif
            if ((fu.le.fw).or.(w.eq.x)) then
               v=w
               fv=fw
               w=u
               fw=fu
            else if ((fu.le.fv).or.(v.eq.x).or.(v.eq.w)) then
               v=u
               fv=fu
            endif
         endif
      enddo
      pause 'r8brent exceeded maximum iterations.'
   3  xmin=x
      r8brent=fx
      return
      end
