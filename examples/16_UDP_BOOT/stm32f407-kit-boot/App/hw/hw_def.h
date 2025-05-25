#ifndef HW_DEF_H
#define HW_DEF_H


#include "main.h"
#include "def.h"



#define _DEF_FIRMWATRE_VERSION    "V250525R1"
#define _DEF_BOARD_NAME           "STM32F407-KIT-BOOT"


#define _USE_HW_FLASH
#define _USE_HW_RESET
#define _USE_HW_ETNET
#define _USE_HW_ETNET_UDP


#define _USE_HW_LED
#define      HW_LED_MAX_CH          3
#define      HW_LED_CH_UPDATE       _DEF_LED2
#define      HW_LED_CH_DOWN         _DEF_LED3


#define _USE_HW_UART
#define      HW_UART_MAX_CH         1

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    32
#define      HW_CLI_CMD_NAME_MAX    16
#define      HW_CLI_LINE_HIS_MAX    8
#define      HW_CLI_LINE_BUF_MAX    64

#define _USE_HW_I2C
#define      HW_I2C_MAX_CH          2
#define      HW_I2C_CH_OLED         _DEF_I2C1

#define _USE_HW_LCD
#define _USE_HW_SSD1306
#define      HW_LCD_WIDTH           128
#define      HW_LCD_HEIGHT          32

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       4
#define      HW_BUTTON_CH_BOOT      _DEF_BUTTON1

#define _USE_HW_RTC
#define      HW_RTC_BOOT_MODE       RTC_BKP_DR3     

#define _USE_HW_CMD
#define      HW_CMD_MAX_DATA_LENGTH 2048


#define FLASH_SIZE_TAG              0x400
#define FLASH_SIZE_VEC              0x400
#define FLASH_SIZE_VER              0x400
#define FLASH_SIZE_FIRM             (384*1024)

#define FLASH_ADDR_BOOT             0x08000000
#define FLASH_ADDR_FIRM             0x08040000
#define FLASH_ADDR_UPDATE           0x080A0000


#define logPrintf printf

void delay(uint32_t ms);
uint32_t millis(void);

#endif