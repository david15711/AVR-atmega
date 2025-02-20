#include <mega128.h>
#include <string.h>
#include <stdio.h>

char key;
char* str;

void UART0_init(void){
    UBRR0H = 0x00;    //38400 baud rate
    UBRR0L = 25;
    UCSR0A |= 0;    //(1<<U2X1);    //2배속
    
    //비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드
    UCSR0C |= 0x06;
    UCSR0B |= (1<<4);    //송수신 가능 RXEN0 TXEN0
    UCSR0B |= (1<<3);
}
void UART0_transmit(char data){
    while( !(UCSR0A & (1 << UDRE0)) );    //송신 가능 대기
    UDR0 = data;    //데이터 전송
}
unsigned char UART0_receive(void){
    while( !(UCSR0A & (1<<RXC0)) );    //수신 대기
    return UDR0;
}
void UART0_print_string(char *str){    //문자열 송신
    int i;
    for(i = 0; str[i]; i++)    //NULL 문자가 나올 때까지
        UART0_transmit(str[i]);    //바이트 단위 출력
}

void UART0_print_float(float f){
    char numString[20] = "0";
    
    sprintf(numString, "%f", f);
    UART0_print_string(numString);
}

void main(void)
{
    DDRG = 0x03;
    PORTG = 0;    
    UART0_init();
    while(1)
    {
        key = UART0_receive();
        if(key == '+')
        { 
            PORTG = 1;
            UART0_print_string("+ Received.\r\n");
        }
        else if (key == '-')
        {
            PORTG = 2;    
            UART0_print_string("- Received.\r\n");
    }   }
}