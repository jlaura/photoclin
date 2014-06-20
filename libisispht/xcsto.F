      SUBROUTINE XCSTO(ZIN,Z,NX,NY,ZBUF,INS,INL,IOCT,ERRVAL)
      IMPLICIT NONE
C
C_TITLE	XCSTO - Fetch a topographic estimate stored in a 32-bit image
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        CHARACTER	ZIN*(*)		!I	Name of file to read
        INTEGER*4	NX,NY		!I	Sample, line dimensions
C						of image in internal
C						orientation
	REAL*4		Z(NX+1,NY+1)	!O	Topographic array in
C						internal orientation
        INTEGER*4	INS,INL		!I	Sample, line dimensions
C						of image in external
C						orientation
	REAL*4		ZBUF(INS+1,INL+1)!O	Topographic array in
C						external orientation
	INTEGER*4	IOCT		!I	Which 45 deg "octant"
C						characteristics of the
C						external image lie in; topo
C						will be rotated/flopped
C						to put characteristics
C						in octant 1
	INTEGER*4	ERRVAL		!O	+ve=error state
C						  0=no error
C
C_DESC	Reads a 32-bit image containing an estimate of topography at
C	pixel corners and puts it in the array Z.  Depending on IOCT
C	the data are rotated and/or flopped to put the azimuth of
C	characteristics in the corresponding image between 0 and 45 deg.
C
C_FILE	ZIN (optional)---32-bit PICS file containing digital topographic
C	   model in "raw" form: elevations at corners of the image pixels,
C	   in units of pixel width, datum subtracted.  Opened/read/
C	   closed with FILEIO; labels checked with DISKIO.
C
C_CALLS	DISKIO,FILEIO,ERRMES (PICS)
C
C_HIST	23-Oct-89 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_END
C
#include "clinom_specpix.inc"
      INTEGER*4 I,J		! Loop counters; position in Z
      INTEGER*4 FID3
      CHARACTER*32 ERRKEY
      CHARACTER*80 ERRMSG
      INTEGER*4 RET
      INTEGER*4 AXES
      INTEGER*4 ORDER
      INTEGER*4 CORE_SIZE(3)
      INTEGER*4 SUF_SIZE(3)
      CHARACTER*30 AXNAME(3)
      INTEGER*4 ITEM_BYTES
      CHARACTER*30 ITEM_TYPE
      REAL*4 ISCALE(2)
      INTEGER*4 TCODE
      INTEGER*4 LIMITS(2,3)
C
      ERRVAL=0
      FID3 = 0
      CALL Q_OPEN(FID3,0,ZIN,1,0,' ',0,0,0,RET)
      IF (RET.LT.0) GOTO 9001
      CALL Q_CHECK_STD(FID3,RET)
      IF (RET.LT.0) GOTO 9002
      CALL Q_GET_SYS_KEYS(FID3,1,AXES,ORDER,CORE_SIZE,SUF_SIZE,
     .                    AXNAME,ITEM_BYTES,ITEM_TYPE,ISCALE,RET)
      IF (RET.LT.0) GOTO 9003
      CALL U_TYPE_CODE(1,ITEM_BYTES,ITEM_TYPE,TCODE)
      IF (TCODE.NE.3) GOTO 9004
      LIMITS(1,1) = 1
      LIMITS(2,1) = CORE_SIZE(1)
      LIMITS(1,2) = 1
      LIMITS(2,2) = CORE_SIZE(2)
      LIMITS(1,3) = 1
      LIMITS(2,3) = 1
      CALL Q_LIO_CBRICK(FID3,1,LIMITS,ZBUF,RET)
      IF (RET.LT.0) GOTO 9006
      CALL Q_CLOSE(FID3,1,RET)
      IF (RET.LT.0) GOTO 9007
      IF ((CORE_SIZE(2).EQ.INL).AND.(CORE_SIZE(1).EQ.INS)) THEN
        ERRVAL = 1
	RETURN
      ENDIF
      IF ((CORE_SIZE(2).NE.INL+1).OR.(CORE_SIZE(1).NE.INS+1)) GOTO 9005
      DO J=1,NY+1
         DO I=1,NX+1
            IF (IOCT.EQ.-2) THEN
               Z(I,J)=ZBUF(J,INL+2-I)
            ELSE IF (IOCT.EQ.-1) THEN
               Z(I,J)=ZBUF(I,INL+2-J)
            ELSE IF (IOCT.EQ.1) THEN
               Z(I,J)=ZBUF(I,J)
            ELSE IF (IOCT.EQ.2) THEN
               Z(I,J)=ZBUF(J,I)
            END IF
C The topographic file should not contain any special pixel values.
C It is the user's responsibility to make sure that it doesn't.
	    IF (Z(I,J).LT.VALID_MIN4.OR.Z(I,J).GT.VALID_MAX4) GOTO 9008
         ENDDO
      ENDDO
      RETURN
C**************************************************************************
C Handle processing errors
C**************************************************************************
9001  ERRKEY = 'XCSTO-OPENERR'
      ERRMSG = 'Error opening file containing topographic estimate'
      ERRVAL = -1
      GOTO 9999

9002  ERRKEY = 'XCSTO-NOTCUBE'
      ERRMSG = 'Input file is not a standard cube'
      ERRVAL = -2
      GOTO 9999

9003  ERRKEY = 'XCSTO-INVCORE'
      ERRMSG = 'Error retrieving core information from '//
     .         'input file'
      ERRVAL = -3
      GOTO 9999

9004  ERRKEY = 'XCSTO-BITTYPE'
      ERRMSG = 'Input file must be 32-bit'
      ERRVAL = -4
      GOTO 9999

9005  ERRKEY = 'XCSTO-INVSIZE'
      ERRMSG = 'Input file is wrong size'
      ERRVAL = -5
      GOTO 9999

9006  ERRKEY = 'XCSTO-READ'
      ERRMSG = 'Error reading data from input file'
      ERRVAL = -6
      GOTO 9999

9007  ERRKEY = 'XCSTO-CLOSE'
      ERRMSG = 'Error closing input file'
      ERRVAL = -7
      GOTO 9999

9008  ERRKEY = 'XCSTO-INVDATA'
      ERRMSG = 'Invalid special pixel value found in ZIN'
      ERRVAL = -8
      GOTO 9999
C**************************************************************************
C Write an error message
C**************************************************************************
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,1)
      RETURN
      END
