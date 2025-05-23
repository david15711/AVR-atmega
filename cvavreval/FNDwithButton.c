/*******************************************************
This program was created by the CodeWizardAVR V4.03 
Automatic Program Generator
� Copyright 1998-2024 Pavel Haiduc, HP InfoTech S.R.L.
http://www.hpinfotech.ro

Project : Two button to Two LED
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
/*
mode == 0 : display my student number
mode == 1 : count 0.1 second and display, clear count when mode change
use PING [n = 3:2] as button input to change mode
*/
#include <mega128.h>
#include <delay.h>
#include <interrupt.h>

unsigned int count = 0, FNDNumber = 0, pressed;
unsigned char seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x6F };

ISR(17){
    count++;
    TCNT0 = 0;
    if(count == 6){
        count = 0;
        FNDNumber++;
        FNDNumber %= 10000;
    }
}

int displayFNDwButton(int point, unsigned int number){
    int mode, i, j;
    for(j = 0; j<4; j++){
        DDRC = (1<<(3-j));
        PORTC = 0;
                
        if(j==0) PORTA = seg[number%10];
        if(j==1) PORTA = seg[(number%100)/10];
        if(j==2) PORTA = seg[(number%1000)/100];
        if(j==3) PORTA = seg[(number%10000)/1000];
        if(point == j) PORTA += 0x80;
        for(i = 0; i<5; i++){
            if( (PING&0x04) && !pressed){ 
                pressed = 1;
                mode = 1;      
            }                      
            if( (PING&0x08) && !pressed){   
                pressed = 1;
                mode = 0;         
            }
            delay_ms(1);
        }
    }
    pressed = 0;
    return mode;
}

void main(void)
{                         
    unsigned int mode = 0, lastMode = 0;
    DDRG= 0x03;
    DDRA = 0xFF;
    TCCR0 = 0x00;   //16000000/1024 = 15625 Hz, 64us
    TIMSK = 1;
    SREG |= 0x80;
    PORTG = 0;   
    PORTA = 0;

    while (1)
    {                                                
        if(mode == 0){
            TCCR0 = 0x00;
            TIMSK = 0;
            FNDNumber = 6019;
            lastMode = 0;
        }
        else if (mode == 1){
            TCCR0 = 0x07;   //16000000/1024 = 15625 Hz, 64us
            TIMSK = 1;
            if(lastMode == 0) FNDNumber = 0;
            lastMode = 1;
        }
        if(mode){     
            mode = displayFNDwButton( 1, FNDNumber);
        }
        else mode = displayFNDwButton( -1, FNDNumber);
    }
}
