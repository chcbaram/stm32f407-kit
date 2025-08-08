#include "remote.h"
#include "event/event.h"


static void cliCmd(cli_args_t *args);


static uint32_t rf_remote_id = 0x087DBB;





bool remoteInit(void)
{
  cliAdd("remote", cliCmd);
  return true;
}


void remoteUpdate(void)
{
  while(rfRemoteAvailable() > 0)
  {
    rfremote_info_t remote_info;

    if (rfRemoteRead(&remote_info))
    {      
      if (remote_info.id == rf_remote_id)
      {
        event_msg_t msg;

        msg.id = EVENT_ID_REMOTE;
        msg.data = remote_info.data;
        eventPut(&msg);
      }
    }
  }
}

void remoteThread(void *argument)
{
  remoteInit();

  logPrintf("[  ] remoteThread()\n");

  while(1)
  {
    remoteUpdate();
    delay(10);
  }
}

void cliCmd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {  
    ret = true;
  }

  if (!ret)
  {
    cliPrintf("remote info\n");
  }
}