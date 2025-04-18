#ifndef FATFS_H_
#define FATFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_FATFS


bool fatfsInit(void);
bool fatfsIsInit(void);

#endif

#ifdef __cplusplus
}
#endif

#endif