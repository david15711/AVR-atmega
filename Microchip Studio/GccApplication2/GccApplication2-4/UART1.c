﻿#include <avr/io.h>

void UART1_init(void){
	UBRR1H = 0x00;	//9,600 baud rate
	UBRR1L = 207;
	UCSR1A |= _BV(U2X1);	//2배속
	
	//비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드
	UCSR1C |= 0x06;
	UCSR1B |= _BV(RXEN1);	//송수신 가능
	
}

void UART0_transmit(char data){
	while( !(UCSR1A & (1 << UDRE1)) );	//송신 가능 대기
	UDR1 = data;	//데이터 전송
}

unsigned char UART1_receive(void){
	while( !(UCSR1A & (1<<RXC1)) );	//수신 대기
	return UDR1;
}

void UART1_print_string(char *str){	//문자열 송신
	for(int i = 0; str[i]; i++)	//NULL 문자가 나올 때까지
		UART0_transmit(str[i]);	//바이트 단위 출력
}

void UART1_print_1_byte_number(uint8_t n){
	char numString[4] = "0";
	int i, index = 0;
	if(n>0){	//문자열 반환
		for(i = 0; n != 0; i++){
			numString[i] = n % 10 + '0';
			n = n / 10;
		}
	numString[i] = '\0';
	index = i - 1;
	}
	for(i = index; i >= 0; i--)	//변환된 문자열 역순 출력
	UART1_transmit(numString[i]);
}
