#include <avr/io.h>
#include <stdio.h>

char Keypad_Read_Row(void){ //not used
  switch( PINF & 0xF0 ){
    case 0x70: return 0;
    case 0xB0: return 1;
    case 0xD0: return 2;
    case 0xE0: return 3;
    case 0xF0: return 4;
  }
}

char Keypad_Read_Col(void){ //not used
  PORTF = 0xF7;
  _delay_us(10);
  if((PINF & 0xF0) != 0xF0) return 0;
  PORTF = 0xFB;
  _delay_us(10);
  if((PINF & 0xF0) != 0xF0) return 1;
  PORTF = 0xFD;
  _delay_us(10);
  if((PINF & 0xF0) != 0xF0) return 2;
  PORTF = 0xFE;
  _delay_us(10);
  if((PINF & 0xF0) != 0xF0) return 3;
  else return 4;
}

int main(void){
  DDRF = 0x0F;
  PORTF = 0x00;
  DDRH = 0x40;    //OC2B PH6 output
  DDRG = 0x20;    //OC0B PG5 output
  
  TCCR2A = 0x23;  //fast PWM
  TCCR2B = 0x06;  //prescaler, 16000000/256 Hz
  TCCR0A = 0x23;  //
  TCCR0B = 0x05;  //16000000/1024 = 15625 Hz

  bool Keypad_Pressed[4][4] = {0};  //입력 판별 행렬

  while(1){ //keypad polling
    for(int i = 0; i<4; i++){
      PORTF = 0xFF & ~(1<<(3-i)); //0b00001111 & (3-i 번째 비트 0)
      _delay_us(10);

      if( !(PINF & 0x80)) Keypad_Pressed[0][i] = 1;
      if( !(PINF & 0x40)) Keypad_Pressed[1][i] = 1;
      if( !(PINF & 0x20)) Keypad_Pressed[2][i] = 1;
      if( !(PINF & 0x10)) Keypad_Pressed[3][i] = 1;
      if( (PINF & 0xF0) == 0xF0 ) {Keypad_Pressed[0][i] = 0; Keypad_Pressed[1][i] = 0; Keypad_Pressed[2][i] = 0; Keypad_Pressed[3][i] = 0;}

/*
      switch( PINB & 0xF0 ){
        case 0x70: Keypad_Pressed[0][i] = 1; break;
        case 0xB0: Keypad_Pressed[1][i] = 1; break;
        case 0xD0: Keypad_Pressed[2][i] = 1; break;
        case 0xE0: Keypad_Pressed[3][i] = 1; break;
        case 0xF0: {Keypad_Pressed[0][i] = 0; Keypad_Pressed[1][i] = 0; Keypad_Pressed[2][i] = 0; Keypad_Pressed[3][i] = 0;}
      }
*/
    }
    if(Keypad_Pressed[0][0]){
      OCR0B = 18;
    }
    else if (Keypad_Pressed[1][1]){
      OCR0B = 24;
    }
    else if (Keypad_Pressed[2][2]){
      OCR0B = 30;
    }
    else if (Keypad_Pressed[3][3]){
      OCR0B = 38;
    }
    else OCR0B = 9;
    //servo mortor 5~10% duty cycle to move, 5% is -90, 10% is +90, 7.5% is 0,
  }
}