/*
 * GccApplication3-1-1.c
 *
 * Created: 2022-11-14 오후 10:50:28
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "UART_support.h"
//serial echo 앵무새
int main(void)
{
	uint8_t read;
	UART0_init();
    while (1){
		while((UCSR0A & (1 << UDRE0))){
			read = UART0_receive();
			UART0_transmit(read);
		}
	}
}

