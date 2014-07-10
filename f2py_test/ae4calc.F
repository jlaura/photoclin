      SUBROUTINE AE4CALC(Z,E,BE,A,NX,NY,BERMS,BDER)
      IMPLICIT NONE
C
C_TITLE	AE4CALC - Set up equations for brightness injection
C
#include "clinom_ops.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		Z(NX+1,NY+1)	!I	Topographic array
	REAL*4		E(NX+1,NY+1)	!I	Gradient vector (RHS
C						of photoclinometry eqns,
C						used to construct RHS of
C						eqns for B here)
	REAL*4		BE(NX,NY)	!O	RHS of equations for B
	REAL*4		A(5,NX,NY)	!O	Matrix inverted on BE
	REAL*4		BERMS		!O	RMS residual
	EXTERNAL	BDER		!I	Brightness fn and derivs
C
C_DESC	Constructs the RHS and matrix for the equations for injecting
C	the brightness onto a mesh of 1/2 resolution in the clinometry
C	problem.  This is the "exact" equation for the brightness, using
C	the actual topography rather than the datum plane, though the
C	brightness ultimately obtained will only reproduce the desired
C	gradient vector for the clinometry problem in a least-squares
C	sense, not exactly.
C
C_CALLS	BDER (CLINOM.OLB)
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      INTEGER*4 NB		! # of brightness values (elements)
      REAL*8 SUM		! Double precision place to accumulate the
C				  statistic BERMS
      INTEGER*4 I,J		! Loop counters
      REAL*4 DZ1,DZ2		! Diagonal elevation differences across
C				  the pixel
      REAL*4 BEST		! Estimated brightness of the pixel
      REAL*4 DB1,DB2		! 1st derivatives of brightness with diagonal
C				  slopes
      REAL*4 DB1P,DB2P		! Same as DB1,DB2 but for a neighbor pixel
C
      NB=NX*NY
      CALL U_FILL4(0.0,5*NB,A)
      SUM=0.0D0
      DO J=1,NY
         DO I=1,NX
            DZ1=Z(I  ,J  )-Z(I+1,J+1)
            DZ2=Z(I  ,J+1)-Z(I+1,J  )
            CALL BDER(I,J,DZ1,DZ2,BEST,DB1,DB2,1)
            BE(I,J)=DB1*(E(I  ,J  )-E(I+1,J+1))
     *             +DB2*(E(I  ,J+1)-E(I+1,J  ))
            SUM=SUM+DBLE(BE(I,J))*DBLE(BE(I,J))
            A(1,I,J)=2.0*(DB1*DB1+DB2*DB2)
            IF (I.NE.NX) THEN
               DZ1=Z(I+1,J  )-Z(I+2,J+1)
               DZ2=Z(I+1,J+1)-Z(I+2,J  )
               CALL BDER(I+1,J,DZ1,DZ2,BEST,DB1P,DB2P,1)
               A(2,I,J)=-DB1*DB2P-DB2*DB1P
            END IF
            IF (J.NE.NY) THEN
               IF (I.NE.1) THEN
                  DZ1=Z(I-1,J+1)-Z(I  ,J+2)
                  DZ2=Z(I-1,J+2)-Z(I  ,J+1)
                  CALL BDER(I-1,J+1,DZ1,DZ2,BEST,DB1P,DB2P,1)
                  A(3,I,J)=-DB2*DB2P
               END IF
               DZ1=Z(I  ,J+1)-Z(I+1,J+2)
               DZ2=Z(I  ,J+2)-Z(I+1,J+1)
               CALL BDER(I,J+1,DZ1,DZ2,BEST,DB1P,DB2P,1)
               A(4,I,J)=DB1*DB2P+DB2*DB1P
               IF (I.NE.NX) THEN
                  DZ1=Z(I+1,J+1)-Z(I+2,J+2)
                  DZ2=Z(I+1,J+2)-Z(I+2,J+1)
                  CALL BDER(I+1,J+1,DZ1,DZ2,BEST,DB1P,DB2P,1)
                  A(5,I,J)=-DB1*DB1P
               END IF
            END IF
         ENDDO
      ENDDO
      BERMS=SQRT(SNGL(SUM)/REAL(NB))
      NOPS=NOPS+29*NB-5*(NX+NY)+6
      RETURN
      END
