	subroutine pht_get_minnaert_emp_pars (datafile,ret)
	implicit none
c******************************************************************************
c_TITLE	pht_get_minnaert_emp_pars - Get the Empirical Minnaert coefficients.
c
c_ARGS	TYPE		VARIABLE	I/O	DESCRIPTION
c
	character*(*)	datafile	!I	Name of the data file 
c						containing Minnaert
c						parameters
	integer*4	ret		!O	return code
c						 0 = ok
c						-1 = Can't obtain LUN
c						-2 = Can't open file
c						-3 = Can't find starting point
c						-4 = Can't find numkaypoints
c						-5 = Can't find kayphase 
c						-6 = Can't find kayval
c						-7 = Can't find numbeepoints
c                                               -8 = Can't find bphase
c                                               -9 = Can't find bval
c
c_DESC	This subroutine opens and reads the "datafile" in order to
c       obtain the pht_minnaert_emp data points for a spline function
c       fitting the points.
c
c_HIST	20 Nov 1998 Jeff Anderson, USGS Flagstaff, Original Version 
c	11 Mar 1999 K Teal Thompson  convert to format that passes the
c		    pht_minnaert_emp coefficients by common.  Change vars
c		    to reflect most recent Randy Kirk implementation of
c		    pht_minnaert_emp.
c       18 Mar 1999 KTT Move photometric vars to pho_common.inc.
c       15 Jul 1999 KTT Duplicated line increment for remaining reads.
c       29 Jul 1999 KTT Copy get_minnaert_emp_pars to get_minnaert_pol_
c		    pars and rewrite get_minnaert_emp to read data points
c		    and calculate a spline rather than just reading poly-
c		    nomial coefficients.
c	16 Mar 2001 Add error messages to help out the users and convert
c		    a large chunk of repeating code to subroutine pht_
c		    get_line_vals.F..  Change name of subroutine from
c		    get_minaert_emp to pht_get_minaert_emp.
c       15 Jul 2003 Janet Barrett, Added code to check for embedded
c                   comment lines.
c_END
c******************************************************************************
#include "pht_common.inc"
 
C Local variables

	integer*4	lun
	integer*4	status
	character*256	line
	integer*4	pos
	logical*4	found
	real*8          yp1,yp2
	character*256   x
	character*1     quote

	data quote /"'"/

C
C Get an unused logical unit number and then try to open the data file
C
	call u_allocate_unit (lun,status)
	if (status .ne. 0) then
	  ret = -1
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]   ',i3,
     .           ':  Can',a,'t obtain LUN')"
     .      )ret,quote
          call u_write_msg(3,x)
	  return 
	end if

	open (unit=lun,file=datafile,status='old',iostat=status)
	if (status .ne. 0) then
	  ret = -2
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]   ',i3,
     .      ':  Can',a,'t open datafile:  ',a128)"
     .      )ret,quote,datafile
          call u_write_msg(3,x)
	  return
	end if

C
C Search through the file for the Minnaert parameters and
C report an error if they can not be found.  Skip over comment lines
C (those starting with #).
C
	found = .false.
	do while (.not. found)
	  read (lun,'(a256)',iostat=status) line
	  if (status .ne. 0) then
	    ret = -3
            write(x,
     .        "(' [pht_get_minnaert_emp_pars]  ',i3,
     .        ':  Can',a,'t find starting point for LUNAR_LAMBERT_EMP',
     .        'in datafile:',a128)"
     .        )ret,quote,datafile
            call u_write_msg(3,x)
	    return
	  end if

	  if (line(1:1) .ne. '#') then
	    if (index(line,'MINNAERT_EMP') .ne. 0) found = .true. 
	  end if
	end do

C
C The next line in the file should contain the numkaypoints parameter.  
C Read it as a string, locate the equal sign, take the data after the 
C equal sign, and convert it to a floating point.
C
 10	read (lun,'(a256)',iostat=status) line
	if (status .ne. 0) then
	  ret = -4
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
     .           ':  Can',a,'t read numkaypoints line.')"
     .      )ret,quote
          call u_write_msg(3,x)
	  return
	end if
	if (line(1:1).eq.'#') goto 10

	pos = index(line,'=')
	if (pos .eq. 0) then
	  ret = -4
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
     .      ':  Can',a,'t find equal sign on line for numkaypoints')"
     .      )ret,quote
          call u_write_msg(3,x)
	  return
	end if
	line = line(pos+1:)

	read (line,'(i2)',iostat=status) numkaypoints
	if (status .ne. 0) then
	  ret = -4
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
     .      ':  Can',a,'t find numkaypoints value')"
     .      )ret,quote
          call u_write_msg(3,x)
	  return
	end if

C
C The next line in the file should contain the kayphase parameter.
C This parameter is an array, and all the values have to be read.
C Read it as a string, locate the equal sign, take the data after the 
C equal sign, and convert it to a floating point.
C
 20	read (lun,'(a256)',iostat=status) line
	if (status .ne. 0) then
	  ret = -5
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
     .      ':  Can',a,'t find kayphase line')"
     .      )ret,quote
          call u_write_msg(3,x)
	  return
	end if
	if (line(1:1).eq.'#') goto 20

        call pht_get_line_vals (line,numkaypoints,kayphase,ret)
        if (ret .ne. 0) then
          ret = -5
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
     .      ':  Can',a,'t load kayphase points  ')"
     .      )ret,quote
          call u_write_msg(3,x)
          return
        end if

C The next line in the file should contain the kayval parameter.
C This parameter is an array, and all the values have to be read.
C Read it as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 30     read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -6
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
     .      ':  Can',a,'t find kval line')"
     .      )ret,quote
          call u_write_msg(3,x)
          return
        end if
	if (line(1:1).eq.'#') goto 30

        call pht_get_line_vals (line,numkaypoints,kayval,ret)
        if (ret .ne. 0) then
          ret = -6
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
     .      ':  Can',a,'t load kval points  ')"
     .      )ret,quote
          call u_write_msg(3,x)
          return
        end if

        yp1 = 1.0d30
        yp2 = 1.0d30
        call r8spline(kayphase,kayval,numkaypoints,yp1,yp2,kay2)

C The next line in the file should contain the numbeepoints parameter.
C Read it as a string, locate the equal sign, take the data after the
C equal sign, and convert it to a floating point.
C
 40     read (lun,'(a256)',iostat=status) line
        if (status .ne. 0) then
          ret = -7
          write(x,
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
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
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
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
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
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
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
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
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
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
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
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
     .      "(' [pht_get_minnaert_emp_pars]  ',i3,
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
