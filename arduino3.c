/*
 * GccApplication2.c
 *
 * Created: 11/30/2023 11:43:47 AM
 * Author : rogun
 */ 

#include <avr/io.h>
#include "motor_sequence.h"
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include <util/delay.h>
#include <string.h>
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

char message[10];
uint16_t angle = 0;
uint8_t step = 0;
uint16_t target = 0;
uint8_t button = 0;


void clockwise_motor1()
{
	if (angle == 360)
	{
		angle = 0;
	}
	clockwise_motor1_seq();
	if (step == 4)
	{
		step = 0;
		angle = angle + 3;
	}
	step ++;
	
	
	
	
}

int main(void)
{
	UART_init(BAUD_PRESCALER);

    while (1) 
    {
		UART_getLine(message,10);
		target = atoi(message);
		while(angle != target ) 
		{
			clockwise_motor1();
			if(angle == target)
			{
				break;
			}
			
			
		}
		_delay_ms(10000);
		
		
    }
}   

