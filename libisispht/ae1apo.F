      SUBROUTINE AE1APO(Z,E,A,NX,NY,MODE)
      IMPLICIT NONE
C
C_TITLE	AE1APO	- Add a priori topo constraints to clinometry eqns
C
#include "clinom_datum.inc"
#include "clinom_ipars.inc"
#include "clinom_ops.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		Z(NX+1,NY+1)	!I	Current topographic est
	REAL*4		E(NX+1,NY+1)	!O	RHS (gradient vector)
	REAL*4		A(5,NX+1,NY+1)	!O	Hessian matrix
	INTEGER*4	MODE		!I	0=calculate E
C						1=calculate the above and A
C
C_DESC	Assembles the contributions to the Hessian matrix and gradient 
C	vector for one Newton-Raphson linearization of the photoclinometry 
C	problem that are related to a priori constraints on the topography.
C	If MODE=0, does not bother to calculate A.
C_CALLS	ITHWRD (RLKLIB.OLB)
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      INTEGER*4 ERRVAL		! Error flag
      LOGICAL*1 ODU		! Is a given logical unit in use?
      INTEGER*4 LUN		! Unit number for APOFILE
      CHARACTER LINE*256	! One line of the file in character form
      CHARACTER WORD*256	! One column of LINE
      INTEGER*4 LEN		! Length of WORD
      LOGICAL*1 INHEAD		! Are we still in the file header?
      INTEGER*4 NPTS		! Number of benchmark points
      REAL*4 XIN,YIN		! Coordinates of benchmark in full-res
C				  pixel indices
      REAL*4 ZIN		! Vertical coord of benchmark in full-res
C				  pixels, relative to datum
      REAL*4 WIN		! Relative weight for this benchmark
      REAL*4 X,Y		! Coordinates of benchmark relative to
C				  the nodes at this resolution
      INTEGER*4 I,J		! Indices of the pixel containing
C				  a benchmark and of its local node 3
      REAL*4 DX,DY		! Relative position of the benchmark
C				  within the element
      REAL*4 C1,C2,C3,C4	! Interpolation coefficients to get the
C				  elevation at the benchmark
      REAL*4 DZ			! Actual elevation minus benchmark
      REAL*4 W			! Weight for this benchmark
      REAL*4 WC1,WC2,WC3,WC4	! Product of W and C1...C4
      CHARACTER ERRKEY*17       ! Keyword value for error message
      CHARACTER ERRMSG*80       ! Error message
      INTEGER*4 IERR            ! Error value
      CHARACTER FMTSTR*25
      CHARACTER CLEN*25
      INTEGER*4 U_SLEN

      ERRVAL=0
      LUN=0
      ODU=.TRUE.
      CALL U_ALLOCATE_UNIT(LUN,IERR)
      OPEN(UNIT=LUN,FILE=APOFILE,STATUS='OLD',ERR=9000)
      INHEAD=.TRUE.
      NPTS=0
   10 CONTINUE
         READ (LUN,1000,ERR=10,END=20) LINE
 1000    FORMAT(A)
         IF (INHEAD) THEN
            IF (LINE(1:3).EQ.'C_E') INHEAD=.FALSE.
            GO TO 10
         END IF
         CALL ITHWRD(1,LINE,WORD,LEN)
         IF (LEN.EQ.0) GO TO 10
	 CALL U_I2CHR(LEN,CLEN)
	 WRITE (FMTSTR,1001) CLEN(1:U_SLEN(CLEN))
 1001    FORMAT('(F',A,'.0)')
         READ (WORD(1:LEN),FMTSTR,ERR=10) XIN
C 1010    FORMAT(F<LEN>.0)
         CALL ITHWRD(2,LINE,WORD,LEN)
         IF (LEN.EQ.0) GO TO 10
	 CALL U_I2CHR(LEN,CLEN)
	 WRITE (FMTSTR,1001) CLEN(1:U_SLEN(CLEN))
         READ (WORD(1:LEN),FMTSTR,ERR=10) YIN
         CALL ITHWRD(3,LINE,WORD,LEN)
         IF (LEN.EQ.0) GO TO 10
	 CALL U_I2CHR(LEN,CLEN)
	 WRITE (FMTSTR,1001) CLEN(1:U_SLEN(CLEN))
         READ (WORD(1:LEN),FMTSTR,ERR=10) ZIN
         CALL ITHWRD(4,LINE,WORD,LEN)
         IF (LEN.EQ.0) THEN
            WIN=1.0
         ELSE
	    CALL U_I2CHR(LEN,CLEN)
	    WRITE (FMTSTR,1001) CLEN(1:U_SLEN(CLEN))
            READ (WORD(1:LEN),FMTSTR,ERR=10) WIN
         END IF
         X=1.0+(XIN-0.5)/RMAG
         Y=1.0+(YIN-0.5)/RMAG
         I=INT(X)
         J=INT(Y)
         IF ((I.GT.NX).OR.(J.GT.NY)) GO TO 10 ! Benchmark could be just
         NPTS=NPTS+1			      ! outside area considered at
         DX=X-REAL(I)			      ! RMAG>1
         DY=Y-REAL(J)
         C1=DX*DY
         C2=(1.0-DX)*DY
         C3=(1.0-DX)*(1.0-DY)
         C4=DX*(1.0-DY)
         DZ=C1*Z(I+1,J+1)+C2*Z(I  ,J+1)
     *     +C3*Z(I  ,J  )+C4*Z(I+1,J  )
     *     -ZIN/RMAG
         W=WIN*WEIGHT0
         WC1=W*C1
         WC2=W*C2
         WC3=W*C3
         WC4=W*C4
         E(I+1,J+1)=E(I+1,J+1)-WC1*DZ
         E(I  ,J+1)=E(I  ,J+1)-WC2*DZ
         E(I  ,J  )=E(I  ,J  )-WC3*DZ
         E(I+1,J  )=E(I+1,J  )-WC4*DZ
         IF (MODE.EQ.1) THEN
            A(1,I+1,J+1)=A(1,I+1,J+1)+WC1*C1
            A(1,I  ,J+1)=A(1,I  ,J+1)+WC2*C2
            A(2,I  ,J+1)=A(2,I  ,J+1)+WC2*C1
            A(1,I  ,J  )=A(1,I  ,J  )+WC3*C3
            A(2,I  ,J  )=A(2,I  ,J  )+WC3*C4
            A(4,I  ,J  )=A(4,I  ,J  )+WC3*C2
            A(5,I  ,J  )=A(5,I  ,J  )+WC3*C1
            A(1,I+1,J  )=A(1,I+1,J  )+WC4*C4
            A(3,I+1,J  )=A(3,I+1,J  )+WC4*C2
            A(4,I+1,J  )=A(4,I+1,J  )+WC4*C1
         END IF
      GO TO 10
   20 CONTINUE
      CLOSE(UNIT=LUN)
      NOPS=NOPS+42*NPTS
      IF (MODE.EQ.1) NOPS=NOPS+20*NPTS
      RETURN
C
C Handle processing errors
C
9000  CONTINUE
      ERRKEY = 'AE1APO-OPENERR'
      ERRMSG = 'Error opening file of a priori topo values'
      ERRVAL = 1
      GOTO 9999
C
C Write an error message
C
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,1)
      RETURN
      END
