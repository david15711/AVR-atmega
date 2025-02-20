

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

float mapping(int x, int in_MIN, int in_MAX, int out_MIN, int out_MAX){
	return (((float)x-in_MIN) * (out_MAX - out_MIN)/(in_MAX - in_MIN) + out_MIN);
}

void ADC_init(unsigned char channel){
	ADMUX |= (1 << REFS0);	//기준 전압 AVCC
	ADCSRA |= 0x07;			//분주비 설정,
	ADCSRA |= (1<<ADEN);	//ADC
	ADCSRA |= (1<<ADFR);	//프리러닝 모드 on
	ADCSRA &= ~(1<<ADIE);	//interrupt disable
	ADMUX = ( (ADMUX & 0xE0) | channel);
	ADCSRA |= (1<<ADSC);	//ADC init
	//sei();
}
/*
ISR(ADC_vect){
	float read;
	
	read = ADC;
	printf("%f\n\r", read);
	_delay_ms(500);
}*/

int main(void){
	stdout = &OUTPUT;
	stdin = &INPUT;
	UART0_init();
	ADC_init(0);
	float read = 0, get_ = 0;
	float print;
	while (1){
		for(int i=0; i<20; i++){
			get_ = ADC;
			//printf("%f  ", get_);
			read += get_;
			
		}
		print = read / 20.0;
		printf("average: %f\n\r", print);
		
		read = 0;
		_delay_ms(500);
	}
	return 0;
}