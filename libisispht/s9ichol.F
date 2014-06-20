      SUBROUTINE S9ICHOL(A,DL,NX,NY,IBAD,IOCT)
      IMPLICIT NONE
C
C_TITLE S9ICHOL - Incomplete Cholesky decomposition routine
C
#include "clinom_ops.inc"
C ARGS  TYPE       VARIABLE      I/O    DESCRIPTION
	INTEGER*4  NX,NY	 !I	Dimensions of mesh on which
C					problem is based:  A is a matrix
C					relating values of a function at
C					a set of NX by NY points
	REAL*4     A(5,NX,NY)	 !I	Matrix to be factored
	REAL*4     DL(5,NX,NY)	 !O	Approximate factorization 
C					(A, DL are both in compressed form)
	INTEGER*4  IBAD		 !O 	Number of bad pivots encountered
	INTEGER*4  IOCT	!I	Controls sweep direction.  The fwd direction is
C					-2 = +samp -line(inner, outer loops)
C					-1 = -line +samp 
C					+1 = +line +samp
C					+2 = +samp +line
C
C_DESC  Incomplete Cholesky decomposition routine: places the approximate
C       factorization of the matrix A in DL. This is like the familiar 
C       decomposition [A]=[L][D][L]T (with the choice of normalization
C       Lii=1 so [D] can share storage with [L]) -- except that if Aij=0,
C       we ignore any contributions to Lij generated along the way ([L] 
C       has the same sparsity as [A]).  Pivoting is not possible, so bad 
C       pivots are "fixed" in an ad hoc way that preserves diagonal dominance. 
C       If there are too many bad pivots the factorization is unlikely to be 
C       useful.
C
C       In each case, sweep for back substitution is the reverse of that 
C       above. The IOCT=+2 sweep direction is the old default, and is used 
C       for all other values of IOCT that may be supplied in error.  
C
C_HIST  10-AUG-89 Randolph Kirk, USGS Flagstaff, Original Version
C       25-Jul-02 RLK Now direction in which rows are swept to vary, 
C                 based on IOCT
C

      INTEGER*4 I,J,K,NXM,NYM,IM,JM,IP,JP
      REAL*4 TOL,C

      TOL=(1.0E-6)
      NXM=NX-1
      NYM=NY-1
C      CALL CLEAR(DL,5*NX*NY)	! Use this if R2R is unavailable
      CALL U_FILL4(0.0,5*NX*NY,DL)
      IBAD=0
      IF (IOCT.EQ.-2) THEN
C Last row			See interior row & column for 
C First column			more detailed comments
         DL(1,1,NY )=A(1,1,NY )
         DL(2,1,NY )=A(2,1,NY )
         DL(4,1,NYM)=A(4,1,NYM)
         DL(3,2,NYM)=A(3,2,NYM)
         C=(ABS(DL(2,1,NY))+ABS(DL(4,1,NYM))+ABS(DL(3,2,NYM)))
         IF (DL(1,1,NY).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) 1,NY,C
 1000       FORMAT('BAD PIVOT FOR NODE ',I3,',',I3,'. C=',F9.4,'.')
            DL(1,1,NY)=C
         ELSE
            C=DL(1,1,NY)
         END IF
         DL(2,1,NY )=DL(2,1,NY )/C
         DL(4,1,NYM)=DL(4,1,NYM)/C
         DL(3,2,NYM)=DL(3,2,NYM)/C
C Interior columns
         DO I=2,NXM
            IM=I-1
            IP=I+1
            DL(1,I ,NY )=A(1,I ,NY )-DL(1,IM,NY)*DL(2,IM,NY)*DL(2,IM,NY)
            DL(2,I ,NY )=A(2,I ,NY )
            DL(5,IM,NYM)=A(5,IM,NYM)
     *                  -DL(1,IM,NY)*DL(2,IM,NY)*DL(4,IM,NYM)
            DL(4,I ,NYM)=A(4,I ,NYM)
     *                  -DL(1,IM,NY)*DL(2,IM,NY)*DL(3,I ,NYM)
            DL(3,IP,NYM)=A(3,IP,NYM)
            C=ABS(DL(2,I,NY))+ABS(DL(5,IM,NYM))
     *       +ABS(DL(4,I,NYM))+ABS(DL(3,IP,NYM))
     *       +ABS(DL(2,IM,NY)*DL(1,IM,NY))
            IF (DL(1,I,NY).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) I,NY,C
               DL(1,I,NY)=C
            ELSE
               C=DL(1,I,NY)
            END IF
            DL(2,I ,NY )=DL(2,I ,NY )/C
            DL(5,IM,NYM)=DL(5,IM,NYM)/C
            DL(4,I ,NYM)=DL(4,I ,NYM)/C
            DL(3,IP,NYM)=DL(3,IP,NYM)/C
         ENDDO
C Last column
         DL(1,NX ,NY )=A(1,NX ,NY )
     *                -DL(1,NXM,NY)*DL(2,NXM,NY)*DL(2,NXM,NY )
         DL(5,NXM,NYM)=A(5,NXM,NYM)
     *                -DL(1,NXM,NY)*DL(2,NXM,NY)*DL(4,NXM,NYM)
         DL(4,NX ,NYM)=A(4,NX ,NYM)
     *                -DL(1,NXM,NY)*DL(2,NXM,NY)*DL(3,NX ,NYM)
         C=ABS(DL(5,NXM,NYM))+ABS(DL(4,NX,NYM))
     *    +ABS(DL(2,NXM,NY)*DL(1,NXM,NY))
         IF (DL(1,NX,NY).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) NX,NY,C
            DL(1,NX,NY)=C
         ELSE
            C=DL(1,NX,NY)
         END IF
         DL(5,NXM,NYM)=DL(5,NXM,NYM)/C
         DL(4,NX ,NYM)=DL(4,NX ,NYM)/C
C Interior rows
         DO J=NYM,2,-1
            JM=J-1
            JP=J+1
C First column
            DL(1,1,J)=A(1,1,J)-DL(1,1,JP)*DL(4,1,J)*DL(4,1,J)
     *                        -DL(1,2,JP)*DL(5,1,J)*DL(5,1,J)
            DL(2,1,J)=A(2,1,J)-DL(1,1,JP)*DL(4,1,J)*DL(3,2,J)
     *                        -DL(1,2,JP)*DL(3,1,J)*DL(4,2,J)
            DL(4,1,JM)=A(4,1,JM)
            DL(3,2,JM)=A(3,2,JM)
            C=ABS(DL(2,1,J))+ABS(DL(4,1,JM))+ABS(DL(3,2,JM))
     *       +ABS(DL(4,1,J)*DL(1,1,JP))
     *       +ABS(DL(5,1,J)*DL(1,2,JP))
            IF (DL(1,1,J).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) 1,J,C
               DL(1,1,J)=C
            ELSE
               C=DL(1,1,J)
            END IF
            DL(2,1,J )=DL(2,1,J )/C
            DL(4,1,JM)=DL(4,1,JM)/C
            DL(3,2,JM)=DL(3,2,JM)/C
C Interior columns
            DO I=2,NXM
               IM=I-1
               IP=I+1
C Calculate contributions to DL below current pivot
               DL(1,I ,J )=A(1,I ,J )
     *                    -DL(1,IM,JP)*DL(3,I ,J )*DL(3,I ,J )
     *                    -DL(1,I ,JP)*DL(4,I ,J )*DL(4,I ,J )
     *                    -DL(1,IP,JP)*DL(5,I ,J )*DL(5,I ,J )
     *                    -DL(1,IM,J )*DL(2,IM,J )*DL(2,IM,J )
               DL(2,I ,J )=A(2,I ,J )
     *                    -DL(1,I ,JP)*DL(4,I ,J )*DL(3,IP,J )
     *                    -DL(1,IP,JP)*DL(5,I ,J )*DL(4,IP,J )
               DL(5,IM,JM)=A(5,IM,JM)
     *                    -DL(1,IM,J )*DL(2,IM,J )*DL(4,IM,JM)
               DL(4,I ,JM)=A(4,I ,JM)
     *                    -DL(1,IM,J )*DL(2,IM,J )*DL(3,I ,JM)
               DL(3,IP,JM)=A(3,IP,JM)
C Calculate sum of off-diagonals and compare to pivot
               C=ABS(DL(2,I,J ))+ABS(DL(5,IM,JM))  !present (not yet normalized)
     *          +ABS(DL(4,I,JM))+ABS(DL(3,IP,JM))
     *          +ABS(DL(3,I ,J)*DL(1,IM,JP)) !previous (undo normalization)
     *          +ABS(DL(4,I ,J )*DL(1,I ,JP))
     *          +ABS(DL(5,I ,J )*DL(1,IP,JP))
     *          +ABS(DL(2,IM,J )*DL(1,IM,J ))
               IF (DL(1,I,J).LE.C*TOL) THEN
C Bad pivot "fix" preserves diagonal dominance
                  IBAD=IBAD+1
                  IF (C.LE.0.0) C=1.0
C                  WRITE (6,1000) I,J,C
                  DL(1,I,J)=C
               ELSE
C Good pivot
                  C=DL(1,I,J)
               END IF
C Normalize to pivot or to fix
               DL(2,I ,J )=DL(2,I ,J )/C
               DL(5,IM,JM)=DL(5,IM,JM)/C
               DL(4,I ,JM)=DL(4,I ,JM)/C
               DL(3,IP,JM)=DL(3,IP,JM)/C
            ENDDO
C Last column
            DL(1,NX ,J )=A(1,NX,J)
     *                  -DL(1,NXM,JP)*DL(3,NX ,J )*DL(3,NX ,J )
     *                  -DL(1,NX ,JP)*DL(4,NX ,J )*DL(4,NX ,J )
     *                  -DL(1,NXM,J )*DL(2,NXM,J )*DL(2,NXM,J )
            DL(5,NXM,JM)=A(5,NXM,JM)
     *                  -DL(1,NXM,J )*DL(2,NXM,J )*DL(4,NXM,JM)
            DL(4,NX ,JM)=A(4,NX,JM)
     *                  -DL(1,NXM,J )*DL(2,NXM,J )*DL(3,NX ,JM)
            C=ABS(DL(5,NXM,JM))+ABS(DL(4,NX ,JM))
     *       +ABS(DL(3,NX ,J )*DL(1,NXM,JP))
     *       +ABS(DL(4,NX ,J )*DL(1,NX ,JP))
     *       +ABS(DL(2,NXM,J )*DL(1,NXM,J ))
            IF (DL(1,NX,J).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) NX,J,C
               DL(1,NX,J)=C
            ELSE
               C=DL(1,NX,J)
            END IF
            DL(5,NXM,JM)=DL(5,NXM,JM)/C
            DL(4,NX ,JM)=DL(4,NX ,JM)/C
         ENDDO
C First row
C First column
         DL(1,1,1)=A(1,1,1)-DL(1,1,2)*DL(4,1,1)*DL(4,1,1)
     *                     -DL(1,2,2)*DL(5,1,1)*DL(5,1,1)
         DL(2,1,1)=A(2,1,1)-DL(1,1,2)*DL(4,1,1)*DL(3,2,1)
     *                     -DL(1,2,2)*DL(5,1,1)*DL(4,2,1)
         C=ABS(DL(2,1,1))
     *    +ABS(DL(4,1,1)*DL(1,1,2))
     *    +ABS(DL(5,1,1)*DL(1,2,2))
         IF (DL(1,1,1).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) 1,1,C
            DL(1,1,1)=C
         ELSE
            C=DL(1,1,1)
         END IF
         DL(2,1,1)=DL(2,1,1)/C
C Interior columns
         DO I=2,NXM
            IM=I-1
            IP=I+1
            DL(1,I,1)=A(1,I,1)
     *                -DL(1,IM,2)*DL(3,I ,1)*DL(3,I ,1)
     *                -DL(1,I ,2)*DL(4,I ,1)*DL(4,I ,1)
     *                -DL(1,IP,2)*DL(5,I ,1)*DL(5,I ,1)
     *                -DL(1,IM,1)*DL(2,IM,1)*DL(2,IM,1)
            DL(2,I,1)=A(2,I,1)
     *                -DL(1,I ,2)*DL(4,I ,1)*DL(3,IP,1)
     *                -DL(1,IP,2)*DL(5,I ,1)*DL(4,IP,1)
            C=ABS(DL(2,I,1))
     *       +ABS(DL(3,I ,1)*DL(1,IM,2))
     *       +ABS(DL(4,I ,1)*DL(1,I ,2))
     *       +ABS(DL(5,I ,1)*DL(1,IP,2))
     *       +ABS(DL(2,IM,1)*DL(1,IM,1))
            IF (DL(1,I,1).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) I,1,C
               DL(1,I,1)=C
            ELSE
               C=DL(1,I,1)
            END IF
            DL(2,I,1)=DL(2,I,1)/C
         ENDDO
C Last column
         DL(1,NX,1)=A(1,NX,1)
     *              -DL(1,NXM,2)*DL(3,NX ,1)*DL(3,NX ,1)
     *              -DL(1,NX ,2)*DL(4,NX ,1)*DL(4,NX ,1)
     *              -DL(1,NXM,1)*DL(2,NXM,1)*DL(2,NXM,1)
         C=ABS(DL(3,NX ,1)*DL(1,NXM,2))
     *    +ABS(DL(4,NX ,1)*DL(1,NX ,2))
     *    +ABS(DL(2,NXM,1)*DL(1,NXM,1))
         IF (DL(1,NX,1).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) NX,1,C
            DL(1,NX,1)=C
         END IF
      ELSE IF (IOCT.EQ.-1) THEN
C First column			See interior row & column for 
C Last row			more detailed comments
         DL(1,1,NY )=A(1,1,NY )
         DL(4,1,NYM)=A(4,1,NYM)
         DL(2,1,NY )=A(2,1,NY )
         DL(3,2,NYM)=A(3,2,NYM)
         C=(ABS(DL(4,1,NYM))+ABS(DL(2,1,NY))+ABS(DL(3,2,NYM)))
         IF (DL(1,1,NY).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) 1,NY,C
            DL(1,1,NY)=C
         ELSE
            C=DL(1,1,NY)
         END IF
         DL(4,1,NYM)=DL(4,1,NYM)/C
         DL(2,1,NY )=DL(2,1,NY )/C
         DL(3,2,NYM)=DL(3,2,NYM)/C
C Interior rows
         DO J=NYM,2,-1
            JM=J-1
            JP=J+1
            DL(1,1,J )=A(1,1,J )-DL(1,1,JP)*DL(4,1,J )*DL(4,1,J )
            DL(4,1,JM)=A(4,1,JM)
            DL(5,1,J )=A(5,1,J )-DL(1,1,JP)*DL(4,1,J )*DL(2,1,JP)
            DL(2,1,J )=A(2,1,J )-DL(1,1,JP)*DL(4,1,J )*DL(3,2,J )
            DL(3,2,JM)=A(3,2,JM)
            C=ABS(DL(4,1,JM))+ABS(DL(5,1,J))
     *       +ABS(DL(2,1,J))+ABS(DL(3,2,JM))
     *       +ABS(DL(4,1,J )*DL(1,1,JP))
            IF (DL(1,1,J).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) 1,J,C
               DL(1,1,J)=C
            ELSE
               C=DL(1,1,J)
            END IF
            DL(4,1,JM)=DL(4,1,JM)/C
            DL(5,1,J )=DL(5,1,J )/C
            DL(2,1,J )=DL(2,1,J )/C
            DL(3,2,JM)=DL(3,2,JM)/C
         ENDDO
C First row
         DL(1,1,1)=A(1,1,1)-DL(1,1,2)*DL(4,1,1)*DL(4,1,1)
         DL(5,1,1)=A(5,1,1)-DL(1,1,2)*DL(4,1,1)*DL(2,1,2)
         DL(2,1,1)=A(2,1,1)-DL(1,1,2)*DL(4,1,1)*DL(3,2,1)
         C=ABS(DL(5,1,1))+ABS(DL(2,1,1))
     *    +ABS(DL(4,1,1)*DL(1,1,2))
         IF (DL(1,1,1).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) 1,1,C
            DL(1,1,1)=C
         ELSE
            C=DL(1,1,1)
         END IF
         DL(5,1,1)=DL(5,1,1)/C
         DL(2,1,1)=DL(2,1,1)/C
C Interior columns
         DO I=2,NXM
            IM=I-1
            IP=I+1
C Last row
            DL(1,I ,NY )=A(1,I ,NY )
     *                  -DL(1,IM,NY )*DL(2,IM,NY )*DL(2,IM,NY )
     *                  -DL(1,IM,NYM)*DL(5,IM,NYM)*DL(5,IM,NYM)
            DL(4,I ,NYM)=A(4,I ,NYM)
     *                  -DL(1,IM,NY )*DL(2,IM,NY )*DL(3,I ,NYM)
     *                  -DL(1,IM,NYM)*DL(5,IM,NYM)*DL(2,IM,NYM)
            DL(2,I ,NY )=A(2,I ,NY )
            DL(3,IP,NYM)=A(3,IP,NYM)
            C=ABS(DL(4,I,NYM))+ABS(DL(2,I,NY))+ABS(DL(3,IP,NYM))
     *       +ABS(DL(2,IM,NY )*DL(1,IM,NY ))
     *       +ABS(DL(5,IM,NYM)*DL(1,IM,NYM))
            IF (DL(1,I,NY).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) I,NY,C
               DL(1,I,NY)=C
            ELSE
               C=DL(1,I,NY)
            END IF
            DL(4,I ,NYM)=DL(4,I ,NYM)/C
            DL(2,I ,NY )=DL(2,I ,NY )/C
            DL(3,IP,NYM)=DL(3,IP,NYM)/C
C Interior rows
            DO J=NYM,2,-1
               JM=J-1
               JP=J+1
C Calculate contributions to DL below current pivot
               DL(1,I ,J )=A(1,I ,J )
     *                    -DL(1,IM,JP)*DL(3,I ,J )*DL(3,I ,J )
     *                    -DL(1,IM,J )*DL(2,IM,J )*DL(2,IM,J )
     *                    -DL(1,IM,JM)*DL(5,IM,JM)*DL(5,IM,JM)
     *                    -DL(1,I ,JP)*DL(4,I ,J )*DL(4,I ,J )
               DL(4,I ,JM)=A(4,I ,JM)
     *                    -DL(1,IM,J )*DL(2,IM,J )*DL(3,I ,JM)
     *                    -DL(1,IM,JM)*DL(5,IM,JM)*DL(2,IM,JM)
               DL(5,I ,J )=A(5,I ,J )
     *                    -DL(1,I ,JP)*DL(4,I ,J )*DL(2,I ,JP)
               DL(2,I ,J )=A(2,I ,J )
     *                    -DL(1,I ,JP)*DL(4,I ,J )*DL(3,IP,J )
               DL(3,IP,JM)=A(3,IP,JM)
C Calculate sum of off-diagonals and compare to pivot
               C=ABS(DL(4,I,JM))+ABS(DL(5,I,J))  !present (not yet normalized)
     *          +ABS(DL(2,I,J))+ABS(DL(3,IP,JM))
     *          +ABS(DL(3,I ,J )*DL(1,IM,JP))  !previous (undo normalization)
     *          +ABS(DL(2,IM,J )*DL(1,IM,J ))
     *          +ABS(DL(5,IM,JM)*DL(1,IM,JM))
     *          +ABS(DL(4,I ,J )*DL(1,I ,JP))
               IF (DL(1,I,J).LE.C*TOL) THEN
C Bad pivot "fix" preserves diagonal dominance
                  IBAD=IBAD+1
                  IF (C.LE.0.0) C=1.0
C                  WRITE (6,1000) I,J,C
                  DL(1,I,J)=C
               ELSE
C Good pivot
                  C=DL(1,I,J)
               END IF
C Normalize to pivot or to fix
               DL(4,I ,JM)=DL(4,I ,JM)/C
               DL(5,I ,J )=DL(5,I ,J )/C
               DL(2,I ,J )=DL(2,I ,J )/C
               DL(3,IP,JM)=DL(3,IP,JM)/C
            ENDDO
C First row
            DL(1,I,1)=A(1,I,1)
     *               -DL(1,IM,2)*DL(3,I ,1)*DL(3,I ,1)
     *               -DL(1,IM,1)*DL(2,IM,1)*DL(2,IM,1)
     *               -DL(1,I ,2)*DL(4,I ,1)*DL(4,I ,1)
            DL(5,I,1)=A(5,I,1)
     *               -DL(1,I ,2)*DL(4,I ,1)*DL(2,I ,2)
            DL(2,I,1)=A(2,I,1)
     *               -DL(1,I ,2)*DL(4,I ,1)*DL(3,IP,1)
            C=ABS(DL(5,I,1))+ABS(DL(2,I,1))
     *       +ABS(DL(3,I ,1)*DL(1,IM,2))
     *       +ABS(DL(2,IM,1)*DL(1,IM,1))
     *       +ABS(DL(4,I ,1)*DL(1,I ,2))
            IF (DL(1,I,1).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) I,1,C
               DL(1,I,1)=C
            ELSE
               C=DL(1,I,1)
            END IF
            DL(5,I,1)=DL(5,I,1)/C
            DL(2,I,1)=DL(2,I,1)/C
         ENDDO
C Last column
C Last row
         DL(1,NX,NY )=A(1,NX,NY )
     *               -DL(1,NXM,NY )*DL(2,NXM,NY )*DL(2,NXM,NY )
     *               -DL(1,NXM,NYM)*DL(5,NXM,NYM)*DL(5,NXM,NYM)
         DL(4,NX,NYM)=A(4,NX,NYM)
     *               -DL(1,NXM,NY )*DL(2,NXM,NY )*DL(3,NX ,NYM)
     *               -DL(1,NXM,NYM)*DL(5,NXM,NYM)*DL(2,NXM,NYM)
         C=ABS(DL(4,NX,NYM))
     *    +ABS(DL(2,NXM,NY )*DL(1,NXM,NY ))
     *    +ABS(DL(5,NXM,NYM)*DL(1,NXM,NYM))
         IF (DL(1,NX,NY).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) NX,NY,C
            DL(1,NX,NY)=C
         ELSE
            C=DL(1,NX,NY)
         END IF
         DL(4,NX,NYM)=DL(4,NX,NYM)/C
C Interior rows
         DO J=NYM,2,-1
            JM=J-1
            JP=J+1
            DL(1,NX,J )=A(1,NX,J )
     *                 -DL(1,NXM,JP)*DL(3,NX ,J )*DL(3,NX ,J )
     *                 -DL(1,NXM,J )*DL(2,NXM,J )*DL(2,NXM,J )
     *                 -DL(1,NXM,JM)*DL(5,NXM,JM)*DL(5,NXM,JM)
     *                 -DL(1,NX ,JP)*DL(4,NX ,J )*DL(4,NX ,J )
            DL(4,NX,JM)=A(4,NX,JM)
     *                 -DL(1,NXM,J )*DL(2,NXM,J )*DL(3,NX ,JM)
     *                 -DL(1,NXM,JM)*DL(5,NXM,JM)*DL(2,NXM,JM)
            C=ABS(DL(4,NX,JM))
     *       +ABS(DL(3,NX ,J )*DL(1,NXM,JP))
     *       +ABS(DL(2,NXM,J )*DL(1,NXM,J ))
     *       +ABS(DL(5,NXM,JM)*DL(1,NXM,JM))
     *       +ABS(DL(4,NX ,J )*DL(1,NX ,JP))
            IF (DL(1,NX,J).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) NX,J,C
               DL(1,NX,J)=C
            ELSE
               C=DL(1,NX,J)
            END IF
            DL(4,NX,JM)=DL(4,NX,JM)/C
         ENDDO
C First row
         DL(1,NX,1)=A(1,NX,1)
     *              -DL(1,NXM,2)*DL(3,NX ,1)*DL(3,NX ,1)
     *              -DL(1,NXM,1)*DL(2,NXM,1)*DL(2,NXM,1)
     *              -DL(1,NX ,2)*DL(4,NX ,1)*DL(4,NX ,1)
         C=ABS(DL(3,NX ,1)*DL(1,NXM,2))
     *    +ABS(DL(2,NXM,1)*DL(1,NXM,1))
     *    +ABS(DL(4,NX ,1)*DL(1,NX ,2))
         IF (DL(1,NX,1).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) NX,1,C
            DL(1,NX,1)=C
         END IF
      ELSE IF (IOCT.EQ.+1) THEN
C First column			See interior row & column for 
C First row			more detailed comments
         DL(1,1,1)=A(1,1,1)
         DL(4,1,1)=A(4,1,1)
         DL(2,1,1)=A(2,1,1)
         DL(5,1,1)=A(5,1,1)
         C=(ABS(DL(4,1,1))+ABS(DL(2,1,1))+ABS(DL(5,1,1)))
         IF (DL(1,1,1).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) 1,1,C
            DL(1,1,1)=C
         ELSE
            C=DL(1,1,1)
         END IF
         DL(4,1,1)=DL(4,1,1)/C
         DL(2,1,1)=DL(2,1,1)/C
         DL(5,1,1)=DL(5,1,1)/C
C Interior rows
         DO J=2,NYM
            JM=J-1
            JP=J+1
            DL(1,1,J )=A(1,1,J )-DL(1,1,JM)*DL(4,1,JM)*DL(4,1,JM)
            DL(4,1,J )=A(4,1,J )
            DL(3,2,JM)=A(3,2,JM)-DL(1,1,JM)*DL(4,1,JM)*DL(2,1,JM)
            DL(2,1,J )=A(2,1,J )-DL(1,1,JM)*DL(4,1,JM)*DL(5,1,JM)
            DL(5,1,J)=A(5,1,J)
            C=ABS(DL(4,1,J))+ABS(DL(3,2,JM))
     *       +ABS(DL(2,1,J))+ABS(DL(5,1,J))
     *       +ABS(DL(4,1,JM)*DL(1,1,JM))
            IF (DL(1,1,J).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) 1,J,C
               DL(1,1,J)=C
            ELSE
               C=DL(1,1,J)
            END IF
            DL(4,1,J )=DL(4,1,J )/C
            DL(3,2,JM)=DL(3,2,JM)/C
            DL(2,1,J )=DL(2,1,J )/C
            DL(5,1,J )=DL(5,1,J )/C
         ENDDO
C Last row
         DL(1,1,NY )=A(1,1,NY )-DL(1,1,NYM)*DL(4,1,NYM)*DL(4,1,NYM)
         DL(3,2,NYM)=A(3,2,NYM)-DL(1,1,NYM)*DL(4,1,NYM)*DL(2,1,NYM)
         DL(2,1,NY )=A(2,1,NY )-DL(1,1,NYM)*DL(4,1,NYM)*DL(5,1,NYM)
         C=ABS(DL(3,2,NYM))+ABS(DL(2,1,NY))
     *    +ABS(DL(4,1,NYM)*DL(1,1,NYM))
         IF (DL(1,1,NY).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) 1,NY,C
            DL(1,1,NY)=C
         ELSE
            C=DL(1,1,NY)
         END IF
         DL(3,2,NYM)=DL(3,2,NYM)/C
         DL(2,1,NY )=DL(2,1,NY )/C
C Interior columns
         DO I=2,NXM
            IM=I-1
            IP=I+1
C First row
            DL(1,I,1)=A(1,I,1)-DL(1,IM,1)*DL(2,IM,1)*DL(2,IM,1)
     *                        -DL(1,IM,2)*DL(3,I ,1)*DL(3,I ,1)
            DL(4,I,1)=A(4,I,1)-DL(1,IM,1)*DL(2,IM,1)*DL(5,IM,1)
     *                        -DL(1,IM,2)*DL(3,I ,1)*DL(2,IM,2)
            DL(2,I,1)=A(2,I,1)
            DL(5,I,1)=A(5,I,1)
            C=ABS(DL(4,I,1))+ABS(DL(2,I,1))+ABS(DL(5,I,1))
     *       +ABS(DL(2,IM,1)*DL(1,IM,1))
     *       +ABS(DL(3,I ,1)*DL(1,IM,2))
            IF (DL(1,I,1).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) I,1,C
               DL(1,I,1)=C
            ELSE
               C=DL(1,I,1)
            END IF
            DL(4,I,1)=DL(4,I,1)/C
            DL(2,I,1)=DL(2,I,1)/C
            DL(5,I,1)=DL(5,I,1)/C
C Interior rows
            DO J=2,NYM
               JM=J-1
               JP=J+1
C Calculate contributions to DL below current pivot
               DL(1,I ,J )=A(1,I ,J )
     *                    -DL(1,IM,JM)*DL(5,IM,JM)*DL(5,IM,JM)
     *                    -DL(1,IM,J )*DL(2,IM,J )*DL(2,IM,J )
     *                    -DL(1,IM,JP)*DL(3,I ,J )*DL(3,I ,J )
     *                    -DL(1,I ,JM)*DL(4,I ,JM)*DL(4,I ,JM)
               DL(4,I ,J )=A(4,I ,J )
     *                    -DL(1,IM,J )*DL(2,IM,J )*DL(5,IM,J )
     *                    -DL(1,IM,JP)*DL(3,I ,J )*DL(2,IM,JP)
               DL(3,IP,JM)=A(3,IP,JM)
     *                    -DL(1,I ,JM)*DL(4,I ,JM)*DL(2,I ,JM)
               DL(2,I ,J )=A(2,I ,J )
     *                    -DL(1,I ,JM)*DL(4,I ,JM)*DL(5,I ,JM)
               DL(5,I ,J )=A(5,I ,J )
C Calculate sum of off-diagonals and compare to pivot
               C=ABS(DL(4,I,J))+ABS(DL(3,IP,JM))  !present (not yet normalized)
     *          +ABS(DL(2,I,J))+ABS(DL(5,I,J))
     *          +ABS(DL(5,IM,JM)*DL(1,IM,JM))  !previous (undo normalization)
     *          +ABS(DL(2,IM,J )*DL(1,IM,J ))
     *          +ABS(DL(3,I ,J )*DL(1,IM,JP))
     *          +ABS(DL(4,I ,JM)*DL(1,I ,JM))
               IF (DL(1,I,J).LE.C*TOL) THEN
C Bad pivot "fix" preserves diagonal dominance
                  IBAD=IBAD+1
                  IF (C.LE.0.0) C=1.0
C                  WRITE (6,1000) I,J,C
                  DL(1,I,J)=C
               ELSE
C Good pivot
                  C=DL(1,I,J)
               END IF
C Normalize to pivot or to fix
               DL(4,I ,J )=DL(4,I ,J )/C
               DL(3,IP,JM)=DL(3,IP,JM)/C
               DL(2,I ,J )=DL(2,I ,J )/C
               DL(5,I ,J )=DL(5,I ,J )/C
            ENDDO
C Last row
            DL(1,I ,NY )=A(1,I ,NY )
     *                -DL(1,IM,NYM)*DL(5,IM,NYM)*DL(5,IM,NYM)
     *                -DL(1,IM,NY )*DL(2,IM,NY )*DL(2,IM,NY )
     *                -DL(1,I ,NYM)*DL(4,I ,NYM)*DL(4,I ,NYM)
            DL(3,IP,NYM)=A(3,IP,NYM)
     *                -DL(1,I ,NYM)*DL(4,I ,NYM)*DL(2,I ,NYM)
            DL(2,I ,NY)=A(2,I ,NY)
     *                -DL(1,I ,NYM)*DL(4,I ,NYM)*DL(5,I ,NYM)
            C=ABS(DL(3,IP,NYM))+ABS(DL(2,I,NY))
     *       +ABS(DL(5,IM,NYM)*DL(1,IM,NYM))
     *       +ABS(DL(2,IM,NY )*DL(1,IM,NY ))
     *       +ABS(DL(4,I ,NYM)*DL(1,I ,NYM))
            IF (DL(1,I,NY).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) I,NY,C
               DL(1,I,NY)=C
            ELSE
               C=DL(1,I,NY)
            END IF
            DL(3,IP,NYM)=DL(3,IP,NYM)/C
            DL(2,I ,NY )=DL(2,I ,NY )/C
         ENDDO
C Last column
C First row
         DL(1,NX,1)=A(1,NX,1)-DL(1,NXM,1)*DL(2,NXM,1)*DL(2,NXM,2)
     *                       -DL(1,NXM,2)*DL(3,NX ,1)*DL(3,NX ,1)
         DL(4,NX,1)=A(4,NX,1)-DL(1,NXM,1)*DL(2,NXM,1)*DL(5,NXM,1)
     *                       -DL(1,NXM,2)*DL(3,NX ,1)*DL(2,NXM,2)
         C=ABS(DL(4,NX,1))
     *    +ABS(DL(2,NXM,1)*DL(1,NXM,1))
     *    +ABS(DL(3,NX ,1)*DL(1,NXM,2))
         IF (DL(1,NX,1).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) NX,1,C
            DL(1,NX,1)=C
         ELSE
            C=DL(1,NX,1)
         END IF
         DL(4,NX,1)=DL(4,NX,1)/C
C Interior rows
         DO J=2,NYM
            JM=J-1
            JP=J+1
            DL(1,NX,J)=A(1,NX,J)
     *                -DL(1,NXM,JM)*DL(5,NXM,JM)*DL(5,NXM,JM)
     *                -DL(1,NXM,J )*DL(2,NXM,J )*DL(2,NXM,J )
     *                -DL(1,NXM,JP)*DL(3,NX ,J )*DL(3,NX ,J )
     *                -DL(1,NX ,JM)*DL(4,NX ,JM)*DL(4,NX ,JM)
            DL(4,NX,J)=A(4,NX,J)
     *                -DL(1,NXM,J )*DL(2,NXM,J )*DL(5,NXM,J )
     *                -DL(1,NXM,JP)*DL(3,NX ,J )*DL(2,NXM,JP)
            C=ABS(DL(4,NX,J))
     *       +ABS(DL(5,NXM,JM)*DL(1,NXM,JM))
     *       +ABS(DL(2,NXM,J )*DL(1,NXM,J ))
     *       +ABS(DL(3,NX ,J )*DL(1,NXM,JP))
     *       +ABS(DL(4,NX ,JM)*DL(1,NX ,JM))
            IF (DL(1,NX,J).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) NX,J,C
               DL(1,NX,J)=C
            ELSE
               C=DL(1,NX,J)
            END IF
            DL(4,NX,J)=DL(4,NX,J)/C
         ENDDO
C Last row
         DL(1,NX,NY)=A(1,NX,NY)
     *              -DL(1,NXM,NYM)*DL(5,NXM,NYM)*DL(5,NXM,NYM)
     *              -DL(1,NXM,NY )*DL(2,NXM,NY )*DL(2,NXM,NY )
     *              -DL(1,NX ,NYM)*DL(4,NX ,NYM)*DL(4,NX ,NYM)
         C=ABS(DL(5,NXM,NYM)*DL(1,NXM,NYM))
     *    +ABS(DL(2,NXM,NY )*DL(1,NXM,NY ))
     *    +ABS(DL(4,NX ,NYM)*DL(1,NX ,NYM))
         IF (DL(1,NX,NY).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) NX,NY,C
            DL(1,NX,NY)=C
         END IF
      ELSE	! Orig algorithm used for IOCT 2 and any bad values
C First row			See interior row & column for 
C First column			more detailed comments
         DL(1,1,1)=A(1,1,1)
         DL(2,1,1)=A(2,1,1)
         DL(4,1,1)=A(4,1,1)
         DL(5,1,1)=A(5,1,1)
         C=(ABS(DL(2,1,1))+ABS(DL(4,1,1))+ABS(DL(5,1,1)))
         IF (DL(1,1,1).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) 1,1,C
            DL(1,1,1)=C
         ELSE
            C=DL(1,1,1)
         END IF
         DL(2,1,1)=DL(2,1,1)/C
         DL(4,1,1)=DL(4,1,1)/C
         DL(5,1,1)=DL(5,1,1)/C
C Interior columns
         DO I=2,NXM
            IM=I-1
            IP=I+1
            DL(1,I,1)=A(1,I,1)-DL(1,IM,1)*DL(2,IM,1)*DL(2,IM,1)
            DL(2,I,1)=A(2,I,1)
            DL(3,I,1)=A(3,I,1)-DL(1,IM,1)*DL(2,IM,1)*DL(4,IM,1)
            DL(4,I,1)=A(4,I,1)-DL(1,IM,1)*DL(2,IM,1)*DL(5,IM,1)
            DL(5,I,1)=A(5,I,1)
            C=ABS(DL(2,I,1))+ABS(DL(3,I,1))
     *       +ABS(DL(4,I,1))+ABS(DL(5,I,1))
     *       +ABS(DL(2,IM,1)*DL(1,IM,1))
            IF (DL(1,I,1).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) I,1,C
               DL(1,I,1)=C
            ELSE
               C=DL(1,I,1)
            END IF
            DL(2,I,1)=DL(2,I,1)/C
            DL(3,I,1)=DL(3,I,1)/C
            DL(4,I,1)=DL(4,I,1)/C
            DL(5,I,1)=DL(5,I,1)/C
         ENDDO
C Last column
         DL(1,NX,1)=A(1,NX,1)-DL(1,NXM,1)*DL(2,NXM,1)*DL(2,NXM,1)
         DL(3,NX,1)=A(3,NX,1)-DL(1,NXM,1)*DL(2,NXM,1)*DL(4,NXM,1)
         DL(4,NX,1)=A(4,NX,1)-DL(1,NXM,1)*DL(2,NXM,1)*DL(5,NXM,1)
         C=ABS(DL(3,NX,1))+ABS(DL(4,NX,1))
     *    +ABS(DL(2,NXM,1)*DL(1,NXM,1))
         IF (DL(1,NX,1).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) NX,1,C
            DL(1,NX,1)=C
         ELSE
            C=DL(1,NX,1)
         END IF
         DL(3,NX,1)=DL(3,NX,1)/C
         DL(4,NX,1)=DL(4,NX,1)/C
C Interior rows
         DO J=2,NYM
            JM=J-1
            JP=J+1
C First column
            DL(1,1,J)=A(1,1,J)-DL(1,1,JM)*DL(4,1,JM)*DL(4,1,JM)
     *                        -DL(1,2,JM)*DL(3,2,JM)*DL(3,2,JM)
            DL(2,1,J)=A(2,1,J)-DL(1,1,JM)*DL(4,1,JM)*DL(5,1,JM)
     *                        -DL(1,2,JM)*DL(3,2,JM)*DL(4,2,JM)
            DL(4,1,J)=A(4,1,J)
            DL(5,1,J)=A(5,1,J)
            C=ABS(DL(2,1,J))+ABS(DL(4,1,J))+ABS(DL(5,1,J))
     *       +ABS(DL(4,1,JM)*DL(1,1,JM))
     *       +ABS(DL(3,2,JM)*DL(1,2,JM))
            IF (DL(1,1,J).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) 1,J,C
               DL(1,1,J)=C
            ELSE
               C=DL(1,1,J)
            END IF
            DL(2,1,J)=DL(2,1,J)/C
            DL(4,1,J)=DL(4,1,J)/C
            DL(5,1,J)=DL(5,1,J)/C
C Interior columns
            DO I=2,NXM
               IM=I-1
               IP=I+1
C Calculate contributions to DL below current pivot
               DL(1,I,J)=A(1,I,J)-DL(1,IM,JM)*DL(5,IM,JM)*DL(5,IM,JM)
     *                           -DL(1,I ,JM)*DL(4,I ,JM)*DL(4,I ,JM)
     *                           -DL(1,IP,JM)*DL(3,IP,JM)*DL(3,IP,JM)
     *                           -DL(1,IM,J )*DL(2,IM,J )*DL(2,IM,J )
               DL(2,I,J)=A(2,I,J)-DL(1,I ,JM)*DL(4,I ,JM)*DL(5,I ,JM)
     *                           -DL(1,IP,JM)*DL(3,IP,JM)*DL(4,IP,JM)
               DL(3,I,J)=A(3,I,J)-DL(1,IM,J )*DL(2,IM,J )*DL(4,IM,J )
               DL(4,I,J)=A(4,I,J)-DL(1,IM,J )*DL(2,IM,J )*DL(5,IM,J )
               DL(5,I,J)=A(5,I,J)
C Calculate sum of off-diagonals and compare to pivot
               C=ABS(DL(2,I,J))+ABS(DL(3,I,J))  !present (not yet normalized)
     *          +ABS(DL(4,I,J))+ABS(DL(5,I,J))
     *          +ABS(DL(5,IM,JM)*DL(1,IM,JM))  !previous (undo normalization)
     *          +ABS(DL(4,I ,JM)*DL(1,I ,JM))
     *          +ABS(DL(3,IP,JM)*DL(1,IP,JM))
     *          +ABS(DL(2,IM,J )*DL(1,IM,J ))
               IF (DL(1,I,J).LE.C*TOL) THEN
C Bad pivot "fix" preserves diagonal dominance
                  IBAD=IBAD+1
                  IF (C.LE.0.0) C=1.0
C                  WRITE (6,1000) I,J,C
                  DL(1,I,J)=C
               ELSE
C Good pivot
                  C=DL(1,I,J)
               END IF
C Normalize to pivot or to fix
               DL(2,I,J)=DL(2,I,J)/C
               DL(3,I,J)=DL(3,I,J)/C
               DL(4,I,J)=DL(4,I,J)/C
               DL(5,I,J)=DL(5,I,J)/C
            ENDDO
C Last column
            DL(1,NX,J)=A(1,NX,J)
     *                -DL(1,NXM,JM)*DL(5,NXM,JM)*DL(5,NXM,JM)
     *                -DL(1,NX ,JM)*DL(4,NX ,JM)*DL(4,NX ,JM)
     *                -DL(1,NXM,J )*DL(2,NXM,J )*DL(2,NXM,J )
            DL(3,NX,J)=A(3,NX,J)
     *                -DL(1,NXM,J )*DL(2,NXM,J )*DL(4,NXM,J )
            DL(4,NX,J)=A(4,NX,J)
     *                -DL(1,NXM,J )*DL(2,NXM,J )*DL(5,NXM,J )
            C=ABS(DL(3,NX ,J))+ABS(DL(4,NX ,J))
     *       +ABS(DL(5,NXM,JM)*DL(1,NXM,JM))
     *       +ABS(DL(4,NX ,JM)*DL(1,NX ,JM))
     *       +ABS(DL(2,NXM,J )*DL(1,NXM,J ))
            IF (DL(1,NX,J).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) NX,J,C
               DL(1,NX,J)=C
            ELSE
               C=DL(1,NX,J)
            END IF
            DL(3,NX,J)=DL(3,NX,J)/C
            DL(4,NX,J)=DL(4,NX,J)/C
         ENDDO
C Last row
C First column
         DL(1,1,NY)=A(1,1,NY)-DL(1,1,NYM)*DL(4,1,NYM)*DL(4,1,NYM)
     *                       -DL(1,2,NYM)*DL(3,2,NYM)*DL(3,2,NYM)
         DL(2,1,NY)=A(2,1,NY)-DL(1,1,NYM)*DL(4,1,NYM)*DL(5,1,NYM)
     *                       -DL(1,2,NYM)*DL(3,2,NYM)*DL(4,2,NYM)
         C=ABS(DL(2,1,NY))
     *    +ABS(DL(4,1,NYM)*DL(1,1,NYM))
     *    +ABS(DL(3,2,NYM)*DL(1,2,NYM))
         IF (DL(1,1,NY).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) 1,NY,C
            DL(1,1,NY)=C
         ELSE
            C=DL(1,1,NY)
         END IF
         DL(2,1,NY)=DL(2,1,NY)/C
C Interior columns
         DO I=2,NXM
            IM=I-1
            IP=I+1
            DL(1,I,NY)=A(1,I,NY)
     *                -DL(1,IM,NYM)*DL(5,IM,NYM)*DL(5,IM,NYM)
     *                -DL(1,I ,NYM)*DL(4,I ,NYM)*DL(4,I ,NYM)
     *                -DL(1,IP,NYM)*DL(3,IP,NYM)*DL(3,IP,NYM)
     *                -DL(1,IM,NY )*DL(2,IM,NY )*DL(2,IM,NY )
            DL(2,I,NY)=A(2,I,NY)
     *                -DL(1,I ,NYM)*DL(4,I ,NYM)*DL(5,I ,NYM)
     *                -DL(1,IP,NYM)*DL(3,IP,NYM)*DL(4,IP,NYM)
            C=ABS(DL(2,I,NY))
     *       +ABS(DL(5,IM,NYM)*DL(1,IM,NYM))
     *       +ABS(DL(4,I ,NYM)*DL(1,I ,NYM))
     *       +ABS(DL(3,IP,NYM)*DL(1,IP,NYM))
     *       +ABS(DL(2,IM,NY )*DL(1,IM,NY ))
            IF (DL(1,I,NY).LE.C*TOL) THEN
               IBAD=IBAD+1
               IF (C.LE.0.0) C=1.0
C               WRITE (6,1000) I,NY,C
               DL(1,I,NY)=C
            ELSE
               C=DL(1,I,NY)
            END IF
            DL(2,I,NY)=DL(2,I,NY)/C
         ENDDO
C Last column
         DL(1,NX,NY)=A(1,NX,NY)
     *              -DL(1,NXM,NYM)*DL(5,NXM,NYM)*DL(5,NXM,NYM)
     *              -DL(1,NX ,NYM)*DL(4,NX ,NYM)*DL(4,NX ,NYM)
     *              -DL(1,NXM,NY )*DL(2,NXM,NY )*DL(2,NXM,NY )
         C=ABS(DL(5,NXM,NYM)*DL(1,NXM,NYM))
     *    +ABS(DL(4,NX ,NYM)*DL(1,NX ,NYM))
     *    +ABS(DL(2,NXM,NY )*DL(1,NXM,NY ))
         IF (DL(1,NX,NY).LE.C*TOL) THEN
            IBAD=IBAD+1
            IF (C.LE.0.0) C=1.0
C            WRITE (6,1000) NX,NY,C
            DL(1,NX,NY)=C
         END IF
      END IF
C      IF (IBAD.NE.0) WRITE (6,1010) IBAD
 1010 FORMAT(I6,' BAD PIVOT(S) ENCOUNTERED DURING INCOMPLETE',
     * ' CHOLESKY DECOMPOSITION.')
      NOPS=NOPS+28*NX*NY-21*(NX+NY)+18
      RETURN
      END
