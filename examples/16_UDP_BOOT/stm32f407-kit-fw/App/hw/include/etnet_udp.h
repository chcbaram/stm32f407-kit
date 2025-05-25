#ifndef ETNET_UDP_H_
#define ETNET_UDP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_ETNET_UDP
#include "lwip/ip.h"
#include "lwip/ip_addr.h"


bool etnetUdpInit(void);
bool etnetUdpOpen(const ip_addr_t *ip_addr, uint16_t port);
bool etnetUdpIsOpen(void);
bool etnetUdpClose(void);

uint32_t etnetUdpAvailable(void);
bool     etnetUdpFlush(void);
bool     etnetUdpRead(uint8_t *p_data, uint32_t length);
int32_t  etnetUdpWrite(uint8_t *p_data, uint32_t length);

#endif

#ifdef __cplusplus
}
#endif

#endif