c       ================================================================
        function my_pht_user( inc, ema, phase )
c       ================================================================
        implicit none
c_title my_pht_user - user photometric model
c
c_args  type         variable  i/o      description
        real*8       inc	!i	incidence angle in degrees
        real*8       ema	!i	emission angle in degrees
        real*8       phase	!i	phase angle in degrees
        real*8       my_pht_user!i	return

c_desc  Subroutine skeleton for my and scientists' testing purposes.
c       
c
c_hist  20 Dec 2000 K Teal Thompson
c       ================================================================
 
#include "pht_common.inc"


	real*8		user_array(9)!You input via TAE parm USARA
	!save		user_array
        real*8		munot,mu,deg2rad
	parameter 	(deg2rad=0.01745329251994329576913d0)

	logical*4	myFirstime
	save		myFirstime
	data myFirstime /.true./

	integer*4 ret, count

	if (myFirstime.eqv..true.) then
	  !get the user_array
	  call u_get_dbl_parm('USRARA',9,1,8,user_array,count,ret)
          if (ret.lt.0) then
            print*,'my_pht_user: user function: user_array: ERROR'
	    user_array(1)=0.d0
	  endif
	  myFirstime=.false.
        else !not myFirstime

          munot=cos(deg2rad*inc)
          mu=cos(deg2rad*ema)

	  !This example uses Lunar Lambert algorithm
	  !Substitute my/your code here:

          if ((munot.le.0.d0).or.(mu.le.0.d0)) then
            my_pht_user =0.d0
          else if (user_array(1).eq.0.d0) then
            my_pht_user=munot
          else if (user_array(1).eq.1.d0) then
            my_pht_user=2.d0*munot/(munot+mu)
          else
            my_pht_user=munot*((1.d0-user_array(1)) +
     .        2.d0*user_array(1)/(munot+mu))
          end if

	  !end Lunar Lambert example or my/your code


        endif !not myFirstime

        return
        end

