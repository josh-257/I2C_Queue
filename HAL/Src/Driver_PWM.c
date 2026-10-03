/*
 * Driver_PWM.c
 *
 *  Created on: 2 Sept 2026
 *      Author: joshb
 */
#include "Driver_PWM.h"

void PWM_Init(void)
{
  //Initialisation for TIM3 channel 3
  //Program ARR register
  TIM3->ARR = ARR_VAL;

  //Select PWM output mode
  TIM3->CCMR2 |= (PWM_MODE_1 << TIM_CCMR2_OC3M_Pos);

  //Set duty cycle
  TIM3->CCR3 = TIM3->ARR / 8;

  //Enable preload delay
  TIM3->CCMR2 |= TIM_CCMR2_OC3CE;

  //Set prescaler value
  TIM3->PSC |= PRESCALE_VAL;

  //Enable channel 3
  TIM3->CCER |= TIM_CCER_CC3E;

  //Start timer
  TIM3->CR1 |= TIM_CR1_CEN;
}

void PWM_SetDutyCycle(uint8_t value)
{
  TIM3->CCR3 = ((uint32_t)value * TIM3->ARR)/100;
}
