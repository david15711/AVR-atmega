/*
 * UART1.c
 *
 * Created: 2022-10-28 오후 6:37:41
 * Author : MS
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include "UART_support.h"
#include <string.h>

int main(void)
{	
	UART0_init();
	
	char str[] = "test using UART0 Library";
	uint8_t num = 128;
	
	UART0_print_string(str);
	UART0_print_string("\n\r");
	
	UART0_print_1_byte_number(num);
	UART0_print_string("\n\r");
	
	UART1_init();
	
	strcpy(str, "test using UART1 Library");

	UART1_print_string(str);
	UART1_print_string("\n\r");
	
	UART1_print_1_byte_number(num);
	UART1_print_string("\n\r");
	
	
	while (1){}
	return 0;
}

