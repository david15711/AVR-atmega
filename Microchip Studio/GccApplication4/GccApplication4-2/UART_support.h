/*
 * UART_support.h
 *
 * Created: 2022-10-28 오후 3:08:46
 *  Author: MS
 */ 


#ifndef UART_SUPPORT_H_
#define UART_SUPPORT_H_

void UART0_init(void);
void UART0_transmit(char data);
unsigned char UART0_receive(void);
void UART0_print_string(char *str);
void UART0_print_1_byte_number(uint8_t n);
void UART0_print_float(float f);

void UART1_init(void);
void UART1_transmit(char data);
unsigned char UART1_receive(void);
void UART1_print_string(char *str);
void UART1_print_1_byte_number(uint8_t n);
void UART1_print_float(float f);

#endif /* UART_SUPPORT_H_ */