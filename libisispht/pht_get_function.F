c       ================================================================
        subroutine pht_get_function(char_pht_function,ifunction)
c       ================================================================
	implicit none

c_title pht_get_function - get the photometric function
c
c_args  type          variable  i/o    description
        character*6   char_pht_function!tae input char val photometric function
        integer*4     ifunction        !corresponding int val for photom func
c
c_desc  this routine will get the corresponding integer value for the tae input
c       character char_pht_function representing the user's requested photometric
c       correction function for purposes of setting up the step 3 goto to
c	appropriate get_whatever function.
c
c_hist  19 nov 98  K Teal Thompson - U.S.G.S. Flagstaff original version
c	12 Aug 99  KTT - Polynomial functions have been added and docu-
c		   mentation checked.  Pass iopt by common.
c       16 Sep 04  Janet Barrett - This subroutine was completely 
c                  ignoring the photometric functions LAMBER and LOMSEL.
c                  This was causing problems in photomet. These two
c                  functions are now given values other than zero.
c
c_end
	integer*4 iopt
	common /get_func_hap/ iopt

	if (char_pht_function.eq.'NONE')   ifunction=0
	if (char_pht_function.eq.'LL_POL') ifunction=1
	if (char_pht_function.eq.'LL_EMP') ifunction=2
	if (char_pht_function.eq.'MN_POL') ifunction=3
	if (char_pht_function.eq.'MN_EMP') ifunction=4
	if (char_pht_function.eq.'HAPHEN') ifunction=5
	if (char_pht_function.eq.'HAPLEG') ifunction=6
	if (char_pht_function.eq.'HAPH_S') ifunction=7
	if (char_pht_function.eq.'HAPL_S') ifunction=8
	if (char_pht_function.eq.'MOONPR') ifunction=9
	if (char_pht_function.eq.'USER')   ifunction=10

	if (char_pht_function.eq.'LAMBER') ifunction=98
	if (char_pht_function.eq.'LOMSEL') ifunction=98

c	Need an ifunction for the following photometric
c	functions to go to the photometric part of the
c	program, but we are not calling a get_whatever
c	routine.
	if (char_pht_function.eq.'LUNLAM') ifunction=99
	if (char_pht_function.eq.'MIN') ifunction=99

c	Set iopt for the Hapke functions
	if (char_pht_function(1:4).eq.'HAPH') iopt=0
	if (char_pht_function(1:4).eq.'HAPL') iopt=1

	return
	end
 
