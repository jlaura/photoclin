/*********************************************************************
*_TITLE clinom_msg.h 
*
*_DESC  Common block with parameters controlling message handling
*
*_HIST  Jun 01 2001  Janet Barrett, USGS Flagstaff, Original Version
**********************************************************************/
#ifndef CLINOM_MSG_H
#define CLINOM_MSG_H 1
#include "isis_limits.h"
#include "isisdef.h"
#include "binding.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

typedef struct clinom_msg_f_common {
  CHAR pcpmsg[512];
} CLINOM_MSG_F_COMMON;

extern CLINOM_MSG_F_COMMON     FTN_COMMON(msg);

static CLINOM_MSG_F_COMMON *C_MSG = &FTN_COMMON(msg);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
