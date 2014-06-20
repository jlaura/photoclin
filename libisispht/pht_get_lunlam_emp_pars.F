	subroutine pht_get_lunlam_emp_pars (datafile,ret)
	implicit none
c******************************************************************************
c_TITLE	pht_get_lunlam_emp_pars - Get the Empirical Lunar (Lommel-Seeliger)-
c	Lambert coefficients.
c
c_ARGS	TYPE		VARIABLE	I/O	DESCRIPTION
c
	character*(*)	datafile	!I	Name of the data file 
c						containing Lunar-Lambert
c						parameters
	integer*4	ret		!O	return code
c						 0 = ok
c						-1 = Can't obtain LUN
c						-2 = Can't open file
c						-3 = Can't find starting point
c						-4 = Can't find numllpoints
c						-5 = Can't find llphase
c						-6 = Can't find llval
c						-7 = Can't find numbeepoints
c						-8 = Can't find bphase
c						-9 = Can't find bval
c
c_DESC	This subroutine opens and reads the "datafile" in order to
c	obtain the pht_lunlam_emp data points for a spline function
c	fitting the points.
c
c_HIST	20 Nov 1998 Jeff Anderson, USGS Flagstaff, Original Version 
c	10 Mar 1999 K Teal Thompson  convert to format that passes the
c		    pht_lunlam_emp coefficients by common.  Change vars
c		    to reflect most recent Randy Kirk implementation of
c		    pht_lunlam_emp.
c       18 Mar 1999 KTT Move photometric vars to pho_common.inc.
c       15 Jul 1999 KTT Duplicated line increment for remaining reads.
c       29 Jul 1999 KTT Copy get_lunlam_emp_pars to get_lunlam_pol_
c		    pars and rewrite get_lunlam_emp to read data points
c		    and calculate a spline rather than just reading poly-
c		    nomial coefficients.
c	16 Mar 2001 Add error messages to help out the users and convert
c		    a large chunk of repeating code to subroutine pht_
c		    get_line_vals.F.  Change name of subroutine from
c		    get_lunlam_emp to pht_get_lunlam_emp.
c       15 Jul 2003 Janet Barrett, Added code to check for embedded
c                   comment lines.
c
c_END
c******************************************************************************
#include "pht_common.inc"

C Local variables

	integer*4	lun
	integer*4	status
	character*256	line
	integer*4	pos
	logical*4	found
	real*8		yp1,yp2
	character*256	x
	character*1	quote

	data quote /"'"/

C
C Get an unused logical unit number and then try to open the data file
C
	call u_allocate_unit (lun,status)
	if (status .ne. 0) then
	  ret = -1
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]   ',i3,
     .           ':  Can',a,'t obtain LUN')"
     .      )ret,quote
	  call u_write_msg(3,x)
	  return 
	end if

	open (unit=lun,file=datafile,status='old',iostat=status)
	if (status .ne. 0) then
	  ret = -2
	  write(x,
     .	    "(' [pht_get_lunlam_emp_pars]   ',i3,
     .	    ':  Can',a,'t open datafile:  ',a128)"
     .	    )ret,quote,datafile
	  call u_write_msg(3,x)
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
	    write(x,
     .        "(' [pht_get_lunlam_emp_pars]  ',i3,
     .        ':  Can',a,'t find starting point for LUNAR_LAMBERT_EMP',
     .        'in datafile:',a128)"
     .        )ret,quote,datafile
	    call u_write_msg(3,x)
	    return
	  end if

	  if (line(1:1) .ne. '#') then
	    if (index(line,'LUNAR_LAMBERT_EMP') .ne. 0) found = .true. 
	  end if
	end do

C
C The next line in the file should contain the numllpoints parameter.  
C Read it as a string, locate the equal sign, take the data after the 
C equal sign, and convert it to a floating point.
C
 10	read (lun,'(a256)',iostat=status) line
	if (status .ne. 0) then
	  ret = -4
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .           ':  Can',a,'t read numllpoints line.')"
     .      )ret,quote
	  call u_write_msg(3,x)
	  return
	end if
	if (line(1:1).eq.'#') goto 10

	pos = index(line,'=')
	if (pos .eq. 0) then
	  ret = -4
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t find equal sign on line for numllpoints')"
     .      )ret,quote
	  call u_write_msg(3,x)
	  return
	end if
	line = line(pos+1:)

	read (line,'(i2)',iostat=status) numllpoints
	if (status .ne. 0) then
	  ret = -4
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t find numllpoints value')"
     .      )ret,quote
	  call u_write_msg(3,x)
	  return
	end if

C
C The next line in the file should contain the llphase parameter.
C This parameter is an array, and all the values have to be read.
C Read it as a string, locate the equal sign, take the data after the 
C equal sign, and convert it to a floating point.
C
 20	read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -5
          write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t find llphase line')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if
	if (line(1:1).eq.'#') goto 20

	call pht_get_line_vals (line,numllpoints,llphase,ret)
        if (ret .ne. 0) then
          ret = -5
          write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t load llphase points  ')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if

C The next line in the file should contain the llval parameter.
C This parameter is an array, and all the values have to be read.
C Read it as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 30     read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -6
          write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t find lval line')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if
	if (line(1:1).eq.'#') goto 30

	call pht_get_line_vals (line,numllpoints,llval,ret)
        if (ret .ne. 0) then
          ret = -6
          write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t load lval points  ')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if

	yp1 = 1.0d30
	yp2 = 1.0d30
	call r8spline(llphase,llval,numllpoints,yp1,yp2,ll2)

C The next line in the file should contain the numbeepoints parameter.
C Read it as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 40     read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -7
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t find numbeepoints line')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if
	if (line(1:1).eq.'#') goto 40
 
        pos = index(line,'=')
        if (pos .eq. 0) then
          ret = -7
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .           ':  Can',a,'t find equal sign on numbeepoints line')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if
        line = line(pos+1:)
 
        read (line,'(i2)',iostat=status) numbeepoints
        if (status .ne. 0) then
          ret = -7
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t find numbeepoints values')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if
 
C
C The next line in the file should contain the bphase parameter.
C This parameter is an array, and all the values have to be read.
C Read it as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 50     read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -8
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t find bphase line')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if
	if (line(1:1).eq.'#') goto 50

        call pht_get_line_vals (line,numbeepoints,bphase,ret)
        if (ret .ne. 0) then
          ret = -8
          write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .         ':  Can',a,'t load bphase points  ')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if

C The next line in the file should contain the bval parameter.
C This parameter is an array, and all the values have to be read. 
C Read it as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 60     read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -9
	  write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t find bval line')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if
	if (line(1:1).eq.'#') goto 60

        call pht_get_line_vals (line,numbeepoints,bval,ret)
        if (ret .ne. 0) then
          ret = -9
          write(x,
     .      "(' [pht_get_lunlam_emp_pars]  ',i3,
     .      ':  Can',a,'t load bval points  ')"
     .      )ret,quote
	  call u_write_msg(3,x)
          return
        end if

        call r8spline(bphase,bval,numbeepoints,yp1,yp2,bee2)

c
c Every thing worked so return with success
c
        close(lun)
	ret = 0	
	return

	end
