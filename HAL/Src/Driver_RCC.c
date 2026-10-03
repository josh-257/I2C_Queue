#include "Driver_RCC.h"

static uint32_t RCC_GetPLLOutputClock(void);

//AHB and APB1 clock division factor lookup array
uint16_t AHB_PreScaler[] = {2, 4, 8, 16, 64, 128, 256, 512};
uint8_t APB_PreScaler[] = {2, 4, 8, 16};

//PLL post divisor lookup array
uint8_t PLL_PostDivisorValues[] = {2, 4, 6, 8};

uint32_t RCC_GetPeripheralClockAPB1Value(void)
{

	uint32_t pclk1, SystemClk = 0;
	uint8_t clksrc, temp, ahbp, apb1p;

	//Check which clock is being used
	clksrc = ((RCC->CFGR >> 2) & 0x3);

	if(clksrc == 0)
	{
		SystemClk = RCC_CLOCK_HSI;
	}
	else if(clksrc == 1)
	{
		SystemClk = RCC_CLOCK_HSE;
	}
	else if(clksrc == 2)
	{
		SystemClk = RCC_GetPLLOutputClock();
	}

	//for ahb
	//Check clock division factor
	temp = (RCC->CFGR >> 4) & 0xF;

	if(temp < 8)
	{
		ahbp = 1;
	}
	else
	{
		ahbp = AHB_PreScaler[temp - 8];
	}

	//for apb1
	temp = ((RCC->CFGR >> 10) & 0x7);

	if(temp < 4)
	{
		apb1p = 1;
	}
	else
	{
		apb1p = APB_PreScaler[temp - 4];
	}

	pclk1 = (SystemClk / ahbp) / apb1p;

	return pclk1;
}

uint32_t RCC_GetPeripheralClockAPB2Value(void){

	uint32_t pclk2, SystemClk = 0;
	uint8_t clksrc, temp, ahbp, apb2p;

	clksrc = ((RCC->CFGR >> 2) & 0x3);

	if(clksrc == 0){
		SystemClk = RCC_CLOCK_HSI;
	}
	else if(clksrc == 1){
		SystemClk = RCC_CLOCK_HSE;
	}
	else if(clksrc == 2){
		SystemClk = RCC_GetPLLOutputClock();
	}

	//for ahb
	temp = (RCC->CFGR >> 4) & 0xF;

	if(temp < 8){
		ahbp = 1;
	}
	else{
		ahbp = AHB_PreScaler[temp - 8];
	}

	//for apb2
	temp = ((RCC->CFGR >> 13) & 0x7);

	if(temp < 4){
		apb2p = 1;
	}
	else{
		apb2p = APB_PreScaler[temp - 4];
	}

	pclk2 = (SystemClk / ahbp) / apb2p;

	return pclk2;
}

static uint32_t RCC_GetPLLOutputClock(void)
{
	uint8_t temp1, div_factor;
	uint16_t multiply_factor;
	uint32_t sysclk;
	//Check the PLL source
	temp1 = (RCC->PLLCFGR >> 22) & 0x1;

	if(temp1)
	{
		//HSE is the source
		sysclk = RCC_CLOCK_HSE;

	}
	else
	{
		//HSI is the source
		sysclk = RCC_CLOCK_HSI;
	}

	//Pre divisor (/M)
	div_factor = RCC->PLLCFGR & 0x3F;
	sysclk /= div_factor;

	//Multiplier (*N)
	multiply_factor = (RCC->PLLCFGR >> 6) & 0x1FF;
	sysclk *= multiply_factor;

	//Post divisor (/P)
	uint8_t index = (RCC->PLLCFGR >> 16) & 0x3;
	div_factor = PLL_PostDivisorValues[index];
	sysclk /= div_factor;

	return sysclk;

}


