/*
 * GccApplication4-2.c
 *
 * Created: 2022-11-18 오후 5:49:00
 * Author : MS
 */ 
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

void ADC_init(unsigned char channel){
	ADMUX |= 0x40;
	ADMUX = ((ADMUX & 0xE0) | channel);
	ADCSRA = 0xE7;
}

ISR(ADC_vect){
	float temperture = (float)ADC * 500.0f / 1023.0f;
	printf("현재 온도는 %4.2f도 입니다.\n\r", temperture);
	if(temperture>=27.0f){
		PORTB |= 0x04;
	} else {PORTB = 0;}
}

ISR(TIMER0_OVF_vect){
	count++;
	TCNT0 = 0;
	if(count == 31){
		count = 0;
		ADCSRA |= (1<<ADSC);
	}
}


//인터럽트 없이 타이머로 기다려서 온도 받아 송신하기
int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	uint8_t prev = 0;
	float temperture;
	ADC_init(0);
	UART1_init();
	TCCR0 = 0x07;
	TCNT0 = 0;
	DDRB = 0xfc;
    while (1){
		while(count<31){
			if(prev>(uint8_t)TCNT0){
				count++;
			}
			prev = TCNT0;
		}
		while( !(ADCSRA & (1<<ADIF)) );
		temperture = (float)ADC * 500.0f / 1023.0f;
		printf("현재 온도는 %4.2f도 입니다.\n\r", temperture);
		if(temperture>=27.0f){
			PORTB |= 0x04;
		} else {PORTB = 0;}
		count = 0;
    }
	return 0;
}
/*
//오버플로우 인터럽트로 0.5초 기다린 후 온도 받아 송신
int main(void){
	stdout = &OUTPUT;
	stdin = &INPUT;
	ADC_init(0);
	ADCSRA &= ~(1<<ADFR);	//프리러닝 off
	ADCSRA |= (1<<ADIE);	//ADC 인터럽트 on
	UART1_init();
	TCCR0 = 0x07;
	TIMSK = 0x01;
	sei();
	DDRB = 0xfc;
	while(1);
	return 0;
}*/