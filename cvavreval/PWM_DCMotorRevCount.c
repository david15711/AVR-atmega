#include <mega128.h>
#include <delay.h>
#include <interrupt.h>

#define DSTop 8000
#define Sw2on (PING & 0x04)
#define Sw3on (PING & 0x08)

unsigned char seg0[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67}; // 7-segment set 0~9
unsigned char seg1[4] = { 0x08, 0x04, 0x02, 0x01 };

long int i = 0;
int j = 0, ppr = 0, revol = 0;
unsigned char PE67 = 0;

interrupt [EXT_INT6] void ext_int6(void)
{
    PE67 = PORTE >> 6;
    if(PE67 == 0x03) i--;
    else if (PE67 == 0x01) i++;
    else if (PE67 == 0) i--;
    else i++;
}

interrupt [EXT_INT7] void ext_int7(void)
{
    PE67 = PORTE >> 6;
    if(PE67 == 0x03) i++;
    else if (PE67 == 0x01) i--;
    else if (PE67 == 0) i++;
    else i--;
}

void main(void)
{
    DDRG |= 0x03;
    DDRB |= 0xE0;
    DDRD |= 0xFF;
    DDRA |= 0xFF;
    DDRE |= 0x00;  
    PORTB = 0;
    
    TCCR1A |= 0xA8;  
    TCCR1B |= 0x11;    
    ICR1 = DSTop;
    TCNT1 = 0;
    OCR1A = 0;
    
    EICRB = 0x50;
    EIMSK = 0x40;
    SREG |= 0x80;
    
    ppr = 16 * 30 * 2;

    while(1)
    {
        if(Sw2on)
        {
            delay_ms(200);
            if(OCR1A < DSTop) OCR1A += 1000;
            PORTG = 0x01;
            OCR1B = 0;
            OCR1CH = OCR1AH;
            OCR1CL = OCR1AL;
        }
        if(Sw3on)
        {
            delay_ms(200);
            if(OCR1A > 0) OCR1A -= 1000;
            PORTG = 0x02;
            OCR1B = 0;
            OCR1CH = OCR1AH;
            OCR1CL = OCR1AL;
        }                     
        
        PORTD = ( 1 << (OCR1A/1000) ) - 1;  
        revol = ((i / ppr)>0) ? i/ppr : -i/ppr ;
        
        for(j = 0; j<4; j++)
        {
            DDRC = seg1[j];             
            //DDRC = ( 1<< (3-j) ); 로 대체 가능         
            if(j==0) PORTA = seg0[ revol%10 ];
            if(j==1) PORTA = seg0[ (revol%100)/10 ];
            if(j==2) PORTA = seg0[ (revol%1000)/100 ];
            if(j==3) PORTA = seg0[ (revol%10000)/1000 ];
            delay_ms(5);
        }
    }
}