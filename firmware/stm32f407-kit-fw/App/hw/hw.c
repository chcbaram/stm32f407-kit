#include "hw.h"




bool hwInit(void)
{
  
  ledInit();
  uartInit();
  for (int i=0; i<UART_MAX_CH; i++)
  {
    uartOpen(i, 115200);
  }
  return true;
}

void delay(uint32_t ms)
{
  HAL_Delay(ms);
}

uint32_t millis(void)
{
  return HAL_GetTick();
}
