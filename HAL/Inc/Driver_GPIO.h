/*
 * Driver_GPIO.h
 *
 *  Created on: 11 Aug 2026
 *      Author: joshb
 */

#ifndef INC_DRIVER_GPIO_H_
#define INC_DRIVER_GPIO_H_

#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f4xx.h"

//Configuration structure for GPIO pin
typedef struct{
  uint8_t PinNumber;
  uint8_t PinMode;
  uint8_t PinSpeed;
  uint8_t PinPuPdControl;
  uint8_t PinOPType;
  uint8_t AltFunMode;
}GPIO_PinConfig_t;

//Handle structure for GPIO pin
typedef struct{
  GPIO_TypeDef *pGPIOx; //Holds base address of port to which the pin belongs to
  GPIO_PinConfig_t GPIO_Config; // Holds GPIO config settings
}GPIO_Handle_t;


//Pin modes
#define GPIO_MODE_IN          0
#define GPIO_MODE_OUT         1
#define GPIO_MODE_ALTFN       2
#define GPIO_MODE_ANALOG      3

//Pin output types
#define GPIO_OP_TYPE_PP     0
#define GPIO_OP_TYPE_OD     1

#define GPIO_SPEED_LOW      0
#define GPIO_SPEED_MED      1
#define GPIO_SPEED_HIGH     2
#define GPIO_SPEED_VHIGH    3

//Pull up/pull down settings
#define GPIO_PIN_NOPUPD     0
#define GPIO_PIN_PU         1
#define GPIO_PIN_PD         2

//Alternate function modes
#define GPIO_AF_AF0         0
#define GPIO_AF_AF1         1
#define GPIO_AF_AF2         2
#define GPIO_AF_AF3         3
#define GPIO_AF_AF4         4
#define GPIO_AF_AF5         5
#define GPIO_AF_AF6         6
#define GPIO_AF_AF7         7
#define GPIO_AF_AF8         8
#define GPIO_AF_AF9         9
#define GPIO_AF_AF10        10
#define GPIO_AF_AF11        11
#define GPIO_AF_AF12        12
#define GPIO_AF_AF13        13
#define GPIO_AF_AF14        14
#define GPIO_AF_AF15        15


/*******************************API's supported by this driver*******************************/

/***************************************************************************
 * @brief   Configures GPIO pin with user-defined GPIO handle structure.
 * @param   pGPIOHandle: Pointer to handle structure containing user-defined
 *          configuration.
 */
void GPIO_Init(GPIO_Handle_t *pGPIOx);

/***************************************************************************
 * @brief   Resets all registers in corresponding GPIO port.
 * @param   pGPIOHandle: Pointer to handle structure containing user-defined
 *          configuration.
 * @note    This function resets the physical port but not the handle
 *          structure.
 */
void GPIO_DeInit(GPIO_TypeDef *pGPIOx);

/***************************************************************************
 * @brief   Returns the current value held in the input data register of
 *          the specified GPIO pin.
 * @param   pGPIOx: Pointer to register definition structure
 *          containing memory address of chosen GPIO port.
 * @param   PinNumber: 0 - 15.
 * @retval  The value of the input data register.
 */
uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber);

/***************************************************************************
 * @brief   Returns the current values held in the input data register of
 *          the specified GPIO port.
 * @param   pGPIOx: Pointer to register definition structure
 *          containing memory address of chosen GPIO port.
 * @retval  The value of the input data register across
 *          all pins for specific port.
 */
uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef *pGPIOx);

/***************************************************************************
 * @brief   Writes specified value (either SET or RESET) to chosen GPIO pin.
 * @param   pGPIOx: Pointer to register definition structure
 *          containing memory address of chosen GPIO port.
 * @param   PinNumber: 0 - 15.
 * @param   value: Either SET(1) or RESET(0).
 */
void GPIO_WriteToOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber, uint8_t value);

/***************************************************************************
 * @brief   Writes specified 16 bit value to set and reset
 *          register of chosen GPIO port.
 * @param   pGPIOx: Pointer to register definition structure
 *          containing memory address of chosen GPIO port.
 * @param   value: Holding set values (0 - 8 bits) and
 *          reset values (bits 8 - 15) for each pin of the GPIO port.
 */
void GPIO_WriteToOutputPort(GPIO_TypeDef *pGPIOx, uint16_t value);

/***************************************************************************
 * @brief   Toggles state of specified GPIO pin.
 * @param   pGPIOx: Pointer to register definition structure
 *          containing memory address of chosen GPIO port.
 * @param   PinNumber: 0 - 15.
 */
void GPIO_ToggleOutputPin(GPIO_TypeDef *pGPIOx, uint8_t PinNumber);


#endif /* INC_DRIVER_GPIO_H_ */
