      SUBROUTINE XCTAE_FILES(ZIN,LZIN,USEZIN,USEDATUM,
     * LOGFILE,LLOG,NOTE,LNOTE)
C     * ZOUT,LZOUT,LOGFILE,LLOG,NOTE,LNOTE)
      IMPLICIT NONE
C
C_TITLE	XCTAE_FILES - Get file info from TAE for clinometry programs
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	CHARACTER	ZIN*(*)		!O	File containing initial 
C						guess for topo
	INTEGER*4	LZIN		!O	Length of ZIN
	LOGICAL*1	USEZIN		!O	Was a ZIN file specified?
	LOGICAL*1	USEDATUM	!O	Was ZIN='DATUM' indicating 
C						start with datum model only,
C						no topo?
C	CHARACTER	ZOUT*(*)	!O	Continuously updated topo 
C						estimate (in correct form 
C						to be used as a ZIN)
C	INTEGER*4	LZOUT		!O	Length of ZOUT
	CHARACTER	LOGFILE*(*)	!O	Optional file to recieve 
C						history of the iteration 
C						process
	INTEGER*4	LLOG		!O	Length of LOGFILE
	CHARACTER	NOTE*(*)	!O	Note to go in LOGFILE, 
C						output label
	INTEGER*4	LNOTE		!O	Length of NOTE
C
C_DESC	Calls TAE to get the names of auxiliary input and output files
C	for the 2-D clinometry programs.
C
C_CALLS XRREAL,XRSTR (TAE)
C
C_HIST	23-Oct-89 Randolph Kirk U.S.G.S. Flagstaff Original Version
C       May 23 2000 Janet Barrett - Ported from VAX to Unix
C
C_END
C
#include "clinom_log.inc"
      INTEGER*4 ICOUNT,ISTAT! Needed for TAE input
      CHARACTER*64 TMPSTR
      CHARACTER*512 U_UPCASE
      INTEGER*4 ERRVAL      ! Error code
      CHARACTER ERRKEY*17   ! Keyword value for error message
      CHARACTER ERRMSG*80   ! Error message
      CHARACTER STR*132     ! String for reporting information
      INTEGER*4 IERR
      INTEGER*4 U_SLEN
      DATA IULOG / 1 /

      CALL U_GET_FILE_PARM('ZIN',1,ZIN,LZIN,ICOUNT,ISTAT)
      IF (ISTAT.NE.0) GOTO 9000
C      USEZIN=(ICOUNT.EQ.1)
      USEZIN=(U_SLEN(ZIN).GT.0)
      IF (USEZIN) THEN
         TMPSTR = U_UPCASE(ZIN)
         USEDATUM=(TMPSTR(1:LZIN).EQ.'DATUM')
      END IF
C      CALL U_GET_FILE_PARM('ZOUT',1,ZOUT,LZOUT,ICOUNT,ISTAT)
C      IF (ISTAT.NE.0) GOTO 9000
      CALL U_GET_FILE_PARM('LOGFILE',1,LOGFILE,LLOG,ICOUNT,ISTAT)
      IF (ISTAT.NE.0) GOTO 9000
      LOGIT=(ICOUNT.EQ.1)
      CALL U_GET_STR_PARM('NOTE',1,NOTE,LNOTE,ICOUNT,ISTAT)
      IF (ISTAT.NE.0) GOTO 9000
      LNOTE = U_SLEN(NOTE)
   21 FORMAT (A,:,A,:,',',A)
   22 FORMAT(A,I6,:,',',I6)
   23 FORMAT(A,1PG14.7,:,',',G14.7)
      IF (USEZIN) THEN
         IF (USEDATUM) THEN
            WRITE (STR,21) 
     *'Initial topographic estimate is DATUM only, no topography.'
         ELSE
            WRITE (STR,21) 
     *'Initial topographic estimate from image file ZIN= ',
     *ZIN(1:LZIN)
         END IF
	 CALL U_WRITE_MSG(3,STR)
      ELSE
         WRITE (STR,21) 
     *'Initial topographic estimate from SSIPSF-PI (no ZIN file)'
	 CALL U_WRITE_MSG(3,STR)
      END IF
C      WRITE (STR,21) 'ZOUT= '//ZOUT(1:LZOUT)
C      CALL U_WRITE_MSG(3,STR)
      IF (.NOT.LOGIT) THEN
         LOGFILE='--'
         LLOG=2
      END IF
C      WRITE (STR,21) 'LOGFILE= '//LOGFILE(1:LLOG)
C      CALL U_WRITE_MSG(3,STR)
C      WRITE (STR,21) 'NOTE= '//NOTE(1:LNOTE)
C      CALL U_WRITE_MSG(3,STR)
      IF (LOGIT) THEN
	 CALL U_ALLOCATE_UNIT(IULOG,IERR)
         OPEN(UNIT=IULOG,FILE=LOGFILE,STATUS='UNKNOWN',
     *ACCESS='APPEND')
      END IF
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
