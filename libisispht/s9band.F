      SUBROUTINE S9BAND(E,DZ,INDX,A,ABAND,AL,NX,NY,DZRMS)
      IMPLICIT NONE
C
C_TITLE S9BAND - Solves [A]{DZ}={E} directly by LU decomposition and substitution
C
#include "clinom_ops.inc"
C_ARGS  TYPE        VARIABLE           I/O  DESC
        INTEGER*4   NX                 ! I
        INTEGER*4   NY                 ! I
        REAL*4      E(NX,NY)           ! I
        REAL*4      DZ(NX,NY)          ! O
        INTEGER*4   INDX(NX,NY)        ! I
        REAL*4      A(5,NX,NY)         ! I
        REAL*4      ABAND(NX,NY,2*NX+3)! O
        REAL*4      AL(NX,NY,NX+1)     ! I
        REAL*4      DZRMS              ! O
C
C_DESC Solves [A]{DZ}={E} directly by LU decomposition and substitution.
C      [A] is a 9-band symmetric matrix as usual for the S9FAST routines.
C      [ABAND] is this matrix partially unpacked into banded form but with
C      both the upper and lower bands and all the zero bands in between
C      explicitly stored.  Numerical Recipes routines BANDEC and BANBKS are 
C      used to solve the equations
C
C_HIST  Dec 10 1996 Randolph Kirk, USGS, Flagstaff - Original Version
C       May 06 1998 Tracie Sucharski, Converted to UNIX/ISIS
C
C_END


      INTEGER*4   N,M1,M2,M,NP,MP,MPL,I,J

      REAL*4      D


      N=NX*NY
      M1=NX+1
      M2=NX+1
      M=M1+M2+1
      NP=N
      MP=M
      MPL=M1
      CALL U_FILL4(0.0,NP*MP,ABAND)
      DO J=1,NY
         DO I=1,NX
            IF (J.GT. 1) THEN
               IF (I.GT. 1) ABAND(I,J,     1)=A(5,I-1,J-1)
                            ABAND(I,J,     2)=A(4,I  ,J-1)
               IF (I.LT.NX) ABAND(I,J,     3)=A(3,I+1,J-1)
            END IF
               IF (I.GT. 1) ABAND(I,J,  NX+1)=A(2,I-1,J  )
                            ABAND(I,J,  NX+2)=A(1,I  ,J  )
               IF (I.LT.NX) ABAND(I,J,  NX+3)=A(2,I  ,J  )
            IF (J.LT.NY) THEN
               IF (I.GT. 1) ABAND(I,J,2*NX+1)=A(3,I  ,J  )
                            ABAND(I,J,2*NX+2)=A(4,I  ,J  )
               IF (I.LT.NX) ABAND(I,J,2*NX+3)=A(5,I  ,J  )
            END IF
         ENDDO
      ENDDO
      CALL U_MOVE4(N,E,DZ)
      CALL BANDEC(ABAND,N,M1,M2,NP,MP,AL,MPL,INDX,D)
      CALL BANBKS(ABAND,N,M1,M2,NP,MP,AL,MPL,INDX,DZ)
      DZRMS=0.
      DO J=1,NY
         DO I=1,NX
            DZRMS=DZRMS+DZ(I,J)*DZ(I,J)
         ENDDO
      ENDDO
      DZRMS=SQRT(DZRMS/REAL(N))
      NOPS=NOPS+N*M1*(1+2*M)+N*2*(M1+M)+2*N

      RETURN
      END

