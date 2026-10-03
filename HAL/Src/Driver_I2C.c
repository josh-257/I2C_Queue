/*
 * Driver_I2C.c
 *
 *  Created on: 11 Aug 2026
 *      Author: joshb
 */

#include "Driver_I2C.h"

#include "Driver_RCC.h"


//Private helper functions
static void I2C_ExecuteAddressPhaseWrite(I2C_TypeDef *pI2Cx, uint8_t Slave_Addr);
static void I2C_ExecuteAddressPhaseRead(I2C_TypeDef *pI2Cx, uint8_t Slave_Addr);
static void I2C_clearFrameADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);

void I2C_GenerateStartCondition(I2C_TypeDef *pI2Cx)
{
  //generate start condition
  pI2Cx->CR1 |= I2C_CR1_START;

}

static void I2C_ExecuteAddressPhaseWrite(I2C_TypeDef *pI2Cx, uint8_t Slave_Addr)
{
  Slave_Addr <<= 1;
  Slave_Addr &= ~(1); //set r/w bit to write
  pI2Cx->DR = Slave_Addr;
}

static void I2C_ExecuteAddressPhaseRead(I2C_TypeDef *pI2Cx, uint8_t Slave_Addr)
{
  Slave_Addr <<= 1;
  Slave_Addr |= 1; //set r/w bit to read
  pI2Cx->DR = Slave_Addr;
}


static void I2C_clearFrameADDRFlag(I2C_Handle_t *pI2CHandle){

  uint32_t dummy_read;

  if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
  {

    if(pI2CHandle->RxSize == 1)
    {
      I2C_ToggleAcking(pI2CHandle->pI2Cx, DISABLE);
    }
  }

  dummy_read = pI2CHandle->pI2Cx->SR1;
  dummy_read = pI2CHandle->pI2Cx->SR2;
  (void)dummy_read;
}

void I2C_GenerateStopCondition(I2C_TypeDef *pI2Cx)
{
  //generate stop condition
  pI2Cx->CR1 |= I2C_CR1_STOP;
}

void I2C_PeripheralControl(I2C_TypeDef *pI2Cx, uint8_t EnorDi)
{
  if(EnorDi == ENABLE)
  {
    pI2Cx->CR1 |= I2C_CR1_PE;
  }
  else
  {
    pI2Cx->CR1 &= ~I2C_CR1_PE;
  }
}

void I2C_Init(I2C_Handle_t *pI2CHandle)
{
  uint32_t tempreg;

  //Configure speed
  tempreg = 0;
  tempreg |= RCC_GetPeripheralClockAPB1Value() / 1000000U;
  pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3F);

  //CCR calculations
  uint16_t ccr_value = 0;
  tempreg = 0;
  if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
  {
    //mode is standard mode
    ccr_value = RCC_GetPeripheralClockAPB1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed);
    tempreg |= ccr_value & 0xFFF;

  }
  else
  {
    //mode is fast mode
    tempreg |= (1 << 15);
    tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << 14);
    if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2_1)
    {
      ccr_value = (RCC_GetPeripheralClockAPB1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
    }
    else
    {
      ccr_value = (RCC_GetPeripheralClockAPB1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
    }
    tempreg |= (ccr_value & 0xFFF);
  }
  pI2CHandle->pI2Cx->CCR = tempreg;

  //Configure rise time
  //5. Configure rise time
  if(pI2CHandle->I2C_Config.I2C_SCLSpeed<= I2C_SCL_SPEED_SM)
  {
    //mode is standard mode
    tempreg = (RCC_GetPeripheralClockAPB1Value() / 1000000U) + 1;
  }
  else
  {
    //mode is fast mode
    tempreg = ((RCC_GetPeripheralClockAPB1Value() * 300) / 1000000000U) + 1;
  }

  pI2CHandle->pI2Cx->TRISE = tempreg & 0x3F;
}

void I2C_DeInit(I2C_TypeDef *pI2Cx)
{
  if(pI2Cx == I2C1)
  {
    I2C1_REG_RESET();
  }
  else if(pI2Cx == I2C2)
  {
    I2C2_REG_RESET();
  }
  else if(pI2Cx == I2C3)
  {
    I2C3_REG_RESET();
  }
}

uint8_t I2C_GetFlagStatus(I2C_TypeDef *pI2Cx, uint32_t FlagName)
{

  if(pI2Cx->SR1 & FlagName)
  {
    return SET;
  }
  return RESET;
}


void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, const uint8_t *pTxBuffer, uint32_t len, uint8_t slave_address, uint8_t Sr)
{
  I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

  //Confirm that start generation is completed by checking the SB flag in the SR1
  //Note: until SB is clear SCL will be stretched (pulled LOW)
  while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_SB));

  //Send the address of the slave with r/w bit set to w(0) (total 8 bits)
  I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, slave_address);

  //Confirm that the address phase is completed by checking the ADDR flag in SR1
  while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_ADDR));

  //clear the ADDR flag according to its software sequence
  //Note: Until ADDR is clear SCL will be stretched (pulled to LOW)
  I2C_clearFrameADDRFlag(pI2CHandle);

  //Send data until len is 0
  while(len > 0)
  {
    while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_TXE)); //Wait until TXE bit set
    pI2CHandle->pI2Cx->DR = *pTxBuffer;
    pTxBuffer++;
    len--;
  }

  //When len becomes zero wait for TXE = 1 and BTF = 1 before generating the STOP condition
  //Note: TXE = 1, BTF = 1, means that both SR and DR are empty and next transmission should begin
  //when BTF = 1SCL will be stretched (pulled to LOW)
  while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_TXE));
  while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_BTF));

  //Generate stop condition
  if(Sr == I2C_SR_DI)
  {
    I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
  }
}

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t len, uint8_t slave_address, uint8_t Sr)
{

  I2C_ToggleAcking(pI2CHandle->pI2Cx, ENABLE);
  //Generate start condition
  I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

  //Confirm that start generation is completed by checking the SB flag in the SR1
  while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_SB));

  //Send the address of the slave with r/nw bit set to R(1) (total 8 bits)
  I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, slave_address);

  //Wait until address phase is completed by checking the ADDR flag in SR1
  while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_ADDR));

  //Procedure to read only 1 byte from slave
  if(len == 1)
  {
    //Disable acking
    I2C_ToggleAcking(pI2CHandle->pI2Cx, DISABLE);

    //clearFrame the ADDR flag
    I2C_clearFrameADDRFlag(pI2CHandle);

    //Wait until RXNE becomes 1
    while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_RXNE));

    //Generate STOP condition
    if(Sr == I2C_SR_DI)
    {
        I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
    }

    //Read data into buffer
    *pRxBuffer = pI2CHandle->pI2Cx->DR;
  }

  if(len > 1){

    //clearFrame the ADDR flag
    I2C_clearFrameADDRFlag(pI2CHandle);

    //Read the data until len becomes zero
    for(uint32_t i = len; i >0; i--)
    {
      //Wait until RXNE becomes 1
      while(!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_SR1_RXNE));

      if(i == 2)
      {
        //Disable acking
        I2C_ToggleAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

        //Generate STOP condition
        if(Sr == I2C_SR_DI)
        {
            I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
        }
      }

      //Read the data from the data register into the buffer
      *pRxBuffer = pI2CHandle->pI2Cx->DR;

      //Increment the buffer address
      pRxBuffer++;
    }
  }
  //Re-enable acking
  if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
  {
    I2C_ToggleAcking(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);
  }
}


void I2C_ToggleAcking(I2C_TypeDef *pI2Cx, uint8_t EnorDi)
{
  if(EnorDi == I2C_ACK_ENABLE){

    pI2Cx->CR1 |= I2C_CR1_ACK;
  }
  else{

    pI2Cx->CR1 &= ~I2C_CR1_ACK;
  }
}

uint8_t  I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t len, uint8_t slaveAddr, uint8_t Sr)
{

  uint8_t busystate = pI2CHandle->TxRxState;

  if((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
  {
    pI2CHandle->pTxBuffer = pTxBuffer;
    pI2CHandle->TxLen = len;
    pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
    pI2CHandle->DevAddr = slaveAddr;
    pI2CHandle->Sr = Sr;

    //Implement code to Generate START Condition
    I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

    //Implement the code to enable ITBUFEN Control Bit
    pI2CHandle->pI2Cx->CR2 |= I2C_CR2_ITBUFEN;

    //Implement the code to enable ITEVTEN Control Bit
    pI2CHandle->pI2Cx->CR2 |= I2C_CR2_ITEVTEN;

    //Implement the code to enable ITERREN Control Bit
    pI2CHandle->pI2Cx->CR2 |= I2C_CR2_ITERREN;
  }

  return busystate;
}

uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle,uint8_t *pRxBuffer, uint32_t len, uint8_t slaveAddr,uint8_t Sr)
{

  uint8_t busystate = pI2CHandle->TxRxState;

  if((busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
  {
    pI2CHandle->pRxBuffer = pRxBuffer;
    pI2CHandle->RxLen = len;
    pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
    pI2CHandle->RxSize = len; //Rxsize is used in the ISR code to manage the data reception
    pI2CHandle->DevAddr = slaveAddr;
    pI2CHandle->Sr = Sr;


    //Implement code to Generate START Condition
    I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

    //Implement the code to enable ITBUFEN Control Bit
    pI2CHandle->pI2Cx->CR2 |= I2C_CR2_ITBUFEN;

    //Implement the code to enable ITEVTEN Control Bit
    pI2CHandle->pI2Cx->CR2 |= I2C_CR2_ITEVTEN;

    //Implement the code to enable ITERREN Control Bit
    pI2CHandle->pI2Cx->CR2 |= I2C_CR2_ITERREN;

  }

  return busystate;
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{


  if(pI2CHandle->TxLen > 0){

    //1. Load the data in to DR
    pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

    //2. Decrement the TxLen
    pI2CHandle->TxLen--;

    //3. Increment buffer address
    pI2CHandle->pTxBuffer++;

  }
}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{

  if(pI2CHandle->RxSize == 1)
  {

    *pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
    pI2CHandle->RxLen--;

  }

  if(pI2CHandle->RxSize > 1){

    if(pI2CHandle->RxLen == 2)
    {

      //clearFrame ack bit
      I2C_ToggleAcking(pI2CHandle->pI2Cx, DISABLE);
    }

    //Read DR
    *pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
    pI2CHandle->pRxBuffer++;
    pI2CHandle->RxLen--;
  }

  if(pI2CHandle->RxLen == 0){

    //Close the I2C data reception and notify the application

    //1. Generate the stop condition
    if(pI2CHandle->Sr == I2C_SR_DI)
    {
      I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
    }

    //2. Close the I2C rx
    I2C_CloseReceiveData(pI2CHandle);

    //3. Notify the application
    I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
  }
}

void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
  //Implement the code to disable ITBUFEN control bit
  pI2CHandle->pI2Cx->CR2 &= ~I2C_CR2_ITBUFEN;

  //Implement the code to disable ITEVTEN control bit
  pI2CHandle->pI2Cx->CR2 &= ~I2C_CR2_ITEVTEN;

  pI2CHandle->TxRxState = I2C_READY;
  pI2CHandle->pRxBuffer = NULL;
  pI2CHandle->RxLen = 0;
  pI2CHandle->RxSize = 0;

  if(pI2CHandle->I2C_Config.I2C_ACKControl == I2C_ACK_ENABLE)
  {

    I2C_ToggleAcking(pI2CHandle->pI2Cx, ENABLE);
  }
}

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle){

  //Implement the code to disable ITBUFEN control bit
  pI2CHandle->pI2Cx->CR2 &= ~I2C_CR2_ITBUFEN;

  //Implement the code to disable ITEVTEN control bit
  pI2CHandle->pI2Cx->CR2 &= ~I2C_CR2_ITEVTEN;

  pI2CHandle->TxRxState = I2C_READY;
  pI2CHandle->pTxBuffer = NULL;
  pI2CHandle->TxLen = 0;
}

void I2C_EVIRQHandling(I2C_Handle_t *pI2CHandle)
{
  //Interrupt handling for master device
  uint32_t temp1, temp2, temp3;

  temp1 = pI2CHandle->pI2Cx->CR2 & I2C_CR2_ITEVTEN;
  temp2 = pI2CHandle->pI2Cx->CR2 & I2C_CR2_ITBUFEN;

  temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_SB;

  //1. Handle for interrupt generated by SB event
  //Note: SB flag is only applicable in Master mode
  if(temp1 && temp3){

    //SB flag is set
    //1. Execute address phase
    if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
    {

      I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
    }
    else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
    {

      I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
    }


  }

  temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_ADDR;
  //2. Handle for interrupt generated by ADDR event
  //Note: when in master mode : Address is sent
  if(temp1 && temp3)
  {

    //ADDR flag is set
    I2C_clearFrameADDRFlag(pI2CHandle);
  }

  temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_BTF;

  //3. Handle for interrupt generated by BTF event
  if(temp1 && temp3)
  {

    //BTF flag is set
    if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
    {

      if(pI2CHandle->pI2Cx->SR1 & I2C_SR1_TXE)
      {

        //BTF, TXE = 1
        if(pI2CHandle->TxLen == 0)
        {

          //1. Generate the STOP condition
          if(pI2CHandle->Sr == I2C_SR_DI)
          {

            I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
          }


          //2. Reset all the member elements of the handle structure
          I2C_CloseSendData(pI2CHandle);

          //3. Notify the application
          I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
        }
      }
    }

  }

  temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_STOPF;
  //4. Handle for interrupt generated by STOPF event
  //Note: Stop detection flag is applicable only in slave mode
  if(temp1 && temp3)
  {

    //STOPF flag is set
    //clearFrame the flag (read SR1, write to CR1)
    pI2CHandle->pI2Cx->CR1 |= 0x0000;

    //Notify the application that STOP is detected
    I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);

  }

  temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_TXE;
  //5. Handle for interrupt generated by TXE event
  if(temp1 && temp2 && temp3)
  {
    //We have to do the data transmission
    if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
    {
      I2C_MasterHandleTXEInterrupt(pI2CHandle);
    }
  }

  temp3 = pI2CHandle->pI2Cx->SR1 & I2C_SR1_RXNE;
  //6. Handle for interrupt generated by RXNE event
  if(temp1 && temp2 && temp3)
  {
    //RXNE flag is set
    if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
    {
      I2C_MasterHandleRXNEInterrupt(pI2CHandle);
    }
  }
}

void I2C_ERIRQHandling(I2C_Handle_t *pI2CHandle)
{

  uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
  temp2 = (pI2CHandle->pI2Cx->CR2) & I2C_CR2_ITERREN;


/***********************Check for Bus error************************************/
  temp1 = (pI2CHandle->pI2Cx->SR1) & I2C_SR1_BERR;
  if(temp1  && temp2 )
  {
    //This is Bus error

    //Implement the code to clearFrame the buss error flag
    pI2CHandle->pI2Cx->SR1 &= ~I2C_SR1_BERR;

    //Implement the code to notify the application about the error
    I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_BERR);
  }

/***********************Check for arbitration lost error************************************/
  temp1 = (pI2CHandle->pI2Cx->SR1) & I2C_SR1_ARLO ;
  if(temp1  && temp2)
  {
    //This is arbitration lost error

    //Implement the code to clearFrame the arbitration lost error flag
    pI2CHandle->pI2Cx->SR1 &= ~I2C_SR1_ARLO;

    //Implement the code to notify the application about the error
    I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
  }

/***********************Check for ACK failure  error************************************/

  temp1 = (pI2CHandle->pI2Cx->SR1) & I2C_SR1_AF;
  if(temp1  && temp2)
  {
    //This is ACK failure error

      //Implement the code to clearFrame the ACK failure error flag
    pI2CHandle->pI2Cx->SR1 &= ~I2C_SR1_AF;

    //Implement the code to notify the application about the error
    I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_AF);
  }

/***********************Check for Overrun/underrun error************************************/
  temp1 = (pI2CHandle->pI2Cx->SR1) & I2C_SR1_OVR;
  if(temp1  && temp2)
  {
    //This is Overrun/underrun

      //Implement the code to clearFrame the Overrun/underrun error flag
    pI2CHandle->pI2Cx->SR1 &= ~I2C_SR1_OVR;

    //Implement the code to notify the application about the error
    I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
  }

/***********************Check for Time out error************************************/
  temp1 = (pI2CHandle->pI2Cx->SR1) & I2C_SR1_TIMEOUT;
  if(temp1  && temp2)
  {
    //This is Time out error

      //Implement the code to clearFrame the Time out error flag
    pI2CHandle->pI2Cx->SR1 &= ~I2C_SR1_TIMEOUT;

    //Implement the code to notify the application about the error
    I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_TIMEOUT);
  }

}

__WEAK void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);
