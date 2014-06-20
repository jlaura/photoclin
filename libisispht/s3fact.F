      SUBROUTINE S3FACT(A,N)
      IMPLICIT NONE
C
C_TITLE S3FACT - Specialization of SBFACT to a tridiagonal symmetric matrix
C
#include "clinom_ops.inc"
C_ARGS  TYPE        VARIABLE        I/O  DESC
        INTEGER*4   N               ! I
        REAL*4      A(N,2)          ! B
C
C_DESC Specialization of SBFACT to a tridiagonal symmetric matrix.
C      NOPS keeps track of the number of floating operations done.
C
C_HIST  Sep 14 1989 Randolph Kirk, USGS, Flagstaff - Original Version
C       Apr 29 1998 Tracie Sucharski, Converted to UNIX/ISIS
C
C_END


      INTEGER*4   I

      REAL*4      C

 

      DO I=1,N-1
         C=A(I,2)/A(I,1)
         A(I+1,1)=A(I+1,1)-C*A(I,2)
         A(I,2)=C
      ENDDO
      NOPS=NOPS+3*N

      RETURN
      END
