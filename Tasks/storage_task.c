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

#include <string.h>

static FATFS sdFatFs;
static FIL sdFile;
FRESULT gfr;

extern osMessageQueueId_t logQueueHandler;

void fRead(char *configFileName, uint8_t *buf, uint32_t num, uint32_t *br);
FRESULT open_append ( FIL* fp, const char* path);

void StorageTask(void *argument) {
	osStatus_t osRes;
	Log_t curLog = {0};
	uint16_t prNum;

  fatfs_init();
  gfr = f_mount(&sdFatFs, "", 1);

  char pBuf[] = "Successful file creation\n";
  char pBuf1[sizeof(Log_t)] = {0};
  char *buf;
  uint32_t bw;
  FIL writeFile;

  do{
	  gfr = open_append(&writeFile, "temp.txt");
  }while (gfr);

  gfr = f_write(&writeFile, pBuf, strlen(pBuf), &bw);
  gfr = f_sync(&writeFile);
  gfr = f_close(&writeFile);

  for(;;)
  {
	  osRes = osMessageQueueGet(logQueueHandler, &curLog, NULL, osWaitForever);

	  do{
		  gfr = open_append(&writeFile, "temp.txt");
	  }while (gfr);

	  prNum = snprintf(pBuf1, sizeof(Log_t), "%d. ", curLog.index);
	  buf = pBuf1+prNum;
	  prNum = snprintf(buf, sizeof(Log_t), "%d:%d:%d %d.%d.%d ", curLog.dataTime.sec, curLog.dataTime.min, curLog.dataTime.hour, curLog.dataTime.day, curLog.dataTime.mon, curLog.dataTime.year );
	  buf = buf+prNum;
	  prNum = snprintf(buf, sizeof(Log_t), "%x,%x,%x,%x,%x,%x,%x,%x,%x,%x ", curLog.result[0], curLog.result[1], curLog.result[2], curLog.result[3], curLog.result[4], curLog.result[5], curLog.result[6], curLog.result[7], curLog.result[8], curLog.result[9]);
	  buf = buf+prNum;
	  prNum = snprintf(buf, sizeof(Log_t), "%f ", curLog.temp[0]);
	  buf = buf+prNum;
	  prNum = snprintf(buf, sizeof(Log_t), "%f ", curLog.temp[1]);
	  buf = buf+prNum;
	  prNum = snprintf(buf, sizeof(Log_t), "%d.%d ", curLog.supplyCurrents[0].intVal, curLog.supplyCurrents[0].fracVal);
	  buf = buf+prNum;
	  prNum = snprintf(buf, sizeof(Log_t), "%d.%d \n", curLog.supplyVoltages[0].intVal, curLog.supplyVoltages[0].fracVal);

	  gfr = f_write(&writeFile, pBuf1, strlen(pBuf1), &bw);
	  gfr = f_sync(&writeFile);
	  gfr = f_close(&writeFile);

	  osThreadYield();
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
