C**************************************************************************
C_TITLE clinom_log.inc
C
C_DESC  Common block for output to log file
C
C_HIST  Feb 28 1989 Randolph Kirk, USGS Flagstaff, Original Version
C**************************************************************************
      COMMON /LOG1/ LOGIT
      COMMON /LOG2/ IULOG

      LOGICAL*1 LOGIT		! Record progress of iteration in a file?
      INTEGER*4 IULOG		! Unit # for log file
C**************************************************************************
