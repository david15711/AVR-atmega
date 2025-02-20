/*
 * GccApplication2-2.c
 *
 * Created: 2022-10-10 오후 3:51:56
 * Author : MS
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void UART0_init(void);
void UART0_transmit(char data);

FILE OUTPUT \
= FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

void UART0_init(void){
	
	UBRR0H = 0x00;	//9600 보율로 시작
	UBRR0L = 207;
	UCSR0A |= _BV(U2X0);	//2배속 모드
	
	// 비동기, 8비트 데이터, 패리티 없음,	1비트 정지 비트 모드
	UCSR0C |= 0x06;
	UCSR0B |= _BV(RXEN0);	//송수신 가능
	UCSR0B |= _BV(TXEN0);
}

void UART0_transmit(char data){
	while( !(UCSR0A & (1 << UDRE0)) );	//송신 가능 대기
	UDR0 = data;	//데이터 전송
}


int main(void)
{
    UART0_init();	//UART0 초기화
	stdout = &OUTPUT;	// printf 사용 설정
	/*
	printf("**Size of Date Types\n\r");
	printf("%d\n\r", sizeof(char));
	printf("%d\n\r", sizeof(int));
	printf("%d\n\r", sizeof(short));
	printf("%d\n\r", sizeof(long));
	printf("%d\n\r", sizeof(float));
	printf("%d\n\r", sizeof(double));
	*/
	char str[100] = "Test String";
	
	printf("Integer : %ld\n\r", 60191859);
	printf("Float : %f\n\r", 3.14);
	printf("String : %s\n\r", str);
	printf("Character : %c\n\r", 'A');
    while (1) {}
	return 0;
}
