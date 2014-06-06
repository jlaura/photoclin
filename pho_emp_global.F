      PROGRAM PHO_EMP_GLOBAL
      IMPLICIT NONE
C
C_TITLE PHO_EMP_GLOBAL - fit empirical photometric functions to Hapke
C
C_ARGS  See pho_emp_global.pdf
C
C_DESC
C This program finds lunar-Lambert or Minnaert photometric functions
C to approximate a more realistic but more complex Hapke model.  At each
C of a range of phase angles, the simpler model is fit to the Hapke
C model by adjusting its one parameter and its overall brightness so that
C the sum-squared-residual between the two is minimized.  Both the
C parameter (which, for both types of simple model, mainly controls
C limb darkening) and the brightness (normalized as an empirical phase
C curve) are reported in a table against phase angle.
C
C This program uses the photometric subroutines developed for the ISIS
C program PHOTOMET.  It is being prototyped by Randy Kirk in a VMS
C non-TAE environment with the intent that it later be made a UNIX/TAE
C program.  
C
C NOTE ON THE INTERFACE for TAE implementation:
C Only the parameters that are here input through Kirk's I/O routines
C (NEWD, NEW2D, NEWI, GETCMD, YN2TF, GETTXOUT...) need to be obtained
C through the TAE interface.  Specifically, a full set of parameters for
C the Hapke photometric function is to be obtained.  This does NOT need
C to be generalized to obtaining the photometric function name and
C possible parameters for functions other than Hapke.  Since it's only
C useful to fit empirical models to Hapke models (not other empiricals
C or non-empirical Minnaert/LL, etc.) only the Hapke parameters are needed.
C
C IN ADDITION TO A STRAIGHT PORT OF PHO_FIT_GLOBAL this program should
C serve as a clear example for developing a new program PHO_FIT_IMAGE
C to fit the parameter of an empirical photometric function to part of
C an actual image.  The differences can be summarized as follows:
C	1) Fit is done only once (not in a table vs. phase angle)
C	   since the input image has a fixed geometry.
C	2) The data to be fit *to* are obtained from an ISIS cube plane
C	   rather than calculated in GET_HAPKE_IMG.
C	3) Similarly, the incidence, emission, and phase angles are
C	   obtained from backplanes of the same cube and maintained
C	   in buffers corresponding point-for-point with the image.
C	   The routine to pull values out of these buffers at the
C	   current pixel can look identical to GET_ANGLES.
C	4) Both image data and angles should be stored in R*4 buffers
C	   and converted to double precision before being used.
C	5) Optionally, a region-of-interest mask could be obtained
C	   from a backplane and only those pixels in the ROI used
C	   in the fit.  This would be in addition on the incidence
C	   and emission angle tests already in the code.
C
C_HIST  Nov 16 1999 Randy Kirk  USGS Flagstaff Original Version
C       Sep 03 2002 Janet Barrett, Ported pho_fit_global from
C                   the VAX and renamed it pho_emp_global
C
C_END

C Include file contains the variable declarations defining a host of 
C parameters that must be passed to various photometric models to make
C them work, and the common blocks used to do the passing.  NOTE: the
C syntax of the INCLUDE command is slightly different in VMS FORTRAN.
#include "pht_common.inc"

C Default values of the Hapke parameters.  These can be changed by
C the user.  The values here are Jeff Johnson's fits to Photometry
C Flats at 651 nm with no opposition effect...
      DATA WH /0.52D0/			! Single-scattering albedo
      DATA HH,B0 /0.0D0,0.0D0/		! Opposition surge width and
					! strength parameters.  ZEROs
					! are strongly advised as the
					! simple models don't fit the
					! opposition effect well
      DATA BH,CH /0.0D0,0.0D0/		! First and second Legendre
					! coefficients of phase function
					! of a single particle
      DATA HG1,HG2 /0.213D0,1.0D0/	! Henyey-Greenstein coefficients
					! for alternate phase function
      DATA THETA /8.0D0/		! Small-scale surface rougness
					! in degrees

C Parameters controlling whether and what atmospheric model to use 
      LOGICAL*1	DOATM			! Include atmospheric effects?
      CHARACTER*3 DO_ATM
      INTEGER*4 IATM			! Type of atmospheric scattering
					! 1 = 1st order isotropic
					! 2 = 2nd order isotropic
					! 3 = 1st order anisotropic
					! 4 = 2nd order anisotropic
      CHARACTER*3 I_ATM
C Detailed atmospheric parameters are in a common block of their own
      DATA TAU /0.5D0/
      DATA HNORM /3.0D-3/
      DATA WHA /0.9D0/
      DATA BHA /0.95D0/
      DATA HGA /0.7D0/

C Parameter controlling which empirical model is fit
      INTEGER*4 IEMP			! 0=lunar-Lambert, 1=Minnaert
      CHARACTER*13 I_EMP
C Parameter controlling whether additive term is used in fit
      INTEGER*4 IORD			! 0=no additive, 1=additive term
      CHARACTER*3 I_ORD

C Parameters controlling the range of angles over which fit is made
      REAL*8 INCMIN,INCMAX		! Incidence angle range
      REAL*8 EMAMIN,EMAMAX0		! Emission angle range
      REAL*8 EMAMAX_PHASE_COEFF,EMAMAX	! Increase EMAMAX proportional
      DATA EMAMAX_PHASE_COEFF /0.111111111111D0/ ! to phase angle

C These parameters control the output table
      REAL*8 PHMIN,PHMAX		! Range of phase angles to model
      INTEGER*4 NPH			! # of phase angles
      INTEGER*4 IPH
      DATA PHMIN,PHMAX,NPH /0.0D0,170.0D0,18/
      REAL*8 DPH

C This stuff is used by Kirk's buffered routines to open a text file with
C a default name supplied to the user.  This will obviously get converted
C to TAE supplying a filename in a single string.
      CHARACTER FITFILE*128
      INTEGER*4 LUN

C For speed, values of the Hapke model (at given phase) are precomputed
C and stored in a large array that is basically an image of a sphere 
C (the top and bottom halves of the sphere are symmetric so only half
C is used).  Dynamic allocation of this buffer based on user input of
C the desired gridding fineness would be a nice touch that would let
C the user tailor the program to their own memory priveleges and desired
C run time.
      INTEGER*4 NLP,NL,NS
      PARAMETER (NLP=51)
      REAL*8 HAPKE_IMG(NLP*2-1,NLP)

C Additional declarations

      CHARACTER NOTE*80			! Note for output file
      REAL*8 PHASE			! Current phase angle
      REAL*8 TOL			! Fractional tolerance for
      DATA TOL /1.0E-6/			! the photometric parameter
      REAL*8 RMS0,RMS1			! RMS errors at parameter 1,0
      REAL*8 PARA,PARB,PARC		! Bracketing guesses for the
					! photometric paameter
      REAL*8 RMSA,RMSB,RMSC		! Corresponding RMS error
      REAL*8 PARMIN,RMSMIN		! Best-fit parameter and RMS error
      REAL*8 LFIT_PHT_GLOBAL		! Function that returns the
      EXTERNAL LFIT_PHT_GLOBAL		! RMS error for given parameter
      REAL*8 R8BRENT
      EXTERNAL R8BRENT
      REAL*8 C0,C1			! Constant and linear coefficients
					! of linear fit of empirical fn to
					! Hapke @ fixed param in LFIT_PHT_GLOBAL
      REAL*8 C1_0			! C1 multiplier at zero phase
      INTEGER*4 NOFIT			! Flag for no fit points found

      INTEGER*4 ICOUNT                  ! TAE parameter counter 
      INTEGER*4 LEN                     ! TAE string length
      INTEGER*4 RET                     ! Error code
      CHARACTER*32 ERRKEY               ! Keyword value for error message
      CHARACTER*256 ERRMSG              ! Error message
      INTEGER*4 ERRVAL                  ! Error message value
      CHARACTER*25 PHASEFUNC            ! Single particle phase function

C Yet another common block (besides those in PHO_COMMON.INC) is used to
C pass variables hidden from the two layers of numerics.  

      COMMON /LFIT_PHT_GLOBAL_COM/  HAPKE_IMG,INCMIN,INCMAX,
     *  EMAMIN,EMAMAX,PHASE,IEMP,IORD,C0,C1,NOFIT,NL,NS

      DATA INCMIN,INCMAX /0.0D0,89.0D0/
      DATA EMAMIN,EMAMAX0 /0.0D0,70.0D0/

C Initialize the system
      CALL U_STD_INIT('PHO_EMP_GLOBAL','2002-09-03',
     *'Fit empirical photometric functions to Hapke')       
      CALL U_ACTIME(1,RET)

      NL=NLP
      NS=NL*2-1

C Get TAE input from user
C
C First, get Hapke parameters 
      CALL U_GET_DBL_PARM('WH',1,1,7,WH,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('THETA',1,1,7,THETA,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('HH',1,1,7,HH,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('B0',1,1,7,B0,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_STR_PARM('PHASEFUNC',1,PHASEFUNC,LEN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      
      IF (PHASEFUNC.EQ.'HENYEY-GREENSTEIN') THEN
        IOPT=0
      ELSE
        IOPT=1
      ENDIF

      IF (IOPT.EQ.0) THEN
        CALL U_GET_DBL_PARM('HG1',1,1,7,HG1,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
        CALL U_GET_DBL_PARM('HG2',1,1,7,HG2,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
        CHAR_PHT_FUNCTION(1:6)='HAPHEN'
      ELSE
        CALL U_GET_DBL_PARM('BH',1,1,7,BH,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
        CALL U_GET_DBL_PARM('CH',1,1,7,CH,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
        CHAR_PHT_FUNCTION(1:6)='HAPLEG'
      END IF

C Next, get atmosphere parameters 
      CALL U_GET_STR_PARM('DOATM',1,DO_ATM,LEN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      IF (DO_ATM.EQ.'YES') THEN
        DOATM = .TRUE.
      ELSE
        DOATM = .FALSE.
      ENDIF
      IF (DOATM) THEN
        CALL U_GET_DBL_PARM('TAU',1,1,7,TAU,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
        CALL U_GET_DBL_PARM('HNORM',1,1,7,HNORM,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
        CALL U_GET_DBL_PARM('WHA',1,1,7,WHA,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
        CALL U_GET_DBL_PARM('HGA',1,1,7,HGA,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
        CALL U_GET_STR_PARM('IATM',1,I_ATM,LEN,ICOUNT,RET)
        IF (RET.LT.0) GOTO 9000
	IF (I_ATM.EQ.'1ST') THEN
	  IATM=1
        ELSE
	  IATM=2
        ENDIF
        IF (ABS(HGA).GE.1.0E-6) IATM=IATM+4
        CALL PHT_GET_AH_TABLE()
      END IF

C User chooses which empirical model to fit to the Hapke model
      CALL U_GET_STR_PARM('IEMP',1,I_EMP,LEN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      IF (I_EMP.EQ.'LUNAR-LAMBERT') THEN
        IEMP=1
      ELSE
        IEMP=2
      ENDIF
      IEMP=IEMP-1

C User chooses whether an additive offset is allowed
      CALL U_GET_STR_PARM('IORD',1,I_ORD,LEN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      IF (I_ORD.EQ.'NO') THEN
        IORD=1
      ELSE
        IORD=2
      ENDIF
      IORD=IORD-1

C User chooses limits on incidence, emission angles for fit
      CALL U_GET_DBL_PARM('INCMIN',1,1,7,INCMIN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('INCMAX',1,1,7,INCMAX,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('EMAMIN',1,1,7,EMAMIN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('EMAMAX',1,1,7,EMAMAX0,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('EMAMAX_PCOEFF',1,1,7,
     *EMAMAX_PHASE_COEFF,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000

C User chooses range and spacing of phase angles for output table
      CALL U_GET_DBL_PARM('PHMIN',1,1,7,PHMIN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('PHMAX',1,1,7,PHMAX,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_INT_PARM('NPH',1,NPH,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      DPH=(PHMAX-PHMIN)/DBLE(NPH-1)

C Allow user to supply a note of up to 80 characters
      CALL U_GET_STR_PARM('NOTE',1,NOTE,LEN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000

C User chooses name of file for output
      CALL U_GET_FILE_PARM('TO',1,FITFILE,LEN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_END_PARM()

      CALL U_ALLOCATE_UNIT(LUN,RET)
      OPEN(UNIT=LUN,FILE=FITFILE,STATUS='NEW',ERR=9001)

C Check to see if the table includes a zero phase point; if not, we
C will evaluate one anyway for normalization, unless we are allowing
C an additive offset, in which case the normalization makes no sense
C and is not used.
      IF (IORD.EQ.0) THEN
        IF (PHMIN.GT.1.0E-6) THEN
          PHASE=0.0D0
          EMAMAX=EMAMAX0
          CALL PHT_GET_HAPKE_IMG(HAPKE_IMG,NL,NS,PHASE,EMAMAX,
     *    DOATM,IATM)
          PARA=0.0D0
          PARB=1.0D0
          CALL R8MNBRAK(PARA,PARB,PARC,RMSA,RMSB,RMSC,
     *    LFIT_PHT_GLOBAL)
          RMSMIN=R8BRENT(PARA,PARB,PARC,LFIT_PHT_GLOBAL,TOL,PARMIN)

C Save the multiplicative normalization
          C1_0=C1
        END IF
      END IF

      WRITE (LUN,1000) WH,THETA,HH,B0
 1000 FORMAT('EMPIRICAL PHOTOMETRIC PARAMETER AND PHASE CURVES',
     *' CREATED BY PHO_FIT_GLOBAL',/,
     *'HAPKE WH = ',F8.6,', THETA = ',F8.4,', H = ',F8.6,
     *', B0 = ',F8.6)
      IF (IOPT.EQ.0) THEN
        WRITE (LUN,1010) HG1,HG2
 1010   FORMAT('SINGLE PARTICLE PHASE FN IS HENYEY-GREENSTEIN',
     *  ' WITH HG1 = ',F9.6,', HG2 = ',F9.6)
      ELSE
        WRITE (LUN,1020) BH,CH
 1020   FORMAT('SINGLE PARTICLE PHASE FUNCTION IS LEGENDRE',
     *  ' WITH B = ',F9.6,', C = ',F9.6)
      END IF
      IF (DOATM) THEN
        IF (IATM.EQ.1) THEN
          WRITE (LUN,1030) 'FIRST ORDER ISOTROPIC'
 1030     FORMAT(A,' ATMOSPHERIC SCATTERING MODEL.')
C            WRITE (LUN,1040) TAU,HNORM,WHA,BHA
          WRITE (LUN,1040) TAU,HNORM,WHA,HGA
 1040     FORMAT('TAU = ',F7.4,', SCALE HEIGHT/PLANET RADIUS = ',
     *    1PG14.7,/,'ATMOSPHERIC SS ALBEDO WHA = ',0PF6.4,:,
C     *      ', LEGENDRE COEFF. BHA = ',F7.4) 
     *    ', H-G ASYMMETRY FAC. HGA = ',F7.4) 
        ELSE IF (IATM.EQ.2) THEN
C            WRITE (LUN,1030) 'SECOND ORDER ISOTROPIC'
C            WRITE (LUN,1040) TAU,HNORM,WHA,BHA
          WRITE (LUN,1030) 'SECOND ORDER ISOTROPIC'
          WRITE (LUN,1040) TAU,HNORM,WHA,BHA
        ELSE IF (IATM.EQ.3) THEN
C            WRITE (LUN,1030) 'FIRST ORDER ANISOTROPIC (LEGENDRE)'
C            WRITE (LUN,1040) TAU,HNORM,WHA,BHA
          WRITE (LUN,1030) 'FIRST ORDER ANISOTROPIC '//
     *    '(HENYEY-GREENSTEIN)'
          WRITE (LUN,1040) TAU,HNORM,WHA,HGA
        ELSE 
C            WRITE (LUN,1030) 'SECOND ORDER ANISOTROPIC (LEGENDRE)'
C            WRITE (LUN,1040) TAU,HNORM,WHA,BHA
          WRITE (LUN,1030) 'SECOND ORDER ANISOTROPIC '//
     *    '(HENYEY-GREENSTEIN)'
          WRITE (LUN,1040) TAU,HNORM,WHA,HGA
        END IF
      END IF
      WRITE (LUN,1050) NL,INCMIN,INCMAX,EMAMIN,EMAMAX0,
     *EMAMAX_PHASE_COEFF
 1050 FORMAT('# GRID POINTS = ',I4,/,
     *'INCIDENCE ANGLE RANGE OF FIT = ',F7.4,' TO ',F7.4,/,
     *'EMISSION  ANGLE RANGE OF FIT = ',F7.4,' TO ',F7.4,/,
     *'INCREASE EMISSION ANGLE MAXIMUM BY PHASE ANGLE TIMES ',F8.6)
      IF (IEMP.EQ.0) THEN
        WRITE (LUN,1060)
 1060   FORMAT('FITTED EMPIRICAL FUNCTION IS LUNAR-LAMBERT ',
     *  '(LIMB-DARKENING PARAMETER IS L)')
      ELSE
        WRITE (LUN,1070)
 1070   FORMAT('FITTED EMPIRICAL FUNCTION IS MINNAERT ',
     *  '(LIMB-DARKENING PARAMETER IS K)')
      ENDIF
      IF (IORD.EQ.0) THEN
        WRITE (LUN,1080) 
 1080   FORMAT('FIT INCLUDES MULTIPLIER ONLY',/,
     *  'COLS = PHASE ANGLE, LIMB-DARKENING PARAMETER, PHASE CURVE,',
     *  ' BEST-FIT MULTIPLIER, RMS ERROR OF FIT')
      ELSE
        WRITE (LUN,1090)
 1090   FORMAT('FIT INCLUDES MULTIPLIER ONLY',/,
     *  'COLS = PHASE ANGLE, LIMB-DARKENING PARAMETER, BEST-FIT',
     *  ' ADDITIVE TERM, BEST-FIT MULTIPLIER, RMS ERROR OF FIT')
      END IF
      WRITE (LUN,1100) NOTE
 1100 FORMAT('NOTE: ',A,/,'FORMAT=(1X,F8.4,4(1X,1PG14.7))',/,'C_END')

C Now loop to create the table of results versus phase angle.
      DO IPH=1,NPH

C Fill the buffer with the Hapke results at the right phase
        PHASE=PHMIN+DPH*DBLE(IPH-1)
        EMAMAX=EMAMAX0+EMAMAX_PHASE_COEFF*PHASE
        IF (PHASE.GE.INCMAX+EMAMAX) GO TO 10	! No valid fit points, quit
        CALL PHT_GET_HAPKE_IMG(HAPKE_IMG,NL,NS,PHASE,EMAMAX,DOATM,IATM)

C Now seek the minimum of the RMS error by adjusting the parameter
        PARA=0.0D0
        RMS0=LFIT_PHT_GLOBAL(PARA)
        IF (NOFIT.NE.0) GO TO 10	! Actual count of fit points fails
        PARB=1.0D0
        RMS1=LFIT_PHT_GLOBAL(PARB)
        CALL R8MNBRAK(PARA,PARB,PARC,RMSA,RMSB,RMSC,LFIT_PHT_GLOBAL)
        RMSMIN=R8BRENT(PARA,PARB,PARC,LFIT_PHT_GLOBAL,TOL,PARMIN)
        IF (PHASE.LT.1.0D-6) C1_0=C1
        IF (IORD.EQ.0) THEN

C Fit with no additive offset:  output multiplier normalized to
C zero phase, which is the desired phase curve B, and unnormalized
          WRITE (LUN,2000) PHASE,PARMIN,C1/C1_0,C1,RMSMIN
          PRINT 2000, PHASE,PARMIN,C1/C1_0,C1,RMSMIN
        ELSE

C Fit with additive offset:  normalizing would make no sense, just
C output additive offset and multiplier from fit
          WRITE (LUN,2000) PHASE,PARMIN,C0,C1,RMSMIN
          PRINT 2000, PHASE,PARMIN,C0,C1,RMSMIN
        END IF
 2000   FORMAT(1X,F8.4,4(1X,1PG14.7))
      ENDDO
   10 CONTINUE	! Jump to here if phase angle gets too big for there
      CLOSE(LUN)! to be any valid fit points
      CALL U_ACTIME(2,RET)
      CALL U_STD_EXIT()
      STOP

C Handle processing errors
9000  ERRKEY = 'PHO_EMP_GLOBAL-TAEERR'
      ERRMSG = 'Error reading TAE parameters from user input'
      ERRVAL = -1
      GOTO 9999

9001  ERRKEY = 'PHO_EMP_GLOBAL-OPNOUT'
      ERRMSG = 'Error opening output file to contain fits'
      ERRVAL = -2
      GOTO 9999

C Write an error message
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,2)
      STOP
      END
