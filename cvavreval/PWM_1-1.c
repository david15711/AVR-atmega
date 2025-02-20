#include <mega128.h>
#include <delay.h>
#include <interrupt.h>

#define DSTop 8000
#define Sw2on ( PING & 0x04 )
#define Sw3on ( PING & 0x08 )    

unsigned int i = 0;

interrupt [TIM1_CAPT] void tim1_capt(void){
    if( ++i == 500 ){
        if( (PORTD <<= 1) == 0) PORTD = 0x01;
        i = 0;
    }
}

void main(void){
    DDRG = 0x03;
    TCCR1A |= 0x80;
    TCCR1B |= 0x11;
    ICR1 = DSTop;
    TCNT1 = 0;
    DDRB |= 0x20;
    PORTB = 0;
    DDRD |= 0xFF;
    TIMSK |= 0x20;
    SREG |= 0x80;
       
    while(1){
        do
        {
            OCR1A += 1000;
            delay_ms(150);
        }
        while (OCR1A != DSTop);
        
        do
        {
            OCR1A -= 1000;
            delay_ms(150);
        }
        while (OCR1A != 0   );
            
    }
}