/*
 * BSP.c
 *
 *  Created on: 7 Sept 2026
 *      Author: joshb
 */

#include "BSP.h"

//Private functions for BSP init
static void I2C1_GPIOInits(void);
static void Clock_init(void);
static void I2C1_inits(void);

static I2C_Handle_t I2CHandle;

void BSP_init(void)
{
    Clock_init();

    I2C1_GPIOInits();
    I2C1_inits();
}

static void I2C1_GPIOInits(void)
{
  GPIO_Handle_t I2CPins;

  I2CPins.pGPIOx = I2C_GPIO_PORT;
  I2CPins.GPIO_Config.PinMode = I2C_PIN_MODE;
  I2CPins.GPIO_Config.PinOPType = I2C_PIN_OP_TYPE;
  I2CPins.GPIO_Config.PinPuPdControl = I2C_PIN_PUPD_SETTING;
  I2CPins.GPIO_Config.AltFunMode = I2C_PIN_ALT_FUN_MODE;
  I2CPins.GPIO_Config.PinSpeed = I2C_PIN_SPEED;

  // SCL (PB6)
  I2CPins.GPIO_Config.PinNumber = I2C_SCL_PIN_NO;
  GPIO_Init(&I2CPins);

  // SDA (PB7)
  I2CPins.GPIO_Config.PinNumber = I2C_SDA_PIN_NO;
  GPIO_Init(&I2CPins);
}

static void Clock_init(void)
{
  GPIOD_PCLK_EN();
  GPIOB_PCLK_EN();
  GPIOA_PCLK_EN();
  SYSCFG_PCLK_EN();
  I2C1_PCLK_EN();
  TIM3_PCLK_EN();
}

static void I2C1_inits(void)
{
  I2CHandle.pI2Cx = I2C_PERI_ADDR;
  I2CHandle.I2C_Config.I2C_ACKControl = I2C_ACKCONTROL;
  I2CHandle.I2C_Config.I2C_DeviceAddress = I2C_DEVICE_ADDR;
  I2CHandle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED;

  I2C_Init(&I2CHandle);
  I2C_PeripheralControl(I2CHandle.pI2Cx, ENABLE);
}





