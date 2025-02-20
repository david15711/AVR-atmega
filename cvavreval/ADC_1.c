#include <mega128.h>
#include <delay.h>
#define Sw2on (PING & 0x04)
#define Sw3on (PING & 0x08)
const unsigned char segment_num[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67}; // 7-segment set 0~9
const unsigned char segment_control[4]={0xf7,0xfb,0xfd,0xfe}; 
unsigned char pressed = 0, fndDisplayMode;
int cnt, data = 0, fndBuffer;
float voltage;
float mapping(int x, int in_MAX, int in_MIN, int out_MAX, int out_MIN)
{
    return ((float)x - (float)in_MIN) * ((float)out_MAX - (float)out_MIN) / ((float)in_MAX - (float)in_MIN) + (float)out_MIN;
}
interrupt [ADC_INT] void adc_int(void)
{
    data = (int)ADCL + ((int)ADCH << 8 );                               //
    voltage = mapping(data, 1024, 0, 5, 0) + 1.0/1024/2;                //
    PORTD = (1<< ( (data+1)>>7) ) - 1;                                  //
    OCR1A = 1023 - data;
    OCR1B = data;                                  
}
interrupt [TIM0_OVF] void tim0_ovf(void)
 {
    if(!fndDisplayMode) fndBuffer = data;
    else fndBuffer = (int)(voltage*1000);
    PORTC = segment_control[cnt]; 
    switch(cnt)
    {   
        case 0:
            PORTA = segment_num[fndBuffer%10];
            break;
        case 1:
            PORTA = segment_num[(fndBuffer%100)/10];
            break;
        case 2:
            PORTA = segment_num[(fndBuffer%1000)/100];
            break;
        case 3:
            PORTA = segment_num[(fndBuffer%10000)/1000];
            if(fndDisplayMode) PORTA += 0x80;
            break;
        default:
            break;
    }             
    if(++cnt == 4) cnt = 0;
    TCNT0 = 0xb2;
}
void main(void)
{
    DDRC |= 0xFF;
    DDRA |= 0xFF;        
    DDRF = 0;     
    PORTC = 0;
    PORTA = 0;              
    DDRG |= 0x03;  
    DDRD |= 0xFF;
    DDRB |= 0x60;           //RGB LED
    PORTB = 0;   
    ADMUX |= 0x47;
    ADCSRA |=0xCF;
    TIMSK |= 0x01;
    TCCR0 |= 0x07;
    TCNT0 |= 0xb2;   
    TCCR1A |= 0xA0;
    TCCR1B |= 0x11;   
    TCNT1 = 0;
    ICR1 = 1023;
    SREG |= 0x80;
    while(1)
    {
        ADCSRA |= 0x40;   
        if(Sw2on && !pressed)
        {
            pressed = 1;
            fndDisplayMode = 1;
            PORTG = 0x01;
        }
        if(Sw3on && !pressed)
        {          
            pressed = 1;
            fndDisplayMode = 0;
            PORTG = 0x02;
        }               
        delay_ms(10);
        if( !Sw2on && !Sw3on ) pressed = 0;//debouncing             
    }
}