#include "rf_remote.h"



#ifdef _USE_HW_RF_REMOTE
#include "cli.h"
#include "qbuffer.h"

#define TIME_RX_BUF_LEN         32
#define TIME_RX_Q_LEN           1024
#define MSG_RX_Q_LEN            16

#define ONE_BIT_TIME            250
#define ONE_BIT_RANGE           10 // Percent
#define BIT_MIN(bit_num)        (ONE_BIT_TIME * (bit_num) - (ONE_BIT_TIME * (bit_num)) * ONE_BIT_RANGE / 100)
#define BIT_MAX(bit_num)        (ONE_BIT_TIME * (bit_num) + (ONE_BIT_TIME * (bit_num)) * ONE_BIT_RANGE / 100)



typedef enum
{
  BIT_PREAMBLE,
  BIT_HIGH,
  BIT_LOW,
  BIT_MAX
} BitData_t;


typedef struct
{
  BitData_t bit_data;
  uint16_t h_min;
  uint16_t h_max;
  uint16_t l_min;
  uint16_t l_max;
} bit_ref_t;



#ifdef _USE_HW_CLI
static void cliCmd(cli_args_t *args);
#endif



static void timCallbackDone(TIM_HandleTypeDef *htim); 
static bool rfRemoteGetBitData(uint32_t bit_time, BitData_t *p_bit_state);


extern TIM_HandleTypeDef htim1;


static bool is_init = true;
static bool is_log = false;
static uint16_t  time_buf[TIME_RX_BUF_LEN];
static qbuffer_t rx_time_q;
static uint16_t  rx_time_buf[TIME_RX_Q_LEN];
static qbuffer_t  rx_msg_q;
static rfremote_info_t rx_msg_buf[MSG_RX_Q_LEN];


static const bit_ref_t bit_ref[BIT_MAX] =
{
  {BIT_PREAMBLE, BIT_MIN(1), BIT_MAX(1), BIT_MIN(31), BIT_MAX(31)},
  {BIT_HIGH,     BIT_MIN(3), BIT_MAX(3), BIT_MIN(1),  BIT_MAX(1) },
  {BIT_LOW,      BIT_MIN(1), BIT_MAX(1), BIT_MIN(3),  BIT_MAX(3) },
};



bool rfRemoteInit(void)
{
  bool ret = true;


  qbufferCreateBySize(&rx_time_q, (uint8_t *)rx_time_buf, sizeof(uint16_t), TIME_RX_Q_LEN);
  qbufferCreateBySize(&rx_msg_q, (uint8_t *)rx_msg_buf, sizeof(rfremote_info_t), MSG_RX_Q_LEN);

  HAL_TIM_RegisterCallback(&htim1, HAL_TIM_IC_CAPTURE_CB_ID, timCallbackDone);
  HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)time_buf, TIME_RX_BUF_LEN);
  
  logPrintf("[%s] rfRemoteInit()\n", ret ? "OK":"E_");

#ifdef _USE_HW_CLI
  cliAdd("rfremote", cliCmd);
#endif

  return ret;
}

uint16_t rfRemoteAvailable(void)
{
  return qbufferAvailable(&rx_msg_q);
}

bool rfRemoteRead(rfremote_info_t *p_info)
{
  bool ret;

  ret = qbufferRead(&rx_msg_q, (uint8_t *)p_info, 1);

  return ret;
}

bool rfRemoteReceive(uint16_t time_data)
{  
  enum
  {
    STATE_SYNC,
    STATE_DATA_H,
    STATE_DATA_L,
  };

  bool ret = false;
  bool bit_ret = false;

  static uint8_t state = STATE_SYNC;
  static uint32_t pre_time;
  static uint32_t bit_time = 0;
  static uint8_t  bit_index = 0;
  static uint32_t data_buf = 0;
  BitData_t bit_data;

  bit_time <<= 16;
  bit_time |= time_data;

  bit_ret = rfRemoteGetBitData(bit_time, &bit_data);
  
  if (millis()-pre_time >= 100)
  {
    state = STATE_SYNC;
  }

  switch(state)
  {
    case STATE_SYNC:
      if (bit_data == BIT_PREAMBLE)
      {
        data_buf = 0;
        bit_index = 0;
        state = STATE_DATA_H;
        // logPrintf("preamble\n");
      }
      pre_time = millis();
      break;

    case STATE_DATA_H:
      state = STATE_DATA_L;
      break;

    case STATE_DATA_L:
      if (bit_data == BIT_HIGH)
        data_buf |= (1<<bit_index);

      state = STATE_DATA_H;

      if (!bit_ret)
      {
        state = STATE_SYNC;
        break;
      }

      bit_index++;
      if (bit_index == 24)
      {
        rfremote_info_t info;

        info.id = data_buf & 0xFFFFF;
        info.data = data_buf>>20;
        qbufferWrite(&rx_msg_q, (uint8_t *)&info, 1);

        state = STATE_SYNC;
        ret = true;
      }
      break;
  }
  return ret;
}

bool rfRemoteGetBitData(uint32_t bit_time, BitData_t *p_bit_data)
{
  bool ret = false;

  uint16_t time_h;
  uint16_t time_l;

  time_h = bit_time >> 16;
  time_l = bit_time & 0xFFFF;


  for (int i=0; i<BIT_MAX; i++)
  {
    if (time_h >= bit_ref[i].h_min && time_h <= bit_ref[i].h_max)
    {
      if (time_l >= bit_ref[i].l_min && time_l <= bit_ref[i].l_max)
      {
        *p_bit_data = i;
        ret = true;
        break;
      }
    }
  }

  return ret;
}

void timCallbackDone(TIM_HandleTypeDef *htim)
{
  for (int i=0; i<TIME_RX_BUF_LEN; i++)
  {
    uint16_t dif_time;

    dif_time = time_buf[i];

    rfRemoteReceive(dif_time);

    if (is_log)
    {
      qbufferWrite(&rx_time_q, (uint8_t *)&dif_time, 1);
    }
  }
  return;
}


#ifdef _USE_HW_CLI
void cliCmd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("is init : %s\n", is_init ? "True":"False");
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "log"))
  {
    uint16_t index = 0;


    qbufferFlush(&rx_time_q);

    is_log = true;
    while(cliKeepLoop() && index < 1024)
    {
      uint16_t time_bit;

      if (qbufferAvailable(&rx_time_q))
      {
        qbufferRead(&rx_time_q, (uint8_t *)&time_bit, 1);
        cliPrintf("%d %d us\n", index, time_bit);

        index++;
      }      
    }
    is_log = false;

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "msg"))
  {
    uint16_t index = 0;


    while(cliKeepLoop() && index < 1024)
    {
      rfremote_info_t info;

      if (qbufferAvailable(&rx_msg_q))
      {
        qbufferRead(&rx_msg_q, (uint8_t *)&info, 1);
        cliPrintf("%d -> id:0x%06X data:%d\n", index, info.id, info.data);
        index++;
      }      
    }
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("rfremote info\n");
    cliPrintf("rfremote log\n");
    cliPrintf("rfremote msg\n");    
  }
}
#endif



#endif