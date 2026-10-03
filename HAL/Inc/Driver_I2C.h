/*
 * Driver_I2C.h
 *
 *  Created on: 11 Aug 2026
 *      Author: joshb
 */

#ifndef INC_DRIVER_I2C_H_
#define INC_DRIVER_I2C_H_

#include <stdio.h>
#include "stm32f407xx.h"
#include "stm32f4xx.h"

#define I2C_SR_DI 0
#define I2C_SR_EN 1

#define I2C_SCL_SPEED_SM       100000
#define I2C_SCL_SPEED_FM4K     400000
#define I2C_SCL_SPEED_FM2K     200000

#define I2C_ACK_ENABLE    1
#define I2C_ACK_DISABLE   0

//Macros for either 2:1 or 16:9 duty cycle (Only for fast mode)
#define I2C_FM_DUTY_2_1   0
#define I2C_FM_DUTY_16_9  1

//I2C Application States
#define I2C_READY         0
#define I2C_BUSY_IN_RX    1
#define I2C_BUSY_IN_TX    2

//I2C Application Events
#define I2C_EV_TX_CMPLT   0
#define I2C_EV_RX_CMPLT   1
#define I2C_EV_STOP       2
#define I2C_ERROR_BERR    3
#define I2C_ERROR_ARLO    4
#define I2C_ERROR_AF      5
#define I2C_ERROR_OVR     6
#define I2C_ERROR_TIMEOUT 7
#define I2C_EV_DATA_REQ   8
#define I2C_EV_DATA_RCV   9

//Configuration structure for I2C peripheral
typedef struct{
  uint32_t I2C_SCLSpeed;
  uint8_t I2C_DeviceAddress;
  uint8_t I2C_ACKControl;
  uint16_t I2C_FMDutyCycle;
}I2C_Config_t;

//Handle structure for I2C peripheral
typedef struct{
  I2C_TypeDef *pI2Cx;
  I2C_Config_t I2C_Config;
  uint8_t *pTxBuffer;
  uint8_t *pRxBuffer;
  uint32_t TxLen;
  uint32_t RxLen;
  uint8_t TxRxState;
  uint8_t DevAddr;
  uint32_t RxSize;
  uint8_t Sr;
}I2C_Handle_t;

/*******************************API's supported by this driver*******************************/

/***************************************************************************
 * @brief Configures I2C peripheral with user-defined I2C handle structure.
 * @param   pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @note  This function enables the peripheral clock, therefore no
 *      requirement to enable before calling function.
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);

/***************************************************************************
 * @brief Enables or Disables I2C peripheral.
 * @param   pI2Cx: Pointer to register definition structure
 *      containing memory address of chosen I2C peripheral.
 * @param EnorDi: ENABLE(1) or DISABLE(0) macro.
 */
void I2C_PeripheralControl(I2C_TypeDef *pI2Cx, uint8_t EnorDi);

/***************************************************************************
 * @brief Resets all registers in corresponding I2C peripheral.
 * @param   pI2Cx: Pointer to register definition structure
 *      containing memory address of chosen I2C peripheral.
 * @note  This function resets the physical peripheral registers but not
 *      the handle structure.
 */
void I2C_DeInit(I2C_TypeDef *pI2Cx);

/***************************************************************************
 * @brief Generates stop condition (SDA low to high while SCL high) by
 *      setting stop bit in CR1 to high.
 * @param   pI2Cx: Pointer to register definition structure
 *      containing memory address of chosen I2C peripheral.
 * @note  Stop condition will only be generated if device is in master mode.
 */
void I2C_GenerateStopCondition(I2C_TypeDef *pI2Cx);

/***************************************************************************
 * @brief Generates start condition (SDA high to low while SCL high) by
 *      setting start bit in CR1 to high.
 * @param   pI2Cx: Pointer to register definition structure
 *      containing memory address of chosen I2C peripheral.
 * @note  If device is in master mode and is already controlling the I2C
 *      bus then a repeated start condition is generated.
 */
void I2C_GenerateStartCondition(I2C_TypeDef *pI2Cx);


/***************************************************************************
 * @brief Sends a complete transmission over I2C.
 * @param   pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @param pTxBuffer: Pointer to the user data buffer
 *      holding the data to be transmitted.
 * @param len: Specifies the length (in bytes) of the transmission.
 * @param slave_address: 7 bit address of the chosen slave to send the
 *      transmission to.
 * @param Sr: Option for repeated start condition, I2C_SR_EN to enable and
 *      I2C_SR_DI to disable.
 * @note  This is a blocking function.
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, const uint8_t *pTxBuffer, uint32_t len, uint8_t slave_address, uint8_t Sr);

/***************************************************************************
 * @brief Receives data from slave device over I2C peripheral.
 * @param   pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @param   pRxBuffer: Pointer to the user receive buffer where the received
 *      data will land.
 * @param len: Specifies the length (in bytes) of the data to be received.
 * @param slave_address: 7 bit address of the chosen slave to read data from.
 * @param Sr: Option for repeated start condition, I2C_SR_EN to enable and
 *      I2C_SR_DI to disable.
 * @note  This is a blocking function.
 */
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t len, uint8_t slave_address, uint8_t Sr);

/***************************************************************************
 * @brief Initiates interrupt driven data transmission over I2C.
 * @param   pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @param   pTxBuffer: Pointer to the user data buffer that will be assigned
 *      to the handle structure.
 * @param len: Specifies the length (in bytes) of the data to be sent.
 * @param slave_address: 7 bit address of the chosen slave to send data to.
 * @param Sr: Option for repeated start condition, I2C_SR_EN to enable and
 *      I2C_SR_DI to disable.
 * @note  This is a non-blocking function.
 */
uint8_t  I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t len, uint8_t slaveAddr, uint8_t Sr);

/***************************************************************************
 * @brief Initiates interrupt driven reception of data from slave over I2C.
 * @param   pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @param   pRxBuffer: Pointer to the user data buffer that will be assigned
 *      to the handle structure.
 * @param len: Specifies the length (in bytes) of the data to be received.
 * @param slave_address: 7 bit address of the chosen slave to receive data
 *      from.
 * @param Sr: Option for repeated start condition, I2C_SR_EN to enable and
 *      I2C_SR_DI to disable.
 * @note  This is a non-blocking function.
 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint32_t len, uint8_t slaveAddr,uint8_t Sr);

/***************************************************************************
 * @brief Disables I2C EV and BUF interrupts, resets all fields of handle
 *      structure configured by I2C_MasterSendDataIT.
 * @param pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration.
 */
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle);

/***************************************************************************
 * @brief Disables I2C EV and BUF interrupts, resets all fields of handle
 *      structure configured by I2C_MasterSendDataIT.
 * @param pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration.
 */
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle);

/***************************************************************************
 * @brief Enables or disables the ack bit inside the CR1 register.
 * @param   pI2Cx: Pointer to register definition structure
 *      containing memory address of chosen I2C peripheral.
 * @param EnorDi: ENABLE(1) or DISABLE(0) macro.
 */
void I2C_ToggleAcking(I2C_TypeDef *pI2Cx, uint8_t EnorDi);

/***************************************************************************
 * @brief Handles all I2C event interrupts through checking the status
 *      flags in the SR registers, calls relevant functions for each
 *      interrupt and notifies application layer.
 * @param pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration and hardware configuration register data.
 */
void I2C_EVIRQHandling(I2C_Handle_t *pI2CHandle);

/***************************************************************************
  * @brief  Handles all I2C error interrupts by checking the status flags
  *     in the SR registers and clearing the relevant flag before
  *     notifying the application layer.
 * @param pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration and hardware configuration register data.
 */
void I2C_ERIRQHandling(I2C_Handle_t *pI2CHandle);

/***************************************************************************
 * @brief This function allow the ISR to inform the application. This
 *      function will be implemented by the application.
 * @param pI2CHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @param AppEv: Application event macro (eg. I2C_EV_TX_CMPLT).
 */
__WEAK void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);

#endif /* INC_DRIVER_I2C_H_ */
