C**************************************************************************
C_TITLE clinom_ops.inc
C
C_DESC  Common block to keep track of computational work
C
C_HIST  Feb 28 1989 Randolph Kirk, USGS Flagstaff, Original Version
C       Aug 26 1989 RLK - changed to double precision
C**************************************************************************
C Common block to keep track of computational work
      COMMON /OPS1/ NOPS
      COMMON /OPS2/ IGUESS

      REAL*4 NOPS
      INTEGER*4 IGUESS   ! Initial guess type:
C                          1 = Datum
C                          2 = Stored Z
C                          3 = SSIPSF-PI
C**************************************************************************
