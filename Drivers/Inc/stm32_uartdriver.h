/*
 * UART.h
 *
 *  Created on: Aug 5, 2026
 *      Author: vyaas
 */

#ifndef INC_STM32_UARTDRIVER_H_
#define INC_STM32_UARTDRIVER_H_

#include "stm32401xx.h"


//1.config struct


typedef struct
{
	uint32_t UART_Mode;
	uint32_t BaudRate;
	uint32_t WordLength;
	uint32_t UART_Parity_Control;
	uint32_t UART_ParitySelection;
	uint32_t UART_HWFlowControl;
	uint32_t UART_STOP;


}UART_Config_t;





/***********************UART HANDLE STRUCT **************************************/
typedef struct
{
	UART_RegDef_t   *pUARTx;
	UART_Config_t   UART_Config;


	/* TX information */
	uint8_t  *pTxBuffer;
	uint32_t TxLen;
	uint8_t  TxState;

	/* RX information */
	uint8_t  *pRxBuffer;
	uint32_t RxLen;
	uint8_t  RxState;



}UART_Handle_t;




/**********************************MACROS FOR UART REG BIT VALUES ***************/


/*
 * UART_MODE
 */

#define UART_MODE_ONLY_TX         0
#define UART_MODE_ONLY_RX         1
#define UART_MODE_TXRX       2




/*
 * UART_BAUD RATE
 */

/* UART Baud Rate Macros */
#define UART_BAUD_1200      1200U
#define UART_BAUD_2400      2400U
#define UART_BAUD_4800      4800U
#define UART_BAUD_9600      9600U
#define UART_BAUD_19200     19200U
#define UART_BAUD_38400     38400U
#define UART_BAUD_57600    57600U
#define UART_BAUD_115200    115200U
#define UART_BAUD_230400    230400U
#define UART_BAUD_460800    460800U
#define UART_BAUD_921600    921600U



/*
 * stop bits - 1,0.5,1.5,2 bits
 */

#define UART_STOPBIT_1             0
#define UART_STOPBIT_0_5           1
#define UART_STOPBIT_2             2
#define UART_STOPBIT_1_5           3


/*
 * word length - 8 or 9 bits
 */

#define UART_WORDLEN_8BITS           0
#define UART_WORDLEN_9BITS           1




/*
 * UART_PARITY
 */


#define UART_PARITY_ENABLE           1
#define UART_PARITY_DISABLE          0


/*
 * UART_EVEN OR ODD PARITY
 */

#define UART_EVENPARITY_ENABLE          0
#define UART_ODDPARITY_ENABLE           1



/*
 * UART_HW FLOW CONTROL
 */

#define UART_HW_FLOW_CTRL_NONE       0
#define UART_HW_FLOW_CTRL_CTS        1
#define UART_HW_FLOW_CTRL_RTS        2
#define UART_HW_FLOW_CTRL_CTS_RTS    3





/*
 * @APPLICATION CALLBACK MACROS
 */

// Normal events
#define UART_EVENT_TX_CMPLT        1
#define UART_EVENT_RX_CMPLT        2
#define UART_EVENT_IDLE            3
#define UART_EVENT_CTS             4

// Error events
#define UART_ERREVENT_PE           5   // Parity error
#define UART_ERREVENT_FE           6   // Framing error
#define UART_ERREVENT_NE           7   // Noise error
#define UART_ERREVENT_ORE          8   // Overrun error




/*
 * @interrupt macros
 */

#define UART_BUSY_IN_TX     1
#define UART_BUSY_IN_RX     2
#define UART_READY          3
#define UART_BUSY           4




/**************** USART SR BIT POSITION ****************/

#define UART_SR_PE        0
#define UART_SR_FE        1
#define UART_SR_NE        2
#define UART_SR_ORE       3
#define UART_SR_IDLE      4
#define UART_SR_RXNE      5
#define UART_SR_TC        6
#define UART_SR_TXE       7
#define UART_SR_LBD       8
#define UART_SR_CTS       9


/**************** USART DR BIT POSITION ****************/

#define USART_DR_DR        0


/**************** USART BRR BIT POSITION ****************/

#define USART_BRR_DIV_Fraction   0
#define USART_BRR_DIV_Mantissa   4


/**************** USART CR1 BIT POSITION ****************/

#define UART_CR1_SBK      0
#define UART_CR1_RWU      1
#define UART_CR1_RE       2
#define UART_CR1_TE       3
#define UART_CR1_IDLEIE   4
#define UART_CR1_RXNEIE   5
#define UART_CR1_TCIE     6
#define UART_CR1_TXEIE    7
#define UART_CR1_PEIE     8
#define UART_CR1_PS       9
#define UART_CR1_PCE      10
#define UART_CR1_WAKE     11
#define UART_CR1_M        12
#define UART_CR1_UE       13
#define UART_CR1_OVER8    15


/**************** USART CR2 BIT POSITION ****************/

#define UART_CR2_ADD      0
#define UART_CR2_LBDL     5
#define UART_CR2_LBDIE    6
#define UART_CR2_LBCL     8
#define UART_CR2_CPHA     9
#define UART_CR2_CPOL     10
#define UART_CR2_CLKEN    11
#define UART_CR2_STOP     12
#define UART_CR2_LINEN    14


/**************** USART CR3 BIT POSITION ****************/

#define UART_CR3_EIE      0
#define UART_CR3_IREN     1
#define UART_CR3_IRLP     2
#define UART_CR3_HDSEL    3
#define UART_CR3_NACK     4
#define UART_CR3_SCEN     5
#define UART_CR3_DMAR     6
#define UART_CR3_DMAT     7
#define UART_CR3_RTSE     8
#define UART_CR3_CTSE     9
#define UART_CR3_CTSIE    10
#define UART_CR3_ONEBIT   11


/* =========================================================
 * USART SR FLAG MACROS
 * ========================================================= */

#define UART_FLAG_PE      (1U << UART_SR_PE)
#define UART_FLAG_FE      (1U << UART_SR_FE)
#define UART_FLAG_NE      (1U << UART_SR_NE)
#define UART_FLAG_ORE     (1U << UART_SR_ORE)
#define UART_FLAG_IDLE    (1U << UART_SR_IDLE)
#define UART_FLAG_RXNE    (1U << UART_SR_RXNE)
#define UART_FLAG_TC      (1U << UART_SR_TC)
#define UART_FLAG_TXE     (1U << UART_SR_TXE)
#define UART_FLAG_LBD     (1U << UART_SR_LBD)
#define UART_FLAG_CTS     (1U << UART_SR_CTS)


/* =========================================================
 * USART CR1 INTERRUPT ENABLE MACROS
 * ========================================================= */

#define USART_IT_IDLEIE    (1U << UART_CR1_IDLEIE)
#define USART_IT_RXNEIE    (1U << UART_CR1_RXNEIE)
#define USART_IT_TCIE      (1U << UART_CR1_TCIE)
#define USART_IT_TXEIE     (1U << UART_CR1_TXEIE)
#define USART_IT_PEIE      (1U << UART_CR1_PEIE)


/* =========================================================
 * USART CR3 INTERRUPT ENABLE MACROS
 * ========================================================= */

#define USART_IT_EIE       (1U << UART_CR3_EIE)
#define USART_IT_CTSIE     (1U << UART_CR3_CTSIE)

/******************************************************************************************************
 *                  3.APIs supported by this driver
 *                  For more info about api check fun defin
 *
 **************************************************************************************************/

/* 1. clock enable */
void UART_periClockControl(UART_RegDef_t *pI2Cx,uint8_t EnorDi);
void UART_periPheralControl(UART_RegDef_t *pI2Cx,uint8_t EnorDi);   // bus clock


/*
 * UART - INIT , DEINT
 */

void UART_Init(UART_Handle_t *pUARTHandle);
void UART_DeInit(UART_RegDef_t*pUARTx);



/*
 * UART - polling - TX , RX
 */


void UART_SendData(UART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len);
void UART_ReceiveData(UART_Handle_t *pUARTHandle,uint8_t *pRxBuff,uint32_t Len);
void UART_SetBaudRate(UART_RegDef_t *pUARTx , uint32_t BaurdRate);

// STATUS FLAGS
uint8_t UART_GetFlagStatus(UART_RegDef_t*pUARTx,uint16_t FlagName);
void UART_ClearFlag(UART_RegDef_t *pUARTx, uint32_t FlagName);



/*
 * UART - Interrupts - TX , RX
 */

uint8_t UART_SendDataIT(UART_Handle_t *pUARTHandle, uint8_t *pTxBuff, uint32_t Len);
uint8_t UART_ReceiveDataIT(UART_Handle_t *pUARTHandle,uint8_t *pRxBuff,uint32_t Len);



/*
 * UART - Interrupts
 */
void UART_IRQInterruptConfig(uint8_t IRQNumber , uint8_t EnorDi);
void UART_IRQPriorityConfig(uint8_t IRQNumber , uint8_t IRQPriority);
void UART_IRQHandling(UART_Handle_t *pUARTHandle);


/*
 * UART - Application call back
 */

void UART_ApplicationEventCallback(UART_Handle_t *pUARTHandle,uint8_t AppEv);


#endif /* INC_STM32_UARTDRIVER_H_ */
