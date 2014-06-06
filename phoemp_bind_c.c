#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binding.h"
#include "isistypes.h"

FLOAT4 FTN_NAME(rando) ()
{
  FLOAT4 rnd;
  extern FLOAT4 CC_NAME(rando) (void);

  rnd = CC_NAME(rando) ();

  return (rnd);
}
