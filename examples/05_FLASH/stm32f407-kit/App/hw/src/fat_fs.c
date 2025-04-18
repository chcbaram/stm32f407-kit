#include "fat_fs.h"

#ifdef _USE_HW_FATFS
#include "fatfs.h"
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

  if (!ret)
  {
    cliPrintf("fatfs info\n");
    cliPrintf("fatfs dir\n");
  }
}



#endif