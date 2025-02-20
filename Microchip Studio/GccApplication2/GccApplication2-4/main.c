/*
 * GccApplication2-4.c
 *
 * Created: 2022-10-28 오후 5:33:44
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART_support.h"

void UART1_init(void);
void UART1_transmit(char data);
unsigned char UART1_receive(void);
void UART1_print_1_byte_number(uint8_t n);

int main(void)
{
    UART1_init();
	char str[] = "test using UART1 Library";
	uint8_t num = 128;
	UART1_print_string(str);
	UART1_print_string("\n\r");
	UART1_print_1_byte_number(num);
	UART1_print_string("\n\r");
    while (1){}
	return 0;
}

