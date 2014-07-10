C**************************************************************************
C_TITLE clinom_ipars.inc
C
C_DESC  Common block with parameters controlling iteration
C
C_HIST  Mar 02 1991 Randolph Kirk, USGS Flagstaff, Original Version
C**************************************************************************
      COMMON /IPARS1/ AL,CONTRAST,WMAX,DIVTOL,DDZTOL,ETOL,BIGTOL,
     * OLDTOL,STD,CPULIMIT,WEIGHT0,TAUFAC,ALASYM,ALRAT
      COMMON /IPARS2/ ITMAX,DEPTHLIM,NDS,MAXPASS,MAXNR
      COMMON /IPARS3/ APOFILE
      COMMON /IPARS4/ EXPERT

      REAL*4 AL			! Inverse of penalty #
      REAL*4 CONTRAST		! Decrease brightness error by this much
      REAL*4 WMAX		! Asymtotic weight for SOR
      REAL*4 DIVTOL		! (Max Z change)/(RMS Z change) value
C				  that signals divergence at a point
C				  (Note that a global maximum is tested
C				  so we know IF divergence occurred, but
C				  not WHERE)
      INTEGER*4 ITMAX		! Max # of SOR steps per linearization
      REAL*4 DDZTOL		! Relinearize when each SOR step is less
C				  than this fraction of the total Z change
      REAL*4 ETOL		! RMS gradient (righthand side) value that
C				  indicates solution has converged, so stop
      REAL*4 BIGTOL		! Increase resolution when RMS gradient < 
C				  this fraction of prev value on finer mesh
      REAL*4 OLDTOL		! Decrease resolution when RMS gradient >
C				  this fraction of value at last linearization
      REAL*4 TAUFAC             ! Fraction of truncation error to achieve
C                                 for convergence
      REAL*4 ALASYM             ! Asymptotic value of AL as iteration proceeds
      REAL*4 ALRAT              ! Controls rate of approach of AL to ALASYM
      INTEGER*4 DEPTHLIM	! Decrease resolution by no more powers of
C				  two than this
      INTEGER*4 NDS(2)		! Size of box in which to take statistics 
C				  to detect local divergence of Z change
C				  NDS(1) is used for initial 'STDZ' pass
C				  and NDS(2) for subsequent 'STD' pass(es).
      REAL*4 STD(2)		! Z change is considered divergent at a pixel 
C				  if its difference from the local mean exceeds
C				  STD times the local std deviation.
C				  If divergent, the pixel value will be set
C				  to a null flag (first pass) or to the
C				  average of the neighboring values (sub-
C				  sequent passes)
      INTEGER*4 MAXPASS		! Max number of times the program will go
C				  through iteration at full resolution 
C				  before quitting
      INTEGER*4 MAXNR
      REAL*4 CPULIMIT		! Max number of cpu minutes the program 
C				  will expend before quitting
      REAL*4 WEIGHT0		! Weight accorded to a priori topographic
C				  constraints.  Set to zero to indicate
C				  no such constraints are to be applied
      CHARACTER APOFILE*80	! Name of file containing a priori topo
C				  values and relative weights
      LOGICAL*1 EXPERT          ! If false, do not prompt user for the
C                                 majority of the parameters
C**************************************************************************
