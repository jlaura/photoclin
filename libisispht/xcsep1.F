      SUBROUTINE XCSEP1(B,Z,DB,AR,AC,NX,NY,BDER)
      IMPLICIT NONE
C
C_TITLE	XCSEP1 - Calculate SSIPSF-PI approximation to topography
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
        INTEGER*4	NX,NY		!I	Sample, line dimensions
        REAL*4		B(NX,NY)	!I	Brightness image
	REAL*4		Z(NX+1,NY+1)	!O	Topographic array
	REAL*4		DB(NX,NY)	!-	Scaled deviation of B 
C						from datum value; also 
C						used as buffer for 
C						destriping
	REAL*4		AR(NX,2)	!- 	Matrix to be inverted on rows
	REAL*4		AC(NY,2)	!-	Matrix to be inverted on cols
	EXTERNAL	BDER		!I	Brightness fn and derivs
C
C_DESC	Rapidly solves the clinometric equations for topography
C	from brightness, linearized about the datum, by exploiting
C	the fact that they are describable by a seperable, spatially
C	invariant point-spread function (SSIPSF) taking Z to B.
C	Exact inversion to get Z would require boundary conditions;
C	in their absence, the pseudo-inverse (PI) is taken, yielding
C	the topography with least RMS amplitude relative to the datum.
C
C	The image must first have been rotated (in XCSEP) so that the 
C	derivative of brightness with respect to additional tilts along 
C	either the row or the column direction vanishes; the result must 
C	then be rotated back into the original coordinate system and the 
C	tilted-plane values added into the increments calculated here.
C
C_CALLS	AE0CALC,DESTRIPE,S3SOLV,S3SOLV2,
C	Z0CALC (CLINOM.OLB)
C
C_HIST	23-AUG-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      INTEGER*4 J		! Loop counter
C
C Calculate RHS, calculate and factor row and column matrices
      CALL AE0CALC(B,DB,AR,AC,NX,NY,BDER)
C Solve row equations, one at a time
      DO J=1,NY
         CALL S3SOLV(AR,DB(1,J),NX)
      ENDDO
C Solve column equations, all in parallel
      CALL S3SOLV2(AC,DB,NX,NY)
C Construct altitudes by multiplying by (A transpose)
      CALL Z0CALC(DB,Z,NX,NY)
C Spatially filter the solution to remove stripes along characteristics
      CALL DESTRIPE(Z,DB,NX,NY)
      RETURN
      END
