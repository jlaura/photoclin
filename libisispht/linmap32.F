      SUBROUTINE LINMAP32(ARRAY1,NX1,NY1,ARRAY2,NX2,NY2,TRI,
     * PADIN,IOPT)
      IMPLICIT NONE
C
C_TITLE	LINMAP32 - General linear coordinate mapping on real data
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	NX1,NY1		!I	Dimensions of source array
	REAL*4		ARRAY1(NX1,NY1)	!I	Source array
	INTEGER*4	NX2,NY2		!I	Dimensions of dest array
	REAL*4		ARRAY2(NX2,NY2)	!O	Destination array
	REAL*4		TRI(6)		!I	Trace of inverse mapping
	REAL*4		PADIN		!I	"No data" value for output
	INTEGER*4	IOPT		!I	1=pad "no data" with PADIN
C						2=pad by extending on cols
C						3=pad by extending on rows
C
C_DESC	Maps a source data array to a destination array under a general
C	linear mapping specified by the trace TRI of the inverse mapping,
C	using backwards bilinear interpolation.  That is, destination
C	pixel ARRAY2(I2,J2), with coordinates X2=I2, Y2=J2 is mapped to
C	the position 
C		X1 = TRI(1) + TRI(2) * X2 + TRI(3) * Y2
C		Y1 = TRI(4) + TRI(5) * X2 + TRI(6) * Y2
C	in ARRAY1 and the data value at this point is bilinearly 
C	interpolated between the 4 surrounding pixel values.
C	IOPT controls what is done if (X1,Y1) lies outside the domain
C	of the source data.  IOPT=1: use the given value PADIN; IOPT=2:
C	use the nearest valid destination point with the same I2; IOPT=3: 
C	use the nearest valid destination point with the same J2.
C	If there is no data at given I2 (IOPT=2) or J2 (IOPT=3) the nearest
C	column/row is copied.  If there is no data anywhere, the array
C	is filled with PADIN.
C
C_CALLS	R2R (PICS)
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      REAL*4 PAD		! Pad value (PADIN or 1E30 to mark where
C				  data must be extended)
      INTEGER*4 I2,J2		! Loop counters (indices in ARRAY2)
      REAL*4 X2,Y2		! Floating I2,J2; coords in ARRAY2
      REAL*4 X1,Y1		! Location in ARRAY1 corresponding to X2,Y2
      INTEGER*4 I1,J1		! Integer part of X1,Y1
      REAL*4 FX,FY		! Weights for sample, line interpolations
      REAL*4 T1,T2		! Results of 2 interps in sample direction
      INTEGER*4 IFIRST,ILAST	! First, last valid pixels on a line
      INTEGER*4 JFIRST,JLAST	! First, last valid pixels in a column
C
      IF (IOPT.EQ.1) THEN
         PAD=PADIN
      ELSE
         PAD=1.0E30
      END IF
      DO J2=1,NY2
         Y2=REAL(J2)
         DO I2=1,NX2
            X2=REAL(I2)
            X1=TRI(1)+TRI(2)*X2+TRI(3)*Y2
            Y1=TRI(4)+TRI(5)*X2+TRI(6)*Y2
            IF ((X1.LT.1.0).OR.(X1.GT.REAL(NX1))
     *      .OR.(Y1.LT.1.0).OR.(Y1.GT.REAL(NY1))) THEN
               ARRAY2(I2,J2)=PAD
            ELSE
               I1=MAX(1,MIN(INT(X1),NX1))
               J1=MAX(1,MIN(INT(Y1),NY1))
               FX=X1-REAL(I1)
               FY=Y1-REAL(J1)
               T1=(1.-FX)*ARRAY1(I1,J1  )+FX*ARRAY1(I1+1,J1  )
               T2=(1.-FX)*ARRAY1(I1,J1+1)+FX*ARRAY1(I1+1,J1+1)
               ARRAY2(I2,J2)=(1.-FY)*T1+FY*T2
            END IF
         ENDDO
      ENDDO
      IF (IOPT.EQ.2) THEN
         IFIRST=0
         DO I2=1,NX2
            JFIRST=1
            DO WHILE ((JFIRST.LT.NY2+1).AND.
     *                (ARRAY2(I2,JFIRST).EQ.PAD))
               JFIRST=JFIRST+1
            ENDDO
            JLAST=NY2
            DO WHILE ((JLAST.GT.JFIRST).AND.
     *                (ARRAY2(I2,JLAST).EQ.PAD))
               JLAST=JLAST-1
            ENDDO
            IF (JFIRST.LT.NY2+1) THEN
               IF (IFIRST.EQ.0) IFIRST=I2
               IF (JFIRST.GT.1) THEN
                  DO J2=1,JFIRST-1
                     ARRAY2(I2,J2)=ARRAY2(I2,JFIRST)
                  ENDDO
               END IF
               IF (JLAST.LT.NY2) THEN
                  DO J2=JLAST+1,NY2
                     ARRAY2(I2,J2)=ARRAY2(I2,JLAST)
                  ENDDO
               END IF
            ELSE IF (IFIRST.NE.0) THEN
               DO J2=1,NY2
                  ARRAY2(I2,J2)=ARRAY2(I2-1,J2)
               ENDDO
            END IF
         ENDDO
         IF (IFIRST.GT.1) THEN
            DO I2=1,IFIRST-1
               DO J2=1,NY2
                  ARRAY2(I2,J2)=ARRAY2(IFIRST,J2)
               ENDDO
            ENDDO
         END IF
         IF (IFIRST.EQ.0) CALL U_FILL4(PADIN,NX2*NY2,ARRAY2)
      ELSE IF (IOPT.EQ.3) THEN
         JFIRST=0
         DO J2=1,NY2
            IFIRST=1
            DO WHILE ((IFIRST.LT.NX2+1).AND.
     *                (ARRAY2(IFIRST,J2).EQ.PAD))
               IFIRST=IFIRST+1
            ENDDO
            ILAST=NX2
            DO WHILE ((ILAST.GT.IFIRST).AND.
     *                (ARRAY2(ILAST,J2).EQ.PAD))
               ILAST=ILAST-1
            ENDDO
            IF (IFIRST.LT.NX2+1) THEN
               IF (JFIRST.EQ.0) JFIRST=J2
               IF (IFIRST.GT.1) THEN
                  DO I2=1,IFIRST-1
                     ARRAY2(I2,J2)=ARRAY2(IFIRST,J2)
                  ENDDO
               END IF
               IF (ILAST.LT.NX2) THEN
                  DO I2=ILAST+1,NX2
                     ARRAY2(I2,J2)=ARRAY2(ILAST,J2)
                  ENDDO
               END IF
            ELSE IF (JFIRST.NE.0) THEN
               DO I2=1,NX2
                  ARRAY2(I2,J2)=ARRAY2(I2,J2-1)
               ENDDO
            END IF
         ENDDO
         IF (JFIRST.GT.1) THEN
            DO J2=1,JFIRST-1
               DO I2=1,NX2
                  ARRAY2(I2,J2)=ARRAY2(I2,JFIRST)
               ENDDO
            ENDDO
         END IF
         IF (JFIRST.EQ.0) CALL U_FILL4(PADIN,NX2*NY2,ARRAY2)
      END IF
      RETURN
      END
