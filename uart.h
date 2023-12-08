#ifndef UART_H
#define UART_H

void UART_init(int prescale);

void UART_send( unsigned char data);

void UART_putstring(char* message);

char UART_getc(void);

void UART_getLine(char* buf, int n);

#endif