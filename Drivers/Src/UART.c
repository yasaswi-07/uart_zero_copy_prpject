/*
 * UART.c
 *
 *  Created on: Aug 5, 2026
 *      Author: vyaas
 */


#include "../Inc/stm32_uartdriver.h"
#include <stdint.h>

//send data using polling
static void UART_Send_8bitData(UART_Handle_t *pUARTHandle,uint8_t *pTxBuffer, uint32_t Len);
static void UART_Send_9bitData(UART_Handle_t *pUARTHandle,uint8_t *pTxBuffer, uint32_t Len);

//receive data using polling
static void UART_Receive_9bitData(UART_Handle_t *pUARTHandle,uint8_t *pTxBuffer, uint32_t Len);
static void UART_Receive_8bitData(UART_Handle_t *pUARTHandle,uint8_t *pTxBuffer, uint32_t Len);

//send DATA USING INTERRUPTS
static void UART_Send_9BitdataIT(UART_Handle_t *pUARTHandle);
static void UART_Send_8BitdataIT(UART_Handle_t *pUARTHandle);

//Receive DATA USING INTERRUPTS
static void UART_Receive_8bitDataIT(UART_Handle_t *pUARTHandle);
static void UART_Receive_9bitDataIT(UART_Handle_t *pUARTHandle);



// weak fun
__attribute__((weak))
void UART_ApplicationEventCallback(UART_Handle_t *pUARTHandle, uint8_t AppEv)
{
	// Default implementation
	// User can override this function
}

/*************************** UART_ClearFlag ***********************************
 * @fn          - UART_ClearFlag
 * @brief       - Clear UART status flags
 * @param[in]   - pUARTx : UART peripheral
 * @param[in]   - FlagName : UART flag to clear
 * @return      - none
 ******************************************************************************/
void UART_ClearFlag(UART_RegDef_t *pUARTx, uint32_t FlagName)
{
	volatile uint32_t temp;

	if(FlagName == UART_FLAG_TC)
	{
		// TC is cleared by writing 0 to TC bit
		pUARTx->UART_SR &= ~(1 << UART_SR_TC);
	}
	else if(FlagName == UART_FLAG_CTS)
	{
		// Read SR then write to SR (or write 0 to CTS bit)
		pUARTx->UART_SR &= ~(1 << UART_SR_CTS);
	}
	else if(FlagName == UART_FLAG_IDLE)
	{
		// Clear sequence : read SR then read DR
		temp = pUARTx->UART_SR;
		temp = pUARTx->UART_DR;
		(void)temp;
	}
	else if(FlagName == UART_FLAG_ORE)
	{
		// Clear sequence : read SR then read DR
		temp = pUARTx->UART_SR;
		temp = pUARTx->UART_DR;
		(void)temp;
	}
	else if(FlagName == UART_FLAG_FE)
	{
		// Clear sequence : read SR then read DR
		temp = pUARTx->UART_SR;
		temp = pUARTx->UART_DR;
		(void)temp;
	}
	else if(FlagName == UART_FLAG_NE)
	{
		// Clear sequence : read SR then read DR
		temp = pUARTx->UART_SR;
		temp = pUARTx->UART_DR;
		(void)temp;
	}
}


//send data using interrupts 8 bit data
static void UART_Send_8BitdataIT(UART_Handle_t *pUARTHandle)
{

	if(pUARTHandle->TxLen > 0)
	{

		pUARTHandle->pUARTx->UART_DR = *(pUARTHandle->pTxBuffer);
		pUARTHandle->pTxBuffer++;
		pUARTHandle->TxLen--;

	}

	if(pUARTHandle->TxLen == 0)
	{
		//enable tcie bcz transmission complete it set tc by h/w so tcie interrupt control bit
		/* stop TXE interrupts */
		pUARTHandle->pUARTx->UART_CR1 &=~(1 << UART_CR1_TXEIE);

		/* wait for complete transmission */
		pUARTHandle->pUARTx->UART_CR1 |= (1 << UART_CR1_TCIE);

	}



}



//9bit sent using interrupt
static void UART_Send_9BitdataIT(UART_Handle_t *pUARTHandle)
{

	uint16_t *pdata;


	if(pUARTHandle->TxLen > 0)
	{

		if(pUARTHandle->UART_Config.UART_Parity_Control == UART_PARITY_ENABLE)
		{


			pUARTHandle->pUARTx->UART_DR = (*(pUARTHandle->pTxBuffer) & 0xFF); // 1 byte
			pUARTHandle->pTxBuffer++;
			pUARTHandle->TxLen--;

		}


		else
		{ //  9 bits with  no parity read 2 bytes but send only 9 bits do mask for it to get 9 bits
			pdata = (uint16_t*)pUARTHandle->pTxBuffer;
			pUARTHandle->pUARTx->UART_DR = (*pdata & 0x01FF);
			pUARTHandle->pTxBuffer +=2;
			pUARTHandle->TxLen -= 2;

		}


	}
	if(pUARTHandle->TxLen == 0)
	{
		//enable tcie bcz transmission complete it set tc by h/w so tcie interrupt control bit
		/* stop TXE interrupts */
		pUARTHandle->pUARTx->UART_CR1 &=~(1 << UART_CR1_TXEIE);

		/* wait for complete transmission */
		pUARTHandle->pUARTx->UART_CR1 |= (1 << UART_CR1_TCIE);

	}



}

// close tx transmission
static void closeTX_Transmission(UART_Handle_t *pUARTHandle)
{
	if(pUARTHandle->TxState == UART_BUSY_IN_TX)
	{
		if(pUARTHandle->TxLen == 0)
		{

			//enable tcie bcz transmission complete it set tc by h/w so tcie interrupt control bit
			//pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_TCIE);

UART_ClearFlag(pUARTHandle->pUARTx, UART_FLAG_TC);
			// Reset state
			pUARTHandle->TxState = UART_READY;

			// Reset buffer
			pUARTHandle->pTxBuffer = NULL;

			// Reset length
			pUARTHandle->TxLen = 0;
		}
	}
}



// close Rx Reception
static void closeRX_Reception(UART_Handle_t *pUARTHandle)
{
	if(pUARTHandle->RxState == UART_BUSY_IN_RX)
	{
		//Check the RxLen . If it is zero then close the data Reception
		if(pUARTHandle->RxLen == 0)
		{
			//CLEAR rxniE interrupt bit

			pUARTHandle->pUARTx->UART_CR1 &= ~(0X1 << UART_CR1_RXNEIE);
			pUARTHandle->RxState = UART_READY;

			// Reset buffer pointer
			pUARTHandle->pRxBuffer = NULL;
			pUARTHandle->RxLen = 0;


		}

	}

}
// 8 bit send using polling
static void UART_Send_8bitData(UART_Handle_t *pUARTHandle,uint8_t *pTxBuffer, uint32_t Len)
{
	uint32_t i;

	for(i=0;i < Len ; i++)
	{
		while(!UART_GetFlagStatus(pUARTHandle->pUARTx, UART_FLAG_TXE));

		pUARTHandle->pUARTx->UART_DR = *(pTxBuffer);
		pTxBuffer++;
	}
	//wait untill all bytes transmission complete
	while(!UART_GetFlagStatus(pUARTHandle->pUARTx, UART_FLAG_TC));


}




// 9.bit data sent using polling
static void UART_Send_9bitData(UART_Handle_t *pUARTHandle,uint8_t *pTxBuffer, uint32_t Len)
{
	uint32_t i=0; uint16_t *pdata;


	while(i < Len)
	{
		while(! UART_GetFlagStatus(pUARTHandle->pUARTx,UART_FLAG_TXE));

		if(pUARTHandle->UART_Config.UART_Parity_Control == UART_PARITY_ENABLE)
		{


			pUARTHandle->pUARTx->UART_DR = (*pTxBuffer & 0xFF); // 1 byte
			pTxBuffer++;
			i++;

		}


		else
		{ //  9 bits with  no parity read 2 bytes but send only 9 bits do mask for it to get 9 bits
			pdata = (uint16_t*)pTxBuffer;
			pUARTHandle->pUARTx->UART_DR = (*pdata & 0x01FF);
			pTxBuffer +=2;
			i += 2;;
		}

	}
	while(!UART_GetFlagStatus(pUARTHandle->pUARTx, UART_FLAG_TC));



}





// 8-bit receive using polling
static void UART_Receive_8bitData(UART_Handle_t *pUARTHandle,uint8_t *pRxBuffer, uint32_t Len)
{
	uint32_t i;

	for(i=0;i < Len ; i++)
	{
		while(!UART_GetFlagStatus(pUARTHandle->pUARTx, UART_FLAG_RXNE));		//We are going to receive 8bit data in a frame

		//check are we using USART_ParityControl control or not
		if(pUARTHandle->UART_Config.UART_Parity_Control == UART_PARITY_ENABLE)
		{

			//Parity is used, so , 7 bits will be of user data and 1 bit is parity
			//read only 7 bits , hence mask the DR with 0X7F
			// 7-bit user data + parity

			*(pRxBuffer) = ( pUARTHandle->pUARTx->UART_DR & 0x7F );
		}
		else
		{
			// Normal 8-bit data

			*(pRxBuffer) = ( pUARTHandle->pUARTx->UART_DR );
		}
		pRxBuffer++;

	}
}



// 9-BIT RECEIVE Using POLLING

static void UART_Receive_9bitData(UART_Handle_t *pUARTHandle,uint8_t *pRxBuffer, uint32_t Len)
{
	uint32_t i=0;


	while(i < Len)
	{
		while(!UART_GetFlagStatus(pUARTHandle->pUARTx, UART_FLAG_RXNE));		//We are going to receive 8bit data in a frame

		if(pUARTHandle->UART_Config.UART_Parity_Control == UART_PARITY_ENABLE)
		{


			*pRxBuffer = (pUARTHandle->pUARTx->UART_DR & 0xFF) ;// 1 byte parity excluded in dr
			pRxBuffer++;
			i++;

		}


		else
		{ //  9 bits with  no parity read 2 bytes but send only 9 bits do mask for it to get 9 bits

			*((uint16_t*)pRxBuffer) = (pUARTHandle->pUARTx->UART_DR & 0x1FF);
			pRxBuffer +=2;
			i +=2;
		}

	}
}




static void UART_Receive_8bitDataIT(UART_Handle_t *pUARTHandle)
{
	if(pUARTHandle->RxLen > 0)
	{
		if(pUARTHandle->UART_Config.UART_Parity_Control == UART_PARITY_ENABLE)
		{

			*(pUARTHandle->pRxBuffer) = (pUARTHandle->pUARTx->UART_DR & 0x7F);
		}
		else
		{
			*(pUARTHandle->pRxBuffer) = (pUARTHandle->pUARTx->UART_DR);

		}
		pUARTHandle->pRxBuffer++;
		pUARTHandle->RxLen--;
	}

	/* Reception complete */

	if(pUARTHandle->RxLen == 0)
	{
		closeRX_Reception(pUARTHandle);
		UART_ApplicationEventCallback(pUARTHandle, UART_EVENT_RX_CMPLT);
	}

}



//9 bit receive using interrupt
static void UART_Receive_9bitDataIT(UART_Handle_t *pUARTHandle)
{
	if(pUARTHandle->RxLen > 0)
	{
		if(pUARTHandle->UART_Config.UART_Parity_Control == UART_PARITY_ENABLE)
		{

			*(pUARTHandle->pRxBuffer) = (pUARTHandle->pUARTx->UART_DR & 0xFF);
			pUARTHandle->pRxBuffer++;
			pUARTHandle->RxLen--;
		}
		else
		{
			*((uint16_t *)(pUARTHandle->pRxBuffer)) = (pUARTHandle->pUARTx->UART_DR & 0x1FF);
			pUARTHandle->pRxBuffer+= 2;
			pUARTHandle->RxLen -= 2;
		}

	}

	/* Reception complete */

	if(pUARTHandle->RxLen == 0)
	{
		closeRX_Reception(pUARTHandle);
		UART_ApplicationEventCallback(pUARTHandle, UART_EVENT_RX_CMPLT);
	}
}

/************************************1.USART_PeriClockControl******************************************************
 * @fn                   - USART_PeriClockControl
 *
 * @brief                - This function enables or disables the peripheral clock for USART
 *
 * @param[in]            - Base address of USART peripheral
 * @param[in]            - ENABLE or DISABLE macro
 *
 * @return               - None
 *
 * @Note                 - Peripheral clock must be enabled before accessing USART registers
 *
 *******************************************************************************/



void UART_periClockControl(UART_RegDef_t *pI2Cx,uint8_t EnorDi)
{
	if(EnorDi==ENABLE)
	{
		if(pI2Cx == UART1)
		{
			UART1_PCLK_EN();
		}
		else if(pI2Cx == UART2)
		{
			UART2_PCLK_EN();
		}
		else if(pI2Cx == UART6)
		{
			UART6_PCLK_EN();
		}
	}
	else if(EnorDi==DISABLE)
	{
		if(pI2Cx == UART1)
		{
			UART1_PCLK_DI();
		}
		else if(pI2Cx == UART2)
		{
			UART2_PCLK_DI();
		}
		else if(pI2Cx == UART6)
		{
			UART6_PCLK_DI();
		}

	}
}


// BRR usartdiv value
void UART_SetBaudRate(UART_RegDef_t *pUARTx , uint32_t BaudRate)
{
	uint32_t pclkx;
	uint32_t usartdiv;
	uint32_t M_part, F_part;
	uint32_t tempreg = 0;

	// Get PCLK
	if(pUARTx == UART1 || pUARTx == UART6)
	{
		pclkx = RCC_GetPclk2Value();
	}
	else
	{
		pclkx = RCC_GetPclk1Value();
	}

	// Calculate USARTDIV * 100
	if(pUARTx->UART_CR1 & (1 << 15)) // OVER8
	{
		usartdiv = (25 * pclkx) / (2 * BaudRate);
	}
	else // OVER16
	{
		usartdiv = (25 * pclkx) / (4 * BaudRate);
	}

	// Extract mantissa and fraction
	M_part = usartdiv / 100;
	F_part = usartdiv % 100;

	if(pUARTx->UART_CR1 & (1 << 15)) // OVER8
	{
		F_part = (((F_part * 8) + 50) / 100) & 0x07;

		tempreg = (M_part << 4) | F_part;

		// BRR bit3 must be 0 in OVER8 mode
		tempreg &= ~(1 << 3);
	}
	else // OVER16
	{
		F_part = (((F_part * 16) + 50) / 100) & 0x0F;

		tempreg = (M_part << 4) | F_part;
	}

	// Write BRR register
	pUARTx->UART_BRR = (tempreg & 0xFFFF);
}

/*******************************2.UART_PeriClockControl**********************************************************
 * @fn                   - UART_PeriClockControl
 *
 * @brief                - This function enables or disables the peripheral clock for UART
 *
 * @param[in]            - pUSARTx : Base address of UART peripheral
 * @param[in]            - EnorDi  : ENABLE or DISABLE clock
 *
 * @return               - None
 *
 ******************************************************************************************/



void UART_periPheralControl(UART_RegDef_t *pI2Cx,uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		pI2Cx->UART_CR1 |=(0x1<<UART_CR1_UE);
	}

	else
	{
		pI2Cx->UART_CR1 &=~(0x1<<UART_CR1_UE);

	}
}






/******************************************************************************************
 * @fn                   - UART_Init
 *
 * @brief                - This function initializes the selected UART peripheral
 *
 * @param[in]            - pUSARTx : Base address of UART peripheral
 *
 * @return               - None
 *
 * @Note                 - This function configures baud rate, word length, stop bits,
 *                          parity, transmitter/receiver mode, and enables the UART peripheral.
 *
 ******************************************************************************************/


void UART_Init(UART_Handle_t *pUARTHandle)
{

	uint32_t temp;

	//1. TX OE RX

	if(pUARTHandle->UART_Config.UART_Mode == UART_MODE_ONLY_TX)
	{
		// TX MODE
		pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_TE);
		pUARTHandle->pUARTx->UART_CR1 |= (0x1 << UART_CR1_TE);

	}
	else if(pUARTHandle->UART_Config.UART_Mode == UART_MODE_ONLY_RX)
	{
		// RX MODE
		pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_RE);
		pUARTHandle->pUARTx->UART_CR1 |= (0x1 << UART_CR1_RE);

	}
	else if(pUARTHandle->UART_Config.UART_Mode == UART_MODE_TXRX)
	{
		// TX MODE
		pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_TE);
		pUARTHandle->pUARTx->UART_CR1 |= (0x1 << UART_CR1_TE);
		// RX MODE
		pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_RE);
		pUARTHandle->pUARTx->UART_CR1 |= (0x1 << UART_CR1_RE);

	}




	//2. set wordlength

	temp = (pUARTHandle->UART_Config.WordLength << UART_CR1_M);

	//clear nd set
	pUARTHandle->pUARTx->UART_CR1 &= ~(1 << UART_CR1_M);
	pUARTHandle->pUARTx->UART_CR1 |= temp;




	//2. set parity bit enable or dis

	if(pUARTHandle->UART_Config.UART_Parity_Control == UART_PARITY_ENABLE)
	{
		// parity enable
		pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_PCE);
		pUARTHandle->pUARTx->UART_CR1 |= (0x1 << UART_CR1_PCE);

	}

	else
	{
		pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_PCE);

	}




	//3.parity even , odd enable

	if(pUARTHandle->UART_Config.UART_Parity_Control == UART_PARITY_ENABLE)
	{
		if(pUARTHandle->UART_Config.UART_ParitySelection == UART_ODDPARITY_ENABLE)
		{
			//1.ODD PARITY

			pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_PS);
			pUARTHandle->pUARTx->UART_CR1 |= (0x1 << UART_CR1_PS);

		}

		else
		{

			// EVEN PARITY
			pUARTHandle->pUARTx->UART_CR1 &= ~(0x1 << UART_CR1_PS);

		}


	}


	//4. HARDWARE flow control

	if(pUARTHandle->UART_Config.UART_HWFlowControl == UART_HW_FLOW_CTRL_CTS)
	{
		// CTS set clear to send
		temp = pUARTHandle->UART_Config.UART_HWFlowControl << UART_CR3_CTSE ;
		pUARTHandle->pUARTx->UART_CR3 |= temp;
	}
	else if(pUARTHandle->UART_Config.UART_HWFlowControl == UART_HW_FLOW_CTRL_RTS)
	{
		//RTS Request to send
		temp = pUARTHandle->UART_Config.UART_HWFlowControl << UART_CR3_RTSE ;
		pUARTHandle->pUARTx->UART_CR3 |= temp;


	}
	else if(pUARTHandle->UART_Config.UART_HWFlowControl == UART_HW_FLOW_CTRL_CTS_RTS)
	{
		// cts set
		temp = pUARTHandle->UART_Config.UART_HWFlowControl << UART_CR3_CTSE;
		pUARTHandle->pUARTx->UART_CR3 |= temp;

		//rts set
		temp = pUARTHandle->UART_Config.UART_HWFlowControl << UART_CR3_RTSE;
		pUARTHandle->pUARTx->UART_CR3 |= temp;



	}



	//5.baudrate value

	UART_SetBaudRate(pUARTHandle->pUARTx, pUARTHandle->UART_Config.BaudRate);


	//6. stop bit

	temp = (pUARTHandle->UART_Config.UART_STOP << UART_CR2_STOP);
	pUARTHandle->pUARTx->UART_CR2 &= ~(temp);


	pUARTHandle->pUARTx->UART_CR2 |= temp;


}

/*********************************5.UART_SendData********************************************************
 * @fn                   - UART_SendData
 *
 * @brief                - This function transmits data over UART in blocking mode
 *
 * @param[in]            - pUSARTx   : Base address of UART peripheral
 * @param[in]            - pTxBuffer : Pointer to transmit buffer
 * @param[in]            - Len       : Number of bytes to transmit
 *
 * @return               - None
 *
 * @Note                 - The function waits until the TXE flag is SET before loading
 *                          each byte into the data register and waits for the TC flag
 *                          after the final byte is transmitted.
 *
 ******************************************************************************************/

void UART_SendData(UART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{

	if(pUSARTHandle->UART_Config.WordLength == UART_WORDLEN_8BITS)
	{    // 8bit data
		UART_Send_8bitData(pUSARTHandle,pTxBuffer, Len);

	}
	else
	{
		UART_Send_9bitData(pUSARTHandle,pTxBuffer, Len);
	}

	//wait till TC flag is set in the SR
	//while( ! UART_GetFlagStatus(pUARTHandle->pUARTx,USART_SR_TC));

}





/*******************************FUN 6.UART_ReceiveData***********************************************************
 * @fn                   - UART_ReceiveData
 *
 * @brief                - This function receives data from UART in polling (blocking) mode
 *
 * @param[in]            - pUSARTx   : Base address of UART peripheral
 * @param[in]            - pRxBuffer : Pointer to receive buffer
 * @param[in]            - Len       : Number of bytes to receive
 *
 * @return               - None
 *
 * @Note                 - The function waits until the RXNE flag becomes SET before
 *                          reading each byte from the data register. It blocks program
 *                          execution until all requested bytes are received.
 *
 ******************************************************************************************/

void UART_ReceiveData(UART_Handle_t *pUARTHandle,uint8_t *pRxBuff,uint32_t Len)
{
	if(pUARTHandle->UART_Config.WordLength == UART_WORDLEN_8BITS)
	{
		//8 bits data rec
		Receive_8bitData(pUARTHandle,pRxBuff, Len);

	}
	else
	{
		Receive_9bitData(pUARTHandle,pRxBuff, Len);
	}
}



/******************************* FUN 11. UART_GetFlagStatus *****************************
 * @fn                   - UART_GetFlagStatus
 *
 * @brief                - Check whether a UART status flag is SET or RESET
 *
 * @param[in]            - pUARTx   : Pointer to UART peripheral
 * @param[in]            - FlagName : UART status flag to check
 *
 * @return               - 1 : Flag is SET
 *                         0 : Flag is RESET
 *
 * @Note                 - This function reads the UART_SR register and returns
 *                         the current state of the specified flag.
 *********************************************************************************/


uint8_t UART_GetFlagStatus(UART_RegDef_t*pUARTx,uint16_t FlagName)
{
	if(pUARTx->UART_SR & FlagName)
	{
		return 1; // SET
	}

	return 0; // RESET
}


/*************************** Fun 12.UART_IRQInterruptConfig ******************************************************************
 * @fn                   - UART_IRQConfig
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





/*************************** Fun 12. UART_SendDataIT *****************************
 * @fn                   - UART_SendDataIT
 *
 * @brief                - Send data using interrupt mode (non-blocking).
 *                          This function stores the TX buffer details in the
 *                          UART handle and enables the TXE interrupt. The
 *                          actual transmission is handled inside the UART IRQ
 *                          handler byte by byte.
 *
 * @param[in]            - pUARTHandle : Pointer to UART handle structure
 * @param[in]            - pTxBuff     : Pointer to transmit buffer
 * @param[in]            - Len         : Number of data items to transmit
 *
 * @return               - Current UART TX state (READY / BUSY_IN_TX)
 *
 * @Note                 - Non-blocking API. Returns immediately after enabling
 *                          the TXE interrupt. Transmission completion is
 *                          notified through the application callback.
 *********************************************************************************/

uint8_t UART_SendDataIT(UART_Handle_t *pUARTHandle, uint8_t *pTxBuff, uint32_t Len)
{
	uint8_t busystate = pUARTHandle->TxState;
	if((busystate !=UART_BUSY_IN_TX) && (busystate != UART_BUSY_IN_RX))
	{

		pUARTHandle->TxLen = Len;
		pUARTHandle->pTxBuffer = pTxBuff;
		pUARTHandle->TxState = UART_BUSY_IN_TX;


		// Enable TXE interrupt
		pUARTHandle->pUARTx->UART_CR1 |= (1 << UART_CR1_TXEIE);

		return busystate;
	}
	return busystate;


}


/*************************** Fun 13. UART_ReceiveDataIT **************************
 * @fn                   - UART_ReceiveDataIT
 *
 * @brief                - Receive data using interrupt mode (non-blocking).
 *                          This function stores the RX buffer details in the
 *                          UART handle and enables the RXNE interrupt. The
 *                          received bytes are copied to the user buffer inside
 *                          the UART IRQ handler.
 *
 * @param[in]            - pUARTHandle : Pointer to UART handle structure
 * @param[in]            - pRxBuff     : Pointer to receive buffer
 * @param[in]            - Len         : Number of data items to receive
 *
 * @return               - Current UART RX state (READY / BUSY_IN_RX)
 *
 * @Note                 - Non-blocking API. Returns immediately after enabling
 *                          the RXNE interrupt. Reception completion is
 *                          notified through the application callback.
 *********************************************************************************/

uint8_t UART_ReceiveDataIT(UART_Handle_t *pUARTHandle, uint8_t *pRxBuff, uint32_t Len)
{
	uint8_t busystate = pUARTHandle->RxState;
	if((busystate !=UART_BUSY_IN_TX) && (busystate != UART_BUSY_IN_RX))
	{

		pUARTHandle->RxLen = Len;
		pUARTHandle->pRxBuffer = pRxBuff;
		pUARTHandle->RxState = UART_BUSY_IN_RX;


		// Enable RXNE interrupt
		pUARTHandle->pUARTx->UART_CR1 |= (1 << UART_CR1_RXNEIE);

		return busystate;
	}
	return busystate;


}


/* irq config nd  irq handling */
void UART_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
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
			*NVIC_ISER2 |=(0x1 << IRQNumber % 32);

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
			*NVIC_ICER2 |=(0x1 << IRQNumber % 32);

		}
	}


}




/*************************** Fun 9.UART_IRQPriorityConfig ******************************************************************
 * @fn                   - I2C_IRQPriorityConfig
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

 *********************************************************************************/

void UART_IRQPriorityConfig(uint8_t IRQNumber , uint8_t IRQPriority)
{
	//1.fIRST lets find out ipr reg
	uint8_t iprx = IRQNumber /4;
	uint8_t iprx_section = IRQNumber %4;
	uint8_t shift_amount = (8* iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	*(NVIC_PR_BASEADDR + (iprx*4)) |= (IRQPriority << shift_amount);

}



/******************************* FUN 17. UART_IRQHandling *******************************
 * @fn                   - UART_IRQHandling
 *
 * @brief                - Handle UART interrupt events and errors
 *
 * @param[in]            - pUARTHandle : Pointer to UART handle structure
 *
 * @return               - None
 *
 * @Note                 - This function checks UART status and control register
 *                         flags and handles the following interrupt sources:
 *
 *                           - TC   : Transmission complete
 *                           - TXE  : Transmit data register empty
 *                           - RXNE : Receive data register not empty
 *                           - CTS  : Clear to send
 *                           - IDLE : Idle line detected
 *                           - ORE  : Overrun error
 *                           - FE   : Framing error
 *                           - NE   : Noise error
 *
 *                         Appropriate callbacks are invoked after handling
 *                         each interrupt event.
 *********************************************************************************/

void UART_IRQHandling(UART_Handle_t *pUARTHandle)
{
	uint32_t temp1=0,temp2=0,temp3=0;



	//1. check TXE FLAG to send data
	temp1 = pUARTHandle->pUARTx->UART_SR & (0x1 << UART_SR_TXE);
	temp2 = pUARTHandle->pUARTx->UART_CR1 & (0x1 << UART_CR1_TXEIE);
	if(temp1 && temp2)
	{
		if(pUARTHandle->UART_Config.WordLength == UART_WORDLEN_8BITS)
		{
			UART_Send_8BitdataIT(pUARTHandle);
		}
		else
		{
			UART_Send_9BitdataIT(pUARTHandle);
		}

	}

	// 2. TC
	temp1 = pUARTHandle->pUARTx->UART_SR & (0x1 << UART_SR_TC);
	temp2 = pUARTHandle->pUARTx->UART_CR1 & (0x1 << UART_CR1_TCIE);


	if(temp1 && temp2)
	{
		/*
		 * this interrupt is bcz of TC flag set
		 * close transmission nd application callback
		 */
		closeTX_Transmission(pUARTHandle);
		UART_ApplicationEventCallback(pUARTHandle, UART_EVENT_TX_CMPLT);
	}


	//3 . RXNE DATA RECEIVE TO DR REG
	temp1 = pUARTHandle->pUARTx->UART_SR & (0x1 << UART_SR_RXNE);
	temp2 = pUARTHandle->pUARTx->UART_CR1 & (0x1 << UART_CR1_RXNEIE);
	if(temp1 && temp2)
	{
		if(pUARTHandle->UART_Config.WordLength == UART_WORDLEN_8BITS)
		{
			UART_Receive_8bitDataIT(pUARTHandle);
		}
		else
		{
			UART_Receive_9bitDataIT(pUARTHandle);
		}

	}

	//3.CHECK CTS FLAG


	//CHECK CTSE ND CTSIE FLAG SET OR NOT
	temp1=pUARTHandle->pUARTx->UART_SR & (0x1 << UART_SR_CTS);

	temp2 = pUARTHandle->pUARTx->UART_CR3 & (0x1 << UART_CR3_CTSE);

	//Implement the code to check the state of CTSIE bit in CR3
	temp3 = pUARTHandle->pUARTx->UART_CR3 & ( 0x1 << UART_CR3_CTSIE);

	if(temp1 && temp2 &&temp3)
	{
		//clear cts flag in status reg
		UART_ClearFlag(pUARTHandle->pUARTx, UART_FLAG_CTS);
		UART_ApplicationEventCallback(pUARTHandle,UART_EVENT_CTS);
	}


	//check IDLE detection flag set nd IDLEIE Control bit set
	temp1= pUARTHandle->pUARTx->UART_SR & (0x1 << UART_SR_IDLE);
	temp2 = pUARTHandle->pUARTx->UART_CR1 & (0x1 << UART_CR1_IDLEIE);
	if(temp1 && temp2)
	{
		//CLEAR FLAG
		UART_ClearFlag(pUARTHandle->pUARTx, UART_FLAG_IDLE);
		UART_ApplicationEventCallback(pUARTHandle,UART_EVENT_IDLE);


	}

	//check ORE detection flag set nd RXNEIE Control bit set
	temp1= pUARTHandle->pUARTx->UART_SR & (0x1 << UART_SR_ORE);
	temp2 = pUARTHandle->pUARTx->UART_CR1 & (0x1 << UART_CR1_RXNEIE);

	if(temp1 && temp2)
	{
		//clear flag overRun flag
		UART_ClearFlag(pUARTHandle->pUARTx, UART_FLAG_ORE);

		//this interrupt is because of Overrun error
		UART_ApplicationEventCallback(pUARTHandle,UART_ERREVENT_ORE);


	}

	//Check for Error Flag

	//Noise Flag, Overrun error and Framing Error in multibuffer communication
	//We dont discuss multibuffer communication in this course. please refer to the RM
	//The blow code will get executed in only if multibuffer mode is used.

	temp2 =  pUARTHandle->pUARTx->UART_CR3 & ( 1 << UART_CR3_EIE) ;

	if( temp2 )
	{
		temp1 = pUARTHandle->pUARTx->UART_SR;
		if(temp1 & ( 1 << UART_SR_FE))
		{
			/*
 					This bit is set by hardware when a de-synchronization, excessive noise or a break character
 					is detected. It is cleared by a software sequence (an read to the USART_SR register
 					followed by a read to the USART_DR register).
			 */
			UART_ApplicationEventCallback(pUARTHandle,UART_ERREVENT_FE);
		}

		if(temp1 & ( 1 << UART_SR_NE) )
		{
			/*
 					This bit is set by hardware when noise is detected on a received frame. It is cleared by a
 					software sequence (an read to the USART_SR register followed by a read to the
 					USART_DR register).
			 */
			UART_ApplicationEventCallback(pUARTHandle,UART_ERREVENT_NE);
		}

		if(temp1 & ( 1 << UART_SR_ORE) )
		{
			UART_ApplicationEventCallback(pUARTHandle,UART_ERREVENT_ORE);
		}
	}


}









