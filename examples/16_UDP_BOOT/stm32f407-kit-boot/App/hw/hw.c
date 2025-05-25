#include "hw.h"



extern uint32_t _fw_flash_begin;

volatile const firm_ver_t firm_ver __attribute__((section(".version"))) = 
{
  .magic_number = VERSION_MAGIC_NUMBER,
  .version_str  = _DEF_FIRMWATRE_VERSION,
  .name_str     = _DEF_BOARD_NAME,
  .firm_addr    = (uint32_t)&_fw_flash_begin
};



bool hwInit(void)
{
  cliInit();
  ledInit();
  uartInit();
  for (int i=0; i<UART_MAX_CH; i++)
  {
    uartOpen(i, 115200);
  }

  logPrintf("\r\n[ Bootloader Begin... ]\r\n");
  logPrintf("Booting..Clock\t: %d Mhz\r\n", (int)HAL_RCC_GetSysClockFreq()/1000000);
  logPrintf("\n");
  
  rtcInit();
  resetInit();
  i2cInit();
  
  lcdInit();
  lcdSetFps(20);

  flashInit();
  buttonInit();

  etnetInit();
  
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