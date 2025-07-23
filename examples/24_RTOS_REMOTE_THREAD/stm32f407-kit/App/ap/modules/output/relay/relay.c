#include "relay.h"
#include "event/event.h"


#define RELAY_MAX_CH      4

typedef struct
{
  uint8_t ch;
  uint8_t on_state;
  uint8_t off_state;
} relay_gpio_t;

typedef struct
{
  uint8_t enable;
  uint8_t enable_req;
  relay_gpio_t *p_gpio;
} relay_t;


static bool relaySetEnable(uint8_t ch, bool enable);
static bool relayReqEnable(uint8_t ch, bool enable);
static void receiveEvent(event_msg_t *p_msg);
static void cliCmd(cli_args_t *args);


static relay_gpio_t gpio_tbl[RELAY_MAX_CH] = 
{
  {GPIO_RELAY_CH1, _DEF_HIGH, _DEF_LOW},
  {GPIO_RELAY_CH2, _DEF_HIGH, _DEF_LOW},
  {GPIO_RELAY_CH3, _DEF_HIGH, _DEF_LOW},
  {GPIO_RELAY_CH4, _DEF_HIGH, _DEF_LOW},
};

static relay_t relay_tbl[RELAY_MAX_CH];




bool relayInit(void)
{
  for (int i=0; i<RELAY_MAX_CH; i++)
  {
    relay_tbl[i].p_gpio = &gpio_tbl[i];
    relay_tbl[i].enable = false;
    relay_tbl[i].enable_req = false;
  }


  eventAddCallback(EVENT_ID_ALL, receiveEvent);

  cliAdd("relay", cliCmd);
  return true;
}

static void receiveEvent(event_msg_t *p_msg)
{
  switch(p_msg->id)
  {
    case EVENT_ID_REMOTE:
      if (p_msg->data & 0x08)
      {
        relayReqEnable(_DEF_CH1, true);
      }
      if (p_msg->data & 0x04)
      {
        relayReqEnable(_DEF_CH1, false);
      }
      break;
  }
}

bool relaySetEnable(uint8_t ch, bool enable)
{
  uint8_t gpio_enable;

  if (ch >= RELAY_MAX_CH)
    return false;


  gpio_enable = enable ? relay_tbl[ch].p_gpio->on_state : relay_tbl[ch].p_gpio->off_state;
  gpioPinWrite(relay_tbl[ch].p_gpio->ch, gpio_enable);
  return true;
}

bool relayReqEnable(uint8_t ch, bool enable)
{
  if (ch >= RELAY_MAX_CH)
    return false;

  relay_tbl[ch].enable_req = enable;

  return true;
}

void relayUpdate(void)
{
  for (int i=0; i<RELAY_MAX_CH; i++)
  {
    if (relay_tbl[i].enable_req != relay_tbl[i].enable)
    {
      logPrintf("[  ] relay ch%d %s -> %s\n", i + 1, 
        relay_tbl[i].enable ? "ON":"OFF",
        relay_tbl[i].enable_req ? "ON":"OFF");

      relaySetEnable(i, relay_tbl[i].enable_req);
      relay_tbl[i].enable = relay_tbl[i].enable_req;
    }
  }
}

void relayThread(void *argument)
{
  relayInit();

  logPrintf("[  ] relayThread()\n");

  while(1)
  {
    relayUpdate();
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

  if (args->argc == 2 && args->isStr(0, "on"))
  {  
    uint8_t relay_ch;

    relay_ch = args->getData(1);

    relayReqEnable(relay_ch, true);
    ret = true;
  }  

  if (args->argc == 2 && args->isStr(0, "off"))
  {  
    uint8_t relay_ch;

    relay_ch = args->getData(1);

    relayReqEnable(relay_ch, false);
    delay(10);
    ret = true;
  }  

  if (!ret)
  {
    cliPrintf("relay info\n");
    cliPrintf("relay on [0~%d]\n", RELAY_MAX_CH - 1);
    cliPrintf("relay off [0~%d]\n", RELAY_MAX_CH - 1);
  }
}