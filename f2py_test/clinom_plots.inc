C**************************************************************************
C_TITLE clinom_plots.inc
C
C_DESC  Common block for storing plot information
C
C_HIST  May 10 2001 Janet Barrett, USGS Flagstaff, Original Version
C**************************************************************************
      REAL*4 ERMSL_PLOT(0:30,6),DDZRMS_PLOT(0:30,6)
      REAL*4 ERMS_PLOT(0:51),DBRMS_PLOT(0:51)
      REAL*4 DZRMS_PLOT(0:51)

      INTEGER*4 SORPLOT,SORPLOTIDX(6),NRPLOTIDX
      INTEGER*4 NRPLOTMIN,NRPLOTMAX

      DATA SORPLOT,SORPLOTIDX/1,0,0,0,0,0,0/
      DATA NRPLOTIDX,NRPLOTMIN,NRPLOTMAX/1,1,50/
C**************************************************************************
