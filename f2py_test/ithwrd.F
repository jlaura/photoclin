      SUBROUTINE ITHWRD(I,STR,WORD,LENW)
      IMPLICIT NONE
C
C_TITLE	ITHWRD - Select Ith "word" from a string
C
C_ARGS	TYPE      	VARIABLE 	I/O	DESCRIPTION
	INTEGER*4	I		!I	Which word to return
	CHARACTER*(*)	STR		!I	String to be parsed
	CHARACTER*(*)	WORD		!O	The desired word
	INTEGER*4	LENW		!O	Length of WORD, ignoring
C						trailing blanks; 0 if
C						there are <I words in STR
C						or Ith word is ',,'
C
C_DESC	Given a string and an integer I, returns the Ith "word" in the
C	string (counting from left to right), where a "word" is a set
C	of characters delimited on either side by a comma, a space, or
C	the limits of the input string.  Also returns the length of the
C	word, not counting trailing spaces.
C
C_HIST	30-Aug-90 Randolph Kirk U.S.G.S. Flagstaff Original Version
C
C_PAUSE
C
      INTEGER*4 LSTR			! Length of input string, not
C					  counting spaces
      INTEGER*4 J			! Loop counter
      INTEGER*4 IW			! Counter for words
      INTEGER*4 IBEG,IEND		! Pointer to beginning, end of
C					  words in STR

      LSTR=LEN(STR)
      DO J=1,LEN(WORD)
         WORD(J:J)=' '
      ENDDO
      IW=0
      IEND=0
   10 CONTINUE
         IW=IW+1
         IBEG=IEND
   20    CONTINUE
            IBEG=IBEG+1
            IF (IBEG.GT.LSTR) THEN
               LENW=0
               RETURN
            END IF
         IF (STR(IBEG:IBEG).EQ.' ') GO TO 20
         IEND=IBEG-1
   30    CONTINUE
            IEND=IEND+1
         IF ((STR(IEND:IEND).NE.',').AND.(STR(IEND:IEND).NE.' ')
     *   .AND.(STR(IEND:IEND).NE.';').AND.(STR(IEND:IEND).NE.'	')
     *   .AND.(IEND.LT.LSTR)) GO TO 30
         IF (IW.LT.I) THEN
         IF (IEND.LT.LSTR) THEN
      GO TO 10
      ELSE
      LENW=0
      END IF
      ELSE
      IF (IEND.LT.LSTR) IEND=IEND-1
      LENW=IEND-IBEG+1
      IF (LENW.GT.0) WORD(1:LENW)=STR(IBEG:IEND)
      END IF
      RETURN
      END
