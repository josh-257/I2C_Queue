/*
 * Driver_RCC.h
 *
 *  Created on: 13 Aug 2026
 *      Author: joshb
 */

#ifndef INC_DRIVER_RCC_H_
#define INC_DRIVER_RCC_H_

#include "stm32f407xx.h"

#define RCC_CLOCK_HSE   8000000
#define RCC_CLOCK_HSI   16000000

/***************************************************************************
 * @brief Returns the current clock speed of the APB1 bus.
 * @retval  Clock speed (in Hz) represented by 32 bit unsigned integer.
 */
uint32_t RCC_GetPeripheralClockAPB1Value(void);

/***************************************************************************
 * @brief Returns the current clock speed of the APB2 bus.
 * @retval  Clock speed (in Hz) represented by 32 bit unsigned integer.
 */
uint32_t RCC_GetPeripheralClockAPB2Value(void);


/*****************************************************
 * @brief   Initialises all peripheral clocks used.
 */
void Clock_Init(void);

#endif /* INC_DRIVER_RCC_H_ */
