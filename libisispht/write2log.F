      SUBROUTINE WRITE2LOG(FLG,INS,INL,NOTE,LNOTE,FROM,CLINC,
     *  AZINC,CLEMI,AZEMI,RES,DIP,AZ,CHARAZGRID,RADIUS,DNDATUM,
     *  ASPECT)
      IMPLICIT NONE
C
C_TITLE	WRITE2LOG - Write info to log file
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4       FLG             !I      
	INTEGER*4	INS             !I	Number of samples in image
	INTEGER*4	INL             !I	Number of lines in image
	CHARACTER	NOTE*(*)	!I	Note to go in LOGFILE, 
	INTEGER*4	LNOTE		!I	Length of NOTE
	CHARACTER       FROM*(*)        !I      Name of image
	REAL*4          CLINC           !I
	REAL*8          AZINC           !I
	REAL*8          CLEMI           !I
	REAL*8          AZEMI           !I
	REAL*8          RES             !I
	REAL*4          DIP             !I
	REAL*4          AZ              !I
	REAL*4          CHARAZGRID      !I
	REAL*4          RADIUS          !I
	REAL*4          DNDATUM         !I
	REAL*4          ASPECT          !I
C
C_DESC	Writes information to the log file
C
C_HIST	July 25 2001 Janet Barrett U.S.G.S. Flagstaff Original Version
C
C_END
C
#include "clinom_log.inc"
#include "clinom_ipars.inc"
#include "clinom_ppars.inc"
#include "clinom_psnpar.inc"
#include "clinom_dnorm.inc"
#include "clinom_datum.inc"
      INTEGER*4 ERRVAL      ! Error code
      CHARACTER ERRKEY*17   ! Keyword value for error message
      CHARACTER ERRMSG*80   ! Error message
      CHARACTER STR*132     ! String for reporting information
      INTEGER*4 IERR
      INTEGER*4 U_SLEN

      IF (LOGIT) THEN
        IF (FLG.EQ.1) THEN
          WRITE (IULOG,'(A,A,/,A,I4,A,I4,A,F9.0,A,/,A,I3)')
     *     'NOTE: ',NOTE(1:LNOTE),'Image is ',INS,' by ',INL,
     *     ', penalty #',1.0/AL,'  Penalty is area squared.',
     *     'Penalty applied with respect to datum of type ',
     *     DATUMTYP
          WRITE (IULOG,'(A,A,/,A,F9.4,A,F9.4,A,/,
     *     A,F9.4,A,F9.4,A,/,A,F9.4,A,1PG14.7,A,/,A,G14.7,A)')
     *     'Monoscopic image: ',FROM(1:U_SLEN(FROM)),
     *     'Sun colatitude ',CLINC,
     *     ' deg. Sun azimuth ',AZINC,
     *     ' deg.','Emission colatitude ',CLEMI,
     *     ' deg. Emission azimuth ',AZEMI,
     *     ' deg.','Phase angle ',PHASE(1),
     *     ' deg. Resolution ',1000.0*RES,'m/pixel.',
     *     'Atmospheric contribution to DN given as ',
     *     DNATM,'.'
	  IF (PHOFUNC.EQ.'HAPH_S') THEN
            WRITE (IULOG,'(A,1PG14.7,A,G14.7,A,G14.7,A)')
     *        'Smooth Hapke-Henyey photom. function with WH=',PWH,
     *        ', HG1=',PHG1,', HG2=',PHG2,'.'
	  ELSEIF (PHOFUNC.EQ.'HAPL_S') THEN
            WRITE (IULOG,'(A,1PG14.7,A,G14.7,A,G14.7,A)')
     *        'Smooth Hapke-Legendre photom. function with WH=',PWH,
     *        ', BH=',PBH,', CH=',PCH,'.'
          ELSEIF (PHOFUNC.EQ.'HAPHEN') THEN
            WRITE (IULOG,'(A,1PG14.7,A,G14.7,A,/,
     *        A,1PG14.7,A,G14.7,A,/,A,1PG14.7,A,G14.7,A)')
     *        'Rough Hapke-Henyey photom. function with WH=',PWH,
     *        ', THETA=',PTHETA,', HH=',PHH,', B0=',PB0,
     *        ', HG1=',PHG1,', HG2=',PHG2,'.'
          ELSEIF (PHOFUNC.EQ.'HAPLEG') THEN
            WRITE (IULOG,'(A,1PG14.7,A,G14.7,A,/,
     *        A,1PG14.7,A,G14.7,A,/,A,1PG14.7,A,G14.7,A)')
     *        'Rough Hapke-Legendre photom. function with WH=',PWH,
     *        ', THETA=',PTHETA,', HH=',PHH,', B0=',PB0,
     *        ', BH=',PBH,', CH=',PCH,'.'
          ELSEIF (PHOFUNC.EQ.'LAMBER') THEN
	    WRITE (IULOG,'(A,1PG14.7,A,G14.7,A)')
     *       'Lambert photom. function with K=',
     *       PEX,', L=',0.5*PL2,'.'
          ELSEIF (PHOFUNC.EQ.'LOMSEL') THEN
	    WRITE (IULOG,'(A,1PG14.7,A,G14.7,A)')
     *       'Lommel-Seeliger photom. function with K=',
     *       PEX,', L=',0.5*PL2,'.'
          ELSEIF (PHOFUNC(1:3).EQ.'MIN') THEN
	    WRITE (IULOG,'(A,1PG14.7,A,G14.7,A)')
     *       'Minnaert photom. function with K=',
     *       PEX,', L=',0.5*PL2,'.'
          ELSEIF (PHOFUNC.EQ.'LUNLAM') THEN
	    WRITE (IULOG,'(A,1PG14.7,A,G14.7,A)')
     *       'Lunar-Lambert photom. function with K=',
     *       PEX,', L=',0.5*PL2,'.'
          ENDIF
	  WRITE (IULOG,'(A,1PG11.4,A,0PF9.4,A,F9.4,A,/,A,
     *     F9.4,A,F9.4,A)')
     *     'Datum Z=',0.0,' at center, dip ',DIP,
     *     ' deg at azimuth ',AZ,' deg.',
     *     'Azimuth of characteristics ',CHARAZGRID,
     *     '.   Aspect ratio ',ASPECT,'.'
          IF (DATUMTYP.EQ.2) WRITE (IULOG,
     *     '(A,1PG11.4,A,G14.7,A,G14.7,A)')
     *     'Datum radius ',RADIUS,' pixels, center at sample ',
     *     X0,' line ',Y0,'.'
        ELSEIF (FLG.EQ.2) THEN
	  WRITE (IULOG,'(A,1PG14.7,A,//,A,A,/,A)')
     *     'DN of datum given as ',DNDATUM,'.',
     *     'IT#   M OPS   MAG  CONTRST        ',
     *     'RMS  E           RMS DB           RMS  Z',
     *     'C_END'
        ENDIF
      ENDIF
      RETURN
C
C Handle processing errors
C
9000  CONTINUE
      ERRKEY = 'XCTAE_FILES-TAE'
      ERRMSG = 'Error reading TAE parameters from user input'
      ERRVAL = -1
      GOTO 9999
C
C Write an error message
C
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,1)
      RETURN
      END
