      SUBROUTINE INJ0(ZBIG,NXBIG,NYBIG,Z,NX,NY,FAC)
      IMPLICIT NONE
C
C_TITLE	INJ0 - Inject 1/4 of array values to a mesh with 1/2 the res
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	NXBIG,NYBIG	!I	Sample, line dimensions on
C						fine mesh
	REAL*4		ZBIG(NXBIG,NYBIG)!I	Data on fine mesh
	INTEGER*4	NX,NY		!I	Sample, line dimensions on
C						coarse mesh (1/2 as big)
	REAL*4		Z(NX,NY)	!O	Data on fine mesh: 
C						alternate lines and samples
C						of ZBIG
	REAL*4		FAC		!I	Factor to multiply by
C						in going to coarse mesh
C
C_DESC	Does the simplest kind of injection: copying 1/4 of the entries
C	of an array into one with half the dimensions.  The values may
C	be scaled by a given factor, e.g., in proportion to the mesh
C	spacing so that pixel differences will still be equal to pixel
C	derivatives in the injected dataset.
C
C_CALLS	None
C
C_HIST	21-Feb-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      INTEGER*4 I,J		! Loop counters (indices in Z)
      INTEGER*4 IB,JB		! Indices in ZB
C 
      IF (FAC.EQ.1.0) THEN
         DO J=1,NY
            JB=J+J-1
            DO I=1,NX
               IB=I+I-1
               Z(I,J)=ZBIG(IB,JB)
            ENDDO
         ENDDO
      ELSE
         DO J=1,NY
            JB=J+J-1
            DO I=1,NX
               IB=I+I-1
               Z(I,J)=FAC*ZBIG(IB,JB)
            ENDDO
      ENDDO
      END IF
      RETURN
      END
