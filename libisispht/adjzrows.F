      SUBROUTINE ADJZROWS(ZR,NXR,NYR,TRI,NX,NY)
      IMPLICIT NONE
C
C_TITLE	ADJZROWS - Adjust SSIPSF-PI topography with row increments
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
C	row so that the RMS Z is minimized over the area falling inside the 
C	original data, rather than over the entire array.  Since a mis-
C	estimate of the image normalization leads to a tilt of the rows 
C	w.r.t. the datum, we first fit to find this tilt, then tie the rows 
C	to it rather than to the datum.  This should remove the "twist" at 
C	the edges of the solution where only the region of the tilted plane 
C	above or below the datum is available.
C
C_CALLS	None
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      REAL*4 XMIN,XMAX		! Sample limits of unrotated topography Z
      REAL*4 YMIN,YMAX		! Sample limits of unrotated topography Z
      REAL*4 SUM		! Sum for averaging row tilts, deviations
C				  from tilted trends of rows
      INTEGER*4 N		! # of points contributing to SUM
      INTEGER*4 IR,JR		! Loop counters (position in ZR)
      REAL*4 XR,YR		! Floating version of IR,JR
      REAL*4 XJR,YJR		! Location in Z at beginning of line in ZR
      REAL*4 X,Y		! Location in Z mapping to (XR,YR) in ZR
      INTEGER*4 IRFIRST,IRLAST	! Range of IR mapping inside Z array
      REAL*4 TILT		! Mean tilt of ZR in regions mapping inside
C				  Z array
      REAL*4 OFFSET		! Vertical offset for row such that the
C				  surface TILT*XR-OFFSET is zero at the
C				  center of the row
      REAL*4 ZBAR		! Mean deviation of a row from a trend TILT
C				  with intercept zero at the row center
C
      XMIN=0.5
      XMAX=REAL(NX+1)+0.5
      YMIN=0.5
      YMAX=REAL(NY+1)+0.5
      SUM=0.0
      N=0
      DO JR=1,NYR+1
         YR=REAL(JR)
         XJR=TRI(1)+TRI(3)*YR
         YJR=TRI(4)+TRI(6)*YR
         IRFIRST=0
         DO IR=1,NXR+1
            XR=REAL(IR)
            Y=YJR+TRI(5)*XR
            X=XJR+TRI(2)*XR
            IF ((Y.GE.YMIN).AND.(Y.LE.YMAX).AND.
     *          (X.GE.XMIN).AND.(X.LE.XMAX)) THEN
               IF (IRFIRST.EQ.0) IRFIRST=IR
               IRLAST=IR
            END IF
         ENDDO
         IF ((IRFIRST.NE.0).AND.(IRFIRST.NE.IRLAST)) THEN
            SUM=SUM+(ZR(IRLAST,JR)-ZR(IRFIRST,JR))/REAL(IRLAST-IRFIRST)
            N=N+1
         END IF
      ENDDO
      TILT=SUM/REAL(N)
      OFFSET=0.5*REAL(NXR+1)*TILT
      DO JR=1,NYR+1
         SUM=0.0
         N=0
         YR=REAL(JR)
         XJR=TRI(1)+TRI(3)*YR
         YJR=TRI(4)+TRI(6)*YR
         DO IR=1,NXR+1
            XR=REAL(IR)
            Y=YJR+TRI(5)*XR
            IF ((Y.GE.YMIN).AND.(Y.LE.YMAX)) THEN
               X=XJR+TRI(2)*XR
               IF ((X.GE.XMIN).AND.(X.LE.XMAX)) THEN
                  SUM=SUM+ZR(IR,JR)+OFFSET-TILT*XR
                  N=N+1
               END IF
            END IF
         ENDDO
         IF (N.GT.0) THEN
            ZBAR=SUM/REAL(N)
            DO IR=1,NXR+1
               ZR(IR,JR)=ZR(IR,JR)-ZBAR
            ENDDO
         END IF
      ENDDO
      RETURN
      END
