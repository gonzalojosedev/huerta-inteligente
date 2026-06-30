/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "lcd_i2c.h"
#include "dht11.h"
#include "roof.h"
#include "pump.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SOIL_DRY_ADC   4066U
#define SOIL_WET_ADC   2332U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t adcValue = 0;
uint32_t soilPercent = 0;
char line[32];

DHT11_Data dhtData;
uint8_t dhtOk = 0;

uint32_t lastDhtRead = 0;
uint32_t lastSoilRead = 0;
uint32_t lastLcdUpdate = 0;
uint32_t lastIrrigationCheck = 0;

const uint32_t DHT_PERIOD_MS = 2000U;
const uint32_t SOIL_PERIOD_MS = 2000U;
const uint32_t LCD_PERIOD_MS = 1000U;
const uint32_t IRRIGATION_PERIOD_MS = 5000U;

uint8_t irrigationActive = 0;

uint32_t irrigationStartTime = 0;
const uint32_t IRRIGATION_DURATION_MS = 10000U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void App_ReadDht11(void);
static void App_ReadSoilSensor(void);
static void App_UpdateIrrigation(void);
static void App_UpdateOutputs(void);
static void App_UpdateLcd(void);

static void App_UpdateRoof(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3);

  LCD_Init(&hi2c1);
  LCD_Clear();

  Roof_Init();

  Pump_Init();

  LCD_SetCursor(0, 0);
  LCD_Print("Huerta OK");
  HAL_Delay(2000);
  LCD_Clear();

  lastDhtRead = HAL_GetTick() - DHT_PERIOD_MS;
  lastSoilRead = HAL_GetTick() - SOIL_PERIOD_MS;
  lastLcdUpdate = HAL_GetTick() - LCD_PERIOD_MS;
  lastIrrigationCheck = HAL_GetTick() - IRRIGATION_PERIOD_MS;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      App_ReadDht11();
      App_ReadSoilSensor();
      App_UpdateIrrigation();
      App_UpdateRoof();
      App_UpdateOutputs();
      App_UpdateLcd();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
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
}

/* USER CODE BEGIN 4 */
static void App_ReadDht11(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - lastDhtRead) >= DHT_PERIOD_MS)
    {
        lastDhtRead = now;

        dhtOk = DHT11_Read(&dhtData);
    }
}

static void App_ReadSoilSensor(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - lastSoilRead) >= SOIL_PERIOD_MS)
    {
        uint32_t sum = 0U;

        lastSoilRead = now;

        for (uint8_t i = 0U; i < 8U; i++)
        {
            HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1, 100U);

            sum += HAL_ADC_GetValue(&hadc1);
        }

        adcValue = sum / 8U;

        if (adcValue >= SOIL_DRY_ADC)
        {
            soilPercent = 0U;
        }
        else if (adcValue <= SOIL_WET_ADC)
        {
            soilPercent = 100U;
        }
        else
        {
            soilPercent =
                ((SOIL_DRY_ADC - adcValue) * 100U) /
                (SOIL_DRY_ADC - SOIL_WET_ADC);
        }
    }
}

static void App_UpdateIrrigation(void)
{
    uint32_t now = HAL_GetTick();

    if (irrigationActive != 0U)
    {
        if ((now - irrigationStartTime) >= IRRIGATION_DURATION_MS)
        {
            irrigationActive = 0U;
        }

        return;
    }

    if ((now - lastIrrigationCheck) >= IRRIGATION_PERIOD_MS)
    {
        lastIrrigationCheck = now;

        if ((soilPercent < 50U) &&
            (dhtData.humidity < 60U) &&
            (dhtData.temperature > 5U) &&
            (dhtOk == 1U))
        {
            irrigationActive = 1U;
            irrigationStartTime = now;
        }
    }
}

static void App_UpdateOutputs(void)
{
    if (irrigationActive != 0U)
    {
        Pump_On();

        HAL_GPIO_WritePin(LED_IRRIGATION_GPIO_Port,
                          LED_IRRIGATION_Pin,
                          GPIO_PIN_SET);

        HAL_GPIO_WritePin(GPIOA,
                          GPIO_PIN_5,
                          GPIO_PIN_RESET);
    }
    else
    {
        Pump_Off();

        HAL_GPIO_WritePin(LED_IRRIGATION_GPIO_Port,
                          LED_IRRIGATION_Pin,
                          GPIO_PIN_RESET);

        HAL_GPIO_WritePin(GPIOA,
                          GPIO_PIN_5,
                          GPIO_PIN_SET);
    }
}

static void App_UpdateLcd(void)
{
    uint32_t now = HAL_GetTick();

    if ((now - lastLcdUpdate) >= LCD_PERIOD_MS)
    {
        lastLcdUpdate = now;

        LCD_SetCursor(0, 0);

        if (dhtOk == 1U)
        {
            snprintf(line, sizeof(line), "T:%2uC H:%2u%%     ",
                     dhtData.temperature,
                     dhtData.humidity);
        }
        else
        {
            snprintf(line, sizeof(line), "DHT ERR:%u       ", dhtOk);
        }

        LCD_Print(line);

        LCD_SetCursor(1, 0);

        if (irrigationActive != 0U)
        {
            snprintf(line, sizeof(line), "Riego ON S:%3lu%% ", soilPercent);

        }
        else
        {
            snprintf(line, sizeof(line), "Riego OFF S:%3lu%%", soilPercent);
        }

        LCD_Print(line);
    }
}

static void App_UpdateRoof(void)
{
    if (dhtOk != 1U)
    {
        return;
    }

    switch (Roof_GetState())
    {
        case ROOF_STOPPED:

            if (dhtData.temperature < 20U)
            {
                Roof_Close();
            }
            else if (dhtData.temperature > 25U)
            {
                Roof_Open();
            }

            break;

        case ROOF_OPENING:

            if (HAL_GPIO_ReadPin(BTN_STOP_OPEN_GPIO_Port,
                                 BTN_STOP_OPEN_Pin) == GPIO_PIN_RESET)
            {
                Roof_Stop();
            }

            break;

        case ROOF_CLOSING:

            if (HAL_GPIO_ReadPin(BTN_STOP_CLOSE_GPIO_Port,
                                 BTN_STOP_CLOSE_Pin) == GPIO_PIN_RESET)
            {
                Roof_Stop();
            }

            break;

        case ROOF_MANUAL_STOP:

            if ((dhtData.temperature >= 20U) &&
                (dhtData.temperature <= 25U))
            {
                Roof_ResetManualStop();
            }

            break;

        default:
            Roof_Stop();
            break;
    }
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
