/*
 * stm32401xx_gpiodriver.c
 *
 *  Created on: Jul 11, 2026
 *      Author: vyaas
 */

#include "../../Drivers/Inc/stm32401xxgpio.h"

/*************************** Fun 1.peripheral clock setup ********************************************************
 * @fn                   - GPIO_PeriClockControl
 *
 * @brief                - This function enables or disables peripheral  clock for given gpio port address
 *
 * @param[in]            - base addr of gpio peripheral
 * @param[in]            - macros ENABLE or DISABLE
 * @param[in]            -
 *
 * @return               - none
 *
 * @Note                 - none


 */

/* peripheral clock setup */
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx,uint8_t EnorDi)
{

	if(EnorDi==ENABLE)
	{
		if(pGPIOx==GPIOA)
		{
			GPIOA_PCLK_EN();       // user send   GPIO_PeriClockControl(GPIOB,ENABLE);  PGIOX=0x40020000   gpiob
		}
		else if(pGPIOx==GPIOB)
		{
			GPIOB_PCLK_EN();
		}
		else if(pGPIOx==GPIOC)
		{
			GPIOC_PCLK_EN();
		}
		else if(pGPIOx==GPIOD)
		{
			GPIOD_PCLK_EN();
		}
		else if(pGPIOx==GPIOE)
		{
			GPIOE_PCLK_EN();
		}
		else if(pGPIOx==GPIOH)
		{
			GPIOH_PCLK_EN();
		}
	}
	else if(pGPIOx==GPIOA)
	{
		GPIOA_PCLK_DI();
	}
	else if(pGPIOx==GPIOB)
	{
		GPIOB_PCLK_DI();
	}
	else if(pGPIOx==GPIOC)
	{
		GPIOC_PCLK_DI();
	}
	else if(pGPIOx==GPIOD)
	{
		GPIOD_PCLK_DI();
	}
	else if(pGPIOx==GPIOE)
	{
		GPIOE_PCLK_DI();
	}
	else if(pGPIOx==GPIOH)
	{
		GPIOH_PCLK_DI();
	}


}

/*************************** Fun 2.GPIO_Init ************************************************************************
 * @fn                   - GPIO_Init
 *
 * @brief                - This function Initilize the peripheral for given gpio port address for particular pin , mode
 *
 * @param[in]            - variable address of type GPIO_Handle_t
 * @param[in]            -
 * @param[in]            -
 *
 * @return               - none
 *
 * @Note                 - none


 */


void GPIO_Init(GPIO_Handle_t *pGPIOHandle )
{
	uint32_t temp=0;
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode<=GPIO_MODE_ANALOG)
	{
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

		//clear nd set
		pGPIOHandle->pGPIOx->MODER &= ~(0x03 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER |=temp;
	}
	else
	{
		//code later interrupt

		/* MODE = FTSR ( HIGH - LOW DETECT ) */
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_IT_FT)
		{
			EXTI->EXTI_FTSR |=(0X1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->EXTI_RTSR &=~(0X1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}
		/* MODE = RTSR ( LOW - HIGH DETECT ) */
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_IT_RT)
		{
			EXTI->EXTI_RTSR |=(0X1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->EXTI_FTSR &=~(0X1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}
		/* MODE = RTSR ( LOW - HIGH DETECT  OR  HIGH - LOW) */
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_IT_RFT)
		{
			EXTI->EXTI_RTSR |=(0X1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->EXTI_FTSR |=(0X1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}

		// CONFIG WHICH PORT CONNECT EXTI (SYSCFG-EXTI)

		uint8_t temp1 , temp2,portcode;
		SYSCFG_PCLK_EN();   // SYSCFG PERI CLOCK ENABLE
		temp1 =pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/4;
		temp2 =pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%4;
		portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		SYSCFG->SYSCFG_EXTICR[temp1] |=(portcode << (4*temp2));
		//3. Enable the exti interrupt delivery using IMR
		EXTI->EXTI_IMR |=(0X1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);




	}

	//2.config pinTYPE

	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	//Clear nd set
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x03 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OTYPER |=temp;


	/*3.config pin as PU or PD */

	temp=0 ; // safer side we use it clean variable always
	temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

	//CLEAR & SET
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));  // CLEAR BITS
	pGPIOHandle->pGPIOx->PUPDR |= temp;  // SET


	/*4.config the speed of pin */

	temp=0;
	temp=(pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));

	//CLEAR & SET
	pGPIOHandle->pGPIOx->OSPEEDER &= ~(0x3 << (2*pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDER |= temp;



	/* if mode is alternate function */

	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode==GPIO_MODE_ALTFUN)
	{
		uint32_t temp1 ,temp2;
		temp1=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/8;
		temp2=pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%8;

		// CLEAR & SET
		pGPIOHandle->pGPIOx->AFR[temp1] &=~(0xF<<(4*temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |=(pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode <<(4*temp2));

	}


}



/*************************** Fun 3.GPIO_DE-Init ******************************************************************
 * @fn                   - GPIO_DE-Init
 *
 * @brief                - This function DeInitilize the peripheral for given gpio port address for particular pin , mode
 *
 * @param[in]            - base address of any GPIO PORT
 * @param[in]            -
 * @param[in]            -
 *
 * @return               - none
 *
 * @Note                 - none
******************************************************************************************************************/

void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{

	if(pGPIOx==GPIOA)
	{
		GPIOA_REG_RST();
	}
	else if(pGPIOx==GPIOB)
	{
		GPIOB_REG_RST();
	}
	else if(pGPIOx==GPIOC)
	{
		GPIOC_REG_RST();
	}
	else if(pGPIOx==GPIOD)
	{
		GPIOD_REG_RST();
	}
	else if(pGPIOx==GPIOE)
	{
		GPIOE_REG_RST();
	}
	else if(pGPIOx==GPIOH)
	{
		GPIOH_REG_RST();
	}

}


/*************************** Fun 4.GPIO_READ-InPUT_PIN ******************************************************************
 * @fn                   - GPIO_READ-InPUT_PIN
 *
 * @brief                - This fun read the bit value of the pin
 *
 * @param[in]            - base address of any GPIO PORT
 * @param[in]            - 8-bit int variable named as pinnumber to read value on that pin
 * @param[in]            -
 *
 * @return               - 0 OR 1
 *
 * @Note                 - none

*****************************************************************************************************************/

/* Data read _ write */

uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t)( ( pGPIOx->IDR >> PinNumber ) & 0x01);
	return value;

}




/*************************** Fun 5.GPIO_READ-InPUT_PORT ******************************************************************
 * @fn                   - GPIO_READ-InPUT_PIN
 *
 * @brief                - This fun read the  value of ALL 16 PINS of any GPIO PORT
 *
 * @param[in]            - base address of any GPIO PORT
 * @param[in]            -
 * @param[in]            -
 *
 * @return               - 0 OR 1
 *
 * @Note                 - none

******************************************************************************************************************/

/* Read From Input Port */

uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;
	value = (uint16_t )pGPIOx->IDR;
	return value;

}


/*************************** Fun 6.GPIO_WRITE-OUTPUT_PIN ******************************************************************
 * @fn                   - GPIO_WRITE-OUTPUT_PIN
 *
 * @brief                - This fun Write the bit value to that  pin num
 *
 * @param[in]            - base address of any GPIO PORT
 * @param[in]            - 8-bit int variable named as pinnumber
 * @param[in]            - Value either 0 or 1 ( this value is written on that pin )
 *
 * @return               - none
 *
 * @Note                 - none

*****************************************************************************************************************/


void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber,uint8_t Value)
{
	if(Value==GPIO_PIN_SET)
	{
		pGPIOx->ODR |= (Value << PinNumber);
	}
	else
		//write 0
		pGPIOx->ODR &= ~(  0x1 << PinNumber );

}


/*************************** Fun 7.GPIO_WRITE-OUTPUT_PORT ******************************************************************
 * @fn                   - GPIO_WRITE-OUTPUT_PORT
 *
 * @brief                - This fun Write value to all 16 pins of any GPIO PORT
 *
 * @param[in]            - base address of any GPIO PORT
 * @param[in]            - 16-bit value variable  contains all 1's (0xffff) or 0's (0x0000) write to all pins of given port
 * @param[in]            -
 *
 * @return               - NONE
 *
 * @Note                 - none

******************************************************************************************************************/

/* WRITE_OUTPUT_PORT */
void GPIO_WRITEToOutputPort(GPIO_RegDef_t *pGPIOx,uint16_t Value)
{
	pGPIOx->ODR |=(Value );   //	//write 1 to all pins


}


/* toggle pin */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx,uint8_t PinNumber)
{
	pGPIOx->ODR ^=(0x1 << PinNumber);
}



/*************************** Fun 8.GPIO_IRQInterruptConfig ******************************************************************
 * @fn                   - GPIO_IRQConfig
 *
 * @brief                - This fun SET particular IRQ
 *
 * @param[in]            - Value SET or RESET( 0 or 1 )
 * @param[in]            - IRQ num (position number in vector table )  of a  interrupt handler
 * @param[in]            -
 *
 * @return               - NONE
 *
 * @Note                 - none

******************************************************************************************************************/


/* irq config nd  irq handling */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		if(IRQNumber <= 31)  // 0 to 31 irq's
		{
			//Programm ICER0 Reg
			*NVIC_ISER0 |=(0x1 << IRQNumber);

		}
		else if((IRQNumber >=32 ) && (IRQNumber <= 63))  //32 to 63 irq's
		{
			// program ICER1 Reg
			*NVIC_ISER1 |=(0x1 << IRQNumber % 32);

		}
		else if((IRQNumber >=64 ) && (IRQNumber <= 95))  //64 to 95 irq's
		{
			// program ICER2 Reg
			*NVIC_ISER2 |=(0x1 << IRQNumber % 64);

		}


	}
	else
	{
		if(IRQNumber <= 31)  // 0 to 31 irq's
		{
			//Programm ICER0 Reg
			*NVIC_ICER0 |=(0x1 << IRQNumber);    // INTERRUPT CLEAR REGISTER

		}
		else if((IRQNumber >=32 ) && (IRQNumber <= 63))  //32 to 63 irq's
		{
			// program ICER1 Reg
			*NVIC_ICER1 |=(0x1 << IRQNumber % 32);

		}
		else if((IRQNumber >=64 ) && (IRQNumber <= 95))  //64 to 95 irq's
		{
			// program ICER Reg
			*NVIC_ICER3 |=(0x1 << IRQNumber % 64);

		}
	}


}


/*************************** Fun 9.GPIO_IRQPriorityConfig ******************************************************************
 * @fn                   - GPIO_IRQPriorityConfig
 *
 * @brief                - This fun SET particular IRQ
 *
 * @param[in]            - Value SET or RESET( 0 or 1 )
 * @param[in]            - IRQ num (position number in vector table )  of a  interrupt handler
 * @param[in]            -
 *
 * @return               - NONE
 *
 * @Note                 - none

******************************************************************************************************************/

void GPIO_IRQPriorityConfig(uint8_t IRQNumber,uint8_t IRQPriority)
{
	//1.fIRST lets find out ipr reg
	uint8_t iprx = IRQNumber /4;
	uint8_t iprx_section = IRQNumber %4;
	uint8_t shift_amount = (8* iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + (iprx*4)) |= (IRQPriority << shift_amount);
}


/*************************** Fun 10.GPIO_IRQHandling ******************************************************************
 * @fn                   - GPIO_IRQHandling
 *
 * @brief                - This fun check if exti-pr reg corresponding bit is set & make it  the pin bit clear BY writing 1
 *
 * @param[in]            - pin number ( 0 to 15 )
 * @param[in]            -
 * @param[in]            -
 *
 * @return               - NONE
 *
 * @Note                 - none

 ********************************************************************************************************/

void GPIO_IRQHandling(uint32_t pinnumber)
{
	if(EXTI->EXTI_PR & (0X1<<pinnumber) )
	{
		EXTI->EXTI_PR |= 0X1<<pinnumber ; // CLEAR BIT BY WRITING 1
	}
}












