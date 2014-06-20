      SUBROUTINE DFSOLV(A,B,IPVT,NDIM,N)
      IMPLICIT NONE
C
C_TITLE DFSOLV - Double precision forward_elimination/back-substitution
C
C_ARGS  TYPE        VARIABLE        I/O  DESC
        INTEGER*4   NDIM            ! I
        INTEGER*4   N               ! I
        REAL*8      A(NDIM,N)       ! I
        REAL*8      B(N)            ! O
        INTEGER*4   IPVT(N)         ! I
C
C_DESC Double precision version of SFSOLV.
C
C_HIST  Sep 14 1989 Randolph Kirk, USGS, Flagstaff - Original Version
C       Apr 27 1998 Tracie Sucharski, Converted to UNIX/ISIS
C
C_END



      REAL*8      T

      INTEGER*4   NM1,K,KP1,M,I,KB,KM1

      IF (N.EQ.1) GO TO 50
      NM1=N-1
      DO 20 K=1,NM1
         KP1=K+1
         M=IPVT(K)
         T=B(M)
         B(M)=B(K)
         B(K)=T
         DO 10 I=KP1,N
            B(I)=B(I)+A(I,K)*T
   10    CONTINUE
   20 CONTINUE
      DO 40 KB=1,NM1
         KM1=N-KB
         K=KM1+1
         B(K)=B(K)/A(K,K)
         T=-B(K)
         DO 30 I=1,KM1
            B(I)=B(I)+A(I,K)*T
   30    CONTINUE
   40 CONTINUE
   50 B(1)=B(1)/A(1,1)

      RETURN
      END
