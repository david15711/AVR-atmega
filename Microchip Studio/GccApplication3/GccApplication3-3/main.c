/*
 * GccApplication3-3.c
 *
 * Created: 2022-11-09 오전 11:33:00
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

void ADC_init(unsigned char channel){
	ADMUX |= (1 << REFS0);	//기준 전압 AVCC
	ADCSRA |= 0x07;			//분주비 설정,
	ADCSRA |= (1<<ADEN);	//ADC
	ADCSRA &= ~(1<<ADFR);	//프리러닝 모드 off
	ADCSRA |= (1<<ADIE);	//interrupt enable
	ADMUX = ( (ADMUX & 0xE0) | channel);
}

void init_INT3(void){
	EIMSK |= (1<<INT3);
	EICRA |= (0x80);
	DDRD = 0x00;
	PORTD = 0xff;
	sei();
}

ISR(ADC_vect){
	int read;
	float voltage;
	
	read = ADC;
	voltage = 5.0 * (float)read / 1023.0;
	printf("ADC: %d Voltage: %6.1f\n\r", read, voltage);
	_delay_ms(1000);
}

ISR(INT3_vect){
	EIMSK &= ~(1<<INT3);
	ADCSRA |= (1<<ADSC);
}

int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	UART0_init();
	ADC_init(0);
	init_INT3();
	DDRB = 0xfe;
    while(1) {
		EIMSK |= (1<<INT3);
		if((PINB & 0x01) == 0x01){	//ADC 인터럽트 테스트용 버튼입력
			PORTB |= 0x3c;
			ADCSRA |= (1<<ADSC);
			PORTB = 0x00;
		}
    }
}

