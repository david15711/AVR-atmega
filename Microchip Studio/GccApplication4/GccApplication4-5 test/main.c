/*
 * GccApplication4-5 test.c
 *
 * Created: 2022-12-02 오후 4:30:15
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

int read_ADC(void){
	while( !(ADCSRA & (1<< ADIF) ) );
	return ADC;
}

void ADC_init(unsigned char channel){
	ADMUX |= (1 << REFS0);	//기준 전압 AVCC
	ADCSRA |= 0x07;			//분주비 설정,
	ADCSRA |= (1<<ADEN);	//ADC
	ADCSRA |= (1<<ADFR);	//프리러닝 모드
	ADMUX = ( (ADMUX & 0xE0) | channel);
	ADCSRA |= (1<<ADSC);	//ADCSRA = 0xA7 ADMUX = 0x40
}


int main(void) {
	stdout = &OUTPUT;
	stdin = &INPUT;
	DDRC = 0x01;
	UART1_init();
	ADC_init(0);
	TCCR0 = 0x07;	//정상모드, 분주비 1024
	TIMSK = 0x00;
	TIFR = 0;
	DDRB = 0xff;
	int count;
	int read;

	
    while (1) {
		//trigger
		PORTC = 0x01;
		_delay_us(10);
		PORTC &= ~0x01;
		while(!(PINC & 0x02));
		TCNT0 = 0;
		while((PINC & 0x02));
		count = TCNT0;	//time (0, 0.016320)
		float distance = 340/2 * count * 0.000064;
		
		
		if(distance>0.3){
			printf("The distance is %.6f [m] \n\r", distance);
		} else if (distance<=0.3){
			read = read_ADC();
			printf("The distance is %.6f [m] : The CdS value is %d \n\r", distance, read);
		}
		if( (read>650) && (distance<=0.3) ){
			PORTB |= 0x01;
			} else {
			PORTB &= ~0x01;
		}
		_delay_ms(1000);
    }
}

