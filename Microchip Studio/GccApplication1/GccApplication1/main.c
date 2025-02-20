/*
 * GccApplication1.c
 *
 * Created: 2022-09-27 오후 9:24:59
 * Author : Jaenam Kim
 *			student number: 60191859
 * Electronic Engineering Dept.
 * Myongji University
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	
    DDRA |= 0x01;
	PORTA = 0x01;
    while (1) 
    {
		PORTA |= 0x01;	// PORTA = PORTA | 0x01 즉 마스크 연산
		_delay_ms(500);
		PORTA &= ~0x01;	// 1 없애기
		_delay_ms(500);
    }
	return 0;
	
}

