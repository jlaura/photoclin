      SUBROUTINE XCMAIN_GUI(IDUM,N10,NX0,NY0,IACTION,SORDIR,AEKNOWN,
     *N1LMAG,N2LMAG,N4LMAG,NXLMAG,NYLMAG,WRTIMG,MAG,MINMAG,
     *RES,AZINC,FASTINJ,RET)
      IMPLICIT NONE
C
C_TITLE XCMAIN_GUI - Driver routine for iterations of clinometry algorithm
C
#include "clinom_aepar.inc"
#include "clinom_dnorm.inc"
#include "clinom_ipars.inc"
#include "clinom_log.inc"
#include "clinom_mem.inc"
#include "clinom_ops.inc"
#include "clinom_msg.inc"
C_ARGS  TYPE        VARIABLE        I/O  DESC
        INTEGER*4   IDUM(1)         ! B  Dummy array containing all
C                                        arrays to be used
        INTEGER*4   N10             ! I  Pointer to beginning of IDUM
        INTEGER*4   NX0             ! B  Sample dimension
        INTEGER*4   NY0             ! B  Line dimension
	INTEGER*4   IACTION         ! I  The action to be taken by this
C                                        program:
C                                        0 - Initialize
C				         1 - Increase resolution
C                                        2 - Decrease resolution
C                                        3 - Boxcar filter
C                                        4 - Perform SOR or Direct iteration
	INTEGER*4   SORDIR          ! I  Type of iteration to perform:
C                                        0 - SOR
C				         1 - DIR
C                                        2 - CGM
        LOGICAL*1   AEKNOWN         ! B
	INTEGER*4   N1LMAG
	INTEGER*4   N2LMAG
	INTEGER*4   N4LMAG
	INTEGER*4   NXLMAG
	INTEGER*4   NYLMAG
	LOGICAL*1   WRTIMG
	INTEGER*4   MAG             ! O  Current resolution magnitude
	INTEGER*4   MINMAG          ! O  Minimum resolution magnitude
	REAL*8      RES
	REAL*8      AZINC
	LOGICAL*1   FASTINJ         ! I
	INTEGER*4   RET             ! O  Error code
C
C_DESC This is a driver routine to call the routines for first and subsequent
C      iterations of the clinometry algorithm:
C      simultaneous solution of clinometry equations using iterative
C      matrix approach (Newton-Raphson solution of the nonlinear set
C      of equations; constraint of minimum rms gradient applied by the penalty
C      method to remove underdetermination).  Matrix solution is by
C      successive over-relaxation (or by the incomplete Cholesky-conjugate
C      gradient method, or by LU decomposition of the banded matrix if the
C      image is small enough). First iteration can be done by exact factorization
C      of the least-squares matrix, which is row/column seperable, giving
C      the minimum-rms-Z solution for the linearized scattering function.
C      This may require temporary rotation of the image, as it only works
C      when the characteristic direction of the equations is a cardinal
C      direction of the the image array.
C
C_HIST  Dec 10 1996 Randolph Kirk, USGS, Flagstaff - Original Version
C       May 06 1998 Tracie Sucharski, Converted to UNIX/ISIS
C
C_END


      LOGICAL*1   ANSWER
      LOGICAL*1   DZEXIST

      CHARACTER   TITLE*80,TYPELONG*17,TYPESHORT*3
      CHARACTER   DEFNAME*80,DEFEXT*80,DEFVER*80,CC*4
      CHARACTER   XNAME*5
      CHARACTER   STR1*256,STR2*256

      INTEGER*4   MAXLMMAX
      PARAMETER   (MAXLMMAX=10)
      INTEGER*4   N1(0:MAXLMMAX),N2(0:MAXLMMAX),N3(0:MAXLMMAX)
      INTEGER*4   N4,N5,N6,N7,N8,N9,N8D,N9D

      INTEGER*4   NX(0:MAXLMMAX),NY(0:MAXLMMAX)
      INTEGER*4   ICHECK,JCHECK,LMAG
      INTEGER*4   NB,NX1,NY1,NZ
      INTEGER*4   LMMAX,IERR,NI,NC,MODE,NXB,NYB
      INTEGER*4   NXOUT,NYOUT,IRET
      INTEGER*4   INVCNT

      REAL*4      ERMS(0:MAXLMMAX),TAURMS(0:MAXLMMAX)
      REAL*4      FMAG,W2,T0,XFRC,XMIN,XMLT,DNSCALE
      REAL*4      T1,FOPS,DBRMS,ZRMS,XNORM,XADB,TOL,STD0
      REAL*4      BSNORM
      REAL*4      SECCNDS
      REAL*4      SCLHT
      REAL*4      DIP
      REAL*4      DIPAZ
      REAL*4      SUNDIP
      REAL*4      RMSDN
      REAL*4      RMSPIX
      REAL*4      RMSM

      EXTERNAL PBDER

      DATA LMAG,FMAG,W2 /0,1.0,0.5/
      DATA DEFNAME,DEFEXT,DEFVER,CC /'PCP','LOG',' ','LIST'/
      DATA XNAME/'PHOTO'/

      COMMON /CHECK/ NX1,NY1,ICHECK,JCHECK

      RET = 0
      WRTIMG = .FALSE.
      DZEXIST = .FALSE.

C
C Initialize variables and data in preparation for doing
C photoclinometry
C
      IF (IACTION.EQ.0) THEN
	ICHECK=0
	JCHECK=0
        NB=NX0*NY0
        N1(0)=N10
        NX(0)=NX0
        NY(0)=NY0
        NX1=NX0+1
        NY1=NY0+1
        NZ=NX1*NY1
        N2(0)=N1(0)+NB
        N3(0)=N2(0)+NZ
        N4=N3(0)+NZ
        N5=N4+NZ
        N6=N5+NZ
        N7=N6+5*NZ
        N8=N7+5*NZ
        N9=N8+NZ
        N8D=N7+NZ*(2*NX1+3)
        N9D=N8D+NZ*(NX1+1)
        CALL U_FILL4(0.0,MAXLMMAX+1,ERMS(0))
        CALL U_FILL4(0.0,MAXLMMAX+1,TAURMS(0))
        PRINT *, MAXLMMAX, ERMS(0), TAURMS(0)
        LMMAX=0
        DO WHILE ((NX(LMMAX).GE.32).AND.(NY(LMMAX).GE.32).AND.
     *            (LMMAX.LT.DEPTHLIM).AND.(LMMAX.LT.MAXLMMAX))
           LMMAX=LMMAX+1
           NX(LMMAX)=NX(LMMAX-1)/2
           NY(LMMAX)=NY(LMMAX-1)/2
        ENDDO
	IF (IGUESS.EQ.1) THEN
	  TYPELONG='ALTITUDES ZEROED.'
	  TYPESHORT='FLT'
        ELSEIF (IGUESS.EQ.2) THEN
	  TYPELONG='ALTITUDES SCALED.'
	  TYPESHORT='STO'
        ELSE
	  TYPELONG='SSIPSF-PI DONE.  '
	  TYPESHORT='SEP'
        ENDIF
        NI=0
        MODE=1
C Mask off limbs if using spherical datum
        CALL BMIX(IDUM(N1(0)),NX(0),NY(0))
        CALL AE1CALC(IDUM(N1(0)),IDUM(N2(0)),IDUM(N3(0)),
     *  IDUM(N6),NX(0),NY(0),ERMS(0),DBRMS,ZRMS,MODE,PBDER)
        WRITE (STR1,1000) TYPELONG,ERMS(0),DBRMS,
     *   ZRMS/ASPECT
 1000   FORMAT(A17,' RMS RHS ',F14.7,
     *  ', RMS B ERR ',F14.7,', RMS Z ',F14.7)
        CALL PCPLOG(STR1,1)
	WRITE (STR1,'(F14.7,A)') ERMS(0),','
	WRITE (STR2,'(F14.7)') DBRMS
	CALL HINTERROR(STR1,STR2,2)
        FOPS=NOPS*1.0E-6
        IF (LOGIT) THEN
	  WRITE (IULOG,'(I3,1X,F9.4,1X,A3,1X,F8.5,
     *    1X,F17.8,1X,F15.8,1X,F13.6)') 
     *    0,FOPS,TYPESHORT,CONTRAST,ERMS(0),DBRMS,
     *    ZRMS/ASPECT
        ENDIF
	WRITE (STR1,'(I3,1X,F9.2,1X,A3,1X,F8.5,
     *  1X,F17.8,1X,F15.8,1X,F13.6)') 
     *  0,FOPS,TYPESHORT,CONTRAST,ERMS(0),DBRMS,
     *  ZRMS/ASPECT
        CALL PCPLOG(STR1,0)
        AEKNOWN=.TRUE.
	SCLHT=FMAG*RES*1000.0
	CALL FIT2DIP(IDUM(N2(LMAG)),NX(LMAG)+1,NY(LMAG)+1,SCLHT,
     *    SCLHT,ASPECT,AZINC,CHARAZ,DIP,DIPAZ,SUNDIP,RMSDN,
     *    RMSPIX,RMSM)
        WRITE(STR1,'(F12.7,A,F12.7)') DIP,',',DIPAZ
	CALL HINTERROR(STR1,'',3)
	WRITE(STR1,'(F12.7)') SUNDIP
	CALL HINTERROR(STR1,'',4)
	WRITE(STR1,'(F14.7,A)') RMSM,','
	WRITE(STR2,'(F14.7)') RMSPIX
	CALL HINTERROR(STR1,STR2,5)
C
C Increase resolution
C
      ELSEIF (IACTION.EQ.1) THEN
        IF (LMAG.EQ.0) THEN
	  PCPMSG = 'RESOLUTION IS ALREADY AT THE MAXIMUM.'
	  CALL PCPMESSAGE(PCPMSG)
        ELSE
          CALL INTERP(IDUM(N2(LMAG-1)),NX(LMAG-1)+1,
     *NY(LMAG-1)+1,IDUM(N2(LMAG)),NX(LMAG)+1,NY(LMAG)+1,2.0)
          IF (LMAG.EQ.1) THEN
            IF (TAURMS(2).NE.0.0) THEN
              TAURMS(0)=TAURMS(1)*TAURMS(1)/TAURMS(2)
            ELSE
              TAURMS(0)=TAURMS(1)*2.0
            END IF
          END IF
	  WRTIMG=.TRUE.
   30     CONTINUE	! JUMP TO HERE IF DECREASING RES FAILED
          LMAG=LMAG-1
          NB=NX(LMAG)*NY(LMAG)
          NX1=NX(LMAG)+1
          NY1=NY(LMAG)+1
          NZ=NX1*NY1
          N4=N3(LMAG)+NZ
          N5=N4+NZ
          N6=N5+NZ
          N7=N6+5*NZ
          N8=N7+5*NZ
          N9=N8+NZ
          N8D=N7+NZ*(2*NX1+3)
          N9D=N8D+NZ*(NX1+1)
          FMAG=REAL(2**LMAG)
          CALL GEOMSET(FMAG)
          AEKNOWN=.FALSE.
        END IF
C
C Decrease resolution
C
      ELSEIF (IACTION.EQ.2) THEN
        IF (LMAG.EQ.LMMAX) THEN
	  PCPMSG = 'RESOLUTION IS ALREADY AT THE MINIMUM.'
	  CALL PCPMESSAGE(PCPMSG)
        ELSE
          IF (.NOT.AEKNOWN) CALL AE1CALC(IDUM(N1(LMAG)),
     *IDUM(N2(LMAG)),IDUM(N3(LMAG)),IDUM(N6),NX(LMAG),NY(LMAG),
     *ERMS(LMAG),DBRMS,ZRMS,0,PBDER)
          LMAG=LMAG+1
          NB=NX(LMAG)*NY(LMAG)
          NX1=NX(LMAG)+1
          NY1=NY(LMAG)+1
          NZ=NX1*NY1
          N1(LMAG)=N3(LMAG-1)+(NX(LMAG-1)+1)*(NY(LMAG-1)+1)
          N2(LMAG)=N1(LMAG)+NB
          N3(LMAG)=N2(LMAG)+NZ
          N4=N3(LMAG)+NZ
          N5=N4+NZ
          N6=N5+NZ
          N7=N6+5*NZ
          N8=N7+5*NZ
          N8D=N7+NZ*(2*NX1+3)
          N9D=N8D+NZ*(NX1+1)
          CALL INJECT(LMAG,FASTINJ,IDUM(N1(LMAG-1)),IDUM(N2(LMAG-1)),
     *IDUM(N3(LMAG-1)),NX(LMAG-1),NY(LMAG-1),IDUM(N1(LMAG)),
     *IDUM(N2(LMAG)),IDUM(N3(LMAG)),IDUM(N4),IDUM(N5),IDUM(N4),
     *IDUM(N3(LMAG)),IDUM(N6),IDUM(N7),IDUM(N8),NX(LMAG),
     *NY(LMAG),ERMS(LMAG),DBRMS,ZRMS,TAURMS(LMAG),PBDER,IERR)
          IF (IERR.NE.0) THEN
	    PCPMSG = 'CANNOT REDUCE RESOLUTION.'
	    CALL PCPMESSAGE(PCPMSG)
            LMMAX=LMAG-1
            GO TO 30
          END IF
          AEKNOWN=.TRUE.
	  WRTIMG=.TRUE.
          NI=-NI
        END IF
C
C Perform a boxcar filter
C
      ELSEIF (IACTION.EQ.3) THEN
        NXB=3
        NYB=3
        XFRC=0.0
        XMIN=0.0
        XMLT=1.0
        CALL FASTFIL32(IDUM(N2(LMAG)),IDUM(N3(LMAG)),IDUM(N4),
     *IDUM(N5),IDUM(N6),NY(LMAG)+1,NX(LMAG)+1,NYB,NXB,XFRC,
     *XMIN,XNORM,XADB,XMLT,TOL,STD0,'LPF',IRET)
        CALL U_MOVE4((NX(LMAG)+1)*(NY(LMAG)+1),
     *IDUM(N3(LMAG)),IDUM(N2(LMAG)))
        AEKNOWN=.FALSE.
	WRTIMG=.TRUE.
C
C Perform SOR or Direct iteration
C
      ELSEIF (IACTION.EQ.4) THEN
        IF ((N9D.GT.NMAX.AND.SORDIR.EQ.1).OR.(SORDIR.EQ.0.AND.
     *((N7.GT.NMAX).OR.((WMAX.LT.0.0).AND.
     *(N8.GT.NMAX)))).OR.(SORDIR.EQ.2.AND.N9.GT.NMAX)) THEN
          PCPMSG = 'REQUESTED OPERATION EXCEEDS AVAILABLE MEMORY.'
	  CALL PCPMESSAGE(PCPMSG)
        ELSE
	  IF (SORDIR.EQ.2) THEN
            CALL XCNEWT_GUI(IDUM(N1(LMAG)),IDUM(N2(LMAG)),
     *IDUM(N3(LMAG)),IDUM(N4),IDUM(N5),IDUM(N6),IDUM(N7),
     *IDUM(N8),IDUM(N8),IDUM(N8),NX(LMAG),NY(LMAG),NI,LMAG,
     *AEKNOWN,ERMS(LMAG),DBRMS,ZRMS,TAURMS(LMAG),ERMS(LMAG-1),
     *'CGM',PBDER,WRTIMG,RES,AZINC)
	  ELSEIF (SORDIR.EQ.1) THEN
            CALL XCNEWT_GUI(IDUM(N1(LMAG)),IDUM(N2(LMAG)),
     *IDUM(N3(LMAG)),IDUM(N4),IDUM(N5),IDUM(N6),IDUM(N6),
     *IDUM(N6),IDUM(N7),IDUM(N8D),NX(LMAG),NY(LMAG),NI,LMAG,
     *AEKNOWN,ERMS(LMAG),DBRMS,ZRMS,TAURMS(LMAG),ERMS(LMAG-1),
     *'DIR',PBDER,WRTIMG,RES,AZINC)
          ELSE
            CALL XCNEWT_GUI(IDUM(N1(LMAG)),IDUM(N2(LMAG)),
     *IDUM(N3(LMAG)),IDUM(N4),IDUM(N5),IDUM(N6),IDUM(N6),
     *IDUM(N7),IDUM(N7),IDUM(N7),NX(LMAG),NY(LMAG),NI,
     *LMAG,AEKNOWN,ERMS(LMAG),DBRMS,ZRMS,TAURMS(LMAG),
     *ERMS(LMAG-1),'SOR',PBDER,WRTIMG,RES,AZINC)
	  ENDIF
          IF (WRTIMG) DZEXIST = .TRUE.
        ENDIF
      ENDIF

      N1LMAG=N1(LMAG)-1
      N2LMAG=N2(LMAG)-1
      N4LMAG=N4-1
      NXLMAG=NX(LMAG)
      NYLMAG=NY(LMAG)
      MAG=LMAG
      MINMAG=LMMAX
      IF (.NOT.DZEXIST) THEN
        CALL U_FILL4(0.0,(NXLMAG+1)*(NYLMAG+1),IDUM(N4))
      ENDIF

      RETURN
      END
