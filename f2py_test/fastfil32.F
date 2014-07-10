      SUBROUTINE FASTFIL32(P,X,S,S2,N,NL,NS,LINE,SAMP,
     * XFRC,XMIN,NORM,XADB,XMLT,TOL,STD,FILT,ERRVAL)
      IMPLICIT NONE
C
C_TITLE	FASTFIL32 - Fast boxcar filter on a REAL*4 array in memory
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4 	NL,NS		!I	Line, sample dimensions of
C						image arrays
	REAL*4	 	P(NS,NL)	!I	Image to be filtered
	REAL*4	 	X(NS,NL)	!O	Result: filtered image
	REAL*4	 	S(NS)		!-	Work buffer: accumulated
C						sums of pixel values
	REAL*4	 	S2(NS)		!-	Work buffer: accumulated
C						sums of squared pixel values
	INTEGER*4	N(NS)		!-	Work buffer: # of valid
C						pixels contributing to S,S2
	INTEGER*4 	LINE		!I	Line dimension of filter
	INTEGER*4 	SAMP		!I	Sample dimension of filter
	REAL*4		XFRC		!I	Fraction of valid pts 
C						required in box
	REAL*4		XMIN		!I	Min # of valid pts in box
	REAL*4		NORM		!I	Additive offset for 
C						highpass, divide filters
	REAL*4		XADB		!I	Fraction of original value
C						used in highpass filter
	REAL*4		XMLT		!I	Multiplicative scaling for
C						output of filters
	REAL*4	 	TOL		!I	Tolerance for noise filter
	REAL*4		STD		!I	Tolerance for std. dev. filter
	CHARACTER*(*) 	FILT		!I	Controls filter option:
C						'LPF'=Lowpass filter
C						'LPFZ'=Lowpass filter nulls only
C						'LPFV'=Lowpass filter non-nulls
C						'HPF'=Highpass filter
C						'DIV'=Divide filter
C						'TRIM'="Trim" (no operation)
C						'NOIS'="Noise filter"
C						'SUM'=Sum filter
C						'STD'=Std dev to lowpass filter
C						'STDZ'=Std dev to null filter
C						'SDEV'=Output the std dev
	INTEGER*4	ERRVAL		!O	0=No error
C						-ve=error in this routine
C
C_DESC	This is an adaptation of the fast, flexible boxcar filter
C	algorithm of Eric Eliason to operation on an array resident in
C	memory.  Further details may be found in the documentation for
C	the PICS programs FLT8B, FLT16B, FLT32B.
C
C EQUATIONS FOR FILTERING OPERATIONS.
C
C    P(I,J) = ORIGINAL DN VALUE OF INPUT IMAGE AT I,J
C    X(I,J) = OUTPUT DN VALUE OF RESULTING FILTER OPERATION
C    S(I,J) = SUM OF POINTS OVER THE BOXCAR CENTERED AT I,J
C    N(I,J) = NUMBER OF VALID POINTS IN THE BOXCAR CENTERED AT I,J
C    S2(I,J)= SUM OF SQAURE OF POINTS OVER THE BOXCAR CENTERED AT I,J
C
C LOW PASS FILTER DEFINITION (LPF,LPFV,LPFZ)
C
C    LPF(I,J) = S(I,J)/N(I,J) = LOW PASS FILTER
C    X(I,J) = XMLT*LPF(I,J) = RESULT IS LOW PASS FILTER TIMES A CONSTANT
C
C HIGH PASS FILTER DEFINITION (HPF)
C
C    HPF(I,J) = HIGH PASS FILTER
C
C    HPF(I,J) = NORM*(1-XADB) + P(I,J)*XMLT*(1+XADB) - LPF(I,J)*XMLT
C
C DIVIDE FILTER DEFINITION (DIV)
C
C    DIV(I,J) = DIVIDE FILTER
C
C    X(I,J)=DIV(I,J)= XMLT*(P(I,J)/LPF(I,J)) - NORM
C
C
C NOISE FILTER OPTION  (NOIS)
C  
C    X(I,J) = P(I,J)   IF  ABS(P(I,J)-LPF(I,J)) <= TOL
C    X(I,J) = LPF(I,J) IF  ABS(P(I,J)-LPF(I,J)) >  TOL
C
C STD filter option
C   
C   STDEV(i,j) = standard deviation of boxcar centered at i,j
C   STDEV(i,j) = (SQRT(S2(i,j)*N(i,j) - S(i,j)**2))/N(i,j)
C
C   X(i,j) = P(i,j)    if ABS(P(i,j)-LPF(i,j)) <= STD*STDEV(i,j)
C   X(i,j) = LPF(i,j)  if ABS(P(i,j)-LPF(i,j)) >  STD*STDEV(i,j)
C					    or >  TOL
C    
C STDZ filter option
C   
C   X(i,j) = P(i,j)    if ABS(P(i,j)-LPF(i,j)) <= STD*STDEV(i,j)
C   X(i,j) = NULL      if ABS(P(i,j)-LPF(i,j)) >  STD*STDEV(i,j)
C					    or >  TOL
C    
C NOTE: the LPF is corrected for the center point (it is
C subtracted) but the STDEV is not in the NOIS, STD, STDZ options.
C Not correcting STDEV forestalls potentially fatal arithmetic
C error due to roundoff and merely makes a given value of STD filter
C more aggressively
C
C SDEV filter option
C
C   X(i,j) = XMLT*STDEV(i,j)
C
C_CALLS	R2R,L2L,ADDFIL32,SUBFIL32,PROCFIL32,ERRMES
C
C_HIST	10-AUG-89 Randolph Kirk U.S.G.S. Flagstaff Original Version
C	11-Sep-89 RLK - added SDEV option
C
C_PAUSE
C
      REAL*4 XPTS			! Area of boxcar (# of pixels)
      INTEGER*4 MIN			! Minimum # of valid pixels in box
C					  for filter to be performed, taking
C					  account of XMIN and XFRC
      INTEGER*4 IFLAG			! Filter option corrsponding to FILT
      INTEGER*4 ORDER			! Order (highest power) of pixel
C					  values accumulated:
C					  0 -> # of pixels only
C					  1 -> sum of DN's
C					  2 -> sum of squares of DN's
      INTEGER*4 IPLINE			! Offset from current line to line
C					  being removed from filter
      INTEGER*4 IFLINE			! Offset from current line to line
C					  being added to filter
      INTEGER*4 IL			! Loop counter: current line
      INTEGER*4 LINEOUT			! Line being removed from filter
      INTEGER*4 LINEIN			! Line being added to filter
      INTEGER*4 I
      CHARACTER ERRKEY*17               ! Keyword value for error
C                                         message
      CHARACTER ERRMSG*80               ! Error message

      ERRVAL=0
      XPTS = REAL(LINE)*REAL(SAMP)
      IF (XFRC.NE.0.0) XMIN = XPTS*XFRC
      IF (XFRC.EQ.0.0.AND.XMIN.NE.0.0) XFRC = XMIN/XPTS
      IF (XMIN.LT.1.0) XMIN=1.0
      MIN=NINT(XMIN)
      IF (FILT.EQ.'LPF') THEN
         IFLAG=1
         ORDER=1
      ELSE IF (FILT.EQ.'LPFZ') THEN
         IFLAG=2
         ORDER=1
      ELSE IF (FILT.EQ.'LPFV') THEN
         IFLAG=3
         ORDER=1
      ELSE IF (FILT.EQ.'HPF') THEN
         IFLAG=4
         ORDER=1
      ELSE IF (FILT.EQ.'DIV') THEN
         IFLAG=5
         ORDER=1
      ELSE IF (FILT.EQ.'TRIM') THEN
         IFLAG=6
         ORDER=0
      ELSE IF (FILT.EQ.'NOIS') THEN
         IFLAG=7
         ORDER=2
      ELSE IF (FILT.EQ.'SUM') THEN
         IFLAG=8
         ORDER=1
      ELSE IF (FILT.EQ.'STD') THEN
         IFLAG=9
         ORDER=2
      ELSE IF (FILT.EQ.'STDZ') THEN
         IFLAG=10
         ORDER=2
      ELSE IF (FILT.EQ.'SDEV') THEN
         IFLAG=11
         ORDER=2
      ELSE
         IFLAG=0
         GO TO 9002
      END IF
      IF (LINE.EQ.LINE/2+LINE/2) GO TO 9001
      IF (SAMP.EQ.SAMP/2+SAMP/2) GO TO 9001
      IPLINE=-(LINE/2+1)
      IFLINE=LINE/2
C
C Initialize filter buffers (including adding the line that will be 
C subtracted before the filter is performed on the first image line)
C
      DO I = 1,NS
        N(I)=0
      ENDDO
C      CALL U_FILL4(0,NS,N)
      IF (ORDER.GE.1) CALL U_FILL4(0.0,NS,S)
      IF (ORDER.GE.2) CALL U_FILL4(0.0,NS,S2)
      IF (IFLINE.GT.0) THEN
         CALL ADDFIL32(P(1,1),S,S2,N,NS,SAMP,ORDER)
         DO IL=2,IFLINE
            CALL ADDFIL32(P(1,IL),S,S2,N,NS,SAMP,ORDER)
            CALL ADDFIL32(P(1,IL),S,S2,N,NS,SAMP,ORDER)
         ENDDO
         CALL ADDFIL32(P(1,IFLINE+1),S,S2,N,NS,SAMP,ORDER)
      END IF
      CALL ADDFIL32(P(1,IFLINE+2),S,S2,N,NS,SAMP,ORDER)
C
C For each line of the image:
C	calculate the line falling out of the filter
C	subtract its contributions from N, S, S2
C	calculate the line falling into the filter
C	add its contributions to N, S, S2
C	run the horizontal boxcar across the accumulation buffers
C          and perform the appropriate filter operation
C
      DO IL=1,NL
         LINEOUT=IL+IPLINE
         IF (LINEOUT.LT.1) LINEOUT=2-LINEOUT
         CALL SUBFIL32(P(1,LINEOUT),S,S2,N,NS,SAMP,ORDER)
         LINEIN=IL+IFLINE
         IF (LINEIN.GT.NL) LINEIN=2*NL-LINEIN
         CALL ADDFIL32(P(1,LINEIN),S,S2,N,NS,SAMP,ORDER)
         CALL PROCFIL32(P(1,IL),X(1,IL),S,S2,N,NS,LINE,SAMP,
     *   MIN,NORM,XADB,XMLT,TOL,STD,IFLAG)
      ENDDO
      RETURN
C
C Handle processing errors
C
9001  CONTINUE
      ERRKEY = 'FASTFIL32-BOXCAR'
      ERRMSG = 'Boxcar dimension must be odd'
      ERRVAL = -1
      GOTO 9999

9002  CONTINUE
      ERRKEY = 'FASTFIL32-FILTER'
      ERRMSG = 'Invalid filter option'
      ERRVAL = -2
      GOTO 9999
C
C Write an error message
C
9999  CONTINUE
      CALL U_ERROR(ERRKEY,ERRMSG,ERRVAL,1)
      RETURN
      END
