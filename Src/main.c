/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "cordic.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "regular_conversion_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  PID_Handle_t * pPIDSpeed;
  PID_Handle_t * pPIDIq;
  PID_Handle_t * pPIDId;
  PID_Handle_t * pPIDFluxWeakening;
  PWMC_Handle_t * pPWMnCurrFdbk;
  RevUpCtrl_Handle_t * pRevupCtrl;
  //未完
}MCT_Handle_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MIN_SPEED 300 //300rpm
#define MAX_SPEED 800 //800rpm
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t Run_Count = 0;
RegConv_t ADC_Userconv;
uint8_t ADC_UserHandle;
uint16_t ADC_UserValue;
uint16_t Set_Speed;
static uint16_t State_Mark;
static uint16_t Fault_Mark;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*Hand on 1 - Sensorless speed mode*/
void HandsOn1(void)
{
  /*300RPM,1000ms*/
  MC_ProgramSpeedRampMotor1(300/6,1000);
  if(Run_Count == 0)
  {
    /*Start motor*/
    MC_StartMotor1();
  }
  HAL_Delay(1000);

  /*600RPM,1000ms*/
  MC_ProgramSpeedRampMotor1(600/6,1000);
  HAL_Delay(1000);

  Run_Count++;
  if(Run_Count > 5)
  {
    Run_Count = 5;
    /*Stop motor*/
    MC_StopMotor1();
    while (1)
    {
      /* code */
      ;
    }
  }
}

/*Hand On 2 - User ADC control speed*/
void HandsOn2_Init(void)
{
  ADC_Userconv.regADC = ADC1;
  ADC_Userconv.channel = MC_ADC_CHANNEL_8;
  ADC_Userconv.samplingTime = ADC_SAMPLETIME_92CYCLES_5;
  ADC_UserHandle = RCM_RegisterRegConv(&ADC_Userconv);

  /*Start motor*/
  MC_StartMotor1();
}

void HandsOn2(void)
{
  HAL_Delay(200);
  /*检查状态*/
  if(RCM_GetUserConvState() == RCM_USERCONV_IDLE)
  {
    /*若Idle，创建新的conversion request*/
    RCM_RequestUserConv(ADC_UserHandle);
  }
  else if(RCM_GetUserConvState() == RCM_USERCONV_EOC)
  {
    /*完成了，读captured value*/
    ADC_UserValue = RCM_GetUserConv();
  }
  /*计算设定速度值*/
  Set_Speed = (ADC_UserValue >>4)/8 + MIN_SPEED;
  if(Set_Speed > MAX_SPEED)
  {
    Set_Speed = MAX_SPEED;
  }
  /*设定新的speed ramp*/
  MC_ProgramSpeedRampMotor1(Set_Speed/6,100);
}

/*Hand On 3 -Fault clear*/
void HandsOn3(void)
{
  /*读电机状态*/
  State_Mark = MC_GetSTMStateMotor1();
  /*读错误状态*/
  Fault_Mark = MC_GetOccurredFaultsMotor1();

  if(Fault_Mark != MC_NO_FAULTS)
  {
    HAL_Delay(2000);
    /*清除错误状态*/
    MC_AcknowledgeFaultMotor1();

    /*执行最后命令*/
    MC_ProgramSpeedRampMotor1(MC_GetLastRampFinalSpeedMotor1(),1000);
  }

  if((State_Mark == IDLE) && (Fault_Mark == MC_NO_FAULTS))
  {
    MC_StartMotor1();
  }
}
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
  //MX_MotorControl_Init();马达控制初始化
  //MX_NVIC_Init();中断优先级控制，使能中断
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_CORDIC_Init();
  MX_DAC1_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_MotorControl_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  //TIM1触发ADC中断，stm32g4xx_mc_it.c的void ADC1_2_IRQHandler()
  //然后在mc_tasks.c的uint8_t TSK_HighFrequencyTask()完成FOC坐标转换和SVPWM
  //最后调整TIM1占空比

  //Systick 中断默认为 500us 的定时中断
  //中频任务在此，状态机
	HandsOn2_Init();
  HAL_TIM_Base_Start_IT(&htim2);  //启动TIM2，周期为500ms
  extern PID_Handle_t *pPIDSpeed[1];
  static int16_t Speed_Kp,Speed_Ki;

  // Speed_Kp = PID_GetKP(pPIDSpeed[0]);
  // Speed_Ki = PID_GetKI(pPIDSpeed[0]);

  //PID_SetKP(pPIDSpeed[0] , Speed_Kp*2);
  //PID_SetKI(pPIDSpeed[0] , Speed_Ki*2);
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //HandsOn2();
		//HandsOn3();
		MC_ProgramSpeedRampMotor1(300/6,0);
		/*Start motor*/
    MC_StartMotor1();
    HAL_Delay(10000);
		
		
    MC_ProgramSpeedRampMotor1(1200/6,0);
		/*Start motor*/
    MC_StartMotor1();
    HAL_Delay(10000);
		// Speed_Kp = PID_GetKP(pPIDSpeed[0]);
    // Speed_Ki = PID_GetKI(pPIDSpeed[0]);
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 3, 1);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* DMA2_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);
  /* TIM1_BRK_TIM15_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, 4, 1);
  HAL_NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
  /* TIM1_UP_TIM16_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
  /* ADC1_2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(ADC1_2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 4 */
/*TIM2中断回调函数*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  static unsigned char ledState = 0;
  if (htim == (&htim2))
  { 
    //电机正常转时500ms周期闪灯
    if(MC_GetSTMStateMotor1() == RUN)
    {
      if (ledState == 0)
          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
      else
          HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
      ledState = !ledState;
    }
    else  //不正常就关灯
    {
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
    }
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
