      SUBROUTINE S9RD(DZ,A,WORK,NX,NY,RD)
      IMPLICIT NONE
C
C_TITLE S9RD - Conjugate gradient internal routine
C
#include "clinom_ops.inc"
C ARGS  TYPE       VARIABLE     I/O     DESCRIPTION
	INTEGER*4  NX,NY	!I	Dimensions of mesh
	REAL*4     DZ(NX,NY)	!O	Basis vector to be added to Z
	REAL*4     A(5,NX,NY)	!I	Matrix, in compressed form
	REAL*4     WORK(NX,NY)	!I/O	Input - Basis vector
C					Output - Change in residual 
C                                       corresponding to adding basis
C					vector to solution
	REAL*4     RD		!O	Denominator of multiple R
C
C_DESC  Conjugate gradient internal routine: calculates the denominator 
C       RD of the multiple R of the current basis vector which will be 
C       added to the solution; this basis vector is passed in as WORK and 
C       out as DZ. Also, calculates the corresponding decrement to the 
C       residual (by multiplying the basis vector by the matrix A).
C
C_HIST  10-AUG-89 Randolph Kirk, USGS Flagstaff, Original Version
C
      INTEGER*4 I,J,NXM,NYM,IM,JM,IP,JP

      NXM=NX-1
      NYM=NY-1
C Start with DZ = WORK = current basis vector
C      CALL COPY(WORK,DZ,NX*NY)
      CALL U_MOVE4(NX*NY,WORK,DZ)
C Multiply WORK by A to get contribution to residual
C First row
C First column
      WORK(1,1)=A(1,1,1)*DZ(1,1)+A(2,1,1)*DZ(2,1)
     *         +A(4,1,1)*DZ(1,2)+A(5,1,1)*DZ(2,2)
C Interior columns
      DO I=2,NXM
         IM=I-1
         IP=I+1
         WORK(I,1)=A(2,IM,1)*DZ(IM,1)+A(1,I,1)*DZ(I ,1)
     *            +A(2,I ,1)*DZ(IP,1)+A(3,I,1)*DZ(IM,2)
     *            +A(4,I ,1)*DZ(I ,2)+A(5,I,1)*DZ(IP,2)
      ENDDO
C Last column
      WORK(NX,1)=A(2,NXM,1)*DZ(NXM,1)+A(1,NX,1)*DZ(NX,1)
     *          +A(3,NX ,1)*DZ(NXM,2)+A(4,NX,1)*DZ(NX,2)
C Interior rows
      DO J=2,NYM
         JM=J-1
         JP=J+1
C First column
         WORK(1,J)=A(4,1,JM)*DZ(1,JM)+A(3,2,JM)*DZ(2,JM)
     *            +A(1,1,J )*DZ(1,J )+A(2,1,J )*DZ(2,J )
     *            +A(4,1,J )*DZ(1,JP)+A(5,1,J )*DZ(2,JP)
C Interior columns
         DO I=2,NXM
            IM=I-1
            IP=I+1 
            WORK(I,J)=A(5,IM,JM)*DZ(IM,JM)+A(4,I ,JM)*DZ(I ,JM)
     *               +A(3,IP,JM)*DZ(IP,JM)+A(2,IM,J )*DZ(IM,J )
     *               +A(1,I ,J )*DZ(I ,J )+A(2,I ,J )*DZ(IP,J )
     *               +A(3,I ,J )*DZ(IM,JP)+A(4,I ,J )*DZ(I ,JP)
     *               +A(5,I ,J )*DZ(IP,JP)
         ENDDO
C Last column
         WORK(NX,J)=A(5,NXM,JM)*DZ(NXM,JM)+A(4,NX,JM)*DZ(NX,JM)
     *             +A(2,NXM,J )*DZ(NXM,J )+A(1,NX,J )*DZ(NX,J )
     *             +A(3,NX ,J )*DZ(NXM,JP)+A(4,NX,J )*DZ(NX,JP)
      ENDDO
C Last row
C First column
      WORK(1,NY)=A(4,1,NYM)*DZ(1,NYM)+A(3,2,NYM)*DZ(2,NYM)
     *          +A(1,1,NY )*DZ(1,NY )+A(2,1,NY )*DZ(2,NY )
C Interior columns
      DO I=2,NXM
         IM=I-1
         IP=I+1
         WORK(I,NY)=A(5,IM,NYM)*DZ(IM,NYM)+A(4,I ,NYM)*DZ(I ,NYM)
     *             +A(3,IP,NYM)*DZ(IP,NYM)+A(2,IM,NY )*DZ(IM,NY )
     *             +A(1,I ,NY )*DZ(I ,NY )+A(2,I ,NY )*DZ(IP,NY )
      ENDDO
C Last column
      WORK(NX,NY)=A(5,NXM,NYM)*DZ(NXM,NYM)+A(4,NX ,NYM)*DZ(NX,NYM)
     *           +A(2,NXM,NY )*DZ(NXM,NY )+A(1,NX ,NY )*DZ(NX,NY )
C Calculate RD as a dot product
      RD=0.0
      DO J=1,NY
         DO I=1,NX
            RD=RD+DZ(I,J)*WORK(I,J)
         ENDDO
      ENDDO
      NOPS=NOPS+19*NX*NY-12*(NX+NY)+8
      RETURN
      END
