      SUBROUTINE PHOXFM(CLINC,AZINC,CLEMI,AZEMI,AZNOR,MAPPED)
      IMPLICIT NONE
C_TITLE	PHOXFM - Xform photom. vectors from one image POV to another
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	REAL*4		CLINC(2)	!B	Sun-to-POV angle
C						(=phase angle for image
C						in its own coords)
	REAL*8		AZINC(2)	!B	Sun-to-POV azimuth
	REAL*8		CLEMI(2)	!B	Surface n-to-POV angle
C						(=emission angle...)
C						Becomes POV-to-POV angle
	REAL*8		AZEMI(2)	!B	Surface n-to-POV azimuth
C						(=emiss. azimuth+180)
C						Becomes POV-to-POV azimuth
	REAL*8		AZNOR(2)	!I	Azimuth to north
	INTEGER*4	MAPPED		!I	Index of the image that
C						is mapped onto the other
C
C_DESC	Takes sets of angles describing sun direction, surface normal
C	direction, and north direction relative to spacecraft point-
C	of-view (POV) for two images.  Replaces the values for the
C	mapped image with the angles from its sun position and surface
C	normal to the POV of the non-mapped image.
C
C_CALLS	MPRDE,TRPDE (PICS)
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_END
C
#include "clinom_trig.inc"

      INTEGER*4 NMAPPED		! The image that the other is mapped onto
      REAL*8 TEMP		! Temporary variable
C Image geometry in coordinates centered on their own POV's
      REAL*8 CLINC11,CLINC22	! Phase angles for dest, source images
      REAL*8 AZINC11,AZINC22	! Sun azimuths for dest, source images
      REAL*8 CLEMI11,CLEMI22	! Emiss angles for dest, source images
      REAL*8 AZEMI11,AZEMI22	! Surface normal azimuths
      REAL*8 AZNOR11,AZNOR22	! North azimuths
      REAL*8 VINC22(3)          ! Incident vectors for dest, source images
      REAL*8 VEMI22(3)          ! Emission vectors for dest, source images
C Transformation matrices
      REAL*8 AS1(3,3)		! Dest image to surface coords
      REAL*8 AS2(3,3)		! Source image to surface coords
      REAL*8 A12(3,3)		! Source image to dest image coords
C Source image geometry in coordinates of destination image POV
      REAL*8 VEMI12(3)		! Source emission vector in dest coords
      REAL*8 VINC12(3)		! Source incident vector in dest coords
      REAL*8 CLINC12		! Source incident to dest POV angle
      REAL*8 AZINC12		! Source incident to dest POV azimuth
      REAL*8 CLEMI12		! Source emission to dest POV angle
      REAL*8 AZEMI12		! Source emission to dest POV angle
C Translate single precision inputs to double precision
      NMAPPED=3-MAPPED
      CLINC11=DBLE(CLINC(NMAPPED))
      CLINC22=DBLE(CLINC( MAPPED))
      AZINC11=DBLE(AZINC(NMAPPED))
      AZINC22=DBLE(AZINC( MAPPED))
      CLEMI11=DBLE(CLEMI(NMAPPED))
      CLEMI22=DBLE(CLEMI( MAPPED))
      AZEMI11=DBLE(AZEMI(NMAPPED))
      AZEMI22=DBLE(AZEMI( MAPPED))
      AZNOR11=DBLE(AZNOR(NMAPPED))
      AZNOR22=DBLE(AZNOR( MAPPED))
C Generate vectors for emission, incidence in source coordinates
      VEMI22(1)=0.0D0
      VEMI22(2)=0.0D0
      VEMI22(3)=1.0D0
      TEMP=SIN(CLINC22*DEG_TO_RAD)
      VINC22(1)=TEMP*COS(AZINC22*DEG_TO_RAD)
      VINC22(2)=TEMP*SIN(AZINC22*DEG_TO_RAD)
      VINC22(3)=COS(CLINC22*DEG_TO_RAD)
C Generate dest. -->surface coordinate xformation
      TEMP=SIN(CLEMI11*DEG_TO_RAD)
      AS1(3,1)=TEMP*COS(AZEMI11*DEG_TO_RAD)
      AS1(3,2)=TEMP*SIN(AZEMI11*DEG_TO_RAD)
      AS1(3,3)=COS(CLEMI11*DEG_TO_RAD)
      AS1(2,1)=COS(AZNOR11*DEG_TO_RAD)
      AS1(2,2)=SIN(AZNOR11*DEG_TO_RAD)
      AS1(2,3)=AS1(2,1)*AS1(3,1)+AS1(2,2)*AS1(3,2)
      IF (AS1(3,3).EQ.0.0D0) THEN
         IF (AS1(2,1).EQ.0.0D0) THEN
            AS1(2,1)=0.0D0
            AS1(2,3)=0.0D0
         ELSE
            AS1(2,1)=0.0D0
            AS1(2,2)=0.0D0
            AS1(2,3)=SIGN(1.0D0,AS1(2,3)) !Sign of limit as DIP->90-
         END IF
      ELSE
         AS1(2,3)=-AS1(2,3)/AS1(3,3)
         TEMP=SQRT(AS1(2,1)**2.0D0+AS1(2,2)**2.0D0+AS1(2,3)**2.0D0)
         AS1(2,1)=AS1(2,1)/TEMP
         AS1(2,2)=AS1(2,2)/TEMP
         AS1(2,3)=AS1(2,3)/TEMP
      END IF
      AS1(1,1)=AS1(2,2)*AS1(3,3)-AS1(2,3)*AS1(3,2)
      AS1(1,2)=AS1(2,3)*AS1(3,1)-AS1(2,1)*AS1(3,3)
      AS1(1,3)=AS1(2,1)*AS1(3,2)-AS1(2,2)*AS1(3,1)
C Generate source-->surface coordinate xformation
      TEMP=SIN(CLEMI22*DEG_TO_RAD)
      AS2(3,1)=TEMP*COS(AZEMI22*DEG_TO_RAD)
      AS2(3,2)=TEMP*SIN(AZEMI22*DEG_TO_RAD)
      AS2(3,3)=COS(CLEMI22*DEG_TO_RAD)
      AS2(2,1)=COS(AZNOR22*DEG_TO_RAD)
      AS2(2,2)=SIN(AZNOR22*DEG_TO_RAD)
      AS2(2,3)=AS2(2,1)*AS2(3,1)+AS2(2,2)*AS2(3,2)
      IF (AS2(3,3).EQ.0.0D0) THEN
         IF (AS2(2,1).EQ.0.0D0) THEN
            AS2(2,1)=0.0D0
            AS2(2,3)=0.0D0
         ELSE
            AS2(2,1)=0.0D0
            AS2(2,2)=0.0D0
            AS2(2,3)=SIGN(1.0D0,AS2(2,3)) !Sign of limit as DIP->90-
         END IF
      ELSE
         AS2(2,3)=-AS2(2,3)/AS2(3,3)
         TEMP=SQRT(AS2(2,1)**2.0D0+AS2(2,2)**2.0D0+AS2(2,3)**2.0D0)
         AS2(2,1)=AS2(2,1)/TEMP
         AS2(2,2)=AS2(2,2)/TEMP
         AS2(2,3)=AS2(2,3)/TEMP
      END IF
      AS2(1,1)=AS2(2,2)*AS2(3,3)-AS2(2,3)*AS2(3,2)
      AS2(1,2)=AS2(2,3)*AS2(3,1)-AS2(2,1)*AS2(3,3)
      AS2(1,3)=AS2(2,1)*AS2(3,2)-AS2(2,2)*AS2(3,1)
C Calculate source-->dest. coordinate xformation
      CALL MAT_MTRX_TRPSE(AS1,AS2,3,3,3,A12)
C Transform source emission, incidence vectors to dest. coordinates
      CALL MAT_MTRX_PROD(A12,VEMI22,3,3,1,VEMI12)
      CALL MAT_MTRX_PROD(A12,VINC22,3,3,1,VINC12)
C Calculate angles describing source image geometry in
C dest. coordinates.
      IF ((VEMI12(1).EQ.0.0D0).AND.(VEMI12(2).EQ.0.0D0)) THEN
         AZEMI12=0.0D0
      ELSE
         AZEMI12=MOD(ATAN2(VEMI12(2),VEMI12(1))*RAD_TO_DEG,3.6D2)
      END IF
      CLEMI12=ACOS(VEMI12(3))*RAD_TO_DEG
      IF ((VINC12(1).EQ.0.0D0).AND.(VINC12(2).EQ.0.0D0)) THEN
         AZINC12=0.0D0
      ELSE
         AZINC12=MOD(ATAN2(VINC12(2),VINC12(1))*RAD_TO_DEG,3.6D2)
      END IF
      CLINC12=ACOS(VINC12(3))*RAD_TO_DEG
C Translate the angles for the source image wrt destination pov
C back to single precision and output them
      CLINC(MAPPED)=SNGL(CLINC12)
      AZINC(MAPPED)=SNGL(AZINC12)
      CLEMI(MAPPED)=SNGL(CLEMI12)
      AZEMI(MAPPED)=SNGL(AZEMI12)
      RETURN
      END
