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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void GPIO_Init(void);
void Timer3_PWM_Init(void);
void Motor_Forward(uint16_t speed);
void Motor_Reverse(uint16_t speed);
void Motor_Stop(void);
void delay(int ms);

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void GPIO_Init(void);
void Timer3_PWM_Init(void);
void Motor_Forward(uint16_t speed);
void Motor_Reverse(uint16_t speed);
void Motor_Stop(void);
void delay(int ms);
/* USER CODE END PTD */
#define PWM_MAX 999   // matches ARR

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/


/* USER CODE BEGIN PFP */

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
    GPIO_Init();
    Timer3_PWM_Init();
  /* USER CODE END 1 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      for (int i = 0; i <= PWM_MAX; i += 10)
      {
    	  Motor_Forward(i); delay(5);
      }
      delay(500);
      for (int i = PWM_MAX; i >= 0; i -= 10) { Motor_Forward(i); delay(5); }
      Motor_Stop();
      delay(1000);

      for (int i = 0; i <= PWM_MAX; i += 10) { Motor_Reverse(i); delay(5); }
      delay(500);
      for (int i = PWM_MAX; i >= 0; i -= 10) { Motor_Reverse(i); delay(5); }
      Motor_Stop();
      delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}



/* USER CODE BEGIN 4 */
void GPIO_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // enable GPIOA clock

    // PA6 -> Alternate Function mode (for TIM3_CH1)
    GPIOA->MODER &= ~(3 << (6 * 2));
    GPIOA->MODER |=  (2 << (6 * 2));       // 10 = Alternate Function
    GPIOA->AFR[0] &= ~(0xF << (6 * 4));
    GPIOA->AFR[0] |=  (2   << (6 * 4));    // AF2 = TIM3 on PA6

    // PA8, PA9 -> General purpose output (IN1, IN2)
    GPIOA->MODER &= ~(3 << (8 * 2));
    GPIOA->MODER |=  (1 << (8 * 2));       // 01 = Output
    GPIOA->MODER &= ~(3 << (9 * 2));
    GPIOA->MODER |=  (1 << (9 * 2));       // 01 = Output
}

void Timer3_PWM_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;    // enable TIM3 clock

    TIM3->PSC = 89;      // 90MHz / (89+1) = 1MHz counter clock
    TIM3->ARR = PWM_MAX; // 1MHz / (999+1) = 1kHz PWM frequency
    TIM3->CNT = 0;

    // Channel 1: PWM mode 1 (110), enable preload
    TIM3->CCMR1 &= ~(7 << 4);
    TIM3->CCMR1 |=  (6 << 4);   // OC1M = 110 (PWM mode 1)
    TIM3->CCMR1 |=  (1 << 3);   // OC1PE = 1 (preload enable)

    TIM3->CCER |= (1 << 0);    // CC1E = 1, enable channel 1 output

    TIM3->CCR1 = 0;             // start at 0% duty cycle

    TIM3->CR1 |= (1 << 7);      // ARPE = 1, auto-reload preload enable
    TIM3->CR1 |= (1 << 0);      // CEN = 1, enable counter
}

void Motor_Forward(uint16_t speed)
{
    GPIOA->BSRR = (1 << 8);         // IN1 (PA8) = HIGH
    GPIOA->BSRR = (1 << (9 + 16));  // IN2 (PA9) = LOW
    TIM3->CCR1 = speed;
}

void Motor_Reverse(uint16_t speed)
{
    GPIOA->BSRR = (1 << (8 + 16));  // IN1 (PA8) = LOW
    GPIOA->BSRR = (1 << 9);         // IN2 (PA9) = HIGH
    TIM3->CCR1 = speed;
}

void Motor_Stop(void)
{
    GPIOA->BSRR = (1 << (8 + 16));  // IN1 = LOW
    GPIOA->BSRR = (1 << (9 + 16));  // IN2 = LOW
    TIM3->CCR1 = 0;
}

void delay(int ms)
{
    for (; ms > 0; ms--)
        for (volatile int i = 0; i < 3195; i++);
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
