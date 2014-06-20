      SUBROUTINE ADJZCOLS(ZR,NXR,NYR,TRI,NX,NY)
      IMPLICIT NONE
C
C_TITLE	ADJZCOLS - Adjust SSIPSF-PI topography with column increments
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NXR,NYR		!I	Sample, line dimensions of 
C						rotated topography array
	REAL*4		ZR(NXR+1,NYR+1)	!B	Topographic array in
C						rotated coordinates
	REAL*4		TRI(6)		!I	Inverse of mapping from
C						normal to rotated coords
        INTEGER*4	NX,NY		!I	Sample, line dimensions of 
C						UNrotated topography array
C
C_DESC	Adjusts the rotated elevation array ZR by adding an offset to each 
C	col so that the RMS Z is minimized over the area falling inside the 
C	original data, rather than over the entire array.  Since a mis-
C	estimate of the image normalization leads to a tilt of the rows 
C	w.r.t. the datum, we first fit to find this tilt, then tie the cols 
C	to it rather than to the datum.  This should remove the "twist" at 
C	the edges of the solution where only the region of the tilted plane 
C	above or below the datum is available.  Similar to ADJZROWS.
C
C_CALLS	None
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      REAL*4 XMIN,XMAX		! Sample limits of unrotated topography Z
      REAL*4 YMIN,YMAX		! Sample limits of unrotated topography Z
      REAL*4 SUM		! Sum for averaging col tilts, deviations
C				  from tilted trends of cols
      INTEGER*4 N		! # of points contributing to SUM
      INTEGER*4 IR,JR		! Loop counters (position in ZR)
      REAL*4 XR,YR		! Floating version of IR,JR
      REAL*4 XIR,YIR		! Location in Z at beginning of col in ZR
      REAL*4 X,Y		! Location in Z mapping to (XR,YR) in ZR
      INTEGER*4 JRFIRST,JRLAST	! Range of JR mapping inside Z array
      REAL*4 TILT		! Mean tilt of ZR in regions mapping inside
C				  Z array
      REAL*4 OFFSET		! Vertical offset for col such that the
C				  surface TILT*XR-OFFSET is zero at the
C				  center of the col
      REAL*4 ZBAR		! Mean deviation of a col from a trend TILT
C				  with intercept zero at the col center
C
      XMIN=0.5
      XMAX=REAL(NX+1)+0.5
      YMIN=0.5
      YMAX=REAL(NY+1)+0.5
      SUM=0.0
      N=0
      DO IR=1,NXR+1
         XR=REAL(IR)
         XIR=TRI(1)+TRI(2)*XR
         YIR=TRI(4)+TRI(5)*XR
         JRFIRST=0
         DO JR=1,NYR+1
            YR=REAL(JR)
            X=XIR+TRI(3)*YR
            Y=YIR+TRI(6)*YR
            IF ((X.GE.XMIN).AND.(X.LE.XMAX).AND.
     *          (Y.GE.YMIN).AND.(Y.LE.YMAX)) THEN
               IF (JRFIRST.EQ.0) JRFIRST=JR
               JRLAST=JR
            END IF
         ENDDO
         IF ((JRFIRST.NE.0).AND.(JRFIRST.NE.JRLAST)) THEN
            SUM=SUM+(ZR(IR,JRLAST)-ZR(IR,JRFIRST))
     *              /REAL(JRLAST-JRFIRST)
            N=N+1
         END IF
      ENDDO
      TILT=SUM/REAL(N)
      OFFSET=0.5*REAL(NYR+1)*TILT
      DO IR=1,NXR+1
         SUM=0.0
         N=0
         XR=REAL(IR)
         XIR=TRI(1)+TRI(2)*XR
         YIR=TRI(4)+TRI(5)*XR
         DO JR=1,NYR+1
            YR=REAL(JR)
            X=XIR+TRI(3)*YR
            IF ((X.GE.XMIN).AND.(X.LE.XMAX)) THEN
               Y=YIR+TRI(6)*YR
               IF ((Y.GE.YMIN).AND.(Y.LE.YMAX)) THEN
                  SUM=SUM+ZR(IR,JR)+OFFSET-TILT*YR
                  N=N+1
               END IF
            END IF
         ENDDO
         IF (N.GT.0) THEN
            ZBAR=SUM/REAL(N)
            DO JR=1,NYR+1
               ZR(IR,JR)=ZR(IR,JR)-ZBAR
            ENDDO
         END IF
      ENDDO
      RETURN
      END
