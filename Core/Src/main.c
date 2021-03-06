/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"       //LIBRERIA OLED
#include "anim_ojos.h"     //LIBRERIA CON ANIMACION OJOS
#include "MY_Keypad4x4.h"  //LIBRERIA KEYPAD
#include <stdbool.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define usTIM TIM1  //TIMER PARA ULTRASONIDOS
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void keypad_init(void);
char keypad_read(void);
// DECLARACION FUNCION QUE IMPRIME ANIMACION OJOS
void animacion_ojos(void);
// DECLARACION FUNCION DELAY PARA SENSOR ULTRASONIDOS
void usDelay (uint16_t uSec);
// DECLARACION FUNCION ULTRASONIDOS
void dist_Ultrasonidos(void);
// DECLARACION FUNCIONES CONTRASE??A
//unsigned char key_press(void);
//unsigned char check_clm(unsigned char pin_var);
//void Disp_pass_key(unsigned char key_var);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* PUERTOS
  PA1--SE??AL IR

  PC8 Y PC7 CONTROL MOTOR
  PD12 PWM MOTOR

  PD13--TRIG
  PA6--ECHO

  PD0-PD3 OUTPUT PAD
  PD4-PD7 INPUT PAD

*/

uint8_t check1;	     // PANTALLAS
uint32_t ADC_value_ir;	     // VALOR IR

uint8_t puerta = 0;			     // 0 CERRADA 1 ABIERTA

//VARIABLES PARA SENSOR ULTRASONIDOS
float distancia;
uint8_t cuenta=0;

uint8_t icFlag = 0;
uint8_t captureIdx=0;
uint32_t edge1Time=0, edge2Time=0;

// VARIABLE CONTRASE??A
uint8_t contrasena_bit = 0;   // 0 INCORRECTO 1 CORRECTO
uint8_t boton = 0;

bool botones_pad[16];
char *contrasena[5] = {"1","4","6","7","5"};

int i=0;

Keypad_WiresTypeDef keypadStruct;
//VARIABLES A USAR DENTRO DEL PROGRAMA

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  // INICIALIZA PANTALLAS
  check1 = ssd1306_Init(&hi2c1);	// INICIALIZA PANTALLA DE INFORMACION
  ssd1306_Fill(0);          		// 0 NEGRO
  ssd1306_UpdateScreen(&hi2c1);     // IMPRIME LOS CAMBIOS
  HAL_Delay(100);

  // LECTURA DATOS IR
  HAL_ADC_Start_DMA(&hadc1, &ADC_value_ir, 1);

  // SALIDAS DIRECCION MOTOR
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

  HAL_TIM_Base_Start(&htim4);
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);  // PWM PARA MOTOR TEMPORIZADOR 4 CHANNEL 1*/

  // INICIALIZACION CODIGO
  keypadStruct.IN0_Port = GPIOD;
  keypadStruct.IN1_Port = GPIOD;
  keypadStruct.IN2_Port = GPIOD;
  keypadStruct.IN3_Port = GPIOD;

  keypadStruct.OUT0_Port = GPIOD;
  keypadStruct.OUT1_Port = GPIOD;
  keypadStruct.OUT2_Port = GPIOD;
  keypadStruct.OUT3_Port = GPIOD;

  keypadStruct.IN0pin = GPIO_PIN_7;
  keypadStruct.IN1pin = GPIO_PIN_6;
  keypadStruct.IN2pin = GPIO_PIN_5;
  keypadStruct.IN3pin = GPIO_PIN_4;

  keypadStruct.OUT0pin = GPIO_PIN_3;
  keypadStruct.OUT1pin = GPIO_PIN_2;
  keypadStruct.OUT2pin = GPIO_PIN_1;
  keypadStruct.OUT3pin = GPIO_PIN_0;

  Keypad4x4_Init(&keypadStruct);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  if(boton == 0 && puerta == 0)
	  {
		  animacion_ojos();	// LLAMA FUNCION QUE IMPRIME LA ANIMACION PARALIZA EL MICRO DURANTE TODA LA ANIMACION
	  }

      // CONTRASE??A
	 if (contrasena_bit == 0 && puerta == 0 && boton == 1)
	  {
		  ssd1306_Fill(0);          		// 0 NEGRO
		  ssd1306_SetCursor( 12, 5 );
		  ssd1306_WriteString("INTRODUZCA", Font_11x18, 1);
		  ssd1306_SetCursor( 12, 20 );
		  ssd1306_WriteString("CODIGO", Font_11x18, 1);
		  ssd1306_UpdateScreen(&hi2c1);		  	  

		  do{
			  Keypad4x4_ReadKeypad(botones_pad);
			  for(uint8_t j=0; j<16; j++)
			  {
		  		  	if(botones_pad[j])
		  		  	{
		  		  		if(strcmp(contrasena[i],Keypad4x4_GetChar(j))/*contrasena[i] != Keypad4x4_GetChar(j)*/)
		  		  		{
		  		  			//print error
		  		  			ssd1306_Fill(0);          		// 0 NEGRO
		  		  			ssd1306_SetCursor( 18, 5 );
		  		  			ssd1306_WriteString("CODIGO", Font_11x18, 1);
		  		  			ssd1306_SetCursor( 10, 20 );
		  		  			ssd1306_WriteString("INCORRECTO", Font_11x18, 1);
		  		  			ssd1306_SetCursor( 18, 40 );
		  		  			ssd1306_WriteString(Keypad4x4_GetChar(j), Font_11x18, 1);
		  		  			ssd1306_UpdateScreen(&hi2c1);

		  		  			HAL_Delay(1000);
		  		  			ssd1306_Fill(0);          		// 0 NEGRO
		  		  			ssd1306_SetCursor( 12, 5 );
		  		  			ssd1306_WriteString("INTRODUZCA", Font_11x18, 1);
		  		  			ssd1306_SetCursor( 12, 20 );
		  		  			ssd1306_WriteString("CODIGO", Font_11x18, 1);
		  		  			ssd1306_UpdateScreen(&hi2c1);
		  		  			i=0;
		  		  		}

		  		  		else
		  		  		{
		  		  			//continua CODIGO BIEN DE MOMENTO
		  		  			ssd1306_SetCursor( 18+10*i, 40 );
		  		  			ssd1306_WriteString(Keypad4x4_GetChar(j), Font_11x18, 1);
		  		  			ssd1306_UpdateScreen(&hi2c1);
		  		  			i++;
		  		  		}
		  		  		//HAL_Delay(100);
		  		  	}
			   }
			   HAL_Delay(100);
		  	}while(i<4);
		    contrasena_bit = 1;
	  }

	  if (contrasena_bit == 1 && puerta == 0)
	  {
		  ssd1306_Fill(0);          		// 0 NEGRO
		  ssd1306_SetCursor( 18, 10 );
		  ssd1306_WriteString("CODIGO", Font_11x18, 1);
		  ssd1306_SetCursor( 18, 30 );
		  ssd1306_WriteString("CORRECT0", Font_11x18, 1);
		  ssd1306_UpdateScreen(&hi2c1);
		  HAL_Delay(1000);

		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET); // ARRANCA MOTOR *MIRAR DIRECCION
		  HAL_Delay(800); // AJUSTAR PARA QUE SALGA DEL AGUJERO
		  ssd1306_Fill(0);          		// 0 NEGRO
		  ssd1306_SetCursor( 18, 10 );
		  ssd1306_WriteString("ABRIENDO", Font_11x18, 1);
		  ssd1306_SetCursor( 18, 30 );
		  ssd1306_WriteString("PUERTA", Font_11x18, 1);
		  ssd1306_UpdateScreen(&hi2c1);

		  do
		  {
			  dist_Ultrasonidos();
		  }while((distancia <= 4.05f));

		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); // PARA MOTOR

		  ssd1306_Fill(0);          		// 0 NEGRO
		  ssd1306_SetCursor( 18, 10 );
		  ssd1306_WriteString("PUERTA", Font_11x18, 1);
		  ssd1306_SetCursor( 18, 30 );
		  ssd1306_WriteString("ABIERTA", Font_11x18, 1);
		  ssd1306_UpdateScreen(&hi2c1);

		  puerta = 1;
		  HAL_Delay(5000);

	  }
	  if(puerta == 1)
	  {
		  while(ADC_value_ir >= 2500)
		  {
			  ssd1306_Fill(0);          		// 0 NEGRO
			  ssd1306_SetCursor( 18, 10 );
			  ssd1306_WriteString("OBJETO", Font_11x18, 1);
			  ssd1306_SetCursor( 18, 30 );
			  ssd1306_WriteString("DETECTADO", Font_11x18, 1);
			  ssd1306_UpdateScreen(&hi2c1);
		  }
	  }

	  if((ADC_value_ir <= 2500) && (ADC_value_ir >= 1000) && (puerta == 1)) // MIRAR DISTANCIA PARA AJUSTAR
	  {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // ARRANCA MOTOR *MIRAR DIRECCION
			HAL_Delay(800); // AJUSTAR PARA QUE SALGA DEL AGUJERO
			ssd1306_Fill(0);          		// 0 NEGRO
			ssd1306_SetCursor( 18, 10 );
			ssd1306_WriteString("CERRANDO", Font_11x18, 1);
			ssd1306_SetCursor( 18, 30 );
			ssd1306_WriteString("PUERTA", Font_11x18, 1);
			ssd1306_UpdateScreen(&hi2c1);

			do
			{
				dist_Ultrasonidos();
			}while(distancia <= 4.05f);

			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // PARA MOTOR

			ssd1306_Fill(0);          		// 0 NEGRO
			ssd1306_SetCursor( 18, 10 );
			ssd1306_WriteString("PUERTA", Font_11x18, 1);
			ssd1306_SetCursor( 18, 30 );
			ssd1306_WriteString("CERRADA", Font_11x18, 1);
			ssd1306_UpdateScreen(&hi2c1);

			boton=0;
			puerta = 0;
			contrasena_bit = 0;
			i=0;

			HAL_Delay(2000);

	  }

	  //__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,75); // CONTROL VELOCIDAD MOTOR, YA ESTA POR DEFECTO A 75%


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 83;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 83;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 5;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 16-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 100;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 75;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11|TRIG_Pin|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD11 TRIG_Pin PD0 PD1
                           PD2 PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|TRIG_Pin|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PC7 PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)// TESTEO CON BOTON PLACA
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */

  boton=1;
}


void usDelay (uint16_t uSec)
{
	if(uSec < 2) uSec = 2;
	usTIM -> ARR = uSec -1;
	usTIM -> EGR = 1;
	usTIM -> SR &= 1;
	usTIM -> CR1 |= 1;
	while((TIM1 -> SR & 0x0001) != 1);
	usTIM -> SR &= ~(0x0001);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		if(captureIdx == 0) //Fisrt edge
		{
			edge1Time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); //__HAL_TIM_GetCounter(&htim3);//

			captureIdx = 1;
		}
		else if(captureIdx == 1) //Second edge
		{
			edge2Time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			captureIdx = 0;
			icFlag = 1;
		}
}


void dist_Ultrasonidos(void)
{
	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
	usDelay(3);

	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);
	usDelay(10);
	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);

	 HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
	//Wait for IC flag
	uint32_t startTick = HAL_GetTick();
	do
	{
	 	if(icFlag) break;
	}while((HAL_GetTick() - startTick) < 500);  //500ms
	icFlag = 0;
	HAL_TIM_IC_Stop_IT(&htim3, TIM_CHANNEL_1);

	//DISTANCIA EN CM
	if(edge2Time > edge1Time)
	{
	  	distancia = ((edge2Time - edge1Time) + 0.0f)*0.0343/2;
	}
	else
	{
		distancia = 0.0f;
	}
}

void animacion_ojos(void)
{
	uint32_t parp = 20;
	uint32_t stop = 1000;
	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_1,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(stop);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_2,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_3,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_4,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_5,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(stop);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_6,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_7,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_8,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_9,128,64,1);
    ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(stop);

	ssd1306_Fill(0);
	ssd1306_DrawBitmap(0,0,ojos_10,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_11,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

	ssd1306_Fill(0);
    ssd1306_DrawBitmap(0,0,ojos_12,128,64,1);
	ssd1306_UpdateScreen(&hi2c1);
	HAL_Delay(parp);

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
