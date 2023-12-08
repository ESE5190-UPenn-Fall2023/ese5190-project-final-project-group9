#include "uart.h"
#include <avr/io.h>



void UART_init(int BAUD_PRESCALER)
{
	
	/*Set baud rate */
	UBRR0H = (unsigned char)(BAUD_PRESCALER>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALER;
	
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	UCSR0C |= (1<<USBS0);
}

void UART_send(unsigned char data)
{
	
	while(!(UCSR0A & (1<<UDRE0)));
	
	UDR0 = data;
	
}

void UART_putstring(char* message)
{
	while(*message != 0x00)
	{
		UART_send(*message);
		message++;
	}
}

char UART_getc(void)
{
	// wait for data
	while(!(UCSR0A & (1 << RXC0)));

	// return data
	return UDR0;
}

void UART_getLine(char* buf, int n)
{
	uint8_t bufIdx = 0;
	char c;
	do
	{
		c = UART_getc();
		buf[bufIdx++] = c;
	}
	while((bufIdx < n) && (c != '\n'));
	buf[bufIdx] = 0;
}
