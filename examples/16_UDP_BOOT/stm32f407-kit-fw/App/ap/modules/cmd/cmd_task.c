#include "cmd_task.h"
#include "driver/cmd_udp.h"
#include "process/cmd_boot.h"



enum
{
  CMD_CH_UDP,
  CMD_CH_MAX,
};


#define CMD_DRIVER_MAX_CH     CMD_CH_MAX


static cmd_t        cmd[CMD_DRIVER_MAX_CH];
static cmd_driver_t cmd_drvier[CMD_DRIVER_MAX_CH];




bool cmdTaskInit(void)
{
  cmdUdpInitDriver(&cmd_drvier[CMD_CH_UDP], NULL, 5100);  
  cmdInit(&cmd[CMD_CH_UDP], &cmd_drvier[CMD_CH_UDP]);
  cmdOpen(&cmd[CMD_CH_UDP]);
  
  return true;
}

bool cmdTaskUpdate(void)
{
  bool rx_ret = false;

  for (int i=0; i<CMD_DRIVER_MAX_CH; i++)
  {
    if (cmd[i].is_init == true)
    {
      if (cmdReceivePacket(&cmd[i]) == true)
      {
        bool ret = true;

        ret &= cmdBootProcess(&cmd[i]);

        #if 0
        logPrintf("[  ] rx cmd\n");
        logPrintf("[  ]   cmd : 0x%02X\n", cmd[i].packet.cmd);
        logPrintf("[  ]   len : %d\n", cmd[i].packet.length);
        #endif
        
        if (ret != true)
        {
          cmdSendResp(&cmd[i], cmd[i].packet.cmd, ERR_CMD_NO_CMD, NULL, 0);
        }

        rx_ret = true;
      }
    }
  }

  return rx_ret;
}
