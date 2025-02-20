#include <avr/io.h>
#include <stdio.h>

void Seven_Segment_Display(char num1, char num2, char num3, char num4){//little endian
  unsigned int i = 0;
  char num[4] = {num1, num2, num3, num4};
  while(i<4){
    PORTK = 0;
    PORTD = 0x0F;
    PORTD &= ~(1 << i);   //자리 선택
    PORTK = num[i];
    _delay_us(250);
    i++;
  }
}

char Keypad_Read_Row(void){ //not used
  switch( PINB & 0xF0 ){
    case 0x70: return 0;
    case 0xB0: return 1;
    case 0xD0: return 2;
    case 0xE0: return 3;
    case 0xF0: return 4;
  }
}

char Keypad_Read_Col(void){ //not used
  PORTB = 0xF7;
  _delay_us(10);
  if((PINB & 0xF0) != 0xF0) return 0;
  PORTB = 0xFB;
  _delay_us(10);
  if((PINB & 0xF0) != 0xF0) return 1;
  PORTB = 0xFD;
  _delay_us(10);
  if((PINB & 0xF0) != 0xF0) return 2;
  PORTB = 0xFE;
  _delay_us(10);
  if((PINB & 0xF0) != 0xF0) return 3;
  else return 4;
}

int main(void){
  DDRD = 0xFF;
  DDRK = 0xFF;
  PORTK = 0x00;
  DDRB = 0x0F;
  PORTB = 0x00;

  bool Keypad_Pressed[4][4] = {0};  //입력 판별 행렬
  char Fnd_Font[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};



  while(1){ //keypad polling
    for(int i = 0; i<4; i++){
      PORTB = 0xFF & ~(1<<(3-i)); //0b00001111 & (3-i 번째 비트 0)
      _delay_us(10);

      if( !(PINB & 0x80)) Keypad_Pressed[0][i] = 1;
      if( !(PINB & 0x40)) Keypad_Pressed[1][i] = 1;
      if( !(PINB & 0x20)) Keypad_Pressed[2][i] = 1;
      if( !(PINB & 0x10)) Keypad_Pressed[3][i] = 1;
      if( (PINB & 0xF0) == 0xF0 ) {Keypad_Pressed[0][i] = 0; Keypad_Pressed[1][i] = 0; Keypad_Pressed[2][i] = 0; Keypad_Pressed[3][i] = 0;}

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

    if(Keypad_Pressed[0][0] && Keypad_Pressed[1][1]){
      Seven_Segment_Display(Fnd_Font[9],Fnd_Font[1],Fnd_Font[0],Fnd_Font[6]);
    }
    else if (Keypad_Pressed[2][2] && Keypad_Pressed[3][3]){
      Seven_Segment_Display(Fnd_Font[9],Fnd_Font[5],Fnd_Font[8],Fnd_Font[1]);
    }
    else Seven_Segment_Display(0x3F,0x3F,0x3F,0x3F);

  }
}