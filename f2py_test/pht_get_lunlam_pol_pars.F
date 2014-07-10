	subroutine pht_get_lunlam_pol_pars (datafile,ret)
	implicit none
C******************************************************************************
C_TITLE	pht_get_lunlam_pol_pars - Get the polynomial Lunar
C	(Lommel-Seeliger)-Lambert coefficients.
C
C_ARGS	TYPE		VARIABLE	I/O	DESCRIPTION
C
	character*(*)	datafile	!I	Name of the data file 
C						containing Lunar-Lambert
C						parameters
	integer*4	ret		!O	return code
C						 0 = ok
C						-1 = Can't obtain LUN
C						-2 = Can't open file
C						-3 = Can't find starting point
C						-4 = Can't find lcof
C						-5 = Can't find numlcoef
C						-6 = Can't find bcof
C						-7 = Can't find numbcoef
C
C_DESC	This subroutine opens and reads the "datafile" in order to
C	obtain the pht_lunlam_emp coefficients and pass them to a
C	common.
C
C_HIST	20 Nov 98 Jeff Anderson, USGS Flagstaff, Original Version 
C	10 Mar 99 K Teal Thompson  convert to format that passes the
C	          pht_lunlam_emp coefficients by common.  Change vars
c                 to reflect most recent Randy Kirk implementation of
c                 pht_lunlam_emp.
c       18 Mar 99 KTT Move photometric vars to pht_common.inc.
c       15 Jul 99 KTT Duplicated line increment for remaining reads.
c       29 Jul 99 KTT Copy get_lunlam_emp_pars to get_lunlam_pol_
c                  pars and rewrite get_lunlam_emp.
c       15 Jul 03 Janet Barrett, Added code to check for embedded
c                 comment lines.
C_END
C******************************************************************************
#include "pht_common.inc"
 
C Local variables
 
	integer*4	lun
	integer*4	status
	character*256	line
	integer*4	pos
	logical*4	found
	integer*4	count

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
C Search through the file for the Lunar-Lambert parameters and
C report an error if they can not be found.  Skip over comment lines
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
	    if (index(line,'LUNAR_LAMBERT_POL') .ne. 0) found = .true. 
	  end if
	end do

C
C The next line in the file should contain the numlcoef parameter.  
C Read it as a string, locate the equal sign, take the data after the 
C equal sign, and convert it to a floating point.
C
 10	read (lun,'(a256)',iostat=status) line
	if (status .ne. 0) then
	  ret = -4
	  return
	end if
	if (line(1:1).eq.'#') goto 10

	pos = index(line,'=')
	if (pos .eq. 0) then
	  ret = -4
	  return
	end if
	line = line(pos+1:)

	read (line,'(i2)',iostat=status) numlcoef
	if (status .ne. 0) then
	  ret = -4
	  return
	end if

C
C The next line in the file should contain the lcof parameter.
C This parameter is an array, and all the values have to be read.
C Read it as a string, locate the equal sign, take the data after the 
C equal sign, and convert it to a floating point.
C
 20	read (lun,'(a256)',iostat=status) line
	if (status .ne. 0) then
	  ret = -5
	  return
	end if
	if (line(1:1).eq.'#') goto 20

	pos = index(line,'=')
	if (pos .eq. 0) then
	  ret = -5
	  return
	end if
	line = line(pos+1:)

	do count=1,numlcoef
	  read (line,'(f15.8)',iostat=status) lcof(count)
c	  set index at the first comma of whatever remains
c	  of line, then line = line from position to end.
c	  repeat until counting out numlcoef (number of l coeff).
          pos = index(line,',')
          line = line(pos+1:)
	  if (status .ne. 0) then
	    ret = -6
	    return
	  end if
	enddo

C
C The next line in the file should contain the numbcoef parameter.
C Read it as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 30     read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -4
          return
        end if
	if (line(1:1).eq.'#') goto 30
 
        pos = index(line,'=')
        if (pos .eq. 0) then
          ret = -4
          return
        end if
        line = line(pos+1:)
 
        read (line,'(i2)',iostat=status) numbcoef
        if (status .ne. 0) then
          ret = -4
          return
        end if
 
C
C The next line in the file should contain the bcof parameter.
C This parameter is an array, and all the values have to be read.
C Read it as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 40     read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -5
          return
        end if
	if (line(1:1).eq.'#') goto 40
 
        pos = index(line,'=')
        if (pos .eq. 0) then
          ret = -5
          return
        end if
        line = line(pos+1:)
 
        do count=1,numbcoef
          read (line,'(f15.8)',iostat=status) bcof(count)
c         set index at the first comma of whatever remains
c         of line, then line = line from position to end.
c         repeat until counting out numlcoef (number of l coeff).
          pos = index(line,',')
          line = line(pos+1:)
          if (status .ne. 0) then
            ret = -6
            return
          end if
        enddo

c
c Every thing worked so return with success
c
        close(lun)
	ret = 0	
	return

	end
