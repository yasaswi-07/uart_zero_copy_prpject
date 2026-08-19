/*
 * UART_Rx.c
 *
 *  Created on: Aug 17, 2026
 *      Author: vyaas
 */


#include "UART_Zero_copy.h"

#include "stm32401xx.h"

#include"stm32_uartdriver.h"


#define MAX_TOKENS 8

tok_t tokens[MAX_TOKENS];



