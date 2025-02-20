/*
 * GccApplication2.c
 *
 * Created: 2022-09-28 오전 11:02:24
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

//_delay_ms(500);
int main(void)
{
	DDRB = 0xfc;	//11111100
	PORTB = 0;
	//int i;
    while (1) 
    {
		char sw = PINB & 0x03;
		//스위치 눌러 포트B 0, 1 때리고 2, 3의 LED 키기
		//pull down resistance
		if( (sw & 0x01) ){
			PORTB |= 0x04;	//00000100 
		}
		else if( (sw & 0x02) ){
			PORTB |= 0x08;	//00001000
		}
		else if( (sw & 0x03) ){
			PORTB |= 0x12;
		}
		PORTB = 0;
		
		
		
		
		/*
		DDRB = 0xff;
		PORTB = 0x01;
		// 1초마다 0 1 2 3 4 5 6 7 건너가며 켜짐
		while( !(PORTB == 0x00) ) {
			PORTB = PORTB << 1;
			_delay_ms(1000);
		}
		continue;

		// 1 3 5 7 순으로 켜지고 꺼지는 거 (안됨)
		for(i=8;i<512;i*=4){
			_delay_ms(1000);
			PORTB += i;
		}
		for(i=8;i<512;i*=4){
			_delay_ms(1000);
			PORTB -= i;
		}
		*/
    }
	
	return 0;
}

