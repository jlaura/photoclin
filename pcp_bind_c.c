#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binding.h"
#include "isistypes.h"

void FTN_NAME(hinterror) (CHAR msg1[],CHAR msg2[],INT4 *dest,INT4 msg1_len,
                          INT4 msg2_len)
{
  CHAR *msg1_mem;
  CHAR *msg2_mem;
  extern void CC_NAME(hinterror) (CHAR msg1[],CHAR msg2[],INT4 dest);

  msg1_mem = b_f2cstr (1,msg1,msg1_len,B_DISCARD_PAD,NULL,(msg1_len+1));
  msg2_mem = b_f2cstr (1,msg2,msg2_len,B_DISCARD_PAD,NULL,(msg2_len+1));
  (void) CC_NAME(hinterror) (msg1_mem,msg2_mem,(*dest));

  (void) b_free(msg1_mem);
  (void) b_free(msg2_mem);
  return;
}

void FTN_NAME(pcplog) (CHAR msg[],INT4 *dest,INT4 msg_len)
{
  CHAR *msg_mem;
  extern void CC_NAME(pcplog) (CHAR msg[],INT4 dest);

  msg_mem = b_f2cstr(1,msg,msg_len,B_DISCARD_PAD,NULL,(msg_len+1));
  (void) CC_NAME(pcplog) (msg_mem,*dest);

  (void) b_free(msg_mem);
  return;
}

void FTN_NAME(pcpmessage) (CHAR msg[],INT4 msg_len)
{
  CHAR *msg_mem;
  extern void CC_NAME(pcpmessage) (CHAR msg[]);

  msg_mem = b_f2cstr (1,msg,msg_len,B_DISCARD_PAD,NULL,(msg_len+1));
  (void) CC_NAME(pcpmessage) (msg_mem);

  (void) b_free(msg_mem);
  return;
}
