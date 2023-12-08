/*
 * GccApplication1.c
 *
 * Created: 11/27/123 9:51:48 AM
 * Author : rogun
 */ 
#define F_CPU 16000000
#include "motor_sequence.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include <util/delay.h>
#include <string.h>
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)


int angle = 0;
uint8_t step = 0;
char message[10];
int target = 0;
uint8_t dir = 1;
uint8_t count =0;
uint8_t button = 0;
void initialize()
{
	// Motor control
	DDRD |= (1<<PORTD2);
	DDRD |= (1<<PORTD3);
	DDRD |= (1<<PORTD4);
	DDRD |= (1<<PORTD5);
	DDRC &= ~(1<<PORTC1);
	DDRC &= ~(1<<PORTC4);
	
	DDRD |= (1<<PORTB2);
	DDRD |= (1<<PORTB3);
	DDRD |= (1<<PORTB4);
	DDRD |= (1<<PORTB5);
	DDRC &= ~(1<<PORTC3);
	
	//Toggle between manual and auto rotation mode
	PORTC &= ~(1<<PORTC1); 
	PORTC &= ~(1<<PORTC3);
	PORTC &= ~(1<<PORTC4); 
	//Joystick control
	ADMUX = (1<<REFS0);
	ADCSRA |= (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	DIDR0 |= (1<<ADC0D);

	
}

void button_press_rot_cntrl_motor1()
{
	if((PINC & (1<<PINC4)) != (1<<PINC4))
	{
		button ^= 1;
		_delay_ms(10);
		
	}

}

void counter_clockwise_motor1()
{
		counter_clockwise_motor1_seq();
		if (step == 0)
		{
			step = 4;
			angle = angle - 3;
		}
		step --;
	
			
}

void clockwise_motor1()
{

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
	initialize();
	UART_init(BAUD_PRESCALER);
	
    while (1) 
    {
 		ADCSRA |= (1<<ADSC);
 		while(ADCSRA & (1<<ADSC));
		 button_press_rot_cntrl_motor1();
		 if(angle == 360)
		 {
			 dir = 0;
		 }
		 else if (angle == 0)
		 {
			 dir = 1;
		 }
		if((button == 0) &&(dir == 0))
		counter_clockwise_motor1();
		else if((button == 0)&&(dir == 1))
		clockwise_motor1();
 		else if((button == 1) && (ADC <200) && (angle >= 0))
 		{
 			counter_clockwise_motor1();
		}
 		else if((button == 1) && (ADC>600) && (angle <=360))
 		{
 			clockwise_motor1();
 		}
	

		 sprintf(message,"%d",angle);
 	 	strcat(message,"\n");
 		UART_putstring(message);
		// _delay_ms(10);
		 
		 
		
	
	}
}

