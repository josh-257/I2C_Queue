/*
 * stm32f407xx_driver_SPI.h
 *
 *  Created on: 15 May 2026
 *      Author: joshb
 */

#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include "stm32f407xx.h"
#include "stm32f4xx.h"

//Configuration structure for SPI peripheral
typedef struct {
  uint8_t SPI_DeviceMode;
  uint8_t SPI_BusConfig;
  uint8_t SPI_SclkSpeed;
  uint8_t SPI_DFF;
  uint8_t SPI_CPOL;
  uint8_t SPI_CPHA;
  uint8_t SPI_SSM;
}SPI_Config_t;

//Handle structure for SPI peripheral
typedef struct{
  SPI_TypeDef *pSPIx;
  SPI_Config_t SPIConfig;
  uint8_t    *pTxBuffer;
  uint8_t    *pRxBuffer;
  uint32_t   Txlen;
  uint32_t   Rxlen;
  uint8_t    TxState;
  uint8_t    RxState;
}SPI_Handle_t;

/*************************SPI configuration macros******************************/


#define SPI_MODE_SLAVE          0
#define SPI_MODE_MASTER         1

#define SPI_BUS_CFG_FD          1
#define SPI_BUS_CFG_HD          2
#define SPI_BUS_CFG_SIMPLEX_RXONLY    3


#define SPI_SCLK_SPEED_DIV2       0
#define SPI_SCLK_SPEED_DIV4       1
#define SPI_SCLK_SPEED_DIV8       2
#define SPI_SCLK_SPEED_DIV16      3
#define SPI_SCLK_SPEED_DIV32      4
#define SPI_SCLK_SPEED_DIV64      5
#define SPI_SCLK_SPEED_DIV128     6
#define SPI_SCLK_SPEED_DIV256     7


#define SPI_DFF_8           0
#define SPI_DFF_16            1

#define SPI_CPOL_HIGH         1
#define SPI_CPOL_LOW          0

#define SPI_CPHA_HIGH         1
#define SPI_CPHA_LOW          0

#define SPI_SSM_EN            1
#define SPI_SSM_DI            0

//SPI Status Flags
#define SPI_FLAG_RXNE      (1 << 0)
#define SPI_FLAG_TXE       (1 << 1)
#define SPI_FLAG_CHSIDE    (1 << 2)
#define SPI_FLAG_UDR       (1 << 3)
#define SPI_FLAG_CRCERR    (1 << 4)
#define SPI_FLAG_MODF      (1 << 5)
#define SPI_FLAG_OVR       (1 << 6)
#define SPI_FLAG_BSY       (1 << 7)


//SPI application states
#define SPI_READY           0
#define SPI_BUSY_IN_RX          1
#define SPI_BUSY_IN_TX          2

//Possible SPI Application events
#define SPI_EVENT_TX_CMPLT        1
#define SPI_EVENT_RX_CMPLT        2
#define SPI_EVENT_OVR_ERR         3
#define SPI_EVENT_CRC_ERR         4

/*******************************API's supported by this driver*******************************/

/***************************************************************************
 * @brief Enables or disables peripheral clock for selected SPI peripheral.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 * @param EnorDi: ENABLE(1) or DISABLE(0) macro.
 */
void SPI_PeripheralClockControl(SPI_TypeDef *pSPIx, uint8_t EnorDi);

/***************************************************************************
 * @brief Configures SPI peripheral with user-defined SPI handle structure.
 * @param   pSPIHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @note  This function enables the peripheral clock, therefore no
 *      requirement to enable before calling function.
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);

/***************************************************************************
 * @brief Resets all registers in corresponding SPI peripheral.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 * @note  This function resets the physical peripheral registers but not
 *      the handle structure.
 */
void SPI_DeInit(SPI_TypeDef *pSPIx);

/***************************************************************************
 * @brief Transmits data (len number of bytes) over SPI.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 * @param pTxBuffer: Pointer to the user data buffer
 *      holding the data to be transmitted.
 * @param len: Specifies the length (in bytes) of the transmission.
 * @note  This is a blocking function.
 */
void SPI_Send(SPI_TypeDef *pSPIx, uint8_t *pTxBuffer, uint32_t len);

/***************************************************************************
 * @brief Receives data (len number of bytes) from slave device over SPI.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 * @param   pRxBuffer: Pointer to the user receive buffer where the received
 *      data will land.
 * @param len: Specifies the length (in bytes) of the data to be received.
 * @note  This is a blocking function.
 */
void SPI_Receive(SPI_TypeDef *pSPIx, uint8_t *pRxBuffer, uint32_t len);


/***************************************************************************
 * @brief Initiates interrupt driven data transmission over SPI by
 *      enabling TXEIE bit.
 * @param   pSPIHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @param   pTxBuffer: Pointer to the user data buffer that will be assigned
 *      to the handle structure.
 * @param len: Specifies the length (in bytes) of the data to be sent.
 * @retval  Txstate that will be assigned to the handle structure indicating
 *      that the transmission is in progress.
 * @note  This is a non-blocking function.
 */
uint8_t SPI_SendIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t len);

/***************************************************************************
 * @brief Initiates interrupt driven data reception over SPI by
 *      enabling RXNEIE bit.
 * @param   pSPIHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @param   pRxBuffer: Pointer to the user data buffer where the data will
 *      land.
 * @param len: Specifies the length (in bytes) of the data to be received.
 * @retval  Rxstate that will be assigned to the handle structure
 *      indicating that the reception is in progress.
 * @note  This is a non-blocking function.
 */
uint8_t SPI_ReceiveIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t len);

void SPI_IRQITConfig(uint8_t IRQ_Number, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQ_Number, uint32_t IRQPriority);

/***************************************************************************
 * @brief Handles SPI interrupts for transmission, receive and over run
 *      errors through checking the status flags in the SR registers,
 *      calls relevant functions for each interrupt and notifies
 *      application layer.
 * @param pSPIHandle: Pointer to handle structure containing user-defined
 *      configuration and hardware configuration register data.
 * @note  Not configured to handle CRC and TI frame format error interrupts.
 */
void SPI_IRQHandling(SPI_Handle_t *pHandle);

/***************************************************************************
 * @brief Enables or Disables SPI peripheral.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 * @param EnorDi: ENABLE(1) or DISABLE(0) macro.
 */
void SPI_PeripheralControl(SPI_TypeDef *pSPIx, uint8_t EnOrDi);

/***************************************************************************
 * @brief Enables or Disables SSI bit inside CR1 register.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 * @param EnorDi: ENABLE(1) or DISABLE(0) macro.
 * @note  The SSI should only be configured if SSM (software slave
 *      management) is set, as SSI substitutes NSS pin.
 */
void SPI_SSIConfig(SPI_TypeDef *pSPIx, uint8_t EnOrDi);

/***************************************************************************
 * @brief Enables or disables SSOE bit in CR2 register.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 * @param EnorDi: ENABLE(1) or DISABLE(0) macro.
 * @note  SSOE bit should only be configured in hardware mode (SSM = 0).
 *      SSOE bit should only be set when using one master and one slave only.
 */
void SPI_SSOEConfig(SPI_TypeDef *pSPIx, uint8_t EnOrDi);

/***************************************************************************
 * @brief Returns the status of specified flag inside the SR1 register.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 * @param FlagName: Macro representing chosen flag of SR1 register to
 *      be checked. (eg. SPI_FLAG_TXE).
 * @note  Must use pre shifted flag macros provided in this header file.
 */
uint8_t SPI_GetFlagStatus(SPI_TypeDef *pSPIx, uint32_t FlagName);

/***************************************************************************
 * @brief Clears over run error flag by reading both DR and SR.
 * @param   pSPIx: Pointer to register definition structure
 *      containing memory address of chosen SPI peripheral.
 */
void SPI_ClearOVRFlag(SPI_TypeDef *pSPIx);

/***************************************************************************
 * @brief Shuts off SPI TXEIE interrupt and resets SPI transmission related
 *      fields in SPI handle (Txlen, Txbuffer and TxState).
 * @param pSPIHandle: Pointer to handle structure containing user-defined
 *      configuration.
 */
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);

/***************************************************************************
 * @brief Shuts off SPI RXNEIE interrupt and resets SPI transmission related
 *      fields in SPI handle (Rxlen, Rxbuffer and RxState).
 * @param pSPIHandle: Pointer to handle structure containing user-defined
 *      configuration.
 */
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);

/***************************************************************************
 * @brief This function allow the ISR to inform the application. This
 *      function will be implemented by the application.
 * @param pSPIHandle: Pointer to handle structure containing user-defined
 *      configuration.
 * @param AppEv: Application event macro (eg. SPI_EVENT_TX_CMPLT).
 */
__WEAK void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);



#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
