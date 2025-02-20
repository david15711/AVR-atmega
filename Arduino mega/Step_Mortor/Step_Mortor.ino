#include <avr/io.h>
#include <stdio.h>

volatile unsigned int var_sec=0, oldv=-1;
volatile unsigned int rev=0, oldr=-1;

void StepMotor(unsigned char i, unsigned char CW){
  if(CW) PORTF = (1<<i);
  else PORTF = (1<<(3-i));
}

void StepMotorCW(unsigned char i){
  PORTF = (1<<(i));
}

void StepMotorCCW(unsigned char i){
  PORTF = (1<<(3-i));
}

ISR(TIMER0_COMPB_vect){
  static unsigned char count_10ms = 0, count_100ms = 0, i = 0;
  static unsigned int j = 0;
  TCNT0 = 0;
  count_10ms++;
  StepMotor(i, 1);
  i++;
  if(i==4){i=0; j++; }
  if(j==512){j=0; rev++; } //32 step == 1 gear, 64 gear == 1 rev, 2048 == 1 rev
  if(count_10ms == 10){ //0.1sec간격으로 실행
    count_10ms = 0;
    count_100ms++;
  }
  if(count_100ms == 10){ //1 sec간격으로 실행
    count_100ms = 0;
    var_sec++;
  }
}


int main(void){
  DDRF |= 0xFF;
  Serial.begin(9600);
  TCCR0B |= 0x05; //normal, N = 1024
  OCR0B = 155;  //156 count, 10ms
  TIMSK0 |= 0x04; //output compare match interrupt enable
  SREG |= (1<<7);



  while(1){
    if(rev != oldr){
      Serial.print("motor revolution = ");
      Serial.println(rev);
      oldr = rev;
    }
    if(var_sec != oldv){
      Serial.print("elapsed time (sec) = ");
      Serial.println(var_sec);
      oldv = var_sec;
    }

  }

}