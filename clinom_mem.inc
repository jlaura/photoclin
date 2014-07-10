C**************************************************************************
C_TITLE clinom_mem.inc
C
C_DESC  Common block defining the memory constraints on pc2d
C
C_HIST  Feb 28 1989 Randolph Kirk, USGS Flagstaff, Original Version
C**************************************************************************
C #include "isis_memory.inc"

      COMMON /MEM/ NC1M,MAXNK
      COMMON /MEM1/ U_BASE,U_MULT,C_BASE,C_MULT
      COMMON /MEM2/ OFF1,NLIMIT
      COMMON /MEM3/ NMAX

      INTEGER*4 NMAX			! Max allocatable memory (size of
C					  buffer memory IDUM) in longwords
      INTEGER*4 NLIMIT                  ! 
      INTEGER*4 OFF1                    ! Index and offset for IDUM array
      INTEGER*4 NC1M			! Max 2-D FFT allowed by memory
      INTEGER*4 MAXNK			! Max # of knots for spline fitting
      REAL*4 U_BASE,U_MULT              ! Scaling parameters used to
C                                         convert data as follows:
C                                         true_value = U_BASE +
C				          (U_MULT * stored_value)
C                                         This scaling is done after 
C                                         converting the cube data from 
C                                         internal format when reading 
C                                         from the ISIS file and is done
C                                         before converting the cube
C                                         data into internal format when
C                                         when writing to the ISIS
C                                         file.
      REAL*4 C_BASE,C_MULT              ! Core scaling parameters used
C                                         for converting ISIS cube data
C                                         to/from internal ISIS format.
C      DATA U_BASE,U_MULT,C_BASE,C_MULT/0.0,1.0,0.0,1.0/
C**************************************************************************
