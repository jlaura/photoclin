      SUBROUTINE S3SOLV(A,B,N)
      IMPLICIT NONE
C
C_TITLE S3SOLV - Specialization of SBSOLV to a tridiagonal matrix
C
#include "clinom_ops.inc"
C_ARGS  TYPE        VARIABLE        I/O  DESC
        INTEGER*4   N               ! I
        REAL*4      A(N,2)          ! I
        REAL*4      B(N)            ! B
C
C_DESC Specialization of SBSOLV to a tridiagonal matrix, used with S3FACT.
C
C_HIST  Sep 14 1989 Randolph Kirk, USGS, Flagstaff - Original Version
C       Apr 29 1998 Tracie Sucharski, Converted to UNIX/ISIS
C
C_END


      INTEGER*4   I
 

      DO I=1,N-1
         B(I+1)=B(I+1)-A(I,2)*B(I)
         B(I)=B(I)/A(I,1)
      ENDDO
      B(N)=B(N)/A(N,1)
      DO I=N,2,-1
         B(I-1)=B(I-1)-A(I-1,2)*B(I)
      ENDDO
      NOPS=NOPS+5*N

      RETURN
      END
