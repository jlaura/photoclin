      SUBROUTINE PCSI_XCSEP(IDUM,NMAX,N1,N2,N3,N4,N5,NX,NY,FUNC)
      IMPLICIT NONE
#include "isis_memory.inc"
#include "clinom_trig.inc"
C
C_TITLE	PCSI_XCSEP - Calculate SSIPSF-PI approximation to topography
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4       NMAX            !I	Size of IDUM
        INTEGER*4	IDUM(1)         !B	Dummy array containing
C						all arrays to be used
	INTEGER*4       N1		!I	Ptr to image in IDUM
	INTEGER*4       N2		!I	Ptr to topo in IDUM
	INTEGER*4       N3		!I	Ptr to rhs in IDUM
	INTEGER*4       N4		!I	Ptr to row matrix in IDUM
	INTEGER*4       N5		!I	Ptr to col matrix in IDUM
	INTEGER*4	NX,NY		!I	Sample, line dimensions of
C						image
	INTEGER*4       FUNC            !I	Brightness fn and derivs
C                                               1 = PBDER
C
C_DESC	Rapidly solves the clinometric equations for topography
C	from brightness, linearized about the datum, by exploiting
C	the fact that they are describable by a seperable, spatially
C	invariant point-spread function (SSIPSF) taking Z to B.
C	Exact inversion to get Z would require boundary conditions;
C	in their absence, the pseudo-inverse (PI) is taken, yielding
C	the topography with least RMS amplitude relative to the datum.
C
C	In the general case, exploiting the seperability of the equations
C	requires rotating the image so that the characteristic direction
C	(the direction in which slopes have the maximum effect on 
C	brightness) lies along the line or sample direction---then rotating
C	the topographic solution back to the original orientation.
C
C_CALLS	ADJZCOLS,ADJZROWS,LINMAP32,XCSEP1 (CLINOM.OLB)
C
C_HIST	28-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C		25-Jun-02 RLK Allow for aspect ratio
C
C_END
C
#include "clinom_aepar.inc"
#include "clinom_datum.inc"
#include "clinom_ops.inc"
#include "clinom_pgeom.inc"
#include "clinom_ppars.inc"
#include "clinom_msg.inc"
      EXTERNAL PBDER
      REAL*4 TR(6),TRI(6)	! Trace matrix for rotating the image to
C				  the new coordinates and its inverse
      REAL*4 BORDER		! Rotated Z will have this many pixels
C				  border around the data for unrotated Z
      REAL*4 XMIN,XMAX		! Sample range of B, including border
      REAL*4 YMIN,YMAX		! Line range of B, including border
      REAL*4 XRMIN,XRMAX	! Sample range of BR, including border
      REAL*4 YRMIN,YRMAX	! Line range of BR, including border
      INTEGER*4 NXR,NYR		! Dimensions of the rotated image
      INTEGER*4 N1R,N2R,N3R,N4R,N5R,N6R ! Pointers to rotated arrays in IDUM
      INTEGER*4 IOPT		! Controls how LINMAP treats pixels mapping
C				  outside the data to be rotated
      INTEGER*4 INDEX		! Index to input images: 1=numerator or the
C				  only image; 2=denominator if any
      REAL*4 SI1OLD(2),SI2OLD(2) ! Save SI1, SI2
      REAL*4 SE1OLD(2),SE2OLD(2) ! Save SE1, SE2
      REAL*4 DZ10OLD,DZ20OLD	! Save DZ10, DZ20
      INTEGER*4 DATUMTYPOLD	! Save DATUMTYP
      REAL*4 ASPECTOLD		! Save ASPECT
C
      REAL*4 COD,SID            ! Cosine and sine of DELAZ
      REAL*4 HAP,HAM		! Half (ASPECT plus/minus 1)
C				  used to convert inc/ema vectors to ASPECT=1
      REAL*4 HAIP,HAIM		! Half (1/ASPECT plus/minus 1)
C				  used to convert datum derivatives to ASPECT=1
      REAL*4 TEMP		! Temp var used in conversion
C
      IF ((DELAZ.EQ.0.0).AND.(ASPECT.EQ.1.0)) THEN
         IF (FUNC.EQ.1) THEN
           CALL XCSEP1(IDUM(N1),IDUM(N2),IDUM(N3),IDUM(N4),IDUM(N5),
     *   NX,NY,PBDER)
	 ENDIF
      ELSE
C Trace for the forward rotation (by DELAZ degrees) taking unrotated
C to rotated B. Translation (TR(1),TR(4)) is chosen so that corresponding
C Z will contain all points in the unrotated Z, with a 1/2 pixel safety
C margin because it is not obvious how to choose a "padding" value of Z
C in the general case of a tilted datum plane.  (With coordinates of pixel
C centers in the B's being their integer indices, the coordinates of pixel
C centers in the Z's will have half-integer values.)
         COD=COS(DELAZ*DEG_TO_RAD)
         SID=SIN(DELAZ*DEG_TO_RAD)
         TR(2)=COD*ASPECT
         TR(3)=-SID
         TR(5)=SID*ASPECT
         TR(6)=COD
         BORDER=0.5
         XMIN=1.0-BORDER
         XMAX=REAL(NX)+BORDER
         YMIN=1.0-BORDER
         YMAX=REAL(NY)+BORDER
         XRMIN=MIN(TR(2)*XMIN+TR(3)*YMIN,TR(2)*XMAX+TR(3)*YMIN,
     *             TR(2)*XMIN+TR(3)*YMAX,TR(2)*XMAX+TR(3)*YMAX)
         XRMAX=MAX(TR(2)*XMIN+TR(3)*YMIN,TR(2)*XMAX+TR(3)*YMIN,
     *             TR(2)*XMIN+TR(3)*YMAX,TR(2)*XMAX+TR(3)*YMAX)
         YRMIN=MIN(TR(5)*XMIN+TR(6)*YMIN,TR(5)*XMAX+TR(6)*YMIN,
     *             TR(5)*XMIN+TR(6)*YMAX,TR(5)*XMAX+TR(6)*YMAX)
         YRMAX=MAX(TR(5)*XMIN+TR(6)*YMIN,TR(5)*XMAX+TR(6)*YMIN,
     *             TR(5)*XMIN+TR(6)*YMAX,TR(5)*XMAX+TR(6)*YMAX)
         TR(1)=BORDER-XRMIN
         TR(4)=BORDER-YRMIN
         NXR=INT(XRMAX-XRMIN+1.0-2.0*BORDER)
         IF (REAL(NXR).LT.XRMAX-XRMIN+1.0-2.0*BORDER) NXR=NXR+1
         NYR=INT(YRMAX-YRMIN+1.0-2.0*BORDER)
         IF (REAL(NYR).LT.YRMAX-YRMIN+1.0-2.0*BORDER) NYR=NYR+1
C Trace for the inverse of the above transformation, actually used in the
C interpolation.
         TRI(2)= TR(6)/ASPECT
         TRI(3)=-TR(3)/ASPECT
         TRI(5)=-TR(5)/ASPECT
         TRI(6)= TR(2)/ASPECT
         TRI(1)=-(TRI(2)*TR(1)+TRI(3)*TR(4))
         TRI(4)=-(TRI(5)*TR(1)+TRI(6)*TR(4))
C Allocate memory, rotate B, and perform SSIPSF-PI photoclinometry
         N1R=N3
         N2R=N1R+NXR*NYR
         N3R=N2R+(NXR+1)*(NYR+1)
         N4R=N3R+NXR*NYR
         N5R=N4R+2*NXR
         N6R=N5R+2*NYR
         IF (N6R.GT.NMAX) GOTO 9000
         IOPT=1
         CALL LINMAP32(IDUM(N1),NX,NY,IDUM(N1R),NXR,NYR,TRI,BNORM,
     *   IOPT)
         HAP=0.5*(ASPECT+1)
         HAM=0.5*(ASPECT-1)
         DO INDEX=1,NPS
            SI1OLD(INDEX)=SI1(INDEX)
            SI2OLD(INDEX)=SI2(INDEX)
            SI1(INDEX)= SI1OLD(INDEX)*HAP+SI2OLD(INDEX)*HAM
            SI2(INDEX)= SI1OLD(INDEX)*HAM+SI2OLD(INDEX)*HAP
            TEMP=       SI1(INDEX)*COD+SI2(INDEX)*SID
            SI2(INDEX)=-SI1(INDEX)*SID+SI2(INDEX)*COD
            SI1(INDEX)=TEMP
            SE1OLD(INDEX)=SE1(INDEX)
            SE2OLD(INDEX)=SE2(INDEX)
            SE1(INDEX)= SE1OLD(INDEX)*HAP+SE2OLD(INDEX)*HAM
            SE2(INDEX)= SE1OLD(INDEX)*HAM+SE2OLD(INDEX)*HAP
            TEMP=       SE1(INDEX)*COD+SE2(INDEX)*SID
            SE2(INDEX)=-SE1(INDEX)*SID+SE2(INDEX)*COD
            SE1(INDEX)=TEMP
         ENDDO
         HAIP=0.5*(1.0/ASPECT+1)
         HAIM=0.5*(1.0/ASPECT-1)
         DZ10OLD=DZ10
         DZ20OLD=DZ20
         DZ10= DZ10*HAIP+DZ20*HAIM
         DZ20=-DZ10*HAIM+DZ20*HAIP
         TEMP= DZ10*COD+DZ20*SID
         DZ20=-DZ10*SID+DZ20*COD
         DZ10=TEMP
         DATUMTYPOLD=DATUMTYP
         IF (DATUMTYPOLD.EQ.2) DATUMTYP=1
         ASPECTOLD=ASPECT
         ASPECT=1.0
	 IF (FUNC.EQ.1) THEN
         CALL XCSEP1(IDUM(N1R),IDUM(N2R),IDUM(N3R),IDUM(N4R),IDUM(N5R),
     *   NXR,NYR,PBDER)
	 ENDIF
         DO INDEX=1,NPS
            SI1(INDEX)=SI1OLD(INDEX)
            SI2(INDEX)=SI2OLD(INDEX)
            SE1(INDEX)=SE1OLD(INDEX)
            SE2(INDEX)=SE2OLD(INDEX)
         ENDDO
         DZ10=DZ10OLD
         DZ20=DZ20OLD
         DATUMTYP=DATUMTYPOLD
         ASPECT=ASPECTOLD
C Modify the forward and inverse transformation traces so that points in
C the Z arrays correspond under their integer indices, rather than under
C their half-integer indices relative to the elements of the B arrays.
         TR(1)=TR(1)+0.5*(1.0-TR(2)-TR(3))
         TR(4)=TR(4)+0.5*(1.0-TR(5)-TR(6))
         TRI(1)=TRI(1)+0.5*(1.0-TRI(2)-TRI(3))
         TRI(4)=TRI(4)+0.5*(1.0-TRI(5)-TRI(6))
C Adjust the altitude array along rows or columns as appropriate, so that
C the solution has minimum RMS Z in the area for which there was really
C data, rather than over the whole rotated frame. this fixes the artifacts
C created by padding the rotated image (maybe).
          IF (DBRAT.GT.0.0) THEN
             CALL ADJZROWS(IDUM(N2R),NXR,NYR,TRI,NX,NY)
          ELSE
             CALL ADJZCOLS(IDUM(N2R),NXR,NYR,TRI,NX,NY)
          END IF
C Rotate the altitude array back to the original orientation.  Now the
C inverse trace of the rotation back is the forward trace of the original
C rotation.
         CALL LINMAP32(IDUM(N2R),NXR+1,NYR+1,IDUM(N2),NX+1,NY+1,TR,0.0,
     *   IOPT)
      END IF
      RETURN
C
C Error messages
C
9000  CALL U_ERROR('PCSI_XCSEP-NOMEM',
     *'Insufficient memory for rotated SSIPSF-PI',
     *-1,1)
      RETURN
      END
