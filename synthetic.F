      SUBROUTINE SYNTHETIC(IMGNAME,IMAGE,Z,NX,NY,DNDATUM,DNATM,
     *FMAG,IFID)
      IMPLICIT NONE
C
C_TITLE	SYNTHETIC - Create a real image from a topo array
C
#include "clinom_aepar.inc"
#include "clinom_datum.inc"
#include "clinom_msg.inc"
#include "clinom_ops.inc"
#include "clinom_rgeom.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        CHARACTER       IMGNAME*(*)     !I      Image file name
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4	        IMAGE(NX)	!I	Image
	REAL*4		Z(NX+1,NY+1)	!I	Topographic array
	REAL*4          DNDATUM         !I      Factor to multiply 
C                                               scaled brightnesses by
	REAL*4		DNATM		!I	Offset to add to scaled
C						brightnesses
        REAL*4          FMAG            !I      Magnification of image
	INTEGER*4       IFID            !I      File id of unmagnified
C                                               image
C
C_DESC	Calculates model brightnesses from the topographic array Z,
C	scales them, and writes them to IMGNAME.  The geometric
C	and photometric parameters for the image must be initialized
C	elsewhere.
C
C_CALLS	DATUM (CLINOM.OLB)
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_END
C
      INTEGER*4 I,J		! Loop counters
      REAL*4 Z01,Z02,Z03,Z04	! The elevations at the 4 corners of a
C				  given pixel (element), ON THE DATUM
      REAL*4 DZ1,DZ2		! Diagonal elevation differences
      REAL*4 BE			! Model brightness
      REAL*4 DB1,DB2		! Brightness derivatives (not used)
      REAL*4 SCALE              ! Scale factor to multiply model
C                                 brightness by
      INTEGER*4 FID
      INTEGER*4 CORSIZ(3)
      INTEGER*4 SUFSIZ(3)
      INTEGER*4 ITEM_BYTES
      CHARACTER*30 ITEM_TYPE
      CHARACTER*32 AXNAME(3) /'SAMPLE','LINE','BAND'/
      REAL*4 OSCALE(2)
      INTEGER*4 LIMITS(2,3)
      INTEGER*4 NVALS
      INTEGER*4 RET
      INTEGER*4 MODE

C
C Remove old synthetic image and create a new file
C
      FID = 0
      CALL Q_OPEN(FID,0,IMGNAME,2,0,' ',0,1,0,RET)
      IF (RET.NE.0) GOTO 9000
      CALL Q_CLOSE(FID,2,RET)
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
      FID = 0
      CALL Q_SET_SYS_KEYS(FID,3,1,CORSIZ,SUFSIZ,AXNAME,
     .               ITEM_BYTES,ITEM_TYPE,OSCALE,RET)
      IF (RET.NE.0) GOTO 9000
      CALL Q_OPEN(FID,IFID,IMGNAME,3,0,' ',0,3,0,RET)
      IF (RET.NE.0) GOTO 9000
      CALL MAP_FIXGRP(IFID,FID,1,1,FMAG,FMAG,0,RET)
c      NVALS=1
c      CALL P_SET_LIT_KEY(FID,'QUBE',' ','CORE_NAME',1,NVALS,
c     .                   'RAW_DATA_NUMBER',15,RET)
c      IF (RET.NE.0) GOTO 9000
c      NVALS=1
c      CALL P_SET_LIT_KEY(FID,'QUBE',' ','CORE_UNIT',1,NVALS,
c     .                   'NONE',4,RET)
c      IF (RET.NE.0) GOTO 9000
c      NVALS=1
c      CALL P_SET_INT_KEY(FID,'QUBE','BAND_BIN',
c     .        'BAND_BIN_ORIGINAL_BAND',1,NVALS,1,RET)
c      IF (RET.NE.0) GOTO 9000
c      NVALS=1
c      CALL P_SET_LIT_KEY(FID,'QUBE','BAND_BIN',
c     .        'BAND_BIN_UNIT',1,NVALS,'NONE',4,RET)
c      IF (RET.NE.0) GOTO 9000
c      NVALS=1
c      CALL P_SET_REAL_KEY(FID,'QUBE','BAND_BIN',
c     .        'BAND_BIN_CENTER',1,NVALS,1.0,1,1,RET)
c      IF (RET.NE.0) GOTO 9000

C
C Create each line of the synthetic image and write it to
C the image file
C
      DO J=1,NY
         DO I=1,NX
            DZ1=Z(I  ,J  )-Z(I+1,J+1)
            DZ2=Z(I  ,J+1)-Z(I+1,J  )
            IF (DATUMTYP.EQ.2) THEN
               CALL DATUM(I,J,4,Z01,Z02,Z03,Z04)
               DZ1=DZ1+Z03-Z01
               DZ2=DZ2+Z02-Z04
            ELSE
               DZ1=DZ1-DZ10
               DZ2=DZ2-DZ20
            END IF
	    MODE=0
            CALL PBDER(I,J,DZ1,DZ2,BE,DB1,DB2,MODE)
            IMAGE(I)=BE/BNORM
         ENDDO
         LIMITS(1,1) = 1
	 LIMITS(2,1) = NX
	 LIMITS(1,2) = J
	 LIMITS(2,2) = J
	 LIMITS(1,3) = 1
	 LIMITS(2,3) = 1
	 CALL Q_LIO_CBRICK(FID,2,LIMITS,IMAGE,RET)
	 IF (RET.NE.0) GOTO 9000
      ENDDO
      CALL Q_CLOSE(FID,1,RET)
      IF (RET.NE.0) GOTO 9000
      RETURN
C 
C Error messages
C
9000  PCPMSG = 'UNABLE TO CREATE SYNTHETIC IMAGE.'
      CALL PCPMESSAGE(PCPMSG)
      RETURN
      END
