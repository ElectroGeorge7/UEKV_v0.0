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

extern osEventFlagsId_t testEvents;
extern osMessageQueueId_t logQueueHandler;
extern osMessageQueueId_t eventQueueHandler;

static uint8_t testFlags = 0;

void fRead(char *configFileName, uint8_t *buf, uint32_t num, uint32_t *br);
FRESULT open_append ( FIL* fp, const char* path);


void StorageTask(void *argument) {
	osStatus_t osRes;
	Log_t curLog = {0};
	uint16_t prNum;
	char buf[sizeof(Log_t)] = {0};
	char *pBuf;
	uint32_t bw;
	FIL writeFile;

    char pBufStart[] = "Successful file creation\n";

  fatfs_init();
  gfr = f_mount(&sdFatFs, "", 1);

  for(;;)
  {

	  // if configuration is not set
	  if (!testFlags){
		  // wait for cmd to find config file
		 if ( osEventFlagsWait(testEvents, TEST_CONFIG_SEARCH, osFlagsWaitAny, osWaitForever) == TEST_CONFIG_SEARCH ){
			 // find config file
			 // extract needed info
			 // encode config to queue type
			 //and send to control task
/*
			if (1){
				Event_t msg;
				osStatus_t res;
				msg.event = TEST_CMD;
				//memcpy(msg.eventStr, Buf, sizeof(msg.eventStr));
				osEventFlagsSet(testEvents, TEST_CONFIG_IS_FIND);
				res = osMessageQueuePut (eventQueueHandler, &msg, 0, 0);
			} else {
				osEventFlagsSet(testEvents, TEST_CONFIG_IS_NOT);
				// get terminal config
				osMessageQueueGet(eventQueueHandler, &msg, NULL, osWaitForever);
			}
*/
			testFlags |= TEST_CONFIG_SEARCH;
		 }

	  } else {

		  if ( !(testFlags & TEST_START) ){

			 if ( osEventFlagsWait(testEvents, TEST_START, osFlagsWaitAny, osWaitForever) == TEST_START ){
				 // open log file
				  do{
					  gfr = open_append(&writeFile, "temp.txt");
				  }while (gfr);
				  // and write config info
				  gfr = f_write(&writeFile, pBufStart, strlen(pBuf), &bw);
				  gfr = f_sync(&writeFile);
				  testFlags |= TEST_START;
			 }

		  } else {

			  //wait
			 if ( osEventFlagsWait(testEvents, TEST_FINISH, osFlagsWaitAny, 0) == TEST_FINISH ){
				 // close log file
				 gfr = f_sync(&writeFile);
				 gfr = f_close(&writeFile);
				 testFlags = 0;
			 }
			 // wait for logs
			 if ( osMessageQueueGet(logQueueHandler, &curLog, NULL, osWaitForever) == osOK ){
				  prNum = snprintf(buf, sizeof(Log_t), "%d. ", curLog.index);
				  pBuf = buf+prNum;
				  prNum = snprintf(pBuf, sizeof(Log_t), "%d:%d:%d %d.%d.%d ", curLog.dataTime.hour, curLog.dataTime.min, curLog.dataTime.sec, curLog.dataTime.day, curLog.dataTime.mon, curLog.dataTime.year );
				  pBuf = pBuf+prNum;
				  prNum = snprintf(pBuf, sizeof(Log_t), "%x,%x,%x,%x,%x,%x,%x,%x,%x,%x ", curLog.result[0], curLog.result[1], curLog.result[2], curLog.result[3], curLog.result[4], curLog.result[5], curLog.result[6], curLog.result[7], curLog.result[8], curLog.result[9]);
				  pBuf = pBuf+prNum;
				  prNum = snprintf(pBuf, sizeof(Log_t), "%f ", curLog.temp[0]);
				  pBuf = pBuf+prNum;
				  prNum = snprintf(pBuf, sizeof(Log_t), "%f ", curLog.temp[1]);
				  pBuf = pBuf+prNum;
				  prNum = snprintf(pBuf, sizeof(Log_t), "%d.%d ", curLog.supplyCurrents[0].intVal, curLog.supplyCurrents[0].fracVal);
				  pBuf = pBuf+prNum;
				  prNum = snprintf(pBuf, sizeof(Log_t), "%d.%d \n", curLog.supplyVoltages[0].intVal, curLog.supplyVoltages[0].fracVal);

				  uartprintf(buf);

				  gfr = f_write(&writeFile, buf, strlen(buf), &bw);
				  gfr = f_sync(&writeFile);
			 }

		  }

	  }

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
