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

#include "test_activity.h"

#include "reliability.h"

#include <string.h>

static FATFS sdFatFs;
static FIL logFile;
FRESULT gfr;

extern osEventFlagsId_t testEvents;
extern osMessageQueueId_t logQueueHandler;
extern osMessageQueueId_t eventQueueHandler;

static uint16_t testFlags = 0;

void fRead(char *configFileName, uint8_t *buf, uint32_t num, uint32_t *br);
FRESULT storage_file_open_create ( FIL* fp, char* path);
HAL_StatusTypeDef storage_config_search(const char *configFileName, TestConfig_t *curConfig);

void StorageTask(void *argument) {
	osStatus_t osRes;
	Log_t curLog = {0};
	uint16_t prNum;
	char buf[256] = {0};
	char *pBuf;
	uint32_t bw;

	uint32_t osEventFlag;

	Event_t msg = {0};
	TestConfig_t curConfig = {0};

    char pBufStart[] = "Successful file creation\n";

  fatfs_init();
  gfr = f_mount(&sdFatFs, "", 1);

  for(;;)
  {

	  // if configuration is not set
	  if (!testFlags){
		  // wait for cmd to find config file
		 if ( (osEventFlag = osEventFlagsWait(testEvents, TEST_CONFIG_SEARCH, osFlagsWaitAny, osWaitForever)) & TEST_CONFIG_SEARCH ){

			 // find config file
			 // encode config to queue type

			 msg.event = TEST_CONFIG_SEND;
			if ( storage_config_search("config.txt", &curConfig) == HAL_OK ){
				memcpy(msg.eventStr, (uint8_t *)&curConfig, sizeof(TestConfig_t));
				//for (uint16_t i=0; i < sizeof(TestConfig_t); i++){
				//	*((&curConfig)+i)
				//}
				//and send to control task
				osMessageQueuePut(eventQueueHandler, &msg, 0, 0);
				osEventFlagsSet(testEvents, TEST_CONFIG_IS_FIND);
			} else {
				osEventFlagsSet(testEvents, TEST_CONFIG_IS_NOT);
				if ( (osEventFlag = osEventFlagsWait(testEvents, TEST_CONFIG_SEND, osFlagsWaitAny, osWaitForever)) & TEST_CONFIG_SEND ){
					// get terminal config
					if( osMessageQueueGet(eventQueueHandler, &msg, NULL, osWaitForever) == osOK ){
						if ( msg.event == TEST_CONFIG_SEND ){
							memcpy((uint8_t *)&curConfig, msg.eventStr, sizeof(TestConfig_t));
						}
					}
				}
			}

			testFlags |= TEST_CONFIG_SEARCH;
		 }

	  } else {

		  if ( ! (testFlags & TEST_START) ){

			 uint32_t osEventFlag = osEventFlagsWait(testEvents, TEST_START | TEST_FINISH, osFlagsWaitAny, osWaitForever);

			 if ( osEventFlag & TEST_START ){

				// create file that named after part number
				uint8_t temp = 5;
				  do{
					  gfr = storage_file_open_create(&logFile, curConfig.partNumber);
				  }while (gfr && --temp);

				 if (!temp)
					 usbprintf("log file opening or creation error");

				if ( bkp_read_data(UEKV_LAST_STATE_REG) == UEKV_IDLE_STATE ){
				// write the configuration to test file
				 f_printf(&logFile, "Part Number: %s \n", curConfig.partNumber );
				 f_printf(&logFile, "MLDR number: %s \n", curConfig.mldrNum );
				 f_printf(&logFile, "Type of test: %s \n", curConfig.testType ? "ETT" : "Reliability" );
				 f_printf(&logFile, "Cell: %d \n", curConfig.cellNum );
				 f_printf(&logFile, "Row: %d \n", curConfig.rowNum );
				 f_printf(&logFile, "Col: %d \n", curConfig.colNum );
				 f_printf(&logFile, "Result check method: %d \n", curConfig.resCheckMethod );
				 f_printf(&logFile, "Test duration in hours: %d h \n", curConfig.testDurationInHours );
				 f_printf(&logFile, "Number of power supplies: %d \n", curConfig.powerSupplyNum );
				 f_printf(&logFile, "Number of PCBs: %d \n", curConfig.pcbNum );
				 gfr = f_sync(&logFile);

				 bkp_write_data(UEKV_LAST_STATE_REG, UEKV_TEST_STATE);
				}

				  testFlags |= TEST_START;

			 } else if (osEventFlag & TEST_FINISH){
				testFlags = 0;
			 }

		  } else {

			  //wait
			  osEventFlag = osEventFlagsWait(testEvents, TEST_FINISH | TEST_LOG_SAVE, osFlagsWaitAny, osWaitForever);

			 if ( osEventFlag & TEST_FINISH ){
				 // close log file
				 gfr = f_sync(&logFile);
				 gfr = f_close(&logFile);
				 bkp_write_data(UEKV_LAST_STATE_REG, UEKV_IDLE_STATE);
				 testFlags = 0;
			 } else if ( osEventFlag & TEST_LOG_SAVE ){
				 // wait for logs
				 if ( osMessageQueueGet(logQueueHandler, &curLog, NULL, osWaitForever) == osOK ){
					  prNum = snprintf(buf, sizeof(Log_t), "%d. ", curLog.index);
					  pBuf = buf+prNum;
					  prNum = snprintf(pBuf, sizeof(Log_t), "%d:%d:%d %d.%d.%d ", curLog.dataTime.hour, curLog.dataTime.min, curLog.dataTime.sec, curLog.dataTime.day, curLog.dataTime.mon, curLog.dataTime.year );
					  pBuf = pBuf+prNum;
					  prNum = snprintf(pBuf, sizeof(Log_t), "%x,%x,%x,%x,%x,%x,%x,%x,%x,%x ", curLog.result[0], curLog.result[1], curLog.result[2], curLog.result[3], curLog.result[4], curLog.result[5], curLog.result[6], curLog.result[7], curLog.result[8], curLog.result[9]);
					  pBuf = pBuf+prNum;
					  prNum = snprintf(pBuf, sizeof(Log_t), "%3.0f ", curLog.temp[0]);
					  pBuf = pBuf+prNum;
					  prNum = snprintf(pBuf, sizeof(Log_t), "%3.0f ", curLog.temp[1]);
					  //pBuf = pBuf+prNum;
					  //prNum = snprintf(pBuf, sizeof(Log_t), "%d.%d ", curLog.supplyCurrents[0].intVal, curLog.supplyCurrents[0].fracVal);
					  //pBuf = pBuf+prNum;
					  //prNum = snprintf(pBuf, sizeof(Log_t), "%d.%d \n", curLog.supplyVoltages[0].intVal, curLog.supplyVoltages[0].fracVal);
					  //pBuf = pBuf+prNum;
					  //prNum = snprintf(pBuf, sizeof(Log_t), "%s \n", curLog.lpsState);

					  uartprintf(buf);

					  gfr = f_write(&logFile, buf, strlen(buf), &bw);
					  gfr = f_sync(&logFile);

					  memset(buf, 0, sizeof(buf));
					  pBuf = buf;
					  if ( curLog.lpsStatusArray != NULL ){
						  for ( uint8_t i = 0; i < lps_get_connected_num(); i++ ){
							  prNum = snprintf(pBuf, sizeof(Log_t), "Lps%2d: %sV, %sA ", curLog.lpsStatusArray[i].addr, curLog.lpsStatusArray[i].volStr,  curLog.lpsStatusArray[i].curStr);
							  pBuf = pBuf+prNum;
						  }
					  }

					  prNum = snprintf(pBuf, sizeof(Log_t), "\n");

					  uartprintf(buf);
					  gfr = f_write(&logFile, buf, strlen(buf), &bw);
					  gfr = f_sync(&logFile);
					  //osEventFlagsSet(testEvents, LPS_LIST_UDATE_READY);


					  memset(&msg, 0, sizeof(Event_t));
					  msg.event = ACTIVITY_UPDATE_EVENT;
					  memcpy(msg.eventStr, (uint8_t *)&curLog, sizeof(Log_t));
					  osRes = osMessageQueuePut (eventQueueHandler, &msg, 0, 0);
					  osEventFlagsSet(testEvents, TEST_LOG_DISPLAY);
				 }

			  }
		  }

	  }

	  osThreadYield();
	  osDelay(10);
  }
}



// Только для записи в конец файла
FRESULT storage_file_open_create( FIL* fp, char* path) {
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

HAL_StatusTypeDef storage_config_search(const char *configFileName, TestConfig_t *curConfig){

	char strBuf[100] = {0};
	FIL readFile;

	int testType = 0;
	int cellNum = 0;
	int rowNum = 0;
	int colNum = 0;
	int resCheckMethod = 0;
	int resCheckPeriod = 0;
	int resCheckOption = 0;
	int testDurationInHours = 0;
	int powerSupplyNum = 0;
	int pcbNum = 0;

	 uint8_t temp = 5;
	 do{
	      gfr = f_open(&readFile, configFileName, FA_READ);
	  }while (gfr && --temp);

	 if (!temp){
		 usbprintf("config.txt file not found");
		 return HAL_ERROR;
	 }
	 // extract needed info
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "part number: %16s", curConfig->partNumber);
		 usbprintf("Part Number: %s", curConfig->partNumber);
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "mldr number: %9s", curConfig->mldrNum);
		 usbprintf("MLDR number: %s", curConfig->mldrNum);
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "type of test: %1d", &testType);
		 curConfig->testType = testType;
		 usbprintf("Type of test: %s", curConfig->testType ? "ETT" : "Reliability");
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "cell: %3d ", &cellNum);
		 curConfig->cellNum = cellNum;
		 usbprintf("Cell: %d", curConfig->cellNum);
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "row: %2d ", &rowNum);
		 curConfig->rowNum = rowNum;
		 usbprintf("Row: %d", curConfig->rowNum);
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "col: %2d ", &colNum);
		 curConfig->colNum = colNum;
		 usbprintf("Col: %d", curConfig->colNum);
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "result check method: %1d", &resCheckMethod);
		 curConfig->resCheckMethod = resCheckMethod;
		 usbprintf("Result check method: %d", curConfig->resCheckMethod);
		 usbprintf("0 - average result,");
		 usbprintf("1 - synchro result for UB");
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "result check period: %2d", &resCheckPeriod);
		 if ( (resCheckPeriod >= 1) && (resCheckPeriod <= 15) )
			 curConfig->resCheckPeriod = resCheckPeriod;
		 else
			 curConfig->resCheckPeriod = 3;
		 usbprintf("Result check period: %d", curConfig->resCheckPeriod);
		 usbprintf("1-15s, check period just for average check");
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "result check option: %1d", &resCheckOption);
		 if ( (resCheckOption >= 0) && (resCheckOption <= 3) )
			 curConfig->resCheckOption = resCheckMethod;
		 else
			 curConfig->resCheckPeriod = 0;
		 usbprintf("Result check option: %d", curConfig->resCheckOption);
		 usbprintf("0 - no option,");
		 usbprintf("1 - save just faults,");
		 usbprintf("2 - trial 1 day,");
		 usbprintf("3 - trial 1 week");
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "test duration in hours: %5d h", &testDurationInHours);
		 curConfig->testDurationInHours = testDurationInHours;
		 usbprintf("Test duration in hours: %d h", curConfig->testDurationInHours);
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "number of power supplies: %2d", &powerSupplyNum);
		 curConfig->powerSupplyNum = powerSupplyNum;
		 usbprintf("Number of power supplies: %d", curConfig->powerSupplyNum);
	 }
	 if ( !f_eof(&readFile) ){
		 f_gets(strBuf, sizeof(strBuf), &readFile);
		 sscanf(strBuf, "number of pcbs: %d", &pcbNum);
		 curConfig->pcbNum = pcbNum;
		 usbprintf("Number of PCBs: %d", curConfig->pcbNum);
	 } else {
		 f_close(&readFile);
		 usbprintf("Wrong config file");
		 return HAL_ERROR;
	 }

	  f_close(&readFile);
	  return HAL_OK;
}
