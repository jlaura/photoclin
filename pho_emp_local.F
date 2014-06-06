      PROGRAM PHO_EMP_LOCAL
      IMPLICIT NONE
C
C_TITLE PHO_EMP_LOCAL - fit empirical photometric functions to Hapke
C
C_ARGS  See pho_emp_local.pdf
C
C_DESC
C This program finds lunar-Lambert or Minnaert photometric functions
C to approximate a more realistic but more complex Hapke model.
C The program is based on PHO_FIT_GLOBAL with the following differences:
C The fit is performed at a single geometry rather than for a range of
C phase angles.  The user specifies the phase angle and the incidence and
C emission angles of the mean ground plane (datum), as well as the RMS
C slope relative to the dataum.  Artificial data are then created, with
C slopes drawn from an isotropic Gaussian distribution relative to the
C datum.  The simpler model is fit at these orientations to the Hapke
C model by adjusting its one parameter and its overall brightness so that
C the sum-squared-residual between the two is minimized.  Both the
C parameter (which, for both types of simple model, mainly controls
C limb darkening) and the brightness (normalized as an empirical phase
C curve) are reported.
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
C       1) Fit is done only once (not in a table vs. phase angle)
C          since the input image has a fixed geometry.
C       2) The data to be fit *to* are obtained from an ISIS cube plane
C          rather than calculated in GET_HAPKE_IMG.
C       3) Similarly, the incidence, emission, and phase angles are
C          obtained from backplanes of the same cube and maintained
C          in buffers corresponding point-for-point with the image.
C          The routine to pull values out of these buffers at the
C          current pixel can look identical to GET_ANGLES.
C       4) Both image data and angles should be stored in R*4 buffers
C          and converted to double precision before being used.
C       5) Optionally, a region-of-interest mask could be obtained
C          from a backplane and only those pixels in the ROI used
C          in the fit.  This would be in addition on the incidence
C          and emission angle tests already in the code.
C
C_HIST  Nov 16 1999 Randy Kirk  USGS Flagstaff Original Version
C       Jan 13 2003 Janet Barrett, Ported pho_fit_local from
C                   the VAX and renamed it pho_emp_local
C
C_END

C Include file contains the variable declarations defining a host of
C parameters that must be passed to various photometric models to make
C them work, and the common blocks used to do the passing.  NOTE: the
C syntax of the INCLUDE command is slightly different in VMS FORTRAN.

#include "pht_common.inc"

C
C       THE FOLLOWING SETUPS ARE THE SAME AS PHO_FIT_GLOBAL
C       THEREFORE IN ISIS PHO_EMP_LOCAL THEY SHOULD COPY PHO_EMP_GLOBAL
C
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

C
C       NOW HERE ARE THE PARAMETERS THAT DIFFER FROM THE GLOBAL
C               FIRST, THE OLD PHO_FIT_GLOBAL DECLARATIONS
C
C Parameters controlling the range of angles over which fit is made
C     REAL*8 INCMIN,INCMAX		! Incidence angle range
C     REAL*8 EMAMIN,EMAMAX0		! Emission angle range
C     REAL*8 EMAMAX_PHASE_COEFF,EMAMAX	! Increase EMAMAX proportional
C     DATA EMAMAX_PHASE_COEFF /0.111111111111D0/ ! to phase angle

C These parameters control the output table
C     REAL*8 PHMIN,PHMAX		! Range of phase angles to model
C     INTEGER*4 NPH			! # of phase angles
C     INTEGER*4 IPH
C     DATA PHMIN,PHMAX,NPH /0.0D0,170.0D0,18/
C     REAL*8 DPH
C
C               NOW, THE NEW DECLARATIONS FOR PHO_FIT_LOCAL
C
C These are the new user parameters
      REAL*8 INCDAT,EMADAT,PHASE    ! Incidence, emission to datum; phase
      DATA INCDAT,EMADAT /45.0D0,0.0D0/
      REAL*8 RMSDEG                     ! RMS adirectional slope in degrees
      DATA RMSDEG /10.0D0/

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
C NOTE THAT THERE IS NO REAL NEED FOR THE ARRAY OF HAPKE VALUES IN
C THIS PROGRAM TO BE SQUARE OR EVEN TWO-DIMENSIONAL, SINCE IT'S JUST A
C LIST OF MONTE CARLO POINTS, NOT AN IMAGE.  BUT IT IS CONVENIENT TO
C RETAIN THE SAME DECLARATION AS IN PHO_FIT_GLOBAL.
      INTEGER*4 NLP,NL,NS
      PARAMETER (NLP=51)
      REAL*8 HAPKE_IMG(NLP*2-1,NLP)
C BECAUSE WE CAN'T CALCULATE THE INCIDENCE AND EMISSION ANGLES FOR EACH
C POINT FROM THE EQUATION OF A SPHERE IN THIS PROGRAM, WE MUST STORE THEM
      REAL*8 INC_IMG(NLP*2-1,NLP),EMA_IMG(NLP*2-1,NLP)

C Additional declarations

      CHARACTER NOTE*80			! Note for output file
C      REAL*8 PHASE			! Current phase angle
      REAL*8 TOL			! Fractional tolerance for
      DATA TOL /1.0E-6/			! the photometric parameter
      REAL*8 RMS0,RMS1			! RMS errors at parameter 1,0
      REAL*8 PARA,PARB,PARC		! Bracketing guesses for the
					! photometric paameter
      REAL*8 RMSA,RMSB,RMSC		! Corresponding RMS error
      REAL*8 PARMIN,RMSMIN		! Best-fit parameter and RMS error
C      REAL*8 LFIT_PHT_GLOBAL		! Function that returns the
C      EXTERNAL LFIT_PHT_GLOBAL		! RMS error for given parameter
      REAL*8 LFIT_PHO_LOCAL             ! Function that returns the
      EXTERNAL LFIT_PHO_LOCAL           ! RMS error for given parameter
      REAL*8 R8BRENT
      EXTERNAL R8BRENT
      REAL*8 C0,C1			! Constant and linear coefficients
					! of linear fit of empirical fn to
					! Hapke @ fixed param in LFIT_PHT_GLOBAL
C     REAL*8 C1_0			! C1 multiplier at zero phase
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
C NOTE CHANGES FROM GLOBAL VERSION
C      COMMON /LFIT_PHT_GLOBAL_COM/  HAPKE_IMG,INCMIN,INCMAX,
C     *  EMAMIN,EMAMAX,PHASE,IEMP,IORD,C0,C1,NOFIT,NL,NS
      COMMON /LFIT_PHO_LOCAL_COM/  HAPKE_IMG,INC_IMG,EMA_IMG,NL,NS,
     *                       PHASE,IEMP,IORD,C0,C1,NOFIT

      DATA PHASE /45.0D0/
C     DATA INCMIN,INCMAX /0.0D0,89.0D0/
C     DATA EMAMIN,EMAMAX0 /0.0D0,70.0D0/

C Initialize the system
      CALL U_STD_INIT('PHO_EMP_LOCAL','2003-01-13',
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

C
C       OLD USER INPUTS FOR PHO_FIT_GLOBAL TABLE VS PHASE ANGLE
C
C User chooses limits on incidence, emission angles for fit
C     CALL U_GET_DBL_PARM('INCMIN',1,1,7,INCMIN,ICOUNT,RET)
C     IF (RET.LT.0) GOTO 9000
C     CALL U_GET_DBL_PARM('INCMAX',1,1,7,INCMAX,ICOUNT,RET)
C     IF (RET.LT.0) GOTO 9000
C     CALL U_GET_DBL_PARM('EMAMIN',1,1,7,EMAMIN,ICOUNT,RET)
C     IF (RET.LT.0) GOTO 9000
C     CALL U_GET_DBL_PARM('EMAMAX',1,1,7,EMAMAX0,ICOUNT,RET)
C     IF (RET.LT.0) GOTO 9000
C     CALL U_GET_DBL_PARM('EMAMAX_PHASE_COEFF',1,1,7,
C    *EMAMAX_PHASE_COEFF,ICOUNT,RET)
C     IF (RET.LT.0) GOTO 9000
C
C User chooses range and spacing of phase angles for output table
C     CALL U_GET_DBL_PARM('PHMIN',1,1,7,PHMIN,ICOUNT,RET)
C     IF (RET.LT.0) GOTO 9000
C     CALL U_GET_DBL_PARM('PHMAX',1,1,7,PHMAX,ICOUNT,RET)
C     IF (RET.LT.0) GOTO 9000
C     CALL U_GET_INT_PARM('NPH',1,NPH,ICOUNT,RET)
C     IF (RET.LT.0) GOTO 9000
C     DPH=(PHMAX-PHMIN)/DBLE(NPH-1)
C
C       NEW USER INPUTS FOR PHO_FIT_LOCAL
C
      CALL U_GET_DBL_PARM('INCDAT',1,1,7,INCDAT,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('EMADAT',1,1,7,EMADAT,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('PHASE',1,1,7,PHASE,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_GET_DBL_PARM('RMSDEG',1,1,7,RMSDEG,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000

C Allow user to supply a note of up to 80 characters
      CALL U_GET_STR_PARM('NOTE',1,NOTE,LEN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000

C User chooses name of file for output
      CALL U_GET_FILE_PARM('TO',1,FITFILE,LEN,ICOUNT,RET)
      IF (RET.LT.0) GOTO 9000
      CALL U_END_PARM()

      CALL U_ALLOCATE_UNIT(LUN,RET)
      OPEN(UNIT=LUN,FILE=FITFILE,STATUS='UNKNOWN',
     .ACCESS='APPEND',ERR=9001)

C
C       THIS CODE IS NOT USED IN PHO_FIT_LOCAL
C
C Check to see if the table includes a zero phase point; if not, we
C will evaluate one anyway for normalization, unless we are allowing
C an additive offset, in which case the normalization makes no sense
C and is not used.
C     IF (IORD.EQ.0) THEN
C       IF (PHMIN.GT.1.0E-6) THEN
C         PHASE=0.0D0
C         EMAMAX=EMAMAX0
C         CALL PHT_GET_HAPKE_IMG(HAPKE_IMG,NL,NS,PHASE,EMAMAX,
C    *    DOATM,IATM)
C         PARA=0.0D0
C         PARB=1.0D0
C         CALL R8MNBRAK(PARA,PARB,PARC,RMSA,RMSB,RMSC,
C    *    LFIT_PHT_GLOBAL)
C         RMSMIN=R8BRENT(PARA,PARB,PARC,LFIT_PHT_GLOBAL,TOL,PARMIN)
C
C Save the multiplicative normalization
C         C1_0=C1
C       END IF
C     END IF

      WRITE (LUN,1000) WH,THETA,HH,B0
 1000 FORMAT('EMPIRICAL PHOTOMETRIC PARAMETER AND PHASE CURVES',
     *' CREATED BY PHO_EMP_LOCAL',/,
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
C
C       OLD HEADER DESCRIPTION OF PARAMETERS
C
C     WRITE (LUN,1050) NL,INCMIN,INCMAX,EMAMIN,EMAMAX0,
C    *EMAMAX_PHASE_COEFF
C1050 FORMAT('# GRID POINTS = ',I4,/,
C    *'INCIDENCE ANGLE RANGE OF FIT = ',F7.4,' TO ',F7.4,/,
C    *'EMISSION  ANGLE RANGE OF FIT = ',F7.4,' TO ',F7.4,/,
C    *'INCREASE EMISSION ANGLE MAXIMUM BY PHASE ANGLE TIMES ',F8.6)
C
C       NEW HEADER DESCRIPTION OF PARAMETERS
C
      WRITE (LUN,1050) NL,INCDAT,EMADAT,PHASE,RMSDEG
 1050 FORMAT('# GRID POINTS = ',I4,/,
     * 'INCIDENCE ANGLE TO DATUM = ',F7.4,/,
     * 'EMISSION  ANGLE TO DATUM = ',F7.4,/,
     * 'PHASE ANGLE = ',F7.4,/,
     * 'RMS SLOPE = ',F7.4)

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
C    *  'COLS = PHASE ANGLE, LIMB-DARKENING PARAMETER, PHASE CURVE,',
     *  'COLS = PHASE ANGLE, LIMB-DARKENING PARAMETER,',
     *  ' BEST-FIT MULTIPLIER, RMS ERROR OF FIT')
      ELSE
        WRITE (LUN,1090)
C1090   FORMAT('FIT INCLUDES MULTIPLIER ONLY',/,
 1090    FORMAT('FIT INCLUDES MULTIPLIER AND OFFSET',/,
     *  'COLS = PHASE ANGLE, LIMB-DARKENING PARAMETER, BEST-FIT',
     *  ' ADDITIVE TERM, BEST-FIT MULTIPLIER, RMS ERROR OF FIT')
      END IF
      WRITE (LUN,1100) NOTE
 1100 FORMAT('NOTE: ',A,/,'FORMAT=(1X,F8.4,4(1X,1PG14.7))',/,'C_END')

C Now loop to create the table of results versus phase angle.
C LOOP IS NO LONGER NEEDED.  I AM COMMENTING OUT THE DO STATEMENT
C ULTIMATELY, SHOULD REMOVE IT AND DE-INDENT THE LOOP CONTENTS.
C     DO IPH=1,NPH
C
C Fill the buffer with the Hapke results at the right phase
C       PHASE=PHMIN+DPH*DBLE(IPH-1)
C       EMAMAX=EMAMAX0+EMAMAX_PHASE_COEFF*PHASE
C       IF (PHASE.GE.INCMAX+EMAMAX) GO TO 10	! No valid fit points, quit
C       CALL PHT_GET_HAPKE_IMG(HAPKE_IMG,NL,NS,PHASE,EMAMAX,DOATM,IATM)
CJB        IF (PHASE.GE.INCDAT+EMADAT) GO TO 10   ! No valid fit points, quit
        IF (PHASE.GT.INCDAT+EMADAT) GO TO 10   ! No valid fit points, quit
	CALL GET_HAPKE_IMG_LOC(HAPKE_IMG,INC_IMG,EMA_IMG,NL,NS,
     *    INCDAT,EMADAT,PHASE,RMSDEG,DOATM,IATM)

C Now seek the minimum of the RMS error by adjusting the parameter
        PARA=0.0D0
        RMS0=LFIT_PHO_LOCAL(PARA)
        IF (NOFIT.NE.0) GO TO 10	! Actual count of fit points fails
        PARB=1.0D0
        RMS1=LFIT_PHO_LOCAL(PARB)
C       CALL R8MNBRAK(PARA,PARB,PARC,RMSA,RMSB,RMSC,LFIT_PHT_GLOBAL)
C       RMSMIN=R8BRENT(PARA,PARB,PARC,LFIT_PHT_GLOBAL,TOL,PARMIN)
        CALL R8MNBRAK(PARA,PARB,PARC,RMSA,RMSB,RMSC,LFIT_PHO_LOCAL)
	RMSMIN=R8BRENT(PARA,PARB,PARC,LFIT_PHO_LOCAL,TOL,PARMIN)
C       IF (PHASE.LT.1.0D-6) C1_0=C1
        IF (IORD.EQ.0) THEN

C Fit with no additive offset:  output multiplier normalized to
C zero phase, which is the desired phase curve B, and unnormalized
C IN PHO_FIT_LOCAL, OMIT THE NORMALIZED PHASE CURVE
C         WRITE (LUN,2000) PHASE,PARMIN,C1/C1_0,C1,RMSMIN
C         PRINT 2000, PHASE,PARMIN,C1/C1_0,C1,RMSMIN
          WRITE (LUN,2000) PHASE,PARMIN,C1,RMSMIN
	  PRINT 2000, PHASE,PARMIN,C1,RMSMIN
        ELSE

C Fit with additive offset:  normalizing would make no sense, just
C output additive offset and multiplier from fit
          WRITE (LUN,2000) PHASE,PARMIN,C0,C1,RMSMIN
          PRINT 2000, PHASE,PARMIN,C0,C1,RMSMIN
        END IF
 2000   FORMAT(1X,F8.4,4(1X,1PG14.7))
C       COMMENT OUT END OF PHASE ANGLE LOOP
C     ENDDO
   10 CONTINUE	! Jump to here if phase angle gets too big for there
      CLOSE(LUN)! to be any valid fit points
      CALL U_ACTIME(2,RET)
      CALL U_STD_EXIT()
      STOP

C Handle processing errors
9000  ERRKEY = 'PHO_EMP_LOCAL-TAEERR'
      ERRMSG = 'Error reading TAE parameters from user input'
      ERRVAL = -1
      GOTO 9999

9001  ERRKEY = 'PHO_EMP_LOCAL-OPNOUT'
      ERRMSG = 'Error opening output file to contain fits'
      ERRVAL = -2
      GOTO 9999

C Write an error message
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,2)
      STOP
      END

      SUBROUTINE GET_HAPKE_IMG_LOC(HAPKE_IMG,INC_IMG,EMA_IMG,NL,NS,
     * INCDAT,EMADAT,PHASE,RMSDEG,DOATM,IATM)
      IMPLICIT NONE
C Fills the buffer HAPKE_IMG with the Hapke-model radiances of
C points with given PHASE and random incidence and emission angles
C stored in INC_IMG, EMA_IMG.
#include "pht_common.inc"
      INTEGER*4 NL,NS
      REAL*8 HAPKE_IMG(NS,NL)		! Array for Hapke image of sphere
      REAL*8 INC_IMG(NS,NL)		! Array for incidence angles
      REAL*8 EMA_IMG(NS,NL)		! Array for emission angles
      REAL*8 INCDAT			! Incidence angle to datum
      REAL*8 EMADAT			! Emission angle to datum
      REAL*8 PHASE			! Phase angle
      REAL*8 RMSDEG			! RMS adirectional slope wrt datum
      LOGICAL*1	DOATM			! Include atmospheric effects?
      INTEGER*4 IATM			! Type of atmospheric scattering
					! 1 = 1st order isotropic
					! 2 = 2nd order isotropic
					! 3 = 1st order anisotropic
					! 4 = 2nd order anisotropic
					! 5 = 1st order Henyey-Greenstein
					! 6 = 2nd order Henyey-Greenstein

      REAL*8 INCX,INCZ	! Sun vector (Y component 0 by definition)
      REAL*8 EMAX,EMAY,EMAZ	! S/C vector
      REAL*8 SCAZRAD		! S/C azimuth; used only in radians
      REAL*8 COSP		! Cosine of phase angle
      REAL*8 RMSBI		! RMS bidirectional slope equiv to RMSDEG
      INTEGER*4 IDUM		! Random number seed...might want to initialize
      REAL*4 U1,U2              ! Uniform random variates
      REAL*4 S,T                ! Temp vars to turn U1,U2 into Gauss deviates
      REAL*4 RANDO              ! Random number generator
      REAL*8 DZDX,DZDY	        ! Local slopes
      REAL*8 DEN		! Denominator for surface normal vector
      REAL*8 INC,EMA		! Local incidence and emission
C      INTEGER*4 IRET		! Error code: -1 = beyond edge of sphere
C				! (not a fatal error, we just want to
C				! ignore these parts of the buffer...)
      REAL*8 MUNOT,MU
      REAL*8 AHI
      REAL*8 PSTD,TRANS,TRANS0,SBAR
      REAL*8 DEG2RAD
      DATA DEG2RAD /0.01745329251994329576913D0/
      INTEGER*4 I,J
      REAL*8 PHT_HAPKE

C Pardon my cheezy error trap.  Probably not needed at all in this
C one-time-use function.
      IF (NS.NE.NL*2-1) STOP 'BAD BUFFER DIMENSIONS.'

C
C	PREPARATORY CALCULATIONS FOR POINT-BY-POINT INC AND EMA
C
      INCX=SIN(INCDAT*DEG2RAD)
      INCZ=COS(INCDAT*DEG2RAD)
      EMAZ=COS(EMADAT*DEG2RAD)
      COSP=COS(PHASE*DEG2RAD)
      IF ((INCDAT.EQ.0.0D0).OR.(EMADAT.EQ.0.0D0)) THEN
         SCAZRAD=0.0D0	! Actually undefined, set to zero
         EMAX=SIN(EMADAT*DEG2RAD)
         EMAY=0.0D0
      ELSE
         EMAX=(COSP-INCZ*EMAZ)/INCX
         EMAY=SIN(EMADAT*DEG2RAD)
         SCAZRAD=ACOS(EMAX/EMAY)
         EMAY=EMAY*SIN(SCAZRAD)
      END IF
      RMSBI=TAN(RMSDEG*DEG2RAD)/SQRT(2.0D0)

      DO J=1,NL
         DO I=1,NS
C
C	OLD GLOBAL CODE CALLED SUBROUTINE TO GET ANGLES ON SPHERE
C
C            CALL GET_ANGLES(NL,NS,J,I,INC,EMA,PHASE,IRET)
C
C	NEW CODE CALCULATES ANGLES BASED ON RANDOM ORIENTATION TO DATUM
C Algorithm for Gaussian random deviates is that of Box and Muller,
C taken from Forsyth, Malcom, and Moler. 
C
            S=0.0
	    DO WHILE ((S.LE.0.0).OR.(S.GT.1.0))
	      U1=2.0*RANDO()-1.0
	      U2=2.0*RANDO()-1.0
	      S=U1*U1+U2*U2
            ENDDO
	    T=SQRT(-2.0*LOG(S)/S)
	    DZDX=RMSBI*U1*T
	    DZDY=RMSBI*U2*T
            DEN=SQRT(1.0D0+DZDX*DZDX+DZDY*DZDY)
            MUNOT=(INCZ-INCX*DZDX)/DEN
            MU=(EMAZ-EMAX*DZDX-EMAY*DZDY)/DEN
            INC=ACOS(MUNOT)/DEG2RAD
            EMA=ACOS(MU)/DEG2RAD
C            IF ((IRET.EQ.0).AND.(EMA.LE.EMAMAX)) THEN
               HAPKE_IMG(I,J)=PHT_HAPKE(INC,EMA,PHASE)
               INC_IMG(I,J)=INC
               EMA_IMG(I,J)=EMA
               IF (DOATM) THEN
C NOTE THAT INCDAT AND EMADAT REPLACE INC AND EMA IN ATMOSPHERE MODELS
                  IF (IATM.EQ.1) THEN
                     CALL ATM_ISO1(INCDAT,EMADAT,PHASE,
     *               PSTD,TRANS,TRANS0,SBAR)
                  ELSE IF (IATM.EQ.2) THEN
                     CALL ATM_ISO2(INCDAT,EMADAT,PHASE,
     *               PSTD,TRANS,TRANS0,SBAR)
                  ELSE IF (IATM.EQ.3) THEN
                     CALL ATM_AN11(INCDAT,EMADAT,PHASE,
     *               PSTD,TRANS,TRANS0,SBAR)
                  ELSE IF (IATM.EQ.4) THEN
                     CALL ATM_AN12(INCDAT,EMADAT,PHASE,
     *               PSTD,TRANS,TRANS0,SBAR)
                  ELSE IF (IATM.EQ.5) THEN
                     CALL ATM_HHG1(INCDAT,EMADAT,PHASE,
     *               PSTD,TRANS,TRANS0,SBAR)
                  ELSE 
                     CALL ATM_HHG2(INCDAT,EMADAT,PHASE,
     *               PSTD,TRANS,TRANS0,SBAR)
                  END IF
                  CALL R8SPLINT(INCTABLE,AHTABLE,AHTABLE2,NINC,INCDAT,
     *                          AHI)
                  MUNOT=COS(DEG2RAD*INCDAT)
                  HAPKE_IMG(I,J)=PSTD+TRANS*MUNOT*AHI/(1.0D0-AB*SBAR)
     *                          +TRANS0*(HAPKE_IMG(I,J)-AHI*MUNOT)
               END IF
C            END IF
         ENDDO
      ENDDO
      RETURN
      END


      FUNCTION LFIT_PHO_LOCAL(PAR)
      IMPLICIT NONE
C Fits a simple photometric model to the Hapke model by linear least
C squares fit at a contant given value of the limb-darkening parameter PAR.
C The RMS error of the fit is returned as the function value; the
C coefficients of the fit C0 (additive) and C1 (multiplicative) are
C returned in the common block used to pass in all the stuff that needs
C to be hidden from R8MNBRAK/R8BRENT.
C DIFFERS FROM LFIT_PHO_GLOBAL BY HAVING A DIFFERENT COMMON BLOCK OF
C HIDDEN PARAMETERS, INCLUDING STORED INC AND EMA IMAGES.
#include "pht_common.inc"
      REAL*8 PAR			! Input:  photometric parameter
      REAL*8 LFIT_PHO_LOCAL		! Output: RMS error of fit
      
      INTEGER*4 NLP,NL,NS
      PARAMETER (NLP=51)
      REAL*8 HAPKE_IMG(NLP*2-1,NLP)
C
C	OLD GLOBAL PARAMETERS
C
C Parameters controlling the range of angles over which fit is made
C      REAL*8 INCMIN,INCMAX		! Incidence angle range
C      REAL*8 EMAMIN,EMAMAX		! Emission angle range
C
C	NEW LOCAL PARAMETERS
C
      REAL*8 INC_IMG(NLP*2-1,NLP)
      REAL*8 EMA_IMG(NLP*2-1,NLP)
      REAL*8 PHASE			! Current phase angle
C Parameter controlling which empirical model is fit
      INTEGER*4 IEMP			! 0=lunar-Lambert, 1=Minnaert
C Parameter controlling whether additive term is used in fit
      INTEGER*4 IORD			! 0=no additive, 1=additive term
      REAL*8 C0,C1			! Constant and linear coefficients
					! of linear fit of empirical fn to
					! Hapke @ fixed PAR
      INTEGER*4 NOFIT			! Flag for no fit points found

C 	NOTE CHANGES FROM GLOBAL VERSION
      COMMON /LFIT_PHO_LOCAL_COM/  HAPKE_IMG,INC_IMG,EMA_IMG,NL,NS,
     *                       PHASE,IEMP,IORD,C0,C1,NOFIT


      REAL*8 SUM1		! Accumulation buffers for least-squares
      REAL*8 SUMX		! fit information: sums of products of
      REAL*8 SUMY		! abcissa and ordinate...
      REAL*8 SUMXX
      REAL*8 SUMXY
      REAL*8 SUMYY
      INTEGER*4 I,J		! Loop counters
      REAL*8 INC,EMA		! Incidence and emission angles
      REAL*8 PHT_LUNLAM,PHT_MINNAERT	! Simple photometric functions
      REAL*8 X,Y		! Here, abcissa and ordinate of fit, i.e.,
				! Hapke model value, simple model value
      REAL*8 DE			! Denominator for least squares calcs
      REAL*8 ARG

      SUM1=0.0D0
      SUMX=0.0D0
      SUMY=0.0D0
      SUMXX=0.0D0
      SUMXY=0.0D0
      SUMYY=0.0D0
      IF (IEMP.EQ.0) THEN
         LL=PAR
      ELSE
         KAY=PAR
      END IF
      BEE=1.0D0
      NOFIT=0

C Accumulate statistics
      DO J=1,NL
         DO I=1,NS
C            CALL GET_ANGLES(NL,NS,J,I,INC,EMA,PHASE,IRET)
            INC=INC_IMG(I,J)
            EMA=EMA_IMG(I,J)
C            IF ((IRET.EQ.0).AND.
C     *          (INC.GE.INCMIN).AND.
C     *          (INC.LE.INCMAX).AND.
C     *          (EMA.GE.EMAMIN).AND.
C     *          (EMA.LE.EMAMAX)) THEN
               IF (IEMP.EQ.0) THEN
                  X=PHT_LUNLAM(INC,EMA,PHASE)
               ELSE
                  X=PHT_MINNAERT(INC,EMA,PHASE)
               END IF
               Y=HAPKE_IMG(I,J)
               SUM1=SUM1+1.0D0
               SUMX=SUMX+X
               SUMY=SUMY+Y
               SUMXX=SUMXX+X*X
               SUMXY=SUMXY+X*Y
               SUMYY=SUMYY+Y*Y
C            END IF
         ENDDO
      ENDDO

C Check that some points were found so the fit can be done
      IF ((SUM1.LT.1.0D0).OR.(SUMXX.LE.0.0D0).OR.
     *    ((IORD.NE.0).AND.(SUM1*SUMXX-SUMX*SUMX.EQ.0.0D0))) THEN
         NOFIT=-1
         RETURN
      END IF

C Evaluate coefficients and RMS error of the linear least-squares fit
      IF (IORD.EQ.0) THEN
         C0=0.0D0
         C1=SUMXY/SUMXX
         ARG=(SUMYY-2.0D0*C1*SUMXY+C1*C1*SUMXX)/SUM1
         IF (ARG.GT.0.0D0) THEN
            LFIT_PHO_LOCAL=SQRT(ARG)
         ELSE
            LFIT_PHO_LOCAL=0.0D0
         END IF
      ELSE
         DE=SUM1*SUMXX-SUMX*SUMX
         C0=(SUMXX*SUMY-SUMX*SUMXY)/DE
         C1=(SUM1*SUMXY-SUMX*SUMY)/DE
         ARG=(SUMYY+2.0D0*(C0*C1*SUMX-C0*SUMY-C1*SUMXY)
     *             +C0*C0*SUM1+C1*C1*SUMXX)/SUM1
         IF (ARG.GT.0.0D0) THEN
            LFIT_PHO_LOCAL=SQRT(ARG)
         ELSE
            LFIT_PHO_LOCAL=0.0D0
         END IF
      END IF
      RETURN
      END
