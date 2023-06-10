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
#include "stdio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */
int i_flash;
int ready=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*to redirect printf to uart with semihosting.*/

int _write(int fd, char* ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, HAL_MAX_DELAY);
    return len;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  int a = 0;
  long int times = 0;
  long int time_start;
  long int time_end;
  int state = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  
  
  
  //初始化HAL库，第一次在程序中执行的指令,初始化所有的外围设备
  HAL_Init();
  


  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  
  
  //系统时钟配置
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */

  //GPIO初始化
  MX_GPIO_Init();

  //对串口中断进行使能
  MX_USART1_UART_Init();


  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  # if START_TIM6
  if(HAL_TIM_Base_Start_IT(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  #endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //此处添加输入输出代码
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_RESET)   /* sw0 控制状态切换*/
      {
        state=0;//RESET位为数据传输状态
        HAL_GPIO_WritePin(GPIOF,GPIO_PIN_7,GPIO_PIN_RESET);//传递信息给接收方实验箱
      }

    else 
      {
        state=1;//SET位为LED灯控制模块
        HAL_GPIO_WritePin(GPIOF,GPIO_PIN_7,GPIO_PIN_SET);
      }


    /*——————————LED灯控制模块——————————*/
    
    while(state)
    {
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_RESET)   /* sw1 控制GPIOF_PIN_5电平向接收方传递信息*/
        HAL_GPIO_WritePin(GPIOF,GPIO_PIN_5, GPIO_PIN_RESET);
      else 
        HAL_GPIO_WritePin(GPIOF,GPIO_PIN_5, GPIO_PIN_SET);

      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_RESET)   /* sw2 控制GPIOF_PIN6电平*/
        HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6, GPIO_PIN_RESET);
      else 
        HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6, GPIO_PIN_SET); 
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_RESET)   /* 若SW0状态改变，切换状态 */
       {
        break;
       }
        
    }
  
    

    /*————————数据传输速度测试模块—————————*/
  if(!state)
    {
      time_start=HAL_GetTick();//开始时记录时间
      times=0;//每次开始该数据传输状态将次数重置为0
    }
    
  //数据传输开始
  while(!state)
  {
    if(a <= 128)
      a++;
    else
      a = 0;//传输数据从0-128，换算为二进制的后五位

    if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_9)==GPIO_PIN_RESET)//判断ack=0,即接收端实验箱处于空闲
      {
        GPIOF->ODR = (GPIOF->ODR & 0xFFE0) | (a & 0x1F);//准备数据
        HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,GPIO_PIN_SET);//数据准备完毕，将ready置1
        // ready=1;
        // printf("1");
      }
    if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_9)==GPIO_PIN_SET)//判断当接收端开始接收数据（ack=1）
      {
        HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,GPIO_PIN_RESET);//复位ready
        times++;//记录完成一次数据传输
        // ready=0; 
        // printf("2");  //调试代码
        time_end=HAL_GetTick();//记录传输完成时间
        // printf("Sending: \n data :  ");
        // int b=a;
        // int j=0;
        // for(j=0;j<5;j++)
        // {
        //   if(b%2)
        //   printf("1");
        //   else
        //   printf("0");
        //   b=b/2;
        // }
        // printf("\n Num : %ld Time : %ld \n\n ",times,time_end-time_start);//输出传输数据，成功传输次数及传输所需时间
    if((time_end-time_start)%1000==0)
    printf("\n\n\n\n\n\n\n\n\n\n Have sent %ld times. \n\n\n\n\n\n\n\n\n\n" , times);//输出每隔1秒，成功传输数据的次数
      }
    
   
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_SET)   /* 根据sw0状态及时切换状态  */
    {
      break; 
    }
    }

  }

  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
  /* USER CODE END 3 */


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
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
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
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 16 * 168 * 2 - 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
   GPIO_InitTypeDef GPIO_Initure;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE(); 
  __HAL_RCC_GPIOC_CLK_ENABLE(); 

  GPIO_Initure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | 
  GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_0 | GPIO_PIN_8;  /* led1/2/3 传输数据*/
  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Initure.Pull = GPIO_NOPULL;
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_9;  /* 握手信号 接收ack */
  GPIO_Initure.Mode = GPIO_MODE_INPUT;
  GPIO_Initure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_Initure);

  GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;  /* 拨码开关设置 */
  GPIO_Initure.Mode = GPIO_MODE_INPUT;
  GPIO_Initure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_Initure);
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
