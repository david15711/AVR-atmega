#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#define F_CPU 16000000L
#define __DELAY_BACKWARD_COMPATIBLE__

double measureTime; //(ms)

ISR(TIMER0_OVF_vect){
  ; // 8bit 256 /(16000000/8) = 0.032768 second later
}

double UltraSonic(void){
  PORTD |= 0x01;
  _delay_us(10);
  PORTD &= 0xFE;

  while( !(PIND & 0x02) );
  TCNT0 = 0;
  while( (PIND & 0x02) );
  measureTime = TCNT0;
  Serial.print(measureTime);
  Serial.print(" ; ");
  return measureTime*1.0976; //(cm) = counter/ (16000000/1024 = 15625) *343 (m/s) / 2 * 100 (m/cm)
}

int main(void) {
  Serial.begin(115200);
  DDRF = 0xFF;  //LED
  DDRK = 0x00;  //Button
  DDRD |= 0x01; //ultra sonic sencor trigger and echo
  TCCR0B = 0x05; //prescaler 16000000/1024 Hz
  //TIMSK0 = 0x01;  //Timer/Counter Overflow interrupt enable TIFR &= !(1<<0)
  double distance, delayTimeTemp = 500.0;
  double delayTime = delayTimeTemp;

  while(1){
    distance = UltraSonic();
    Serial.println(distance);
    if(distance >= 50){
      delayTime = 1000;
    }
    else if(distance >= 5){
      delayTime = distance * 20;  //100cm is 1000 ms delay
    }
    else delayTime = 100;
    /*
    PORTF ^= 0x01;
    for(int i = 0; i<(delayTime); i++){ //delay and button polling
      _delay_ms(1);
      if( (PINK & 0x01) && (delayTimeTemp<1000) ){  //slower
        while((PINK & 0x01)); //debouncing
        _delay_ms(20);
        delayTimeTemp *= 2;
      }
      else if( (PINK & 0x02) && (delayTimeTemp>=125) ){  //faster
        while((PINK & 0x02)); //debouncing
        _delay_ms(20);
        delayTimeTemp /= 2;
      }
    }
    delayTime = delayTimeTemp;
    */
  }
}
