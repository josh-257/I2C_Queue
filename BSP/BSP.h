/*
 * BSP.h
 *
 *  Created on: 7 Sept 2026
 *      Author: joshb
 */

#ifndef BSP_H_
#define BSP_H_

#include "Driver_GPIO.h"
#include "Driver_I2C.h"
#include "Driver_PWM.h"

#include <stdint.h>

//I2C setup - SCL (PB6), SDA (PB7)
#define I2C_SCL_PIN_NO         6
#define I2C_SDA_PIN_NO         7
#define I2C_GPIO_PORT          GPIOB
#define I2C_PIN_MODE           GPIO_MODE_ALTFN
#define I2C_PIN_OP_TYPE        GPIO_OP_TYPE_OD
#define I2C_PIN_PUPD_SETTING   GPIO_PIN_NOPUPD
#define I2C_PIN_ALT_FUN_MODE   GPIO_AF_AF4
#define I2C_PIN_SPEED          GPIO_SPEED_HIGH

#define I2C_PERI_ADDR          I2C1
#define I2C_ACKCONTROL         I2C_ACK_ENABLE
#define I2C_DEVICE_ADDR        0x61
#define I2C_SCL_SPEED          I2C_SCL_SPEED_SM

/*****************************************************
 * @brief   Initialises required peripheral clocks, GPIO
 *          pins, I2C and PWM peripherals.
 */
void BSP_init(void);


#endif /* BSP_H_ */
