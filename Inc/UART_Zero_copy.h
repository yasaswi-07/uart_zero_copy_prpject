/*
 * UART_Zero_copy.h
 *
 *  Created on: Aug 17, 2026
 *      Author: vyaas
 */

#ifndef UART_ZERO_COPY_H_
#define UART_ZERO_COPY_H_

#include <stdint.h>



//token type
typedef enum
{
    TOKEN_WORD,   //0
    TOKEN_NUMBER,   //1
    TOKEN_EOL,     //2

} tok_type_t;


//token stsruct

typedef struct
{
	tok_type_t type;
	uint8_t start;
	uint8_t end;

}tok_t;


//command macros
#define LED          0x1
#define BUZZER       0x2
#define UARTBAUD     0X3




//ACTION macros
#define  ON        0x1
#define  OFF       0x2
#define  BLINK     0x3



//TOEKN count greater than max tokensize
//#define ERROR_TOKEN






int cmd_tokenize(const char *buf, int len, tok_t *tokens, int max_tokens);

#endif /* UART_ZERO_COPY_H_ */
