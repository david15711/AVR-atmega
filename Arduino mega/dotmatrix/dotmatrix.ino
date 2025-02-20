#include <avr/io.h>
#include <math.h>
#define logB(x) log(x)/log(2)

inline void LED_Row(char num){    //3cycle = 3/16000000 second
  PORTF = 1 << (num - 1);
  /*switch(num){
    case 1: PORTF = 0x01; break; 
    case 2: PORTF = 0x02; break;
    case 3: PORTF = 0x04; break;
    case 4: PORTF = 0x08; break;
    case 5: PORTF = 0x10; break;
    case 6: PORTF = 0x20; break;
    case 7: PORTF = 0x40; break;
    case 8: PORTF = 0x80; break;
    default: PORTF = 0xFF; break;
  }*/
}
inline void LED_Col(char num){
  PORTK = ~(1 << (num - 1));
  /*switch(num){
    case 1: PORTK = 0xFE; break; 
    case 2: PORTK = 0xFD; break;
    case 3: PORTK = 0xFB; break;
    case 4: PORTK = 0xF7; break;
    case 5: PORTK = 0xEF; break;
    case 6: PORTK = 0xDF; break;
    case 7: PORTK = 0xBF; break;
    case 8: PORTK = 0x7F; break;
    default: PORTK = 0x00; break;
  }*/
}

inline void LED_ROW_toggle(char num){
  PORTF ^= 1 << (num - 1);
}
inline void LED_COL_toggle(char num){
  PORTK ^= 1 << (num - 1);
}

inline void LED_RowCol(char noRow, char noCol){
  //if( (logB(PORTF) + 1) != noRow ) PORTF = 0x00;  //현재 켜져있는 비트의 수를 찾아서 켜져있다면 초기화 안함
  //if( (logB(~PORTK) + 1) != noCol ) PORTK = 0xFF;
  LED_Row(noRow);
  LED_Col(noCol);
  }

int main(void){
  unsigned int i = 1;
  unsigned int ctrl = 0;
  bool LED_Dot[8][8] = { {0,0,0,0,0,0,0,0}, {0,1,1,1,0,1,1,1}, {0,0,0,1,0,1,0,1}, {0,0,0,1,0,1,0,1}, {0,0,0,1,0,1,1,1}, {0,1,1,1,1,0,0,0}, {0,0,1,0,0,0,0,0}, {0,0,0,0,0,0,0,0} };
  DDRF = 0xFF;
  DDRK = 0xFF;
  PORTF |= 0x01;
  PORTK = 0x00;

  while(1){
    //_delay_ms(500);
    //PORTF = PORTF << 1;
    //if(PORTF > 8) PORTF = 0x01;
    //if(!PORTF) PORTF = !PORTF;

    /*
    if(!ctrl){
      PORTK = 0x00;
      LED_ROW(i++);
      if(i>8){
        i = 1;
        ctrl = 1;
      }
    }
    else {
      PORTF = 0xFF;
      LED_COL(i++);
      if(i > 8){
        i = 1;
        ctrl = 0;
      }
    }
    */

    /*
    LED_RowCol(2,2); _delay_us(5);
    LED_RowCol(2,3); _delay_us(5);
    LED_RowCol(2,4); _delay_us(5);
    LED_RowCol(2,6); _delay_us(5);
    LED_RowCol(2,7); _delay_us(5);
    LED_RowCol(2,8); _delay_us(5);
    LED_RowCol(3,4); _delay_us(5);
    LED_RowCol(3,6); _delay_us(5);
    LED_RowCol(3,8); _delay_us(5);
    LED_RowCol(4,4); _delay_us(5);
    LED_RowCol(4,6); _delay_us(5);
    LED_RowCol(4,8); _delay_us(5);
    LED_RowCol(5,4); _delay_us(5);
    LED_RowCol(5,6); _delay_us(5);
    LED_RowCol(5,7); _delay_us(5);
    LED_RowCol(5,8); _delay_us(5);
    LED_RowCol(6,2); _delay_us(5);
    LED_RowCol(6,3); _delay_us(5);
    LED_RowCol(6,4); _delay_us(5);
    LED_RowCol(6,5); _delay_us(5);
    LED_RowCol(7,3); _delay_us(5);
    */
    
    for(int i = 0;i<8;i++){
      PORTF = 0x00;                  //행이 달라질때 클리어, 잔상 없앰
      for(int j = 0;j<8;j++){
        if(LED_Dot[i][j]) {          //빈 도트 무시
          PORTK = 0xFF;              //같은 행 안에서 열이 달라질때 클리어
          LED_RowCol(i+1, j+1);
          _delay_us(5);
        }
      }
    }
    

  }
}