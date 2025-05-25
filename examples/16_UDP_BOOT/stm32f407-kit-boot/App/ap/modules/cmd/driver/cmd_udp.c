#include "cmd_udp.h"





typedef struct
{
  char     ip_addr[32];
  uint32_t port;
} cmd_udp_args_t;


static bool open_(void *args);
static bool close_(void *args);  
static uint32_t available(void *args);
static bool flush(void *args);
static uint8_t read(void *args);
static uint32_t write(void *args, uint8_t *p_data, uint32_t length);  

static bool is_init = false;
static bool is_open = false;







bool cmdUdpInitDriver(cmd_driver_t *p_driver, const char *ip_addr, uint32_t port)
{
  cmd_udp_args_t *p_args = (cmd_udp_args_t *)p_driver->args;


  p_args->port  = port;
  strncpy(p_args->ip_addr, ip_addr, 32);

  p_driver->open      = open_;
  p_driver->close     = close_;
  p_driver->available = available;
  p_driver->flush     = flush;
  p_driver->read      = read;
  p_driver->write     = write;


  is_init = true;

  return true;
}

bool open_(void *args)
{
  bool   ret = false;
  cmd_udp_args_t *p_args = (cmd_udp_args_t *)args;

  if (!is_init)
    return false;


  ret = etnetUdpOpen(IP_ADDR_ANY, p_args->port);

  logPrintf("[%s] cmdUdpOpen()\n", ret ? "OK":"E_");

  is_open = ret;
  return ret;
}

bool close_(void *args)
{
  if (is_open == false) return true;

  is_open = false;

  etnetUdpClose();

  return true;  
}

uint32_t available(void *args)
{
  uint32_t ret;

  ret = etnetUdpAvailable();
  return ret;
}

bool flush(void *args)
{
  etnetUdpFlush();
  return true;
}

uint8_t read(void *args)
{
  uint8_t ret;

  etnetUdpRead(&ret, 1);

  return ret;
}

uint32_t write(void *args, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  uint32_t pre_time;
  int32_t  udp_ret;

  if (is_init == false) 
    return 0;

  if (is_open == true)
  {
    uint32_t tx_index;

   
    tx_index = 0;
    pre_time = millis();
    while(millis()-pre_time < 500)
    {      
      udp_ret = etnetUdpWrite(&p_data[tx_index], length-tx_index);
      if (tx_index == length)
      {
        ret = tx_index;
        break;
      }
      if (udp_ret < 0)
      {
        break;
      }
      tx_index += udp_ret;
    }
  }

  return ret;
}
