#ifndef BOOT_H_
#define BOOT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ap_def.h"


bool bootInit(void);
bool bootUpdateFromUSB(const char *file_name);
bool bootJump(void);

#ifdef __cplusplus
}
#endif


#endif