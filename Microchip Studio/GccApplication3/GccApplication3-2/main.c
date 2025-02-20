/*
 * GccApplication3-2.c
 *
 * Created: 2022-11-07 오후 1:49:50
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART_support.h"

FILE OUTPUT \
= FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT \
= FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

int mapping(int x, int in_MIN, int in_MAX, int out_MIN, int out_MAX){
	return (((float)x-in_MIN) * (out_MAX - out_MIN)/(in_MAX - in_MIN) + out_MIN);
}

void ADC_init(unsigned char channel){
	ADMUX |= (1 << REFS0);	//기준 전압 AVCC
	ADCSRA |= 0x07;			//분주비 설정,
	ADCSRA |= (1<<ADEN);	//ADC
	ADCSRA &= ~(1<<ADFR);	//프리러닝 모드 off
	ADCSRA |= (1<<ADIE);	//interrupt enable
	ADMUX = ( (ADMUX & 0xE0) | channel);
	//ADCSRA |= (1<<ADSC);	//ADCSRA = 0xA7 ADMUX = 0x40
	sei();
}

ISR(ADC_vect){
	int read;
	float voltage;
	float angle;
	
	read = ADC;
	voltage = 5.0 * (float)read / 1023.0;
	angle = voltage * 52.0;
	printf("ADC: %d Voltage: %6.1f Angle: %6.1f\n\r", read, voltage, angle);
	_delay_ms(1000);
}

int main(void){
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	UART0_init();
	ADC_init(0);
	printf("\n\rMyongji University, Electron engineering, 60191859 Kim jaenam\n\n\r");
	while (1){
		ADCSRA |= (1<<ADSC);
		;
		;
	}
	return 0;
}