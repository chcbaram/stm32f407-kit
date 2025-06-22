#include "cli_thread.h"




bool cliThreadInit(void)
{
  cliOpen(_DEF_UART1, 115200);
  cliLogo();

  return true;
}

void cliThread(void *argument)
{
  while(1)
  {
    cliMain();
    delay(1);
  }
}