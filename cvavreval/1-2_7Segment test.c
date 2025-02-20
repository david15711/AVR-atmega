/*******************************************************
This program was created by the CodeWizardAVR V4.03 
Automatic Program Generator
© Copyright 1998-2024 Pavel Haiduc, HP InfoTech S.R.L.
http://www.hpinfotech.ro

Project : 
Version : 
Date    : 2024-09-23
Author  : 
Company : 
Comments: 


Chip type               : ATmega128
Program type            : Application
AVR Core Clock frequency: 16.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 1024
*******************************************************/

// I/O Registers definitions
#include <mega128.h>
#include <delay.h>

unsigned int pressed = 0;

void main(void)
{
    DDRC= 0xFF;
    PORTC = 0;

    while (1)
    {         
        if( (PING&0x04) && !pressed){ 
            pressed = 1;
            PORTC ^= 0x01;
            delay_ms(20);
            while( PING&0x04 );            //polling sleep
        }                      
        
        else if ( (PING&0x08) && !pressed) {   
            pressed = 1;
            PORTC ^= 0x02;              
            delay_ms(20);
            while( PING&0x08 );            //polling sleep
        }
        pressed = 0;
    }
}
