C     -*- fortran -*-
C     This file is autogenerated with f2py (version:2)
C     It contains Fortran 77 wrappers to fortran functions.

      subroutine f2pyinitCHECK(setupfunc)
      external setupfunc
      integer*4 NX1
      integer*4 NY1
      integer*4 ICHECK
      integer*4 JCHECK
      common /CHECK/ NX1,NY1,ICHECK,JCHECK
      call setupfunc(NX1,NY1,ICHECK,JCHECK)
      end


