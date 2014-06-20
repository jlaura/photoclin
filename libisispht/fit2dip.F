      SUBROUTINE FIT2DIP(BUF,NX1,NY1,DNSCL,PIXHT,ASPECT,SUNAZ,
     * CHARAZ,DIP,DIPAZ,SUNDIP,RMSDN,RMSPIX,RMSM)
      IMPLICIT NONE
C
C_TITLE FIT2DIP - Fit LSQ plane to data and output dip, etc.
C
C_ARGS  TYPE        VARIABLE        I/O  DESC
       INTEGER*4 NX1           !I  Sample dimension
       INTEGER*4 NY1           !I  Line dimension
       REAL*4    BUF(NX1,NY1)  !I  Array of elevations to fit
       REAL*4    DNSCL         !I  Scale of BUF data, m/DN
       REAL*4    PIXHT         !I  Res of Buf data, m/pixel ht
       REAL*4    ASPECT        !I  Aspect ratio pix wid/ht
       REAL*4    SUNAZ         !I  Sun az wrt anisotropic grid
       REAL*4    CHARAZ        !I  Az of characteristics, Cartesian
       REAL*4    DIP           !O  Dip of LSQ plane
       REAL*4    DIPAZ         !O  Azimuth of dip
       REAL*4    SUNDIP        !O  Slope in direction of sun
       REAL*4    RMSDN         !O  RMS residual to fit in DN
       REAL*4    RMSPIX        !O  RMS residual to fit in pix ht
       REAL*4    RMSM          !O  RMS residual to fit in m
C
C_DESC Performs a least squares linear fit vs sample and line
C      to a buffer of data assumed to be elevations.  From the
C      fit coefficients, calculates and returns the dip, dip
C      azimuth, component of slope to the sun, and residuals to
C      the fit in various units.
C
C_HIST  Jan 15 2003 Randolph Kirk, USGS, Flagstaff - Original Version
C
C_CALLS DFFACT,DFSOLV (RLKLIB)
C
C_END

      REAL*4      PIDEG
      PARAMETER   (PIDEG=1.745329252E-2)
      REAL*4      DEGPI
      PARAMETER   (DEGPI=57.29578)

      REAL*8      USCL,UOFF   !Scale sample to U in [-1,1]
      REAL*8      VSCL,VOFF   !Scale line to V in [-1,1]
      REAL*8      U,V,Z       !Scaled coords of pixel
      REAL*8      ZOFF        !Mean Z, to pre-subtract
      INTEGER*4   I,J         !Loop indices
      REAL*8      A(3,3),B(3) !Matrix, RHS for normal eqns
      REAL*8      WORK(3)     !Work array for solution
      INTEGER*4   IPVT(3)     !Pivot array for solution
      REAL*8      COND        !Condition number of solution
      REAL*8      DZ          !Pixel residual of fit
      REAL*8      DRMSDN      !R*8 version of RMSDN
      REAL*4      DZX,DZY     !Derivatives of fit plane in m/m
      REAL*4      SUNX,SUNY   !Unit vector in direction (in plane) of sun
      REAL*4      DEN         !Denominator to scale previous to Cartesian
      REAL*4      CHARX,CHARY !Unit vector in direction (in plane) of
C                              characteristic strips, toward sun


C To avoid roundoff problems, prescale line and sample to U and V,
C which go from -1 to 1 over the array.
      USCL= 2.0D0/(NX1-1)
      UOFF=-0.5D0*(NX1+1)*USCL
      VSCL= 2.0D0/(NY1-1)
      VOFF=-0.5D0*(NY1+1)*VSCL
C For the same reason, start by finding, subtracting mean value
      ZOFF=0.0D0
      DO J=1,NY1
         DO I=1,NX1
            ZOFF=ZOFF+DBLE(BUF(I,J))
         ENDDO
      ENDDO
      ZOFF=-ZOFF/DBLE(NX1*NY1)
C Clear the arrays for the normal equations
      DO J=1,3
         DO I=1,3
            A(I,J)=0.0D0
         ENDDO
         B(J)=0.0D0
      ENDDO
C Now collect the data for a least-squares fit
      DO J=1,NY1
         V=VSCL*J+VOFF
         DO I=1,NX1
            U=USCL*I+UOFF
            Z=DBLE(BUF(I,J))+ZOFF
            A(1,1)=A(1,1)+1.0D0
            A(1,2)=A(1,2)+U
            A(1,3)=A(1,3)+V
            A(2,2)=A(2,2)+U*U
            A(2,3)=A(2,3)+U*V
            A(3,3)=A(3,3)+V*V
            B(1)=B(1)+Z
            B(2)=B(2)+Z*U
            B(3)=B(3)+Z*V
         ENDDO
      ENDDO
C Now solve the normal equations for the fit coefficients
      IF (A(1,1).LT.3.0D0) THEN
C With only two points, can't solve so set the plane to zero
         DO J=1,3
            B(J)=0.0D0
         ENDDO
      ELSE
C First fill in lower triangle of normal equations
         A(2,1)=A(1,2)
         A(3,1)=A(1,3)
         A(3,2)=A(2,3)
C Factor the normal equations
         CALL DFFACT(A,WORK,IPVT,3,3,COND)
         IF (COND.GT.1.0D10) THEN
C Ill-conditioned, e.g., can't find line coefficient if image only
C has one line
            DO J=1,3
               B(J)=0.0D0
            ENDDO
         ELSE
            CALL DFSOLV(A,B,IPVT,3,3)
         END IF
      END IF
C Now evaluate the residuals in DN units
      DRMSDN=0.0D0
      DO J=1,NY1
         V=VSCL*J+VOFF
         DO I=1,NX1
            U=USCL*I+UOFF
            Z=DBLE(BUF(I,J))+ZOFF
            DZ=Z-B(1)-B(2)*U-B(3)*V
            DRMSDN=DRMSDN+DZ*DZ
         ENDDO
      ENDDO
      IF (NX1*NY1.LT.2) THEN
         DRMSDN=0.0D0
      ELSE
         DRMSDN=SQRT(DRMSDN/DBLE(NX1*NY1-1))
      END IF
C Scale the residuals to pixels and meters
      RMSDN=SNGL(DRMSDN)
      RMSM=RMSDN*DNSCL
      RMSPIX=RMSM/PIXHT
C Determine dip and azimuth from derivatives
C First, go from derivs of pixels with U, V to those of m with X, Y in m
      DZX=SNGL(B(2))*DNSCL*USCL/(PIXHT*ASPECT)
      DZY=SNGL(B(3))*DNSCL*VSCL/PIXHT
      IF ((DZX.EQ.0.0).AND.(DZY.EQ.0.0)) THEN
C No slope, azimuth is undefined so set it to zero
C Many of the special conditions above lead to here
         DIP=0.0
         DIPAZ=0.0
         SUNDIP=0.0
      ELSE
C Dip is RSS of true Cartesian components.  Convert to angle in deg
         DIP=ATAN(SQRT(DZX*DZX+DZY*DZY))*DEGPI
C Put the aspect back into the X derivative to get grid azimuth in deg
C Since "dip" is +ve downhill, need the -ve of the derivatives here
         DIPAZ=ATAN2(-DZY,-DZX*ASPECT)*DEGPI
C SUNAZ is with respect to grid so we need to convert to Cartesian
C before calculating dip to sun
         SUNX=COS(SUNAZ*PIDEG)*ASPECT
         SUNY=SIN(SUNAZ*PIDEG)
         DEN=SQRT(SUNX*SUNX+SUNY*SUNY)
         SUNX=SUNX/DEN
         SUNY=SUNY/DEN
C Now calculate components of characteristic strip direction. This
C is already in Cartesian but may be toward sun or away from sun.
C If the latter, it must be reversed.
         CHARX=COS(CHARAZ*PIDEG)
	 CHARY=SIN(CHARAZ*PIDEG)
	 IF (CHARX*SUNX+CHARY*SUNY.LT.0.0) THEN
	   CHARX=-CHARX
	   CHARY=-CHARY
         END IF
         SUNDIP=ATAN(-CHARX*DZX-CHARY*DZY)*DEGPI
      END IF
      RETURN
      END
