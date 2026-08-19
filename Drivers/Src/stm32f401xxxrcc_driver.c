/*
 * stm32f401xxxrcc_driver.c
 *
 *  Created on: Aug 10, 2026
 *      Author: vyaas
 */



#include "stm32f401xxxrcc.driver.h"




// apb1-bus clock pclk1
uint32_t RCC_GetPclk1Value(void)
{
	uint32_t pclk2,systemclk,AHB_p,APB1_p;
	uint16_t AHB_Prescaler[8] = {2,4,8,16,64,128,256,512};
	uint8_t APB1_Prescaler[4] = {2,4,8,16};

	uint32_t clksrc,tempHpre;
	clksrc = ( (RCC->RCC_CFGR >> 2) & 0x3 );
	if(clksrc==0)
	{
		systemclk = 16000000;    // HSI 16MHZ

	}
	else if(clksrc==1)
	{
		systemclk = 8000000;    //HSE 8MHZ
	}
	/*	else if(clksrc==2)
	{
		systemclk = RCC_GetPLLOutputClock();   //PLL
	}
	 */



	//AHB_P
	tempHpre = ((RCC->RCC_CFGR >> 4) & 0xF);

	if(tempHpre < 8)
	{
		AHB_p = 1;
	}
	else
	{
		AHB_p = AHB_Prescaler[tempHpre - 8];
	}


	//APB1 - PRESCALER VALUE CHECK
	tempHpre = ((RCC->RCC_CFGR >> 10) &0x07);

	if(tempHpre < 4)
	{
		APB1_p = 1;   //APB1 PRESCALER VALUE = 1
	}
	else
	{
		APB1_p = APB1_Prescaler[tempHpre - 4];
	}

	pclk2 =	( ( systemclk / AHB_p ) / APB1_p );

	return pclk2;

}





// pclk2 clock apb2 bus max 84mhz
uint32_t RCC_GetPclk2Value(void)
{
	uint32_t pclk2,systemclk,AHB_p,APB2_p;
	uint16_t AHB_Prescaler[8] = {2,4,8,16,64,128,256,512};
	uint8_t APB2_Prescaler[4] = {2,4,8,16};

	uint32_t clksrc,tempHpre;
	clksrc = ( (RCC->RCC_CFGR >> 2) & 0x3 );
	if(clksrc==0)
	{
		systemclk = 16000000;    // HSI 16MHZ

	}
	else if(clksrc==1)
	{
		systemclk = 8000000;    //HSE 8MHZ
	}
	/*	else if(clksrc==2)
	{
		systemclk = RCC_GetPLLOutputClock();   //PLL
	}
	 */



	//AHB_P
	tempHpre = ((RCC->RCC_CFGR >> 4) & 0xF);

	if(tempHpre < 8)
	{
		AHB_p = 1;
	}
	else
	{
		AHB_p = AHB_Prescaler[tempHpre - 8];
	}


	//APB2 - PRESCALER VALUE CHECK
	tempHpre = ((RCC->RCC_CFGR >> 10) &0x07);

	if(tempHpre < 4)
	{
		APB2_p = 1;   //APB2 PRESCALER VALUE = 1
	}
	else
	{
		APB2_p = APB2_Prescaler[tempHpre - 4];
	}

	pclk2 =	( ( systemclk / AHB_p ) / APB2_p );

	return pclk2;

}

