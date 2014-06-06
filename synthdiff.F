      SUBROUTINE SYNTHDIFF(DIFNAME,SYNNAME,IMAGE,WORK,NX,NY,
     *FMAG,IFID)
      IMPLICIT NONE
C
C_TITLE	SYNTHDIFF - Subtract synthetic image from target image
C
#include "clinom_aepar.inc"
#include "clinom_datum.inc"
#include "clinom_msg.inc"
#include "clinom_ops.inc"
#include "clinom_rgeom.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        CHARACTER       DIFNAME*(*)     !I      Difference image name
        CHARACTER       SYNNAME*(*)     !I      Synthetic image name
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4	        IMAGE(NX,NY)	!I	Target image buffer
	REAL*4		WORK(NX)	!I	Difference array
	REAL*4          FMAG            !I      Magnification of image
	INTEGER*4       IFID            !I      File id of unmagnified
C                                               image
C
C_DESC	Subtracts synthetic image from target image and puts 
C       results in the file DIFNAME.
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_END
C
      INTEGER*4 I,J		! Loop counters
      INTEGER*4 FID1,FID2
      INTEGER*4 CORSIZ(3)
      INTEGER*4 SUFSIZ(3)
      INTEGER*4 ITEM_BYTES
      CHARACTER*30 ITEM_TYPE
      CHARACTER*32 AXNAME(3) /'SAMPLE','LINE','BAND'/
      REAL*4 OSCALE(2)
      INTEGER*4 LIMITS(2,3)
      INTEGER*4 NVALS
      INTEGER*4 AXES,ORDER
      INTEGER*4 RET

C
C Remove old difference image and create a new file
C
      FID1 = 0
      CALL Q_OPEN(FID1,0,DIFNAME,2,0,' ',0,1,0,RET)
      IF (RET.NE.0) GOTO 9000
      CALL Q_CLOSE(FID1,2,RET)
      IF (RET.NE.0) GOTO 9000
      CORSIZ(1) = NX
      CORSIZ(2) = NY
      CORSIZ(3) = 1
      SUFSIZ(1) = 0
      SUFSIZ(2) = 0
      SUFSIZ(3) = 0
      CALL U_TYPE_KEYS(3,ITEM_BYTES,ITEM_TYPE,RET)
      IF (RET.NE.0) GOTO 9000
      OSCALE(1) = 0.0
      OSCALE(2) = 1.0
      FID1 = 0
      CALL Q_SET_SYS_KEYS(FID1,3,1,CORSIZ,SUFSIZ,AXNAME,
     .               ITEM_BYTES,ITEM_TYPE,OSCALE,RET)
      IF (RET.NE.0) GOTO 9000
      CALL Q_OPEN(FID1,IFID,DIFNAME,3,0,' ',0,3,0,RET)
      IF (RET.NE.0) GOTO 9000
      CALL MAP_FIXGRP(IFID,FID1,1,1,FMAG,FMAG,0,RET)
c      NVALS=1
c      CALL P_SET_LIT_KEY(FID1,'QUBE',' ','CORE_NAME',1,NVALS,
c     .                   'RAW_DATA_NUMBER',15,RET)
c      IF (RET.NE.0) GOTO 9000
c      NVALS=1
c      CALL P_SET_LIT_KEY(FID1,'QUBE',' ','CORE_UNIT',1,NVALS,
c     .                   'NONE',4,RET)
c      IF (RET.NE.0) GOTO 9000
c      NVALS=1
c      CALL P_SET_INT_KEY(FID1,'QUBE','BAND_BIN',
c     .        'BAND_BIN_ORIGINAL_BAND',1,NVALS,1,RET)
c      IF (RET.NE.0) GOTO 9000
c      NVALS=1
c      CALL P_SET_LIT_KEY(FID1,'QUBE','BAND_BIN',
c     .        'BAND_BIN_UNIT',1,NVALS,'NONE',4,RET)
c      IF (RET.NE.0) GOTO 9000
c      NVALS=1
c      CALL P_SET_REAL_KEY(FID1,'QUBE','BAND_BIN',
c     .        'BAND_BIN_CENTER',1,NVALS,1.0,1,1,RET)
c      IF (RET.NE.0) GOTO 9000

C
C Open the synthetic image file
C
      FID2 = 0
      CALL Q_OPEN(FID2,0,SYNNAME,1,0,' ',0,0,0,RET)
      IF (RET.NE.0) GOTO 9000
      CALL Q_CHECK_STD(FID2,RET)
      IF (RET.NE.0) GOTO 9000
      CALL Q_GET_SYS_KEYS(FID2,1,AXES,ORDER,CORSIZ,SUFSIZ,
     .       AXNAME,ITEM_BYTES,ITEM_TYPE,OSCALE,RET)
      IF (RET.NE.0) GOTO 9000

C
C Create each line of the difference image and write it to
C the difference file
C
      DO J=1,NY
         LIMITS(1,1) = 1
	 LIMITS(2,1) = NX
	 LIMITS(1,2) = J
	 LIMITS(2,2) = J
	 LIMITS(1,3) = 1
	 LIMITS(2,3) = 1
	 CALL Q_LIO_CBRICK(FID2,1,LIMITS,WORK,RET)
	 IF (RET.NE.0) GOTO 9000
         DO I=1,NX
            WORK(I)=IMAGE(I,J)/BNORM-WORK(I)
         ENDDO
         LIMITS(1,1) = 1
	 LIMITS(2,1) = NX
	 LIMITS(1,2) = J
	 LIMITS(2,2) = J
	 LIMITS(1,3) = 1
	 LIMITS(2,3) = 1
	 CALL Q_LIO_CBRICK(FID1,2,LIMITS,WORK,RET)
	 IF (RET.NE.0) GOTO 9000
      ENDDO
      CALL Q_CLOSE(FID1,1,RET)
      IF (RET.NE.0) GOTO 9000
      CALL Q_CLOSE(FID2,1,RET)
      IF (RET.NE.0) GOTO 9000
      RETURN
C 
C Error messages
C
9000  PCPMSG = 'UNABLE TO CREATE DIFFERENCE IMAGE.'
      CALL PCPMESSAGE(PCPMSG)
      RETURN
      END
