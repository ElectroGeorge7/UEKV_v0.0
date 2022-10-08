
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lm_timer.h"

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim12;
DMA_HandleTypeDef hdma_tim1_ch3;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart6;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM9_Init(void);
static void MX_UART4_Init(void);
static void MX_I2C3_Init(void);
static void MX_TIM12_Init(void);
static void MX_USART6_UART_Init(void);


#define DELAY_LEN 80
#define LED_COUNT 140
#define ARRAY_LEN DELAY_LEN + LED_COUNT*24

#define HIGH 65
#define LOW 26

#define BitIsSet(reg, bit) ((reg & (1<<bit)) != 0)

uint16_t BUF_DMA[ARRAY_LEN] = {0};
uint8_t rgb_temp[12][3];




void ws2812_init(void){
  int i;

  for(i=DELAY_LEN;i<ARRAY_LEN;i++) 
    BUF_DMA[i] = LOW;
}


void ws2812_pixel_rgb_to_buf_dma(uint8_t Rpixel , uint8_t Gpixel, uint8_t Bpixel, uint16_t posX){
  volatile uint16_t i;

  for(i=0;i<8;i++){

    if (BitIsSet(Rpixel,(7-i)) == 1){
      BUF_DMA[DELAY_LEN+posX*24+i+8] = HIGH;
    }else{
      BUF_DMA[DELAY_LEN+posX*24+i+8] = LOW;
    }

    if (BitIsSet(Gpixel,(7-i)) == 1){
      BUF_DMA[DELAY_LEN+posX*24+i+0] = HIGH;
    }else{
      BUF_DMA[DELAY_LEN+posX*24+i+0] = LOW;
    }

    if (BitIsSet(Bpixel,(7-i)) == 1){
      BUF_DMA[DELAY_LEN+posX*24+i+16] = HIGH;
    }else{
      BUF_DMA[DELAY_LEN+posX*24+i+16] = LOW;
    }

  }

}

void ws2812_prepareValue (  uint8_t r00, uint8_t g00, uint8_t b00,
                            uint8_t r01, uint8_t g01, uint8_t b01,
                            uint8_t r02, uint8_t g02, uint8_t b02,
                            uint8_t r03, uint8_t g03, uint8_t b03,
                            uint8_t r04, uint8_t g04, uint8_t b04,
                            uint8_t r05, uint8_t g05, uint8_t b05,
                            uint8_t r06, uint8_t g06, uint8_t b06,
                            uint8_t r07, uint8_t g07, uint8_t b07,
                            uint8_t r08, uint8_t g08, uint8_t b08,
                            uint8_t r09, uint8_t g09, uint8_t b09,
                            uint8_t r10, uint8_t g10, uint8_t b10,
                            uint8_t r11, uint8_t g11, uint8_t b11  )
{
  rgb_temp[0][0]=r00; rgb_temp[0][1]=g00; rgb_temp[0][2]=b00;
  rgb_temp[1][0]=r01; rgb_temp[1][1]=g01; rgb_temp[1][2]=b01;
  rgb_temp[2][0]=r02; rgb_temp[2][1]=g02; rgb_temp[2][2]=b02;
  rgb_temp[3][0]=r03; rgb_temp[3][1]=g03; rgb_temp[3][2]=b03;
  rgb_temp[4][0]=r04; rgb_temp[4][1]=g04; rgb_temp[4][2]=b04;
  rgb_temp[5][0]=r05; rgb_temp[5][1]=g05; rgb_temp[5][2]=b05;
  rgb_temp[6][0]=r06; rgb_temp[6][1]=g06; rgb_temp[6][2]=b06;
  rgb_temp[7][0]=r07; rgb_temp[7][1]=g07; rgb_temp[7][2]=b07;
  rgb_temp[8][0]=r08; rgb_temp[8][1]=g08; rgb_temp[8][2]=b08;
  rgb_temp[9][0]=r09; rgb_temp[9][1]=g09; rgb_temp[9][2]=b09;
  rgb_temp[10][0]=r10; rgb_temp[10][1]=g10; rgb_temp[10][2]=b10;
  rgb_temp[11][0]=r11; rgb_temp[11][1]=g11; rgb_temp[11][2]=b11;
}

void ws2812_setValue(void){
  uint8_t n=0;

  for(n=0;n<12;n++){
    ws2812_pixel_rgb_to_buf_dma( rgb_temp[0][0], rgb_temp[0][1], rgb_temp[0][2], n*12);
    ws2812_pixel_rgb_to_buf_dma( rgb_temp[1][0], rgb_temp[1][1], rgb_temp[1][2], n*12+1);
    ws2812_pixel_rgb_to_buf_dma( rgb_temp[2][0], rgb_temp[2][1], rgb_temp[2][2], n*12+2);
    ws2812_pixel_rgb_to_buf_dma( rgb_temp[3][0], rgb_temp[3][1], rgb_temp[3][2], n*12+3);
    ws2812_pixel_rgb_to_buf_dma( rgb_temp[4][0], rgb_temp[4][1], rgb_temp[4][2], n*12+4);
    ws2812_pixel_rgb_to_buf_dma( rgb_temp[5][0], rgb_temp[5][1], rgb_temp[5][2], n*12+5);
    ws2812_pixel_rgb_to_buf_dma( rgb_temp[6][0], rgb_temp[6][1], rgb_temp[6][2], n*12+6);
    ws2812_pixel_rgb_to_buf_dma( rgb_temp[7][0], rgb_temp[7][1], rgb_temp[7][2], n*12+7);
    if (n!=11){
        ws2812_pixel_rgb_to_buf_dma( rgb_temp[8][0], rgb_temp[8][1], rgb_temp[8][2], n*12+8);
        ws2812_pixel_rgb_to_buf_dma( rgb_temp[9][0], rgb_temp[9][1], rgb_temp[9][2], n*12+9);
        ws2812_pixel_rgb_to_buf_dma( rgb_temp[10][0], rgb_temp[10][1], rgb_temp[10][2], n*12+10);
        ws2812_pixel_rgb_to_buf_dma( rgb_temp[11][0], rgb_temp[11][1], rgb_temp[11][2], n*12+11);
    }
  }
}

void ws2812_light(void)
{
  lm_timer_start(BUF_DMA, ARRAY_LEN);
}



int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  HAL_Delay(500);

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  lm_timer_init();

  ws2812_init();
  HAL_Delay(1000);
  ws2812_prepareValue(32,   0,   0,

                        0, 32,   0,

                        0,   0, 32,

                       32,  32,   0,

                        0,  32,  32,

                       32,   0,  32,

                       32,  16,   0,

                       32,   0,  16,

                       16,  32,   0,

                        0,  32,  16,

                        0,  16,  32,

                       16,   0,  32
  );

  ws2812_setValue();
  HAL_Delay(500);
  ws2812_light();
  HAL_Delay(500);

  while (1)
  {

  }
 
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	  /** Configure the main internal regulator output voltage
	  */
	  __HAL_RCC_PWR_CLK_ENABLE();
	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
	  /** Initializes the RCC Oscillators according to the specified parameters
	  * in the RCC_OscInitTypeDef structure.
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLM = 12;
	  RCC_OscInitStruct.PLL.PLLN = 144;
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = 6;
	  RCC_OscInitStruct.PLL.PLLR = 2;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Initializes the CPU, AHB and APB buses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDIO|RCC_PERIPHCLK_CLK48;
	  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
	  PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  if (HAL_SD_Init(&hsd) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}


/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 0;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 65535;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * @brief TIM12 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM12_Init(void)
{

  /* USER CODE BEGIN TIM12_Init 0 */

  /* USER CODE END TIM12_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM12_Init 1 */

  /* USER CODE END TIM12_Init 1 */
  htim12.Instance = TIM12;
  htim12.Init.Prescaler = 0;
  htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim12.Init.Period = 65535;
  htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim12, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM12_Init 2 */

  /* USER CODE END TIM12_Init 2 */
  HAL_TIM_MspPostInit(&htim12);

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}



/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_ERROR_Pin|LED_PROCESS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, TEMP_SENS_CS1_Pin|TEMP_SENS_CS2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS485_nRE_Pin|RS485_DE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin|SPI_ETT_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_ERROR_Pin TEMP_SENS_CS1_Pin TEMP_SENS_CS2_Pin LED_PROCESS_Pin */
  GPIO_InitStruct.Pin = LED_ERROR_Pin|TEMP_SENS_CS1_Pin|TEMP_SENS_CS2_Pin|LED_PROCESS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RS485_nRE_Pin RS485_DE_Pin */
  GPIO_InitStruct.Pin = RS485_nRE_Pin|RS485_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI_ETT_CS2_Pin SPI_ETT_CS3_Pin SPI_ETT_CS4_Pin SPI_ETT_CS_Pin */
  GPIO_InitStruct.Pin = SPI_ETT_CS2_Pin|SPI_ETT_CS3_Pin|SPI_ETT_CS4_Pin|SPI_ETT_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : uSD_DETECT_Pin */
  GPIO_InitStruct.Pin = uSD_DETECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(uSD_DETECT_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
