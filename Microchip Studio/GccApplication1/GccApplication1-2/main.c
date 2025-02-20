/*
 * GccApplication1-2.c
 *
 * Created: 2022-10-19 오전 10:13:46
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRB = 0xfc;
	PORTB = 0x02;
	int i;
	char direction = 1;
    while (1) 
    {	
		direction = (PINB & 0x03) % 2;
		//switch case로 제어하기
		while( direction != 3){
			switch(direction){
				case 1: {
					for(i=0;i<500;i++){
						_delay_ms(1);
						if ((PINB & 0x03) == 0x02){
							direction = 0;
						}
					}
					PORTB = PORTB << 1;	//2 3 4 5				
					if(PORTB >= 0x40){	//6 도달시
						PORTB = 0x04;	//2로 이동
						continue;
					}
				}
						
				case 0: {
					for(i=0;i<500;i++){
						_delay_ms(1);
						if ((PINB & 0x03) == 0x01){
							direction = 1;
						}
					}			
					PORTB = PORTB >> 1;	//5 4 3 2					
					if(PORTB <= 0x02){	//1 도달시
						PORTB = 0x20;	//5로 이동
						continue;
					}
				}
				
				default:
				continue;
			}
		}
		//		
    }
	return 0;
}

