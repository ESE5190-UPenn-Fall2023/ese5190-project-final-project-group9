/*
 * motor_sequence.c
 *
 * Created: 12/2/2023 11:09:08 PM
 *  Author: rogun
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "motor_sequence.h"


void clockwise_motor1_seq()
{
	PORTD |= (1<<PORTD2);
	PORTD |= (1<<PORTD5);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD5);
	_delay_ms(2);
	PORTD |= (1<<PORTD3);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD2);
	_delay_ms(2);
	PORTD |= (1<<PORTD4);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD3);
	_delay_ms(2);
	PORTD |= (1<<PORTD5);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD4);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD5);
	_delay_ms(2);
}

void counter_clockwise_motor1_seq()
{
	PORTD |= (1<<PORTD5);
	_delay_ms(2);
	PORTD |= (1<<PORTD4);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD5);
	_delay_ms(2);
	PORTD |= (1<<PORTD3);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD4);
	_delay_ms(2);
	PORTD |= (1<<PORTD2);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD3);
	_delay_ms(2);
	PORTD |= (1<<PORTD5);
	_delay_ms(2);
	PORTD &= ~(1<<PORTD2);
	PORTD &= ~(1<<PORTD5);
	_delay_ms(2);
}

