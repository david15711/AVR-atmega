#include <mega128.h>
#include <delay.h>
#include <interrupt.h>

#define DSTop 8000
#define Sw2on ( PING & 0x04 )
#define Sw3on ( PING & 0x08 )    

unsigned int i = 0, mode = 0;           //mode description: 0 == R + G, 1 == G-R, 2 == G + B, 3 == B - G, 4 == G + R, 5 == R -G 
unsigned char seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x6F };
void displayFND(int point, unsigned int number);

interrupt [TIM1_CAPT] void tim1_capt(void){
    if( ++i == 500 ){
        if( (PORTD <<= 1) == 0) PORTD = 1;
        i = 0;    
    }
}

interrupt [TIM1_OVF] void tim1_ovf(void)
{
    switch (mode)
    {
           case 0:{
            if(OCR1B == DSTop) {
                mode +=1;
                break;
            }
            OCR1B++;
            break;
           }

           case 1:{
            if( !OCR1A ) {
                mode +=1;
                break;
            }
            OCR1A--;
            break;
           }
           
           case 2:{
            if( ((OCR1CH << 8) | OCR1CL) == DSTop) {
                mode += 1;
                break;
            }
            if( !(++OCR1CL) ){
                OCR1CH++;
            }
            break;
           }
           
           case 3:{
            if( !OCR1B ) {
                mode += 1;
                break;
            }
            OCR1B--;
            break;
           }
           
           case 4:{
            if( OCR1A == DSTop ) {
                mode += 1;
                break;
            }
            OCR1A++;
            break;
           } 
           
           case 5:{
            if( !((OCR1CH << 8) | OCR1CL) ) {
                mode = 0;
                break;
            }
            if ( --OCR1CL == 255 ){
                OCR1CH--;
            }
            break;
           }
    }
}

void displayFND(int point, unsigned int number){
    int j;
    for(j = 0; j<4; j++){
        DDRC = (1<<(3-j));
                
        if(j==0) PORTA = seg[number%10];
        if(j==1) PORTA = seg[(number%100)/10];
        if(j==2) PORTA = seg[(number%1000)/100];
        if(j==3) PORTA = seg[(number%10000)/1000];
        if(point == j) PORTA += 0x80;
        delay_ms(5);
    }
}

void main(void){
    DDRA = 0xFF;            //7segment
    DDRC = 0x08;            //7segment 0V Ãâ·Â
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
    TIMSK |= 0x24;
    SREG |= 0x80;
       
    while(1){                 
       ; 
    }
}