/*
 * UART_Zero_copy.c
 *
 *  Created on: Aug 17, 2026
 *      Author: vyaas
 */




#include "UART_Zero_copy.h"


uint8_t t;     // token count

//1. token  parser

int cmd_tokenize(const char *buf, int len, tok_t *tokens, int max_tokens)
{
	uint8_t action,n=1,cmd,i;

	for(i=0;i<len;i++)
	{
		if(buf[i]=='\n')
		{
			//end of message
			tokens[t].end = i-1;	 // end of value phase

			t++;
			tokens[t].type = TOKEN_EOL;

			tokens[t].start = i;


		}
		//cmd phase
		else if(buf[i]=='L' && buf[i+1] =='E' && buf[i+2] =='D')
		{
			tokens[t].type = TOKEN_WORD;
			tokens[t].start = i;
			cmd = LED;   //0x1
		}
		else if(buf[i] == '_')
		{
			tokens[t].end = i-1;   // end of cmd
			t++;   // next token for opeartion start nd end
			if(n==1)     // start off action operation
			{
				if(buf[i+1]=='O' && buf[i+2] =='N')
				{
					tokens[t].type = TOKEN_WORD;

					tokens[t].start=i+1;
					action = ON; //ON  macro
					n=2;
				}
				else if(buf[i+1]=='O' && buf[i+2] =='F' &&  buf[i+3] =='F')
				{
					tokens[t].type = TOKEN_WORD;

					tokens[t].start=i+1;
					action = OFF; //OFF  macro
					n=2;
				}
			}

			else if(n==2)
			{
				tokens[t].end = i-1;   // end of action
				t++;   // next token for value start nd end
				// type is number type TOK_NUM
				if (buf[i] == '1' && buf[i + 1] == '0' && buf[i + 2] == '0' && buf[i + 3] == '0')
				{
					// value is 1000
					tokens[t].type = TOKEN_NUMBER;
					tokens[t].start = i;

				}

			}


		}
	}
	if (t >= max_tokens)
	{
		return -1;
	}
	else
	{
		return t;
	}
}
