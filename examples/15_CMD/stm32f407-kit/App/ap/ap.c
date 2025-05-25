#include "ap.h"
#include "modules.h"


void updateLCD(void);




void apInit(void)
{
  cliOpen(_DEF_UART1, 115200);
  cliLogo();

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
}

void apMain(void)
{
  uint32_t pre_time;


  cmdTaskInit();

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }

    cliMain();
    etnetUpdate();
    updateLCD();
    cmdTaskUpdate();
  }
}

void updateLCD(void)
{
  int16_t         x_offset = 10;
  static uint8_t  menu     = 0;
  uint8_t         menu_max = 2;
  uint8_t         menu_cur = 0;
  cmd_boot_info_t cmd_boot_info;
  

  if (!lcdIsInit())
  {
    return;
  }

  if (buttonGetPressed(_DEF_BUTTON1))
  {
    delay(10);
    while(buttonGetPressed(_DEF_BUTTON1));
    
    menu = (menu + 1) % menu_max;
  }

  if (cmdBootIsBusy())
  {
    menu_cur = menu_max;
  }
  else
  {
    menu_cur = menu;
  }


  if (lcdDrawAvailable())
  {
    lcdClearBuffer(black);

    lcdDrawRect(0, 0, 4, 32, white);
    for (int i=0; i<menu_max; i++)
    {
      if (i == menu)
        lcdDrawFillRect(0, i*(32/menu_max), 4, (32/menu_max), white);
    }


    if (menu_cur == 0)
    {
      if (etnetIsLink() == true)
      {
        if (etnetIsGetIP() == true)
        {
          etnet_info_t net_info;

          etnetGetInfo(&net_info);

          lcdPrintf(x_offset,  0, white,
                    "IP %d.%d.%d.%d", 
                    net_info.ip[0], 
                    net_info.ip[1],
                    net_info.ip[2],
                    net_info.ip[3]);
          lcdPrintf(x_offset, 16, white,
                    "DHCP : %s\n", etnetIsGetIP() ? "True":"False");          
        }
        else
        {
          lcdPrintf(x_offset, 8, white, "Getting_IP..");        
        }      
      }
      else
      {
        lcdPrintf(x_offset, 8, white, "Not Connected");        
      }
    }

    if (menu_cur == 1)
    {
      rtc_time_t rtc_time;
      rtc_date_t rtc_date;
      const char *week_str[] = {"일", "월", "화", "수", "목", "금", "토"};

      rtcGetTime(&rtc_time);
      rtcGetDate(&rtc_date);

      lcdPrintf(x_offset, 0, white,
                "%02d-%02d-%02d (%s)",
                rtc_date.year, rtc_date.month, rtc_date.day, week_str[rtc_date.week]);

      lcdPrintf(x_offset, 16, white,
                "%02d:%02d:%02d",
                rtc_time.hours, rtc_time.minutes, rtc_time.seconds);
    }

    if (cmdBootIsBusy())
    {
      uint16_t percent;

      cmdBootGetInfo(&cmd_boot_info);

      percent = cmd_boot_info.fw_receive_size * 100 / cmd_boot_info.fw_size;
      lcdClearBuffer(black);
      lcdPrintf(96, 0, white, "%3d%%", percent);
      lcdDrawRect(0, 16, 128, 16, white);
      lcdDrawFillRect(2, 19, percent * 124 / 100, 10, white);      
    }

    lcdRequestDraw();
  }
}