	subroutine pht_get_line_vals (line,dataCount,dataPoints,ret)
	implicit none
c******************************************************************************
c
c_TITLE pht_get_line_vals - Get the data points for the calling empirical
c			    function from the datafile line passed in.
c
c_ARGS	TYPE		VARIABLE	I/O	DESCRIPTION
c
	character*(*)	line		!I      line out of the data file
c                                               containing Lunar-Lambert
c                                               parameters
	integer*4	dataCount	!I      return code
	real*8		dataPoints(*)	!O      Declared in pht_common.inc
	integer*4	ret		!O      return code
c                                                0 = ok
c                                               -1 = Can't find equal sign
c						     on line 
c                                               -2 = Wrong number of dataPoint
c						     values
c                                               -3 = Error reading data points
c
c_DESC  This subroutine reads the "line" passed from datafile in order to
c       obtain the data points for the calling empirical function.
c
c_HIST  15 Mar 2001 K T Thompson - convert repeating code in pht_get*pars
c		    files to subroutine.
c
c_END
c******************************************************************************

#include "pht_common.inc"


        integer*4       pos
        integer*4       count
        integer*4       status
	character*256   x
	character*1     quote

	data quote /"'"/

c******************************************************************************
c The line in the file should contain the dataPoints parameter.
c This parameter is an array, and all the values have to be read.
c Read it as a string, locate the equal sign, take the data after the
c equal sign, and convert it to a floating point.
c******************************************************************************


        pos = index(line,'=')
        if (pos .eq. 0) then
          ret = -1

c******************************************************************************
c The following code works but gives an INFO when compiling on Alphas:
c fort: Info: pht_get_line_vals.F, line 94: Character or Hollerith constant cont
c inued across lines; may be non-portable
c      .      "(' [pht_get_line_vals]  ',i3,
c -----------------------------------------^

c         write(x, 
c    .    "(' [pht_get_line_vals]  ',i3,
c    .    ':  Can',a,'t find equal sign on line')"
c    .    )ret,quote
c
c Instead I'm using two write statements for the error messages.
c******************************************************************************

          write(x, 
     .    "(' [pht_get_line_vals]:  ',i3)")ret
          call u_write_msg(3,x)

          write(x,"(' Can',a1,'t find equal sign on line')")quote
          call u_write_msg(3,x)

          return
        end if

        line = line(pos+1:)

        call u_write_msg(3,'dataPoints:  ')

        do count=1,dataCount

          read (line,'(f15.8)',iostat=status) dataPoints(count)
          if (status .ne. 0 ) then

             ret = -3

             write(x, 
     .       "(' [pht_get_line_vals]:  ',i3)")ret
             call u_write_msg(3,x)

             write(x, 
     .       "(' Error reading dataPoints values')")
             call u_write_msg(3,x)

             return

          end if

          write(x, "(f15.8)")dataPoints(count)
          call u_write_msg(3,x)

c         set position at index of first comma of whatever remains
c         of line, then line = line from position to end.
c         repeat until counting out dataCount (number of
c         Lunar-Lambert points).  Note that if there is no comma,
c         pos will return from index() as 0 and line will remain
c         the same. (line = line(1:))

          pos = index(line,',')

          line = line(pos+1:)

c         *********************************************************************
c         Here I'm trying to find a trailing comma combined with not
c         enough cefficients.  The final comma can give the false im-
c         pression that there are dataCount rather than dataCount
c         - 1 points.  Also finds dataCount - 1 points without the
c         trailing comma.
c         *********************************************************************

          if ( (pos .eq. 0 .and. count.ne.dataCount)	!short a value
     .       .or.line.eq.' ') then			!trailing comma

             ret = -2

             write(x, 
     .       "(' [pht_get_line_vals]:  ',i3)")ret
             call u_write_msg(3,x)

             write(x, 
     .       "('  Check number dataPoints values.')")
             call u_write_msg(3,x)

             return

          endif


        enddo



        if (pos .ne. 0) then
            write(x, 
     .      "(' WARNING:  remainder of line not used: ')")line
            call u_write_msg(3,x)
	endif


	ret = 0
	return
	end
