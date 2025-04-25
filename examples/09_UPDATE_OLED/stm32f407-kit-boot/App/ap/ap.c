#include "ap.h"
#include "boot/boot.h"


static bool is_req_update = false;




void apInit(void)
{
  bootInit();

  if (buttonGetPressed(_DEF_BUTTON1))
  {
    is_req_update = true;
  }
  else
  {
    if (!buttonGetPressed(_DEF_BUTTON2))
    {
      logPrintf("[  ] bootJump()\n");
      bootJump();
      logPrintf("[E_] bootJump()\n");
    }
  }

  cliOpen(_DEF_UART1, 115200);
  cliLogo();

  if (!is_req_update)
  {
    for (int i = 0; i < 32; i += 1)
    {
      lcdClearBuffer(black);
      lcdPrintfResize(0, 40 - i, green, 16, "   -- BOOT --");
      lcdDrawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, white);
      lcdUpdateDraw();
      delay(10);
    }
    delay(500);
    lcdClear(black);    
  }
  else
  {
    lcdClearBuffer(black);
    lcdPrintfResize(0, 8, green, 16, "  -- UPDATE --");
    lcdDrawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, white);
    lcdUpdateDraw();    
  }
}

void apMain(void)
{
  uint32_t pre_time;

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 100)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }

    cliMain();
    usbhProcess();

    if (usbhIsConnected() && is_req_update)
    {
      is_req_update = false;

      lcdClearBuffer(black);
      lcdPrintfResize(0, 8, green, 16, "  -- UPDATE --");
      lcdDrawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, white);
      lcdUpdateDraw();

      logPrintf("[  ] bootUpdateFromUSB()\n");
      if (bootUpdateFromUSB("stm32f407-kit-fw.bin"))
      {
        logPrintf("[  ] bootJump()\n");
        bootJump();
      }
      logPrintf("[E_] bootUpdateFromUSB()\n");

      lcdClear(black);   
    }
  }
}