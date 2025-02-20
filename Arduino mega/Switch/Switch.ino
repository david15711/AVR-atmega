#include <avr/io.h>
#include <stdio.h>

void Seven_Segment_Display(char num1, char num2, char num3, char num4){
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

char Keypad_Read_Row(void){
  switch( PINB & 0xF0 ){
    case 0x70: return 0;
    case 0xB0: return 1;
    case 0xD0: return 2;
    case 0xE0: return 3;
    case 0xF0: return 4;
  }
}

char Keypad_Read_Col(void){
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
  DDRD = 0xFF;  //FND
  DDRK = 0xFF; //FND
  PORTK = 0x00;
  DDRB = 0x0F;  //keypad
  PORTB = 0x00;
  unsigned int i=0, num=0;
  unsigned char key, Row, Col, Display_Num[4], Save_Num[4], Read_Num, State;
  char Keypad_RowCol[4][4] = { {'1', '2', '3', 'A'}, {'4', '5', '6', 'B'}, {'7', '8', '9', 'C'}, {'*', '0', '#', 'D'} };
  char Fnd_Font[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

  /*
  Serial.begin(9600);
  while(1){
    _delay_ms(260);
    
    Row = Keypad_Read_Row();
    Col = Keypad_Read_Col();
    Serial.print("Row 값 =  ");
    Serial.print(Row);
    Serial.print("  Col 값 =  ");
    Serial.println(Col);
    if( (Row == 4) && (Col == 4) ) {Serial.println("none"); continue;}  //RowCol값이 0,1,2,3이 아니면
    Serial.println(Keypad_RowCol[Row][Col]);
  }
  
  
  while(1) {
    Save_Num[0]=Fnd_Font[(num/100)%10];
    Save_Num[1]=Fnd_Font[(num/1000)%10];
    Save_Num[2]=Fnd_Font[(num/10000)%10];
    Save_Num[3]=Fnd_Font[(num/100000)%10];
    Seven_Segment_Display(Save_Num[0],Save_Num[1]+128,Save_Num[2],Save_Num[3]);
    num = num + 1;
    if (num == 999999) num=0;
  }
*/

  
  while(1){
    Read_Num = 0;
    Seven_Segment_Display(Fnd_Font[Display_Num[0]],Fnd_Font[Display_Num[1]],Fnd_Font[Display_Num[2]],Fnd_Font[Display_Num[3]]);
    Row = Keypad_Read_Row();                                           //keypad의 현재 행렬 읽기
    Col = Keypad_Read_Col();
    if(Row == 4 || Col == 4) State = 0;                                //안눌린상태 판단
    if( State == 0 && (Row != 4 && Col != 4)) {                        //안눌린상태에서 눌렸을때
      State = 1;
      Read_Num = Keypad_RowCol[Row][Col];                              //get ascii from keypad matrix
      if(Read_Num == '#'){                                             //if get enter
        i=-1;
        Display_Num[0] = Save_Num[0];
        Display_Num[1] = Save_Num[1];
        Display_Num[2] = Save_Num[2];
        Display_Num[3] = Save_Num[3];
        Save_Num[0] = 0;
        Save_Num[1] = 0;
        Save_Num[2] = 0;
        Save_Num[3] = 0;
      }
      else if( (Read_Num > 47) && (Read_Num < 58) ){                     //'0' <= read <= '9'  
        if(i>3) continue;
        Save_Num[i] = Read_Num - '0';                                    //ascii to num
      }
      i++;
    }
  }
    

}