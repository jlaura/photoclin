	subroutine pht_get_hapke_pars (datafile,ret)
	implicit none

C******************************************************************************
C
C_TITLE	pht_get_hapke_pars - Get the parameters for the Hapke - Henyey -
C       Greenstein or Legendre Polynomial photometric correction.
C
C_ARGS	TYPE		VARIABLE	I/O	DESCRIPTION
C
C_PARM	character*(*)	datafile	I	Name of the data file 
C						containing Hapke - Henyey -
C                                               Greenstein or Legendre par-
C						ameters
C_PARM	integer*4	ret		O	return code
C						 0 = ok
C						-1 = Can't obtain LUN
C						-2 = Can't open file
C						-3 = Can't find starting point
C						-4 = Can't find wh
C						-5 = Can't find hh
C						-6 = Can't find b0
C						-7 = Can't find theta
C						-8 = Can't find iopt
C						-9 = Can't find hg1
C						-10= Can't find hg2
C						-11= Can't find bh
C						-12= Can't find ch
C
C_DESC	This subroutine opens and reads the "datafile" in
C       order to get wh,hh,b0,theta,hg1, hg2, bh, and ch
c	parameters.  Only hg1 and hg2 for iopt = o and le-
c	gendre1 and ch for iopt = 1.
C
C_HIST	03 Dec 98 K Teal Thompson, USGS Flagstaff, Original Version 
C                 using J Anderson's get_lunar_lambert_pars.F as template
c	10 Mar 99 KTT Change format to pass parameters to common.
c	18 Mar 99 KTT Move photometric vars to pht_common.inc.
C
C_END
C******************************************************************************
#include "pht_common.inc"

C
C Subroutine parameters
C

	character*(*)	datafile	!PARM
	integer*4	ret		!PARM

C
C Local variables
C

	integer*4	lun
	integer*4	status
	character*256	line
	integer*4	pos
	logical*4	found
	character*23	option_string
C	integer*	iopt			!in common
C						0 = Greenstein
C						1 = Legendre

C Set up option string

	if (iopt.eq.0)option_string='HAPKE_HENYEY_GREENSTEIN'
	if (iopt.eq.1)option_string='HAPKE_LEGENDRE'
C
C Get an unused logical unit number and then try to open the data file
C

	call u_allocate_unit (lun,status)
	if (status .ne. 0) then
	  ret = -1
	  return 
	end if

	open (unit=lun,file=datafile,status='old',iostat=status)
	if (status .ne. 0) then
	  ret = -2
	  return
	end if

C
C Search through the file for the Hapke - Henyey - Greenstein parameters
C and report an error if they can not be found.  Skip over comment lines
C (those starting with #).
C

	found = .false.
	do while (.not. found)
	  read (lun,'(a256)',iostat=status) line
	  if (status .ne. 0) then
	    ret = -3
	    return
	  end if

	  if (line(1:1) .ne. '#') then
	    if (index(line,option_string) .ne. 0)
     .      found = .true. 
	  end if
	end do

C
C The next line in the file should contain the single scattering albedo
C Read it as a string, locate the equal sign, take the data after the 
C equal sign, and convert it to a floating point.
C

	read (lun,'(a256)',iostat=status) line
	if (status .ne. 0) then
	  ret = -4
	  return
	end if

	pos = index(line,'=')
	if (pos .eq. 0) then
	  ret = -4
	  return
	end if
	line = line(pos+1:)

	read (line,'(f15.8)',iostat=status) wh
	if (status .ne. 0) then
	  ret = -4
	  return
	end if

C
C The next line in the file should contain the optical depth.
C Read it as a string, locate the equal sign, take the data
C after the equal sign, and convert it to a floating point.
C

	read (lun,'(a256)',iostat=status) line
	if (status .ne. 0) then
	  ret = -5
	  return
	end if

	pos = index(line,'=')
	if (pos .eq. 0) then
	  ret = -5
	  return
	end if
	line = line(pos+1:)

	read (line,'(f15.8)',iostat=status) hh 
	if (status .ne. 0) then
	  ret = -5
	  return
	end if

C
C The next line in the file should contain the b0, ratio of
C thickness of spherical shell to radius of planet.  Read it
C as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 
        read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -6
          return
        end if
 
        pos = index(line,'=')
        if (pos .eq. 0) then
          ret = -6
          return
        end if
        line = line(pos+1:)
 
        read (line,'(f15.8)',iostat=status) b0
        if (status .ne. 0) then
          ret = -6
          return
        end if
 
 
C The next line in the file should contain the theta, average
C topographic slope angle of surface roughness at subresolution
C scales.  Read it as a string, locate the equal sign, take the
C data after the equal sign, and convert it to a floating point.

 
        read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -7
          return
        end if
 
        pos = index(line,'=')
        if (pos .eq. 0) then
          ret = -7
          return
        end if
        line = line(pos+1:)
 
        read (line,'(f15.8)',iostat=status) theta
        if (status .ne. 0) then
          ret = -7
          return
        end if
 
 
 
c ## ------ iopt read deleted.  Passed in common ------ ##

c ----------------------------------------------------------------------
c Get Hapke-Henyey-Greenstein asymmetry factors only if iopt=0
c ----------------------------------------------------------------------
	if (iopt.eq.0) then
C The next line in the file should contain the hg1, asymmetry
C factor of the Henyey-Greenstein particle phase function.
C Read it as a string, locate the equal sign, take the data
C after the equal sign, and convert it to a floating point.

 
          read (lun,'(a256)',iostat=status) line
          if (status .ne. 0) then
            ret = -9
            return
          end if
 
          pos = index(line,'=')
          if (pos .eq. 0) then
            ret = -9
            return
          end if
          line = line(pos+1:)
 
          read (line,'(f15.8)',iostat=status) hg1
          if (status .ne. 0) then
            ret = -9
            return
          end if
 
 
C The next line in the file should contain the hg2, second asymmetry
C factor for 2-lobed Henyey-Greenstein particle phase function.
C Read it as a string, locate the equal sign, take the data after
C the equal sign, and convert it to a floating point.

 
          read (lun,'(a256)',iostat=status) line
          if (status .ne. 0) then
            ret = -10
            return
          end if
 
          pos = index(line,'=')
          if (pos .eq. 0) then
            ret = -10
            return
          end if
          line = line(pos+1:)
 
          read (line,'(f15.8)',iostat=status) hg2
          if (status .ne. 0) then
            ret = -10
            return
          end if
 
c ----------------------------------------------------------------------
c Get Hapke-Legendre Polynomials only if iopt=1
c ----------------------------------------------------------------------
        else !iopt.eq.1
 
C The next line in the file should contain the bh
C polynomial.
 
          read (lun,'(a256)',iostat=status) line
          if (status .ne. 0) then
            ret = -11
            return
          end if
 
          pos = index(line,'=')
          if (pos .eq. 0) then
            ret = -11
            return
          end if
          line = line(pos+1:)
 
          read (line,'(f15.8)',iostat=status) bh
          if (status .ne. 0) then
            ret = -11
            return
          end if
 
 
C The next line in the file should contain the ch
C polynomial.
 
 
          read (lun,'(a256)',iostat=status) line
          if (status .ne. 0) then
            ret = -12
            return
          end if
 
          pos = index(line,'=')
          if (pos .eq. 0) then
            ret = -12
            return
          end if
          line = line(pos+1:)
 
          read (line,'(f15.8)',iostat=status) ch
          if (status .ne. 0) then
            ret = -12
            return
          end if
 
	endif !iopt.eq.0.else.iopt.eq.1

 
c
c
c Every thing worked so return with success
c

        close(lun)
	ret = 0	
	return

	end
