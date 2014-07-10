      SUBROUTINE INTERP(VBIG,NXBIG,NYBIG,V,NX,NY,FAC)
      IMPLICIT NONE
C
C_TITLE INTERP - INTERPOLATES A CORRECTIVE CHANGE IN AN ARRAY
C
#include "clinom_ops.inc"      
C_ARGS  TYPE        VARIABLE          I/O  DESC
        INTEGER*4   NXBIG             ! I
        INTEGER*4   NYBIG             ! I
        REAL*4      VBIG(NXBIG,NYBIG) ! B
        INTEGER*4   NX                ! I
        INTEGER*4   NY                ! I
        REAL*4      V(NX,NY)          ! B
        REAL*4      FAC               ! I
C
C_DESC INTERPOLATES A CORRECTIVE CHANGE IN AN ARRAY BY A FACTOR OF TWO:
C      VBIG := VBIG + INTERP( FAC * V - INJ(VBIG) )
C      WHERE INTERP IS BILINEAR INTERPOLATION, AND INJ IS INJECTION, I.E.
C      SELECTING THE 1/4 OF THE ENTRIES OF VBIG COLLOCATED WITH THE ENTRIES
C      OF V.  FAC IS A SCALE FACTOR RELATING V TO VBIG.
C
C_HIST  Jan 27 1991 Randolph Kirk, USGS, Flagstaff - Original Version
C       Apr 23 1998 Tracie Sucharski, Converted to UNIX/ISIS
C
C_END


      REAL*4      DV1,DV2,DV3

      INTEGER*4   I,J,IB,JB

      DO J=1,NY
         JB=J+J-1
         DO I=1,NX
            IB=I+I-1
            V(I,J)=FAC*V(I,J)-VBIG(IB,JB)
         ENDDO
      ENDDO
      DO J=1,NY-1
         JB=J+J-1
         DO I=1,NX-1
            IB=I+I-1
            VBIG(IB,JB)=VBIG(IB,JB)+V(I,J)
            DV1=0.5*(V(I+1,J  )+V(I  ,J  ))
            DV2=0.5*(V(I  ,J+1)+V(I  ,J  ))
            DV3=0.5*(V(I+1,J+1)+V(I  ,J+1))
            VBIG(IB+1,JB  )=VBIG(IB+1,JB  )+DV1
            VBIG(IB  ,JB+1)=VBIG(IB  ,JB+1)+DV2
            VBIG(IB+1,JB+1)=VBIG(IB+1,JB+1)+0.5*(DV1+DV3)
         ENDDO
         I=NX
         IB=I+I-1
         VBIG(IB  ,JB  )=VBIG(IB  ,JB  )+V(I,J)
         DV2=0.5*(V(I  ,J+1)+V(I  ,J  ))
         VBIG(IB  ,JB+1)=VBIG(IB  ,JB+1)+DV2
         IF (NXBIG.GT.IB) THEN
            VBIG(IB+1,JB  )=VBIG(IB+1,JB  )+V(I,J)
            VBIG(IB+1,JB+1)=VBIG(IB+1,JB+1)+DV2
         END IF
      ENDDO
      J=NY
      JB=J+J-1
      DO I=1,NX-1
         IB=I+I-1
         VBIG(IB  ,JB  )=VBIG(IB  ,JB  )+V(I,J)
         DV1=0.5*(V(I+1,J  )+V(I  ,J  ))
         VBIG(IB+1,JB  )=VBIG(IB+1,JB  )+DV1
         IF (NYBIG.GT.JB) THEN
            VBIG(IB  ,JB+1)=VBIG(IB  ,JB+1)+V(I,J)
            VBIG(IB+1,JB+1)=VBIG(IB+1,JB+1)+DV1
         END IF
      ENDDO
      I=NX
      IB=I+I-1
      VBIG(IB  ,JB  )=VBIG(IB  ,JB  )+V(I,J)
      IF (NYBIG.GT.JB) VBIG(IB  ,JB+1)=VBIG(IB  ,JB+1)+V(I,J)
      IF (NXBIG.GT.IB) THEN
         VBIG(IB+1,JB  )=VBIG(IB+1,JB  )+V(I,J)
         IF (NYBIG.GT.JB) VBIG(IB+1,JB+1)=VBIG(IB+1,JB+1)+V(I,J)
      END IF
      NOPS=NOPS+12*NX*NY+2*(NX+1)*(NY+1)

      RETURN
      END
