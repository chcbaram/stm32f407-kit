#include "fat_fs.h"

#ifdef _USE_HW_FATFS
#include "fatfs.h"
#include "flash.h"
#include "cli.h"


static void cliCmd(cli_args_t *args);

static bool is_init = false;



bool fatfsInit(void)
{
  if(f_mount(&USBHFatFS, (const TCHAR*) USBHPath, 0) == FR_OK)
  {
    is_init = true;
  } 

  cliAdd("fatfs", cliCmd);

  return is_init;
}

static FRESULT list_dir(const char *path)
{
  FRESULT res;
  DIR     dir;
  FILINFO fno;
  int     nfile, ndir;

  res = f_opendir(&dir, (TCHAR *)path); /* Open the directory */
  if (res == FR_OK)
  {
    nfile = ndir = 0;
    for (;;)
    {
      res = f_readdir(&dir, &fno);                  /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break; /* Error or end of dir */
      if (fno.fattrib & AM_DIR)
      {                                             /* Directory */
        printf("   <DIR>   %s\n", (char *)fno.fname);
        ndir++;
      }
      else
      {                                             /* File */
        printf("%10lu %s\n", fno.fsize, (char *)fno.fname);
        nfile++;
      }
    }
    f_closedir(&dir);
    printf("%d dirs, %d files.\n", ndir, nfile);
  }
  else
  {
    printf("Failed to open %s (%u)\n", path, res);
  }
  return res;
}

void cliCmd(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("is_init : %s\n", is_init ? "True":"False");
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "dir"))
  {
    list_dir("");
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "flash"))
  {
    FIL      fil;
    FRESULT  fr;
    UINT     read_bytes;
    char    *file_name;
    BYTE     buffer[512];
    uint32_t flash_addr = 0x08040000;

    file_name = args->getStr(1);


    fr = f_open(&fil, file_name, FA_READ);
    if (fr == FR_OK)
    {
      bool flash_ret;
      uint32_t file_size;


      file_size = f_size(&fil);
      cliPrintf("file size : %d bytes\n", file_size);

      flash_ret = flashErase(flash_addr, file_size);
      cliPrintf("flashErase() %s\n", flash_ret ? "OK":"FAIL");


      uint32_t read_index = 0;
      while(read_index < file_size)
      {
        uint32_t read_size;

        read_size = constrain(file_size - read_index, 0, sizeof(buffer));
        read_bytes = 0;
        fr = f_read(&fil, buffer, read_size, &read_bytes);
        if (fr != FR_OK)
        {
          cliPrintf("f_read() FAIL\n");
          break;
        }
        
        flash_ret = flashWrite(flash_addr + read_index, buffer, read_bytes);
        if (!flash_ret)
        {
          cliPrintf("flashWrite() FAIL\n");
          break;
        }

        read_index += read_bytes;
      }

      if (read_index == file_size)
      {
        cliPrintf("flashWrite() OK\n");  
      }

      f_close(&fil);
    }

    ret = true;
  }

  if (!ret)
  {
    cliPrintf("fatfs info\n");
    cliPrintf("fatfs dir\n");
    cliPrintf("fatfs flash filename\n");
  }
}



#endif