C**************************************************************************
C
      SUBROUTINE S9SOR(A,DZ,NX,NY,W,IOCT)
      IMPLICIT NONE
C
C Relaxation internal routine: calculates the increment to the solution for
C over, under, or Gauss-Seidel relaxation using the lower triangle of A.
C       21 Jul 2002 RLK Added IOCT parameter controlling sweep direction
C
#include "clinom_ops.inc"
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
	INTEGER*4  NX,NY	!I	Dimensions of mesh
	REAL*4     A(5,NX,NY)	!I	Matrix, in compressed form
	REAL*4     DZ(NX,NY)	!I/O	Input - Righthand side
C					Output - Increment to solution
	REAL*4     W		!I	Weight for SOR
	INTEGER*4  IOCT	!I	Controls SOR sweep direction
C					-2 = -line +samp (inner, outer loops)
C					-1 = +samp -line
C					+1 = +samp +line
C					+2 = +line + samp
C The IOCT=+1 sweep direction is the old default, and is used for all
C other values of IOCT that may be supplied in error.
C
      INTEGER*4 I,J,IM,IP,JM,JP,NXM,NYM
      REAL*4 D

      NXM=NX-1
      NYM=NY-1
      IF (IOCT.EQ.-2) THEN
C First and interior columns
         DO I=1,NXM
            IP=I+1
C Last row
            IF (A(1,I,NY).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(I,NY)/A(1,I,NY)
            END IF
            DZ(I ,NY )=D
            DZ(I ,NYM)=DZ(I ,NYM)-D*A(4,I ,NYM)
            DZ(IP,NY )=DZ(IP,NY )-D*A(2,I ,NY )
            DZ(IP,NYM)=DZ(IP,NYM)-D*A(3,IP,NYM)
C Interior rows
            DO J=NYM,2,-1
               JM=J-1
               JP=J+1
               IF (A(1,I,J).EQ.0.0) THEN
                  D=0.0
               ELSE
                  D=W*DZ(I,J)/A(1,I,J)
               END IF
               DZ(I ,J )=D
               DZ(I ,JM)=DZ(I ,JM)-D*A(4,I ,JM)
               DZ(IP,JP)=DZ(IP,JP)-D*A(5,I ,J )
               DZ(IP,J )=DZ(IP,J )-D*A(2,I ,J )
               DZ(IP,JM)=DZ(IP,JM)-D*A(3,IP,JM)
            ENDDO
C First row
            IF (A(1,I,1).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(I,1)/A(1,I,1)
            END IF
            DZ(I ,1)=D
            DZ(IP,2)=DZ(IP,2)-D*A(5,I,1)
            DZ(IP,1)=DZ(IP,1)-D*A(2,I,1)
         ENDDO
C Last column
C Last and interior rows
         DO J=NY,2,-1
            JM=J-1
            IF (A(1,NX,J).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(NX,J)/A(1,NX,J)
            END IF
            DZ(NX,J)=D
            DZ(NX,JM)=DZ(NX,JM)-D*A(4,NX,JM)
         ENDDO
C First row
         IF (A(1,NX,1).EQ.0.0) THEN
            D=0.0
         ELSE
            D=W*DZ(NX,1)/A(1,NX,1)
         END IF
         DZ(NX,1)=D
      ELSE IF (IOCT.EQ.-1) THEN
C Last and interior rows
         DO J=NY,2,-1
            JM=J-1
C First column
            IF (A(1,1,J).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(1,J)/A(1,1,J)
            END IF
            DZ(1,J )=D
            DZ(2,J )=DZ(2,J )-D*A(2,1,J )
            DZ(1,JM)=DZ(1,JM)-D*A(4,1,JM)
            DZ(2,JM)=DZ(2,JM)-D*A(3,2,JM)
C Interior columns
            DO I=2,NXM
               IM=I-1
               IP=I+1
               IF (A(1,I,J).EQ.0.0) THEN
                  D=0.0
               ELSE
                  D=W*DZ(I,J)/A(1,I,J)
               END IF
               DZ(I ,J )=D
               DZ(IP,J )=DZ(IP,J )-D*A(2,I ,J )
               DZ(IM,JM)=DZ(IM,JM)-D*A(5,IM,JM)
               DZ(I ,JM)=DZ(I ,JM)-D*A(4,I ,JM)
               DZ(IP,JM)=DZ(IP,JM)-D*A(3,IP,JM)
            ENDDO
C Last column
            IF (A(1,NX,J).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(NX,J)/A(1,NX,J)
            END IF
            DZ(NX ,J )=D
            DZ(NXM,JM)=DZ(NXM,JM)-D*A(5,NXM,JM)
            DZ(NX ,JM)=DZ(NX ,JM)-D*A(4,NX ,JM)
         ENDDO
C First row
C First and interior columns
         DO I=1,NXM
            IP=I+1
            IF (A(1,I,1).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(I,1)/A(1,I,1)
            END IF
            DZ(I ,1)=D
            DZ(IP,1)=DZ(IP,1)-D*A(2,I,1)
         ENDDO
C Last column
         IF (A(1,NX,1).EQ.0.0) THEN
            D=0.0
         ELSE
            D=W*DZ(NX,1)/A(1,NX,1)
         END IF
         DZ(NX,1)=D
      ELSE IF (IOCT.EQ.2) THEN
C First and interior columns
         DO I=1,NXM
            IP=I+1
C First row
            IF (A(1,I,1).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(I,1)/A(1,I,1)
            END IF
            DZ(I ,1)=D
            DZ(I ,2)=DZ(I ,2)-D*A(4,I,1)
            DZ(IP,1)=DZ(IP,1)-D*A(2,I,1)
            DZ(IP,2)=DZ(IP,2)-D*A(5,I,1)
C Interior rows
            DO J=2,NYM
               JM=J-1
               JP=J+1
               IF (A(1,I,J).EQ.0.0) THEN
                  D=0.0
               ELSE
                  D=W*DZ(I,J)/A(1,I,J)
               END IF
               DZ(I,J )=D
               DZ(I ,JP)=DZ(I ,JP)-D*A(4,I ,J )
               DZ(IP,JM)=DZ(IP,JM)-D*A(3,IP,JM)
               DZ(IP,J )=DZ(IP,J )-D*A(2,I ,J )
               DZ(IP,JP)=DZ(IP,JP)-D*A(5,I ,J )
            ENDDO
C Last row
            IF (A(1,I,NY).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(I,NY)/A(1,I,NY)
            END IF
            DZ(I ,NY )=D
            DZ(IP,NYM)=DZ(IP,NYM)-D*A(3,IP,NYM)
            DZ(IP,NY )=DZ(IP,NY )-D*A(2,I ,NY )
         ENDDO
C Last column
C First and interior rows
         DO J=1,NYM
            JP=J+1
            IF (A(1,NX,J).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(NX,J)/A(1,NX,J)
            END IF
            DZ(NX,J )=D
            DZ(NX,JP)=DZ(NX,JP)-D*A(4,NX,J)
         ENDDO
C Last row
         IF (A(1,NX,NY).EQ.0.0) THEN
            D=0.0
         ELSE
            D=W*DZ(NX,NY)/A(1,NX,NY)
         END IF
         DZ(NX,NY)=D
      ELSE	! IOCT=+1 or any other (technically invalid) value
C First and interior rows
         DO J=1,NYM
            JP=J+1
C First column
            IF (A(1,1,J).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(1,J)/A(1,1,J)
            END IF
            DZ(1,J )=D
            DZ(2,J )=DZ(2,J )-D*A(2,1,J)
            DZ(1,JP)=DZ(1,JP)-D*A(4,1,J)
            DZ(2,JP)=DZ(2,JP)-D*A(5,1,J)
C Interior columns
            DO I=2,NXM
               IM=I-1
               IP=I+1
               IF (A(1,I,J).EQ.0.0) THEN
                  D=0.0
               ELSE
                  D=W*DZ(I,J)/A(1,I,J)
               END IF
               DZ(I ,J )=D
               DZ(IP,J )=DZ(IP,J )-D*A(2,I,J)
               DZ(IM,JP)=DZ(IM,JP)-D*A(3,I,J)
               DZ(I ,JP)=DZ(I ,JP)-D*A(4,I,J)
               DZ(IP,JP)=DZ(IP,JP)-D*A(5,I,J)
            ENDDO
C Last column
            IF (A(1,NX,J).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(NX,J)/A(1,NX,J)
            END IF
            DZ(NX ,J )=D
            DZ(NXM,JP)=DZ(NXM,JP)-D*A(3,NX,J)
            DZ(NX ,JP)=DZ(NX ,JP)-D*A(4,NX,J)
         ENDDO
C Last row
C First and interior columns
         DO I=1,NXM
            IP=I+1
            IF (A(1,I,NY).EQ.0.0) THEN
               D=0.0
            ELSE
               D=W*DZ(I,NY)/A(1,I,NY)
            END IF
            DZ(I ,NY)=D
            DZ(IP,NY)=DZ(IP,NY)-D*A(2,I,NY)
         ENDDO
C Last column
         IF (A(1,NX,NY).EQ.0.0) THEN
            D=0.0
         ELSE
            D=W*DZ(NX,NY)/A(1,NX,NY)
         END IF
         DZ(NX,NY)=D
      END IF
      NOPS=NOPS+10*NX*NY-6*(NX+NY)+4
      RETURN
      END
