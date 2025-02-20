/*
 * GccApplication1-3.c
 *
 * Created: 2022-11-03 오후 6:23:42
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	
	DDRB = 0x0fe;
	int state = 0, value = 0, oldvalue = 0;

    while (1) 
    {
		if((PINB & 0x03) == 0x01){
			value = (PINB & 0x03);
		}	else (value = 0);

		if( oldvalue == 0 && value == 1){
			state = (state + 1 ) % 2;
		}
		if(state == 1){
			PORTB |= 0x04;
		} else {
			PORTB &= ~0x04;
		}
		oldvalue = value;
    }
}

