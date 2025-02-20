/*
 * GccApplication4-3.c
 *
 * Created: 2022-11-23 오전 10:53:30
 * Author : MS
 */ 
// 타이머 카운터로 초음파 거리 재기
#define F_CPU 16000000L
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART_support.h"

FILE OUTPUT \
= FDEV_SETUP_STREAM(UART1_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT \
= FDEV_SETUP_STREAM(NULL, UART1_receive, _FDEV_SETUP_READ);

int count = 0;
ISR(TIMER0_OVF_vect){
	count++;
}

int main(void)
{	
	stdout = &OUTPUT;
	stdin = &INPUT;
	DDRC = 0x01;
	UART1_init();
	TCCR0 = 0x07;	//정상모드, 분주비 1024
	TIMSK = 0x00;
	TIFR = 0;
	cli();
	
    while (1) {
		//trigger
		PORTC = 0x01;
		_delay_us(10);
		PORTC &= ~0x01;
		while(!(PINC & 0x02)) ;
		TCNT0 = 0;
		while((PINC & 0x02)) ;
		count = TCNT0;	//time (0, 0.016320)
		float distance = 340/2 * count * 0.000064;
		printf("측정한 거리: %f\n\r", distance);
		_delay_ms(500);
    }
}