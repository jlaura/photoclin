      SUBROUTINE AE1CALC(B,Z,E,A,NX,NY,ERMS,DBRMS,ZRMS,MODE,BDER)
      IMPLICIT NONE
C
C_TITLE	AE1CALC - Set up linearized equations for clinometry
C
#include "clinom_aepar.inc"
#include "clinom_ipars.inc"
#include "clinom_ops.inc"
#include "clinom_specpix.inc"
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
	REAL*4		B(NX,NY)	!I	Brightness image
	REAL*4		Z(NX+1,NY+1)	!I	Current topographic est
	REAL*4		E(NX+1,NY+1)	!O	RHS (gradient vector)
	REAL*4		A(5,NX+1,NY+1)	!O	Hessian matrix
	REAL*4		ERMS		!O	RMS value of E
	REAL*4		DBRMS		!O	RMS difference between
C						B and calculated brightness
	REAL*4		ZRMS		!O	RMS topography
	INTEGER*4	MODE		!I	0=calculate E,ERMS,DBRMS,
C						  ZRMS
C						1=calculate the above and A
	EXTERNAL	BDER		!I	Brightness fn and derivs
C
C_DESC	Assembles the individual pixel contributions to the Hessian
C	matrix and gradient vector for one Newton-Raphson linearization
C	of the clinometry problem.  If MODE=0, does not bother to
C	calculate A.  There are three parts to the gradient and Hessian:
C	those due to the error in the brightness estimate, which are
C	calculated from the model brightness and its derivatives as
C	returned by BDER (evaluated at the center of the pixel, or
C	equivalently, integrated by 1 point Gauss quadrature), those
C	due to a priori topographic constraints (added in AE1APO), and
C	those due to the "roughness" penalty.  The roughness penalty
C	adopted here is the square of the "soap film area" of the
C	topographic surface.  This is also equal (up to a constant)
C	to the RMS gradient over the domain.  This function (unlike
C	the soap film area itself) can be integrated analytically in
C	each pixel and yields a quadratic form in the nodal elevations.
C	It thus reduces the computational burden tremendously.  Other
C	good properties of the area-squared penalty are its sensitivity
C	to both the tilt and the "saddle-like" twist of the element,
C	and its insensitivity to a constant elevation value across the
C	pixel (it depends only on the derivatives).  The latter leads
C	to smooth interpolation of the surrounding elevations across
C	"null" regions where no image information is available.
C	Forcing the elevations toward the (possibly oblique) datum
C	is handled approximately, by computing the penalty function
C	on the difference of the actual and datum elevations, rather
C	than projecting the surface perpendicularly toward the datum.
C
C_CALLS	AE1APO,BDER (CLINOM.OLB)
C	R2R (PICS)
C
C_HIST	24-Oct-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      REAL*4 ASELF		! Contribution to Hessian matrix of
C				  a node interacting with itself,
C				  for the area-squared penalty
      REAL*4 AHNEI,AVNEI	! As ASELF, but for a node with its
C				  nearest neighbor in the same row
C				  and same column, respectively.
      REAL*4 ADIAG		! As ASELF, but for a node with its
C				  diagonal neighbor
      INTEGER*4 NB		! # of brightness values (elements)
      INTEGER*4 NZ		! # of topography values (nodes)
      REAL*8 SUM1,SUM2,SUM3,SUM4! Double precision places to accumulate
C				 the statistics ERMS, DBRMS, ZRMS
      INTEGER*4 I,J		! Loop counters
      REAL*4 Z1,Z2,Z3,Z4	! The elevations at the 4 corners of a
C				  given pixel (element)
      REAL*4 DZ1,DZ2		! Diagonal elevation differences across
C				  the pixel, w.r.t. datum
      REAL*4 BIJ		! Observed brightness in the pixel
      REAL*4 BE			! Model estimate of the pixel brightness
      REAL*4 DB1,DB2		! Derivatives of BE wrt DZ1, DZ2
      REAL*4 DB			! Error in model brightness, this pixel
      REAL*4 E1,E2		! Two independent contributions to E from
C				  a given pixel's brightness
      REAL*4 A11,A12,A22	! Three independent contributions to A from
C				  a given pixel's brightness
C
      ASELF=AL*(ASPECT/ 3.0+1.0/( 3.0*ASPECT))
      AHNEI=AL*(ASPECT/ 6.0+1.0/(-3.0*ASPECT))
      AVNEI=AL*(ASPECT/-3.0+1.0/( 6.0*ASPECT))
      ADIAG=AL*(ASPECT/-6.0+1.0/(-6.0*ASPECT))
      NB=NX*NY
      NZ=(NX+1)*(NY+1)
      CALL U_FILL4(0.0,NZ,E)
      IF (MODE.EQ.1) CALL U_FILL4(0.0,5*NZ,A)
      IF (WEIGHT0.NE.0.0) CALL AE1APO(Z,E,A,NX,NY,MODE)
      SUM1=0.0
      SUM2=0.0
      SUM3=0.0
      SUM4=0.0
      DO J=1,NY
         DO I=1,NX
            Z1=Z(I+1,J+1)
            Z2=Z(I  ,J+1)
            Z3=Z(I  ,J  )
            Z4=Z(I+1,J  )
            DZ1=Z3-Z1
            DZ2=Z2-Z4
            BIJ=B(I,J)
            CALL BDER(I,J,DZ1,DZ2,BE,DB1,DB2,1)
C            IF ((BIJ.EQ.BNULL).OR.(BIJ.GE.BMAX)) THEN
            IF ((BIJ.LT.VALID_MIN4.OR.BIJ.GT.VALID_MAX4)
     *          .OR.(BIJ.GE.BMAX)) THEN
               DB=0.0
               E1=0.0
               E2=0.0
            ELSE
               DB=(BIJ-BE)*CONTRAST
               E1=DB1*DB
               E2=DB2*DB
            END IF
            E(I+1,J+1)=E(I+1,J+1)-E1
     *                           -ASELF*Z1-AHNEI*Z2-ADIAG*Z3-AVNEI*Z4
            E(I  ,J+1)=E(I  ,J+1)+E2
     *                           -AHNEI*Z1-ASELF*Z2-AVNEI*Z3-ADIAG*Z4
            E(I  ,J  )=E(I  ,J  )+E1
     *                           -ADIAG*Z1-AVNEI*Z2-ASELF*Z3-AHNEI*Z4
            E(I+1,J  )=E(I+1,J  )-E2
     *                           -AVNEI*Z1-ADIAG*Z2-AHNEI*Z3-ASELF*Z4
            SUM3=SUM3+Z3*Z3
            SUM4=SUM4+Z3
            IF (J.EQ.NY) THEN
               SUM3=SUM3+Z2*Z2
               SUM4=SUM4+Z2
               IF (I.EQ.NX) THEN
                  SUM3=SUM3+Z1*Z1+Z4*Z4
                  SUM4=SUM4+Z1+Z4
               END IF
            ELSE
               IF (I.EQ.NX) THEN
                  SUM3=SUM3+Z4*Z4
                  SUM4=SUM4+Z4
               END IF
            END IF
            SUM2=SUM2+DB*DB
            IF (MODE.EQ.1) THEN
C               IF ((BIJ.EQ.BNULL).OR.(BE.GE.BMAX)) THEN
               IF ((BIJ.LT.VALID_MIN4.OR.BIJ.GT.VALID_MAX4)
     *             .OR.(BE.GE.BMAX)) THEN
                  A11=0.0
                  A12=0.0
                  A22=0.0
               ELSE
                  A11=DB1*DB1
                  A12=DB1*DB2
                  A22=DB2*DB2
               END IF
               A(1,I+1,J+1)=A(1,I+1,J+1)+A11+ASELF
               A(1,I  ,J+1)=A(1,I  ,J+1)+A22+ASELF
               A(2,I  ,J+1)=A(2,I  ,J+1)-A12+AHNEI
               A(1,I  ,J  )=A(1,I  ,J  )+A11+ASELF
               A(2,I  ,J  )=A(2,I  ,J  )-A12+AHNEI
               A(4,I  ,J  )=A(4,I  ,J  )+A12+AVNEI
               A(5,I  ,J  )=A(5,I  ,J  )-A11+ADIAG
               A(1,I+1,J  )=A(1,I+1,J  )+A22+ASELF
               A(3,I+1,J  )=A(3,I+1,J  )-A22+ADIAG
               A(4,I+1,J  )=A(4,I+1,J  )+A12+AVNEI
            END IF
         ENDDO
      ENDDO
      DO J=1,NY+1
         DO I=1,NX+1
            SUM1=SUM1+E(I,J)*E(I,J)
         ENDDO
      ENDDO
      ERMS=SQRT(SNGL(SUM1)/REAL(NZ))/CONTRAST
      DBRMS=SQRT(SNGL(SUM2)/REAL(NB))/CONTRAST
      IF (.NOT.LOGIMG) THEN
         ERMS=ERMS/(BNORM*BNORM)
         DBRMS=DBRMS/BNORM
      END IF
      ZRMS=SQRT(MAX(0.0,SNGL((SUM3-SUM4*SUM4/REAL(NZ))/REAL(NZ))))
      NOPS=NOPS+44*NB+5*NZ
      IF (MODE.EQ.1) NOPS=NOPS+23*NB
      RETURN
      END
