#include <mega128.h>
#include <delay.h>
#include <interrupt.h>

#define DSTop 8000
#define Sw2on ( PING & 0x04 )
#define Sw3on ( PING & 0x08 )    

unsigned int pressed, LEDLevel = 0;           //LEDlevel: 0~8 level
unsigned char seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x6F };
 
void FNDn8LED(void){
    PORTA = seg[LEDLevel];
    PORTD = (1<<LEDLevel) - 1;
}

void main(void){
    DDRA |= 0xFF;           //7-segment output
    DDRG |= 0x03;           //PORTG 1:0, Orange LED
    DDRB |= 0xE0;           //PORTB 7:5 출력, RGB LED
    PORTB = 0;
    DDRD |= 0xFF;           //8 LED 출력
    DDRC |= (1<<3);         //FND 1의 자리   
    PORTC = 0;
    
    TCCR1A |= 0xA8;         //COMA,COMB,COMC Clear on compare match
    TCCR1B |= 0x11;         //Phase and Frequency Correct PWM, Clock Select clk/1 prescaler
    ICR1 = DSTop;           //set TCNT1 top
    TCNT1 = 0;
    OCR1A = 0;
    OCR1B = 0;
    OCR1CH = 0;
    OCR1CL = 0;
       
    while(1){
        if( Sw2on && !pressed ){    
            delay_ms(20);           //debouncing delay 
            pressed = 1;
            PORTG = 1;
            if(LEDLevel == 8) continue;
            LEDLevel++;
        }
        else if( Sw3on && !pressed ){
            delay_ms(20);           //debouncing delay 
            pressed = 1;
            PORTG = 2;
            if( !LEDLevel ) continue;
            LEDLevel--;
        }  
        OCR1A = LEDLevel * 1000;
        OCR1B = OCR1A;
        OCR1CH = OCR1AH;
        OCR1CL = OCR1AL;
        FNDn8LED();    
        if( !Sw2on && !Sw3on ) pressed = 0;//debouncing  
    }
}