      SUBROUTINE S3SOLV2(A,B,NX,NY)
      IMPLICIT NONE
C
C_TITLE S3SOLV2 - SAME AS S3SOLV BUT WORKS ON MANY SETS OF EQUATIONS
C
#include "clinom_ops.inc"
C_ARGS  TYPE        VARIABLE        I/O  DESC
        INTEGER*4   NX              ! I
        INTEGER*4   NY              ! I
        REAL*4      A(NY,2)         ! I
        REAL*4      B(NX,NY)        ! B
C
C_DESC SAME AS S3SOLV BUT WORKS ON MANY SETS OF EQUATIONS (RHS IN THE
C      COLUMNS OF B) AT ONCE TO SAVE I/O TIME.
C
C_HIST  Sep 14 1989 Randolph Kirk, USGS, Flagstaff - Original Version
C       Apr 29 1998 Tracie Sucharski, Converted to UNIX/ISIS
C
C_END


      INTEGER*4   I,J
      


      DO J=1,NY-1
         DO I=1,NX
            B(I,J+1)=B(I,J+1)-A(J,2)*B(I,J)
            B(I,J)=B(I,J)/A(J,1)
         ENDDO
      ENDDO
      DO I=1,NX
         B(I,NY)=B(I,NY)/A(NY,1)
      ENDDO
      DO J=NY,2,-1
         DO I=1,NX
            B(I,J-1)=B(I,J-1)-A(J-1,2)*B(I,J)
         ENDDO
      ENDDO
      NOPS=NOPS+5*NX*NY

      RETURN
      END
