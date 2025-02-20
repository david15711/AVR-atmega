/*
 * GccApplication4.c
 *
 * Created: 2022-11-18 오후 4:57:08
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART_support.h"
//가변저항 ADC to LED PWM
FILE OUTPUT \
= FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT \
= FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

void ADC_init(unsigned char channel){
	ADMUX |= 0x40;
	ADMUX = ((ADMUX & 0xE0) | channel);
	ADCSRA = 0xE7;
}

int read_ADC(void){
	while( !(ADCSRA & (1<<ADIF)) );
	return ADC;
}

int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	int read;
	UART0_init;
	ADC_init(0);
	DDRB = 0x10;
	TCCR0 =	0x7f;	//분주비 1024, compare match and clear TCNT
    while (1){
		read = read_ADC();
		printf("%d\n\r", read);
		
		read = read>>2;	//ADC 10비트와 OCR0 8비트 맞추기
		OCR0 = read;
		
		_delay_ms(1000);	
	}
	return 0;
}

