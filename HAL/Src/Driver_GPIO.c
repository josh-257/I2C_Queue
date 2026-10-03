/*
 * Driver_GPIO.c
 *
 *  Created on: 11 Aug 2026
 *      Author: joshb
 */


#include "Driver_GPIO.h"

void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
  uint32_t temp, shift_val;
  shift_val = pGPIOHandle->GPIO_Config.PinNumber * 2;

  //Configure mode
  temp = pGPIOHandle->GPIO_Config.PinMode << shift_val;
  pGPIOHandle->pGPIOx->MODER &= ~(0x3 << shift_val);
  pGPIOHandle->pGPIOx->MODER |= temp;

  //Configure speed
  temp = pGPIOHandle->GPIO_Config.PinSpeed << shift_val;
  pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << shift_val);
  pGPIOHandle->pGPIOx->OSPEEDR |= temp;

  //Configure pupd settings
  temp = pGPIOHandle->GPIO_Config.PinPuPdControl << shift_val;
  pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << shift_val);
  pGPIOHandle->pGPIOx->PUPDR |= temp;

  //Configure output type
  temp = pGPIOHandle->GPIO_Config.PinOPType << pGPIOHandle->GPIO_Config.PinNumber;
  pGPIOHandle->pGPIOx->OTYPER &= ~(0x3 << pGPIOHandle->GPIO_Config.PinNumber);
  pGPIOHandle->pGPIOx->OTYPER |= temp;

  //Configure alt functionality
  if(pGPIOHandle->GPIO_Config.PinMode == GPIO_MODE_ALTFN)
  {
    //Select appropriate alt fun reg (0-7 Alt fun low, 8-15 Alt fun high)
    uint8_t temp1 = pGPIOHandle->GPIO_Config.PinNumber / 8;
    uint8_t temp2 = pGPIOHandle->GPIO_Config.PinNumber % 8;

    pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xf << (4 * temp2));
    pGPIOHandle->pGPIOx->AFR[temp1] |= pGPIOHandle->GPIO_Config.AltFunMode << (4 * temp2);
  }
}

void GPIO_DeInit(GPIO_TypeDef *pGPIOx)
{
  if(pGPIOx == GPIOA)
  {
    GPIOA_REG_RESET();
  }
  else if (pGPIOx == GPIOB)
  {
    GPIOB_REG_RESET();
  }
  else if (pGPIOx == GPIOC)
  {
    GPIOC_REG_RESET();
  }
  else if (pGPIOx == GPIOD)
  {
    GPIOD_REG_RESET();
  }
  else if (pGPIOx == GPIOE)
  {
    GPIOE_REG_RESET();
  }
  else if (pGPIOx == GPIOF)
  {
    GPIOF_REG_RESET();
  }
  else if (pGPIOx == GPIOG)
  {
    GPIOG_REG_RESET();
  }
  else if (pGPIOx == GPIOH)
  {
    GPIOH_REG_RESET();
  }
  else if (pGPIOx == GPIOI)
  {
    GPIOI_REG_RESET();
  }
}

uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber)
{
  return (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);
}

uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef *pGPIOx)
{
  return (uint16_t)pGPIOx->IDR;
}

void GPIO_WriteToOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber, uint8_t value)
{
  if(value == SET)
  {
    pGPIOx->ODR |= (1 << PinNumber);
  }
  else
  {
    pGPIOx->ODR &= ~(1 << PinNumber);
  }
}

void GPIO_WriteToOutputPort(GPIO_TypeDef *pGPIOx, uint16_t value)
{
  pGPIOx->ODR = value;
}

void GPIO_ToggleOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber)
{
  pGPIOx->ODR ^= (1 << PinNumber);
}

