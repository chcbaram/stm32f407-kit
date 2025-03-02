#include "ap.h"



void apInit(void)
{
}


void apMain(void)
{
  uint32_t pre_time;
  uint8_t led_cnt = 0;


  pre_time = millis();
  while(1)
  {  
    if (millis() - pre_time >= 500)
    {
      pre_time = millis();

      ledToggle(_DEF_LED1);

      for (int i=1; i<LED_MAX_CH; i++)
      {
        if ((led_cnt + 1) == i)
          ledOn(i);
        else
          ledOff(i);
      }

      led_cnt = (led_cnt + 1) % 3;
    }

    if (uartAvailable(_DEF_UART1))
    {
      uartPrintf(_DEF_UART1, "rx : 0x%X\n", uartRead(_DEF_UART1));
    }
  }
} 
