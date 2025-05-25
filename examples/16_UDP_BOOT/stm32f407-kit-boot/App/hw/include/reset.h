#ifndef RESET_H_
#define RESET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_RESET


enum
{
  MODE_BIT_BOOT = 0,
  MODE_BIT_UPDATE,
  MODE_BIT_MAX
};


bool resetInit(void);
void resetToBoot(void);
void resetToReset(void);
void resetSetBootMode(uint32_t mode);
uint32_t resetGetBootMode(void);


#endif

#ifdef __cplusplus
}
#endif

#endif