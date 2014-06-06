#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binding.h"
#include "isistypes.h"

void FTN_NAME(pcsizout) ()
{
  extern void CC_NAME(pcsizout) (void);

  (void) CC_NAME(pcsizout) ();

  return;
}
