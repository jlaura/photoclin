      SUBROUTINE pht_ITHWRD(I,STR,WORD,LENW)
C Selects the Ith "word" from a string, as delimited by commas or spaces.
C Added semicolons and tabs as delimiters, 10/22/90.
C Input:
C I     =which word to look for
C STR   =string to be parsed
C Output:
C WORD  =the desired word, extracted from STR
C LENW  =number of chars. in the word; 0 if there are < I words or Ith word
C        is ',,'
      CHARACTER STR*(*),WORD*(*)
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
