/*
 * Driver_PWM.c
 *
 *  Created on: 2 Sept 2026
 *      Author: joshb
 */

#ifndef INC_DRIVER_PWM_C_
#define INC_DRIVER_PWM_C_
#include "stm32f407xx.h"

#define PWM_MODE_1 6U
#define ARR_VAL 639U
#define PRESCALE_VAL 0U

void PWM_Init(void);
void PWM_SetDutyCycle(uint8_t value);

#endif /* INC_DRIVER_PWM_C_ */
