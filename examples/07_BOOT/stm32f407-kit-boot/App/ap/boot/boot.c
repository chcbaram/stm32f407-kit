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

void cliBoot(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "jump"))
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

    cliPrintf("Jump Failed\n");
    ret = true;
  }


  if (!ret)
  {
    cliPrintf("boot jump\n");
  }
}
