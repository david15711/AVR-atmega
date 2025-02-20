#include <stdio.h>
#include <avr/io.h>
#define F_CPU 16000000L

volatile float diff, dist;
ISR(TIMER5_CAPT_vect){
  static char flag = 1;
  static unsigned int n1_start, n2_end;

  switch(flag){
    case 1:{
      n1_start = ICR5;
      flag = 0;
      TCCR5B &= 0xBF;
      break;
    }
    case 0:{
      n2_end = ICR5;
      flag = 1;
      TCCR5B |= 0x40;
      diff = n2_end - n1_start;
      TCNT5 = 0;

    }
  }


}

int main(void){
  DDRF |= 0x01;  //Ultrasonic sensor
  TCCR5B |= 0x42; //16000000/8 Hz, T = 1/2000000 = 0.5 us
  TIMSK5 |= 0x20;
  SREG |= (1<<7);
  Serial.begin(9600);

  while(1){
    PORTF &= 0xFE;
    _delay_us(2);
    PORTF |= 0x01;
    _delay_us(10);
    PORTF &= 0xFE;

    dist = diff*0.0085; // counter / 2 * 340 / 1000 (cm/us),
    Serial.println(dist);
  }
}