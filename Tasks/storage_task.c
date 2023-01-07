/*
 * storage_task.c
 *
 *  Created on: 7 янв. 2023 г.
 *      Author: George
 */

#include "storage_task.h"

#include "main.h"
#include "terminal.h"

#include "cmsis_os2.h"
#include "fatfs.h"

static FATFS sdFatFs;
static FIL sdFile;
FRESULT gfr;

void fRead(char *configFileName, uint8_t *buf, uint32_t num, uint32_t *br);
FRESULT open_append ( FIL* fp, const char* path);

void StorageTask(void *argument) {

  fatfs_init();
  gfr = f_mount(&sdFatFs, "", 1);

  char pBuf[] = "Successful file creation";
  uint32_t bw;
  FIL writeFile;

  do{
	  gfr = open_append(&writeFile, "temp.txt");
  }while (gfr);

  gfr = f_write(&writeFile, pBuf, sizeof(pBuf), &bw);
  gfr = f_sync(&writeFile);
  gfr = f_close(&writeFile);

  for(;;)
  {

    osDelay(1000);
  }
}



uint8_t gfileBuf[80] = {0};
uint32_t gbr;
void fRead(char *configFileName, uint8_t *buf, uint32_t num, uint32_t *br){
  FIL readFile;

  do{
      //fr = open_append(&sdFile, "camconf.txt");
      gfr = f_open(&readFile, configFileName, FA_READ);
  }while (gfr);

  gfr = f_read(&readFile, buf, num, br);

  // Close the file
  f_close(&readFile);
}

// Только для записи в конец файла
FRESULT open_append ( FIL* fp, const char* path) {
    FRESULT fr;

    // Opens an existing file. If not exist, creates a new file.
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        // Seek to end of the file to append data
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}
