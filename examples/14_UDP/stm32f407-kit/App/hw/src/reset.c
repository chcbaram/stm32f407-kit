#include "reset.h"


#ifdef _USE_HW_RESET
#include "rtc.h"
#include "cli.h"

#ifdef _USE_HW_CLI
static void cliReset(cli_args_t *args);
#endif

static const char *mode_bit_str[] = 
  {
    "MODE_BIT_BOOT",
    "MODE_BIT_UPDATE",
  };

static bool is_init = false;
static uint32_t boot_mode = 0;  







bool resetInit(void)
{
  bool ret;


  rtcGetReg(HW_RTC_BOOT_MODE, &boot_mode);
  rtcSetReg(HW_RTC_BOOT_MODE, 0);  

  
  cliPrintf("[OK] resetInit()\n");

  for (int i=0; i<MODE_BIT_MAX; i++)
  {
    if (boot_mode & (1<<i))
    {
      cliPrintf("     %s\n", mode_bit_str[i]);
    }
  }  

  is_init = true;
  cliAdd("reset", cliReset);

  ret = is_init;
  return ret;  
}

void resetToBoot(void)
{
  resetSetBootMode(1<<MODE_BIT_BOOT);
  resetToReset();
}

void resetToReset(void)
{
  HAL_NVIC_SystemReset();
}

void resetSetBootMode(uint32_t mode)
{
  boot_mode = mode;
  rtcSetReg(HW_RTC_BOOT_MODE, mode);
}

uint32_t resetGetBootMode(void)
{
  return boot_mode;
}

#ifdef _USE_HW_CLI
void cliReset(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "boot"))
  {
    resetSetBootMode(1<<MODE_BIT_BOOT);
    resetToReset();
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "update"))
  {
    resetSetBootMode(1<<MODE_BIT_UPDATE);
    resetToReset();
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "reset"))
  {
    resetToReset();
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("reset info\n");
    cliPrintf("reset boot\n");
    cliPrintf("reset update\n");
    cliPrintf("reset reset\n");
  }
}
#endif
#endif