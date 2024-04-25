/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "cmsis_os.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STACK_SIZE 256

#define TASK1_PRIORITY 1
#define TASK2_PRIORITY 2

#define TASK1_DELAY 1
#define TASK2_DELAY 2

int leddelay = 1;
int spamdelay = 1;

QueueHandle_t QueueHandle;
UBaseType_t uxQueueLength = 10, uxItemSize = sizeof(int);
TaskHandle_t handle_blink_led, handle_echo_uart, handle_givetask, handle_taketask, handle_shell,handle_spam,handle_bidon;

SemaphoreHandle_t timeMutex;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		BaseType_t xHigherPriorityTaskWoken; //
		vTaskNotifyGiveFromISR( handle_shell, &xHigherPriorityTaskWoken );

		portYIELD_FROM_ISR( xHigherPriorityTaskWoken ); //
	}

}

void task1(void * pvParameters)
{
	int delay = (int) pvParameters;

	for(;;)
	{
		if (xSemaphoreTake(timeMutex, portMAX_DELAY) == pdTRUE){
			printf("Je suis la tache 1 et je m'endors pour %d ticks\r\n", delay);
			xSemaphoreGive(timeMutex);
		}
		vTaskDelay(delay);
	}
}

void task2(void * pvParameters)
{
	int delay = (int) pvParameters;

	for(;;)
	{
		if (xSemaphoreTake(timeMutex, portMAX_DELAY) == pdTRUE){
			printf("Je suis la tache 2 et je m'endors pour %d ticks\r\n", delay);
			xSemaphoreGive(timeMutex);

		}
		vTaskDelay(delay);
	}
}

/*void task_blink_led(void * pvParameters)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOI,LED1_Pin);
		vTaskDelay(leddelay);
	}
}
*/

/*void task_spam(void * pvParameters)
{
	for(;;)
	{
		if (spamdelay != 0)
		{

			printf("%s\r\n",msg);
			vTaskDelay(spamdelay);
		}

		else
		{
			vTaskSuspend(handle_spam);
		}
	}
}
*/

void task_bidon(void * pvParameters)
{
	for(;;)
	{

	}

}

void task_give(void * unused)
{
	int q_value_send;
	for(;;)
	{
		for (int i = 0; i < 20; i++)
				{
					q_value_send = i;
					xQueueSend(QueueHandle, &q_value_send ,portMAX_DELAY);
					vTaskDelay(portTICK_PERIOD_MS*100*i);
				}
	}
}

void task_take(void * unused)
{
	int q_value_receive;
	BaseType_t ret_q;
	for(;;)
	{
		ret_q = xQueueReceive(QueueHandle, &q_value_receive ,1000);
	}
}

int fonction(int argc, char ** argv)
{
	printf("argc = %d\r\n", argc);

	for(int itr = 0; itr < argc; itr++)
	{
		printf("argv[%d] = %s\r\n", itr, argv[itr]);
	}

	return 0;
}

int led(int argc, char ** argv)
{
	if(argc != 2)
	{
		printf("Error\r\n");
		return -1;
	}
	else
	{
		leddelay = atoi(argv[1]);
	}

	return 0;
}

int spam(int argc, char ** argv)
{
	if(argc != 2)
	{
		printf("Error\r\n");
		return -1;
	}
	else
	{
		for(int i = 0; i <= 10; i++)
		{
			printf("%s\r\n",argv[1]);
		}
	}

	return 0;
}

void configureTImerForRunTimeStats(void)
{
	HAL_TIM_Base_Start(&htim1);
}

unsigned long getRunTimeCounterValue(void)
{
	//return __HAL_TIM_GET_COUNTER(&htim1);
	unsigned long time = __HAL_TIM_GET_COUNTER(&htim1);
	//printf("Run Time Counter Value: %lu\r\n", time);
	return 0;
}

int show_runtime_stats(int argc, char **argv) {
    char buffer[512];
    vTaskGetRunTimeStats(buffer);
    printf("Run Time Stats:\r\n%s\r\n", buffer);
    return 0;
}

int show_task_list(int argc, char **argv) {
    char buffer[512];
    vTaskList(buffer);
    printf("Task List:\r\n%s\r\n", buffer);
    return 0;
}

int write_read_adx(int argc, char ** argv)
{
	unsigned char write, read = (1<<7) + 0 + 0;

	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2,&write,1,HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi2,&read,1,HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);

	if (read == 0xE5)
		printf("C'est connecté\r\n");

	else
		printf("Erreur\r\n");

	return 0;
}


int addition (int argc, char ** argv)
{
	//récupérer deux paramètre ni plus ni moins
	//convertir les deux paramètres en nombres entier
	//les additioners et afficher le résultat

	if(argc != 3)
	{
		//faire planter proprement si on a plus de deux paramètre
		printf("Error, excpected 2 argument\r\n");
		return -1;
	}
	else
	{
		int a,b, result;
		// permet de récupérer les deux paramètres et de les convertir en entier
		a = atoi(argv[1]);
		b = atoi(argv[2]);

		result = a+b;
		printf("%d + %d = %d\r\n", a, b, result);
	}

	return 0;
}

int vApplicationStackOverflowHook(TaskHandle_t xTask,unsigned char *pcTaskName)
{
	printf("Overflow task %s\r\n", pcTaskName);
}

void shell(void * unused)
{
	shell_init();
	shell_add('l', led, "Blink");
	shell_add('a', addition, "Add 2 value");
	shell_add('s', spam, "spam");
	//shell_add('t',getRunTimeCounterValue,"time");
	//shell_add('r',show_runtime_stats,"runtime");
	//shell_add('T',show_task_list,"task");
	//shell_add('w', write_read_adx, "write read adx");
	shell_run();
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  BaseType_t ret;
  TaskHandle_t h_task1 = NULL;
  TaskHandle_t h_task2 = NULL;

	QueueHandle =  xQueueCreate( uxQueueLength, uxItemSize);

	timeMutex = xSemaphoreCreateMutex();

	/* Create the task, storing the handle. */
	/*ret = xTaskCreate(task1, "Tache 1", STACK_SIZE, (void *) TASK1_DELAY, TASK1_PRIORITY, &h_task1);
	configASSERT(pdPASS == ret);
	ret = xTaskCreate(task2, "Tache 2", STACK_SIZE, (void *) TASK2_DELAY, TASK2_PRIORITY, &h_task2);
	configASSERT(pdPASS == ret);/*

	/*ret = xTaskCreate(task_give,"Give",256,NULL,6,&handle_givetask);
	configASSERT(pdPASS == ret);

	ret = xTaskCreate(task_take,"Take",256,NULL,7,&handle_taketask);
	configASSERT(pdPASS == ret);*/

	//ret = xTaskCreate(task_blink_led,"Blink",256,NULL,5,&handle_blink_led);
	configASSERT(pdPASS == ret);

	if(ret != pdPASS)
	{
		printf("Error led\r\n");
		Error_Handler();
	}

	/*for(int i = 0;i<50;i++){
		if (ret == pdPASS){
			ret = xTaskCreate(task_bidon,"Bidon",256,NULL,11,&handle_bidon);
			printf("task %d created\r\n",i);
		}
		else
		{
			printf("Cant create task\r\n");
			Error_Handler();
		}
	}*/

	ret = xTaskCreate(shell,"Shell",256,NULL,10,&handle_shell);
	configASSERT(pdPASS == ret);

	if(ret != pdPASS)
		{
			printf("Error shell\r\n");
			Error_Handler();
		}


	vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
