      SUBROUTINE PROCFIL32(P,X,S,S2,N,NS,LINE,SAMP,
     * MIN,NORM,XADB,XMLT,TOL,STD,IFLAG)
      IMPLICIT NONE
C
C_TITLE	PROCFIL32 - Perform filter on one image line for FASTFIL32
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4 	NS		!I	Sample dimension of image
C						image arrays
	REAL*4	 	P(NS)		!I	Image to be filtered
	REAL*4	 	X(NS)		!O	Result: filtered image
	REAL*4	 	S(NS)		!I	Work buffer: accumulated
C						sums of pixel values
	REAL*4	 	S2(NS)		!I	Work buffer: accumulated
C						sums of squared pixel values
	INTEGER*4	N(NS)		!I	Work buffer: # of valid
C						pixels contributing to S,S2
	INTEGER*4 	LINE		!I	Line dimension of filter
	INTEGER*4 	SAMP		!I	Sample dimension of filter
	INTEGER*4	MIN		!I	Min # of valid pts in box
	REAL*4		NORM		!I	Additive offset for 
C						highpass, divide filters
	REAL*4		XADB		!I	Fraction of original value
C						used in highpass filter
	REAL*4		XMLT		!I	Multiplicative scaling for
C						output of filters
	REAL*4	 	TOL		!I	Tolerance for noise filter
	REAL*4		STD		!I	Tolerance for std. dev. filter
	INTEGER*4	IFLAG		!I	Controls filter option:
C						1=Lowpass filter
C						2=Lowpass filter nulls only
C						3=Lowpass filter non-nulls
C						4=Highpass filter
C						5=Divide filter
C						6="Trim" (no operation)
C						7="Noise filter"
C						8=Sum filter
C						9=Std dev to lowpass filter
C						10=Std dev to null filter
C						11=Output std dev
C
C_DESC	This is an adaptation of the fast, flexible boxcar filter
C	algorithm of Eric Eliason to operation on an array resident in
C	memory.  Further details may be found in the documentation for
C	the PICS programs FLT8B, FLT16B, FLT32B.  This routine calculates
C	one line of the output (filtered) image, given the input line and
C	the accumulation arrays with the number of valid pixels, sum of
C	pixels, and sum of squares of pixels at each point.
C
C_HIST	10-AUG-89 Randolph Kirk U.S.G.S. Flagstaff Original Version
C	15-AUG-89 RLK - No longer correct STDEV for center pixel
C	              - Accumulate SI, S2I in double precision
C	11-SEP-89 RLK - Add SDEV mode (11) to output std deviation
C
C_PAUSE
C
#include "clinom_specpix.inc"
C      REAL*4 NULL			! Null pixel flag (all bits on)
      INTEGER*4 ORDER			! Order (highest power) of pixel
C					  values accumulated:
C					  0 -> # of pixels only
C					  1 -> sum of DN's
C					  2 -> sum of squares of DN's
      INTEGER*4 IPSAMP			! Offset from current sample to sample
C					  being removed from filter
      INTEGER*4 IFSAMP			! Offset from current sample to sample
C					  being added to filter
      REAL*4 XV1,XV2,XV3		! Coefficients for highpass filter
      INTEGER*4 IS			! Loop counter: current sample
      INTEGER*4 SAMPOUT			! Sample being removed from filter
      INTEGER*4 SAMPIN			! Sample being added to filter
      INTEGER*4 NI			! # of pixels contributing to filter
      REAL*8 SI				! Sum of pixels in filter
      REAL*8 S2I			! Sum of squares of pixels in filter
      REAL*4 LPF			! Boxcar average (lowpass filter)
      REAL*4 LPFC			! LPF corrected for center pixel value
      REAL*4 ADIF			! Abs difference between pixel value
C					  and lowpass filter value
      REAL*4 STDEV			! Boxcar standard deviation

C      NULL='FFFFFFFF'X
      IPSAMP=-(SAMP/2+1)
      IFSAMP=SAMP/2
      IF (IFLAG.EQ.1) THEN
         ORDER=1
      ELSE IF (IFLAG.EQ.2) THEN
         ORDER=1
      ELSE IF (IFLAG.EQ.3) THEN
         ORDER=1
      ELSE IF (IFLAG.EQ.4) THEN
         ORDER=1
         XV1=NORM*(1.0-XADB)
         XV2=XMLT*(1.0+XADB)
         XV3=-XMLT
      ELSE IF (IFLAG.EQ.5) THEN
         ORDER=1
      ELSE IF (IFLAG.EQ.6) THEN
         ORDER=0
      ELSE IF (IFLAG.EQ.7) THEN
         ORDER=1
      ELSE IF (IFLAG.EQ.8) THEN
         ORDER=1
      ELSE IF (IFLAG.EQ.9) THEN
         ORDER=2
      ELSE IF (IFLAG.EQ.10) THEN
         ORDER=2
      ELSE IF (IFLAG.EQ.11) THEN
         ORDER=2
      END IF
C
C Initialize the variables for the horizontal boxcar operation, which
C will be updated sample by sample and added to the corresponding samples
C of the accumulation buffers
C
      NI=0
      IF (ORDER.GE.1) SI=0.0D0
      IF (ORDER.GE.2) S2I=0.0D0
      IF (IFSAMP.GT.0) THEN
         NI=NI+N(1)
         IF (ORDER.GE.1) SI=SI+DBLE(S(1))
         IF (ORDER.GE.2) S2I=S2I+DBLE(S2(1))
         DO IS=2,IFSAMP
            NI=NI+2*N(IS)
            IF (ORDER.GE.1) SI=SI+2.0D0*DBLE(S(IS))
            IF (ORDER.GE.2) S2I=S2I+2.0D0*DBLE(S2(IS))
         ENDDO
         NI=NI+N(IFSAMP+1)
         IF (ORDER.GE.1) SI=SI+DBLE(S(IFSAMP+1))
         IF (ORDER.GE.2) S2I=S2I+DBLE(S2(IFSAMP+1))
      END IF
      NI=NI+N(IFSAMP+2)
      IF (ORDER.GE.1) SI=SI+DBLE(S(IFSAMP+2))
      IF (ORDER.GE.2) S2I=S2I+DBLE(S2(IFSAMP+2))
C
C For each sample of the image line:
C	calculate the sample falling out of the filter
C	subtract its contributions from NI, SI, S2I
C	calculate the sample falling into the filter
C	add its contributions to NI, SI, S2I
C	use the accumulated quantities to perform the filter calculation
C
      DO IS=1,NS
         SAMPOUT=IS+IPSAMP
         IF (SAMPOUT.LT.1) SAMPOUT=2-SAMPOUT
         NI=NI-N(SAMPOUT)
         IF (ORDER.GE.1) SI=SI-DBLE(S(SAMPOUT))
         IF (ORDER.GE.2) S2I=S2I-DBLE(S2(SAMPOUT))
         SAMPIN=IS+IFSAMP
         IF (SAMPIN.GT.NS) SAMPIN=2*NS-SAMPIN
         NI=NI+N(SAMPIN)
         IF (ORDER.GE.1) SI=SI+DBLE(S(SAMPIN))
         IF (ORDER.GE.2) S2I=S2I+DBLE(S2(SAMPIN))
C
C Perform the filter on each sample of the image line
C
         IF (NI.LT.MIN) THEN
C            X(IS)=NULL
            X(IS)=NULL4
         ELSE
            LPF=SNGL(SI/REAL(NI))
            IF (IFLAG.EQ.1) THEN				! LPF
               X(IS)=XMLT*LPF
            ELSE IF (IFLAG.EQ.2) THEN				! LPFZ
C               IF (P(IS).EQ.NULL) THEN
               IF (P(IS).LT.VALID_MIN4.OR.P(IS).GT.VALID_MAX4) THEN
                  X(IS)=XMLT*LPF
               ELSE
                  X(IS)=P(IS)
               END IF
            ELSE IF (IFLAG.EQ.3) THEN				! LPFV
C               IF (P(IS).EQ.NULL) THEN
               IF (P(IS).LT.VALID_MIN4.OR.P(IS).GT.VALID_MAX4) THEN
                  X(IS)=P(IS)
               ELSE
                  X(IS)=LPF
               END IF
            ELSE IF (IFLAG.EQ.4) THEN				! HPF
C               IF (P(IS).NE.NULL) THEN
               IF (P(IS).GE.VALID_MIN4.AND.P(IS).LE.VALID_MAX4) THEN
                  X(IS)=XV1+XV2*P(IS)+XV3*LPF
               ELSE
C                  X(IS)=NULL
                  X(IS)=NULL4
               END IF
            ELSE IF (IFLAG.EQ.5) THEN				! DIV
C               IF ((P(IS).NE.NULL).AND.(SI.NE.0.0)) THEN
               IF ((P(IS).GE.VALID_MIN4.AND.P(IS).LE.VALID_MAX4)
     .             .AND.(SI.NE.0.0)) THEN
                  X(IS)=XMLT*P(IS)/LPF-NORM
               ELSE
C                  X(IS)=NULL
                  X(IS)=NULL4
               END IF
            ELSE IF (IFLAG.EQ.6) THEN				! TRIM
                  X(IS)=P(IS)
            ELSE IF (IFLAG.EQ.7) THEN				! NOIS
C               IF (P(IS).EQ.NULL) THEN
               IF (P(IS).LT.VALID_MIN4.OR.P(IS).GT.VALID_MAX4) THEN
                  X(IS)=LPF
               ELSE IF (NI.LE.1) THEN
                  X(IS)=P(IS)
               ELSE
                  LPFC=SNGL((SI-DBLE(P(IS)))/REAL(NI-1))
                  IF (ABS(P(IS)-LPFC).GT.TOL) THEN
                     X(IS)=LPFC
                  ELSE
                     X(IS)=P(IS)
                  END IF
               END IF
            ELSE IF (IFLAG.EQ.8) THEN				! SUM
               X(IS)=SNGL(SI)
            ELSE IF (IFLAG.EQ.9) THEN				! STD
C               IF (P(IS).EQ.NULL) THEN
               IF (P(IS).LT.VALID_MIN4.OR.P(IS).GT.VALID_MAX4) THEN
                  X(IS)=LPF
               ELSE IF (NI.LE.1) THEN
                  X(IS)=P(IS)
               ELSE
                  LPFC=SNGL((SI-DBLE(P(IS)))/REAL(NI-1))
                  ADIF=ABS(P(IS)-LPFC)
                  IF (ADIF.GT.TOL) THEN
                     X(IS)=LPFC
                  ELSE IF (NI.LE.2) THEN
                     X(IS)=P(IS)
                  ELSE
                     STDEV=SNGL(SQRT(MAX(1.0D-30,
     *                     S2I/REAL(NI)-DBLE(LPF)*DBLE(LPF))))
                     IF (ADIF.GT.STD*STDEV) THEN
                        X(IS)=LPFC
                     ELSE
                        X(IS)=P(IS)
                     END IF
                  END IF
               END IF
            ELSE IF (IFLAG.EQ.10) THEN				! STDZ
C               IF (P(IS).EQ.NULL) THEN
               IF (P(IS).LT.VALID_MIN4.OR.P(IS).GT.VALID_MAX4) THEN
C                  X(IS)=NULL
                  X(IS)=NULL4
               ELSE IF (NI.LE.1) THEN
                  X(IS)=P(IS)
               ELSE
                  LPFC=SNGL((SI-DBLE(P(IS)))/REAL(NI-1))
                  ADIF=ABS(P(IS)-LPFC)
                  IF (ADIF.GT.TOL) THEN
C                     X(IS)=NULL
                     X(IS)=NULL4
                  ELSE IF (NI.LE.2) THEN
                     X(IS)=P(IS)
                  ELSE
                     STDEV=SNGL(SQRT(MAX(1.0D-30,
     *                     S2I/REAL(NI)-DBLE(LPF)*DBLE(LPF))))
                     IF (ADIF.GT.STD*STDEV) THEN
C                        X(IS)=NULL
                        X(IS)=NULL4
                     ELSE
                        X(IS)=P(IS)
                     END IF
                  END IF
               END IF
            ELSE IF (IFLAG.EQ.11) THEN			! SDEV
               IF (NI.LE.2) THEN
C                  X(IS)=NULL
                  X(IS)=NULL4
               ELSE
                  X(IS)=XMLT*SNGL(SQRT(MAX(1.0D-30,
     *                     S2I/REAL(NI)-DBLE(LPF)*DBLE(LPF))))
               END IF
            END IF
         END IF
      ENDDO
      RETURN
      END
