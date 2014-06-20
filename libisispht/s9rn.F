C**************************************************************************
C
      SUBROUTINE S9RN(B,DL,WORK,NX,NY,RN,MODE,IOCT)
      IMPLICIT NONE
C
C Conjugate gradient internal routine: calculates the numerator of R, 
C the multiple of the basis vector DZ which will be added to Z in the 
C current CGM step, and in the process calculates a vector which will
C be used by S9RD to calculate the basis vector itself.
C
#include "clinom_ops.inc"
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
	INTEGER*4  NX,NY	!I	Dimensions of mesh
	REAL*4     B(NX,NY)	!I	Current residual
	REAL*4     DL(5,NX,NY)	!I	Approximate factorization of matrix
	REAL*4     WORK(NX,NY)	!O	B or ([L][D][L]T)**-1 times B (used 
C					by S9RD to calculate basis vector)
	REAL*4     RN		!O	Numerator of multiple R
	INTEGER*4  MODE		!I	Factorization DL is used if MODE=1
	INTEGER*4  IOCT	!I	Controls sweep direction.  The fwd direction is
C					-2 = +samp -line(inner, outer loops)
C					-1 = -line +samp 
C					+1 = +line +samp
C					+2 = +samp +line
C In each case, sweep for back substitution is the reverse of that above.
C The IOCT=+2 sweep direction is the old default, and is used for all
C other values of IOCT that may be supplied in error.  
C
C Note: DL is used only for the incomplete Cholesky-conjugate gradient 
C method.  If MODE<>1 it can be a dummy array rather than one of full size.
C
C 24-Jul-02 NOTE: Routine has been accessing invalid elements of WORK
C		(erroneously modifying last column) because first column is
C		not handled as a separate case.  Fix this bug before implementing
C		variable sweep direction.
C 25-Jul-02 RLK Now direction in which rows are swept to vary, based on IOCT

      INTEGER*4 I,J,IM,JM,IP,JP,NXM,NYM
      REAL*4 W

      NXM=NX-1
      NYM=NY-1
C Start with WORK = B = residual
C      CALL COPY(B,WORK,NX*NY)
      CALL U_MOVE4(NX*NY,B,WORK)
C If desired, invert the approximate factorization of the matrix on WORK
      IF (MODE.EQ.1) THEN
         IF (IOCT.EQ.-2) THEN
C Forward reduction
C Last and interior rows
            DO J=NY,2,-1
               JM=J-1
C First column
               W=WORK(1,J)
               WORK(2,J )=WORK(2,J )-DL(2,1,J )*W
               WORK(1,JM)=WORK(1,JM)-DL(4,1,JM)*W
               WORK(2,JM)=WORK(2,JM)-DL(3,2,JM)*W
C Interior columns
               DO I=2,NXM
                  IM=I-1
                  IP=I+1
                  W=WORK(I,J)
                  WORK(IP,J )=WORK(IP,J )-DL(2,I ,J )*W
                  WORK(IM,JM)=WORK(IM,JM)-DL(5,IM,JM)*W
                  WORK(I ,JM)=WORK(I ,JM)-DL(4,I ,JM)*W
                  WORK(IP,JM)=WORK(IP,JM)-DL(3,IP,JM)*W
               ENDDO
C Last column
               W=WORK(NX,J)
               WORK(NXM,JM)=WORK(NXM,JM)-DL(5,NXM,JM)*W
               WORK(NX ,JM)=WORK(NX ,JM)-DL(4,NX ,JM)*W
            ENDDO
C First row
C First and interior columns
            DO I=1,NXM
               IP=I+1
               W=WORK(I,1)
               WORK(IP,1)=WORK(IP,1)-DL(2,I,1)*W
            ENDDO
C Nothing happens to first row last column
C Normalize by diagonal
            DO J=1,NY
               DO I=1,NX
                  WORK(I,J)=WORK(I,J)/DL(1,I,J)
               ENDDO
            ENDDO
C Back substitution
C First and interior rows
            DO J=1,NYM
               JP=J+1
C Last column
               W=WORK(NX,J)
               WORK(NXM,J )=WORK(NXM,J )-DL(2,NXM,J)*W
               WORK(NX ,JP)=WORK(NX ,JP)-DL(4,NX ,J)*W
               WORK(NXM,JP)=WORK(NXM,JP)-DL(3,NX,J)*W
C Interior columns
               DO I=NX-1,2,-1
                  IM=I-1
                  IP=I+1
                  W=WORK(I,J)
                  WORK(IM,J )=WORK(IM,J )-DL(2,IM,J)*W
                  WORK(IP,JP)=WORK(IP,JP)-DL(5,I ,J)*W
                  WORK(I ,JP)=WORK(I ,JP)-DL(4,I ,J)*W
                  WORK(IM,JP)=WORK(IM,JP)-DL(3,I ,J)*W
               ENDDO
C First column
               W=WORK(1,J)
               WORK(2,JP)=WORK(2,JP)-DL(5,1,J)*W
               WORK(1,JP)=WORK(1,JP)-DL(4,1,J)*W
            ENDDO
C Last row
C Last and interior columns
            DO I=NX,2,-1
               IM=I-1
               W=WORK(I,NY)
               WORK(IM,NY)=WORK(IM,NY)-DL(2,IM,NY)*W
            ENDDO
C Nothing happens to last row first column
         ELSE IF (IOCT.EQ.-1) THEN
C Forward reduction
C First and interior columns
            DO i=1,NXM
               IP=I+1
C Last row
               W=WORK(I,NY)
               WORK(I ,NYM)=WORK(I ,NYM)-DL(4,I ,NYM)*W
               WORK(IP,NY )=WORK(IP,NY )-DL(2,I ,NY )*W
               WORK(IP,NYM)=WORK(IP,NYM)-DL(3,IP,NYM)*W
C Interior rows
               DO J=NYM,2,-1
                  JM=J-1
                  JP=J+1
                  W=WORK(I,J)
                  WORK(I ,JM)=WORK(I ,JM)-DL(4,I ,JM)*W
                  WORK(IP,JP)=WORK(IP,JP)-DL(5,I ,J )*W
                  WORK(IP,J )=WORK(IP,J )-DL(2,I ,J )*W
                  WORK(IP,JM)=WORK(IP,JM)-DL(3,IP,JM)*W
               ENDDO
C First row
               W=WORK(I,1)
               WORK(IP,2)=WORK(IP,2)-DL(5,I,1)*W
               WORK(IP,1)=WORK(IP,1)-DL(2,I,1)*W
            ENDDO
C Last column
C Last and interior rows
            DO J=NY,2,-1
               JM=J-1
               W=WORK(NX,J)
               WORK(NX,JM)=WORK(NX,JM)-DL(4,NX,JM)*W
            ENDDO
C Nothing happens to last column first row
C Normalize by diagonal
            DO J=1,NY
               DO I=1,NX
                  WORK(I,J)=WORK(I,J)/DL(1,I,J)
               ENDDO
            ENDDO
C Back substitution
C Last and interior columns
            DO I=NX,2,-1
               IM=I-1
C First row
               W=WORK(I,1)
               WORK(I ,2)=WORK(I ,2)-DL(4,I ,1)*W
               WORK(IM,1)=WORK(IM,1)-DL(2,IM,1)*W
               WORK(IM,2)=WORK(IM,2)-DL(3,I ,1)*W
C Interior rows
               DO J=2,NYM
                  JM=J-1
                  JP=J+1
                  W=WORK(I,J)
                  WORK(I ,JP)=WORK(I ,JP)-DL(4,I ,J )*W
                  WORK(IM,JM)=WORK(IM,JM)-DL(5,IM,JM)*W
                  WORK(IM,J )=WORK(IM,J )-DL(2,IM,J )*W
                  WORK(IM,JP)=WORK(IM,JP)-DL(3,I ,J )*W
               ENDDO
C Last row
               W=WORK(I,NY)
               WORK(IM,NYM)=WORK(IM,NYM)-DL(5,IM,NYM)*W
               WORK(IM,NY )=WORK(IM,NY )-DL(2,IM,NY )*W
            ENDDO
C First column
C First and interior rows
            DO J=1,NYM
               JP=J+1
               W=WORK(1,J)
               WORK(1,JP)=WORK(1,JP)-DL(4,1,J)*W
            ENDDO
C Nothing happens to first column last row
         ELSE IF (IOCT.EQ.+1) THEN
C Forward reduction
C First and interior columns
            DO i=1,NXM
               IP=I+1
C First row
               W=WORK(I,1)
               WORK(I ,2)=WORK(I ,2)-DL(4,I,1)*W
               WORK(IP,1)=WORK(IP,1)-DL(2,I,1)*W
               WORK(IP,2)=WORK(IP,2)-DL(5,I,1)*W
C Interior rows
               DO J=2,NYM
                  JM=J-1
                  JP=J+1
                  W=WORK(I,J)
                  WORK(I ,JP)=WORK(I ,JP)-DL(4,I ,J )*W
                  WORK(IP,JM)=WORK(IP,JM)-DL(3,IP,JM)*W
                  WORK(IP,J )=WORK(IP,J )-DL(2,I ,J )*W
                  WORK(IP,JP)=WORK(IP,JP)-DL(5,I ,J )*W
               ENDDO
C Last row
               W=WORK(I,NY)
               WORK(IP,NYM)=WORK(IP,NYM)-DL(3,IP,NYM)*W
               WORK(IP,NY )=WORK(IP,NY )-DL(2,I ,NY )*W
            ENDDO
C Last column
C First and interior rows
            DO J=1,NYM
               JP=J+1
               W=WORK(NX,J)
               WORK(NX,JP)=WORK(NX,JP)-DL(4,NX,J)*W
            ENDDO
C Nothing happens to last column last row
C Normalize by diagonal
            DO J=1,NY
               DO I=1,NX
                  WORK(I,J)=WORK(I,J)/DL(1,I,J)
               ENDDO
            ENDDO
C Back substitution
C Last and interior columns
            DO I=NX,2,-1
               IM=I-1
C Last row
               W=WORK(I,NY)
               WORK(I ,NYM)=WORK(I ,NYM)-DL(4,I ,NYM)*W
               WORK(IM,NY )=WORK(IM,NY )-DL(2,IM,NY )*W
               WORK(IM,NYM)=WORK(IM,NYM)-DL(5,IM,NYM)*W
C Interior rows
               DO J=NY-1,2,-1
                  JM=J-1
                  JP=J+1
                  W=WORK(I,J)
                  WORK(I ,JM)=WORK(I ,JM)-DL(4,I ,JM)*W
                  WORK(IM,JP)=WORK(IM,JP)-DL(3,I ,J )*W
                  WORK(IM,J )=WORK(IM,J )-DL(2,IM,J )*W
                  WORK(IM,JM)=WORK(IM,JM)-DL(5,IM,JM)*W
               ENDDO
C First row
               W=WORK(I,1)
               WORK(IM,2)=WORK(IM,2)-DL(3,I ,1)*W
               WORK(IM,1)=WORK(IM,1)-DL(2,IM,1)*W
            ENDDO
C First column
C Last and interior rows
            DO J=NY,2,-1
               JM=J-1
               W=WORK(1,J)
               WORK(1,JM)=WORK(1,JM)-DL(4,1,JM)*W
            ENDDO
C Nothing happens to first column first row
         ELSE	! Orig algorithm used for IOCT 2 and any bad values
C Forward reduction
C First and interior rows
            DO J=1,NYM
               JP=J+1
C First column
               W=WORK(1,J)
               WORK(2,J )=WORK(2,J )-DL(2,1,J)*W
               WORK(1,JP)=WORK(1,JP)-DL(4,1,J)*W
               WORK(2,JP)=WORK(2,JP)-DL(5,1,J)*W
C Interior columns
               DO I=2,NXM
                  IM=I-1
                  IP=I+1
                  W=WORK(I,J)
                  WORK(IP,J )=WORK(IP,J )-DL(2,I,J)*W
                  WORK(IM,JP)=WORK(IM,JP)-DL(3,I,J)*W
                  WORK(I ,JP)=WORK(I ,JP)-DL(4,I,J)*W
                  WORK(IP,JP)=WORK(IP,JP)-DL(5,I,J)*W
               ENDDO
C Last column
               W=WORK(NX,J)
               WORK(NXM,JP)=WORK(NXM,JP)-DL(3,NX,J)*W
               WORK(NX ,JP)=WORK(NX ,JP)-DL(4,NX,J)*W
            ENDDO
C Last row
C First and interior columns
            DO I=1,NXM
               IP=I+1
               W=WORK(I,NY)
               WORK(IP,NY)=WORK(IP,NY)-DL(2,I,NY)*W
            ENDDO
C Nothing happens to last row last column
C Normalize by diagonal
            DO J=1,NY
               DO I=1,NX
                  WORK(I,J)=WORK(I,J)/DL(1,I,J)
               ENDDO
            ENDDO
C Back substitution
C Last and interior rows
            DO J=NY,2,-1
               JM=J-1
C Last column
               W=WORK(NX,J)
               WORK(NXM,J )=WORK(NXM,J )-DL(2,NXM,J )*W
               WORK(NX ,JM)=WORK(NX ,JM)-DL(4,NX ,JM)*W
               WORK(NXM,JM)=WORK(NXM,JM)-DL(5,NXM,JM)*W
C Interior columns
               DO I=NX-1,2,-1
                  IM=I-1
                  IP=I+1
                  W=WORK(I,J)
                  WORK(IM,J )=WORK(IM,J )-DL(2,IM,J )*W
                  WORK(IP,JM)=WORK(IP,JM)-DL(3,IP,JM)*W
                  WORK(I ,JM)=WORK(I ,JM)-DL(4,I ,JM)*W
                  WORK(IM,JM)=WORK(IM,JM)-DL(5,IM,JM)*W
               ENDDO
C First column
               W=WORK(1,J)
               WORK(2,JM)=WORK(2,JM)-DL(3,2,JM)*W
               WORK(1,JM)=WORK(1,JM)-DL(4,1,JM)*W
            ENDDO
C First row
C Last and interior columns
            DO I=NX,2,-1
               IM=I-1
               W=WORK(I,1)
               WORK(IM,1)=WORK(IM,1)-DL(2,IM,1)*W
            ENDDO
C Nothing happens to first row first column
         END IF
         NOPS=NOPS+17*NX*NY-12*NX-8*NY+4
      END IF
C Calculate RN as a dot product
      RN=0.0
      DO J=1,NY
         DO I=1,NX
            RN=RN+B(I,J)*WORK(I,J)
         ENDDO
      ENDDO
      NOPS=NOPS+2*NX*NY
      RETURN
      END
