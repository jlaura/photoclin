#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "isisdef.h"
#include "isistypes.h"

FLOAT4 rando(void)
/*********************************************************************
*_Title rando Random number generator
*
*_Descr Random number generator
*
*_Hist  Mar 12 2003 Janet Barrett USGS Flagstaff Original Version
*
*_End
*********************************************************************/
{
  FLOAT4 rnd;
  INT4 i,stime,ltime;
  static INT4 firstime=1;

  if (firstime) {
    ltime = time(NULL);
    stime = (UINT4) ltime/2;
    srand(stime);
    firstime = 0;
  }
  i = rand();
  rnd = (FLOAT4)i/(FLOAT4)RAND_MAX;

  return(rnd);
}
