#include "hw.h"




bool hwInit(void)
{
  
  ledInit();

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
