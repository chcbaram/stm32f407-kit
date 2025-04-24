#ifndef USBH_H_
#define USBH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_USBH



bool usbhInit(void);
bool usbhIsConnected(void);
bool usbhProcess(void);

#endif

#ifdef __cplusplus
}
#endif

#endif