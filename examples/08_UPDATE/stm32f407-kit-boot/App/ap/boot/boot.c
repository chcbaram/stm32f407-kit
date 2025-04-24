#include "boot.h"



#define FLASH_ADDR_FIRM       0x08040000
#define FLASH_SIZE_FIRM       (768*1024)




static void cliBoot(cli_args_t *args);


bool bootInit(void)
{

  logPrintf("[OK] bootInit()\n");
  
  cliAdd("boot", cliBoot);
  return true;
}

bool bootUpdateFromUSB(const char *file_name)
{
  bool     ret = false;
  FIL      fil;
  FRESULT  fr;
  UINT     read_bytes;
  BYTE     buffer[512];
  uint32_t flash_addr = FLASH_ADDR_FIRM;
  uint8_t  percent = 0;


  fr = f_open(&fil, file_name, FA_READ);
  if (fr == FR_OK)
  {
    bool flash_ret;
    uint32_t file_size;

    file_size = f_size(&fil);
    cliPrintf("[  ] file size : %d bytes\n", file_size);

    flash_ret = flashErase(flash_addr, file_size);
    cliPrintf("[%s] flashErase()\n", flash_ret ? "OK":"E_");


    uint32_t read_index = 0;
    while(read_index < file_size)
    {
      uint32_t read_size;

      read_size = constrain(file_size - read_index, 0, sizeof(buffer));
      read_bytes = 0;
      fr = f_read(&fil, buffer, read_size, &read_bytes);
      if (fr != FR_OK)
      {
        cliPrintf("[E_] f_read()\n");
        break;
      }
      
      flash_ret = flashWrite(flash_addr + read_index, buffer, read_bytes);
      if (!flash_ret)
      {
        cliPrintf("[E_] flashWrite()\n");
        break;
      }

      read_index += read_bytes;

      percent = read_index * 100 / file_size;

      cliPrintf("[  ] flashWrite() - %d %%\r", percent);
    }

    if (read_index == file_size)
    {
      cliPrintf("[OK] flashWrite()\n");  
      ret = true;
    }

    f_close(&fil);
  }

  return ret;
}

bool bootJump(void)
{
  void (**jump_func)(void) = (void (**)(void))(FLASH_ADDR_FIRM + 4);

    
  // 점프 주소 유효성 검사
  //
  if (((uint32_t)(*jump_func)) >= FLASH_ADDR_FIRM && ((uint32_t)(*jump_func)) < (FLASH_ADDR_FIRM + FLASH_SIZE_FIRM))
  {
    // 점프하기전 인터럽트 Disable
    //
    HAL_RCC_DeInit();

    // Disable Interrupts
    //
    for (int i=0; i<8; i++)
    {
      NVIC->ICER[i] = 0xFFFFFFFF;
      __DSB();
      __ISB();
    }
    SysTick->CTRL = 0;

    (*jump_func)();
  }
  return false;
}

void cliBoot(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "jump"))
  {
    bootJump();    
    cliPrintf("Jump Failed\n");
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "update"))
  {
    if (bootUpdateFromUSB("stm32f407-kit-fw.bin"))
    {
      cliPrintf("Update OK\n");
    }
    else
    {
      cliPrintf("Update OK\n");
    }
    
    ret = true;
  }

  if (!ret)
  {
    cliPrintf("boot jump\n");
    cliPrintf("boot update\n");
  }
}
