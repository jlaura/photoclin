      subroutine r8mnbrak(ax,bx,cx,fa,fb,fc,func)
C Solution bracketing for 1-D minimization routine.
C Double precision version of MNBRAK.  Numerical Recipes, p. 281.
      implicit real*8 (a-h,o-z)
      parameter (gold=1.618034d0, glimit=100.d0, tiny=1.0d-20)
      fa=func(ax)
      fb=func(bx)
      if(fb.gt.fa)then
        dum=ax
        ax=bx
        bx=dum
        dum=fb
        fb=fa
        fa=dum
      endif
      cx=bx+gold*(bx-ax)
      fc=func(cx)
1     if(fb.ge.fc)then
        r=(bx-ax)*(fb-fc)
        q=(bx-cx)*(fb-fa)
        u=bx-((bx-cx)*q-(bx-ax)*r)/(2.0d0*sign(max(abs(q-r),tiny),q-r))
        ulim=bx+glimit*(cx-bx)
        if((bx-u)*(u-cx).gt.0.0d0)then
          fu=func(u)
          if(fu.lt.fc)then
            ax=bx
            fa=fb
            bx=u
            fb=fu
            go to 1
          else if(fu.gt.fb)then
            cx=u
            fc=fu
            go to 1
          endif
          u=cx+gold*(cx-bx)
          fu=func(u)
        else if((cx-u)*(u-ulim).gt.0.0d0)then
          fu=func(u)
          if(fu.lt.fc)then
            bx=cx
            cx=u
            u=cx+gold*(cx-bx)
            fb=fc
            fc=fu
            fu=func(u)
          endif
        else if((u-ulim)*(ulim-cx).ge.0.0d0)then
          u=ulim
          fu=func(u)
        else
          u=cx+gold*(cx-bx)
          fu=func(u)
        endif
        ax=bx
        bx=cx
        cx=u
        fa=fb
        fb=fc
        fc=fu
        go to 1
      endif
      return
      end
