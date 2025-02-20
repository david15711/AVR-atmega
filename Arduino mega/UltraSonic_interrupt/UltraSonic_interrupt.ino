#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#define F_CPU 16000000L
#define __DELAY_BACKWARD_COMPATIBLE__

double distance = 50.0, measureTime; //(ms)

ISR(TIMER2_OVF_vect){
  PORTD &= 0xFE;
  Serial.println("interrupt occured");
  while( !(PIND & 0x02) );
  TCNT0 = 0;
  while( (PIND & 0x02) );
  measureTime = TCNT0;
  distance = measureTime*0.0686; //(cm) = counter/ (16000000/64 = 250,000) *343 (m/s) / 2 * 100 (m/cm)
}

int main(void) {
  Serial.begin(115200);
  DDRF = 0xFF;  //LED
  DDRK = 0x00;  //Button
  DDRD |= 0x01; //ultra sonic sensor trigger and echo
  TCCR2A |= 0;  //normal
  TCCR2B |= 0x03; //prescaler 16000000/32 = 500,000 Hz, 2 us
  TCCR0A |= 0;
  TCCR0B |= 0x03; //16000000/64
  SREG |= (1<<7); //global interrupt enable
  TIMSK2 |= 0x01;  //Timer/Counter overflow interrupt enable TIFR |= (1<<0)
  //double delayTimeTemp = 500;
  double delayTime = 500;

  while(1){  
    PORTD |= 0x01;
    TCNT2 = 250;  //6 * 2 us

    if(distance >= 50){
      delayTime = 1000;
    }
    else if(distance >= 5){
      delayTime = distance * 20;  //50cm is 1000 ms delay
    }
    else {
      delayTime = 100;
    }

    Serial.print(delayTime);
    Serial.println("A");
    PORTF ^= 0x01;
    for(int i = 0; i<delayTime; i++){ //delay
      _delay_ms(1);/*
      if( (PINK & 0x01) && (delayTimeTemp<1000) ){  //button polling slower
        while((PINK & 0x01)); //debouncing
        _delay_ms(20);
        delayTimeTemp *= 2;
      }
      else if( (PINK & 0x02) && (delayTimeTemp>=125) ){  //faster
        while((PINK & 0x02)); //debouncing
        _delay_ms(20);
        delayTimeTemp /= 2;
      }*/
    }
    //delayTime = delayTimeTemp;

  }
  return 0;
}
