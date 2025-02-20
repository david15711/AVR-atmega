#include <mega128.h>
#include <delay.h>

void main(void)
{
    unsigned int LEDA = 1; 
    DDRG  = 0x03;
    PORTG = 0x03;
    DDRA = 0xFF;
    DDRC = 0x0F;

    while(1)
    {        
        LEDA = LEDA<<1;
        if( LEDA > 128) LEDA = 1;
        PORTA = LEDA;
        delay_ms(1000);
    }

}