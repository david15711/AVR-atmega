/*
 * GccApplication3-1.c
 *
 * Created: 2022-11-05 오후 7:49:07
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include "UART_support.h"

int main(void)
{
	char receivestring, trash;
	UART0_init();
	while(UCSR0A & (1<<RXC0))
	trash = UDR0;
	UART0_print_string((char*)"serial 연결 성공\n\r");
	
    while (1){
		receivestring = UART0_receive();
		switch(receivestring){
			case 'a':
				UART0_print_string((char *)"atmega128 a 수신함\n\r");
				while(UCSR0A & (1<<RXC0))
					trash = UDR0;
				_delay_ms(1000);
				break;
			case 'b':
				UART0_print_string((char *)"atmega128 b 수신함\n\r");
				while(UCSR0A & (1<<RXC0))
				trash = UDR0;
				_delay_ms(1000);
				break;
			default:
				UART0_transmit('\n');
				while(UCSR0A & (1<<RXC0))
				trash = UDR0;
				_delay_ms(1000);
				break;
		}
    }
}

