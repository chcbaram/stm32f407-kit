#include "ap.h"
#include "cli/cli_thread.h"




void apInit(void)
{
  for (int i = 0; i < 32; i += 1)
  {
    lcdClearBuffer(black);
    lcdPrintfResize(0, 40 - i, green, 16, "  -- BARAM --");
    lcdDrawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, white);
    lcdUpdateDraw();
    delay(10);
  }
  delay(500);
  lcdClear(black);    

  cliThreadInit();
}

void apMain(void)
{
  uint32_t pre_time;  

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
    delay(1);
  }
}