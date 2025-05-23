#include <mega128.h>
#include <delay.h>
#include <interrupt.h>

#define DSTop 8000
#define Sw2on ( PING & 0x04 )
#define Sw3on ( PING & 0x08 )    

unsigned int i = 0;  
unsigned int tcnt;      
unsigned char seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x6F };

interrupt [TIM1_CAPT] void tim1_capt(void){

        tcnt = TCNT1;
    if( ++i == 500 ){
        if( (PORTD <<= 1) == 0) PORTD = 1;
        i = 0;         
        DDRC = (1<<3);
        PORTA = seg[tcnt%10];
        delay_ms(200);
        DDRC = (1<<2);                                                               7967
        PORTA = seg[(tcnt%100)/10];   
        delay_ms(200);
        DDRC = (1<<1);
        PORTA = seg[(tcnt%1000)/100]; 
        delay_ms(200);  
        DDRC = (1<<0);
        PORTA = seg[(tcnt%10000)/1000]; 
        delay_ms(200);               
        DDRC = (1<<3);
        PORTA = seg[(tcnt%100000)/10000];
        delay_ms(200);
        DDRC = (1<<2);
        PORTA = seg[(tcnt%1000000)/100000];   
        delay_ms(200);
    }
}

void main(void){
    DDRG = 0x03;
    DDRA = 0xFF;            //7segment
    DDRC = 0x08;            //7segment 0V ���
    DDRG = 0x03;            //button
    DDRB |= 0xE0;           //RGB LED
    PORTB = 0;
    DDRD |= 0xFF;           //8 LED
    
    TCCR1A |= 0xA8;
    TCCR1B |= 0x11;
    ICR1 = DSTop;
    TCNT1 = 0;
    OCR1A = DSTop;
    OCR1B = 0;
    OCR1CH = 0;
    OCR1CL = 0;
    TIMSK |= 0x20;
    SREG |= 0x80;
       
    while(1){                 
        do
        {
            OCR1B += 1;   
            while( TCNT1 );
        }
        while (OCR1B != DSTop);
        
        do
        {
            OCR1A -= 1;
            while( TCNT1 );
        }
        while (OCR1A != 0 );
        
        do
        {   
            if( !(++OCR1CL) ){
                OCR1CH++;
            }
            while( TCNT1 );
        }
        while ( ((OCR1CH << 8) | OCR1CL) != DSTop);
        
        do
        {
            OCR1B -= 1;
            while( TCNT1 );
        }
        while (OCR1B != 0   );
        
        do
        {
            OCR1A += 1;
            while( TCNT1 );
        }
        while (OCR1A != DSTop);
        
        do
        {   
            if ( --OCR1CL == 255 ){
                OCR1CH--;
            }
            while( TCNT1 );
        }
        while ( ((OCR1CH << 8) | OCR1CL) != 0   );    
    }
}