      SUBROUTINE ZINTERP(ZIN,ZOUT,NX,NY,SCALE,COSEMI,ERRVAL)
      IMPLICIT NONE
C
C_TITLE ZINTERP - Interpolates and scales the topography
C
#include "clinom_ops.inc"
C_ARGS  TYPE        VARIABLE        I/O  DESC
        INTEGER*4   NX              ! I  Number of samples in
C                                        unscaled array
        INTEGER*4   NY              ! I  Number of lines in
C                                        unscaled array
        REAL*4      ZIN(NX,NY)      ! I  Unscaled array
        REAL*4      ZOUT(NX+1,NY+1) ! O  Scaled array
        REAL*4      SCALE           ! I  Scale factor
	REAL*4      COSEMI          ! I  Used to scale ZIN from
C                                        vertical to line of sight
	INTEGER*4   ERRVAL          ! O  Error status
C
C_DESC Interpolates the ZIN & scales it so the ZOUT is 1 pixel
C      bigger in each direction.
C
C_HIST  Feb 10 2003 Janet Barrett, USGS, Flagstaff - Original
C                   version based on Randy Kirk's itoz1.F routine
C
C_END
C
#include "clinom_specpix.inc"

      INTEGER*4   I,J
      REAL*4      SCALE4,ZBIJ
      CHARACTER*32 ERRKEY
      CHARACTER*80 ERRMSG

      ERRVAL=0
      SCALE4=SCALE*4.
      CALL U_FILL4(0.0,(NX+1)*(NY+1),ZOUT)
      DO J=1,NY
        DO I=1,NX
	  IF (ZIN(I,J).LT.VALID_MIN4.OR.ZIN(I,J).GT.VALID_MAX4) GOTO 9001
	  ZIN(I,J) = ZIN(I,J)/COSEMI
        ENDDO
      ENDDO
      DO J=1,NY
         DO I=1,NX
            ZBIJ=ZIN(I,J)/SCALE4
            ZOUT(I  ,J  )=ZOUT(I  ,J  )+ZBIJ
            ZOUT(I+1,J  )=ZOUT(I+1,J  )+ZBIJ
            ZOUT(I  ,J+1)=ZOUT(I  ,J+1)+ZBIJ
            ZOUT(I+1,J+1)=ZOUT(I+1,J+1)+ZBIJ
         ENDDO
      ENDDO
      DO I=1,NX
         ZBIJ=ZIN(I,1)/SCALE4
         ZOUT(I  ,1)=ZOUT(I  ,1)+ZBIJ
         ZOUT(I+1,1)=ZOUT(I+1,1)+ZBIJ
         ZBIJ=ZIN(I,NY)/SCALE4
         ZOUT(I  ,NY+1)=ZOUT(I  ,NY+1)+ZBIJ
         ZOUT(I+1,NY+1)=ZOUT(I+1,NY+1)+ZBIJ
      ENDDO
      DO J=1,NY
         ZBIJ=ZIN(1,J)/SCALE4
         ZOUT(1,J  )=ZOUT(1,J  )+ZBIJ
         ZOUT(1,J+1)=ZOUT(1,J+1)+ZBIJ
         ZBIJ=ZIN(NX,J)/SCALE4
         ZOUT(NX+1,J  )=ZOUT(NX+1,J  )+ZBIJ
         ZOUT(NX+1,J+1)=ZOUT(NX+1,J+1)+ZBIJ
      ENDDO
      ZOUT(1   ,1   )=ZOUT(1   ,1   )+ZIN(1 ,1 )/SCALE4
      ZOUT(NX+1,1   )=ZOUT(NX+1,1   )+ZIN(NX,1 )/SCALE4
      ZOUT(1   ,NY+1)=ZOUT(1   ,NY+1)+ZIN(1 ,NY)/SCALE4
      ZOUT(NX+1,NY+1)=ZOUT(NX+1,NY+1)+ZIN(NX,NY)/SCALE4
      NOPS=NOPS+5*(NX+1)*(NY+1)

      RETURN
C**************************************************************************
C Handle processing errors
C**************************************************************************
9001  ERRKEY = 'ZINTERP-INVDATA'
      ERRMSG = 'Invalid special pixel value found in ZIN'
      ERRVAL = -1
      GOTO 9999
C**************************************************************************
C Write an error message
C**************************************************************************
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,1)
      RETURN
      END
