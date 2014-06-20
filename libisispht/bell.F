      SUBROUTINE BELL
      IMPLICIT NONE
C Rings the bell on the terminal.
      CHARACTER BEL*1
      BEL(1:1)= CHAR(7)
      PRINT 1000,BEL
 1000 FORMAT (A,$)
      RETURN
      END
