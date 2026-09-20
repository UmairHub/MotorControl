#include "main.h"

void GPIO_Init(void);
void Timer3_PWM_Init(void);
void Motor_Forward(uint16_t speed);
void Motor_Reverse(uint16_t speed);
void Motor_Stop(void);
void delay(int ms);


#define PWM_MAX 999   // matches ARR

int main(void)
{
   GPIO_Init();
   Timer3_PWM_Init();

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
    }

}

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
    TIM3->PSC = 89;
    TIM3->ARR = PWM_MAX;
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


