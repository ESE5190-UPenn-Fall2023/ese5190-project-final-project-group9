#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
 
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include "time.h"
#include <string.h>
 
int circle_spacing = 15; int circle_num = 4;
float angle_screen = 0;
 
//Pulse to Sonar
int trig = 0;
int ovfCount = 0;
int angle1 = 0;
//Sonar Record and Print
int falling = 0;
int printVar = 0;
int elapsed;
int cmVal = 0;
 
char message[10];
 
ISR(TIMER1_COMPA_vect) {
	if(!trig) {
		//Pulse for 10 us
		OCR1A = 20;
		trig = 1;
	}
	else {
		//Stop Pulse
		OCR1A = 0;
		trig = 0;
		//Disable Output Compare Toggle
		TCCR1A &= ~(1<<COM1A0);
		TCCR1A &= ~(1<<COM1A1);
		TCNT1 = 0;
		ovfCount = 0;
	}
}
 
//Trig Pulse Delay
ISR(TIMER1_OVF_vect) {
	ovfCount += 1;
	//Enable Output Compare after 2 Overflows
	//if(ovfCount >= 2) {
	//TCCR1A |= (1<<COM1A0);
	//TCCR1A &= ~(1<<COM1A1);
	//ovfCount = 0;
	//}
}
void PingDistance() {
	//Enables output compare to send sensor trig
	TCCR1A |= (1<<COM1A0);
	TCCR1A &= ~(1<<COM1A1);
	//Echo read automatically disables output compare
}
//Echo Capture
ISR(TIMER1_CAPT_vect) {
	if (!falling) {
		//Record Pulse Start
		elapsed = TCNT1;
		falling = 1;
		} else {
		//Record Pulse Width
		if(elapsed > TCNT1) {
			//Account for Overflow
			elapsed = (65535 - elapsed) + TCNT1;
			} else {
			elapsed = TCNT1 - elapsed;
		}
		falling = 0;
		printVar = 1;
	}
	//Toggle whether looking for Rising or Falling Edge
	TCCR1B ^= (1<<ICES1);
}
 
 
void init(){
	cli();
	//SONAR SENSOR
	//Set PB1 to OUTPUT (TRIG) and init LOW
	DDRB |= (1<<DDB1);
	PORTB &= ~(1<<PORTB1);
	//Set PB0 to INPUT (ECHO)
	DDRB &= ~(1<<DDB0);
	//Timer 1 (Prescale by 8)
	TCCR1B &= ~(1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12);
	//normal mode
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);
	//Enable output compare, toggle OC1A on OCR1A match
	TCCR1A |= (1<<COM1A0);
	TCCR1A &= ~(1<<COM1A1);
	//Rising edge for echo input capture
	TCCR1B |= (1<<ICES1);
	//Enable Compare,InputCapture,Overflow Interrupt
	TIMSK1 |= (1<<OCIE1A);
	TIMSK1 |= (1<<ICIE1);
	TIMSK1 |= (1<<TOIE1);
	//Set Initial Compare Value
	OCR1A = 0;
	sei();
 
}
 
 
int Mapfield(void){
	int mid_y = LCD_HEIGHT/2; int mid_x = LCD_WIDTH/2;  
	LCD_resetScreen();
	int i;
	for (i = circle_spacing*circle_num; i>0; i-= circle_spacing){
		LCD_drawHollowCircle(mid_x, mid_y, i, WHITE);
	}
	//LCD_drawHollowCircle(mid_x, mid_y, 20, 1, WHITE, BLACK);
}
 
int objectMap(uint16_t angle, int dist){
	float max_dist_glo = 60;
	if (dist > max_dist_glo) {
		dist = max_dist_glo;
	}
	float angleRad = (float)angle * 3.14159/180.0;
	float max_dist_scr = circle_spacing*circle_num;
	static float x0;
	static float y0;
	float conv = max_dist_scr/max_dist_glo;
	x0 = dist*cos(angleRad)*conv + LCD_WIDTH/2; 
	y0 = dist*sin(angleRad)*conv + LCD_HEIGHT/2;
	//LCD_drawPixel((int)x0, (int)y0, GREEN);
	LCD_drawFilledCircle((int)x0, (int)y0, 1, GREEN);
 
}
 
int scanner(float angle_screen, int mode){
	float max_dist_scr = circle_spacing*circle_num;
	float x0 = (float)max_dist_scr*cos(angle_screen) + LCD_WIDTH/2; float y0 = (float)max_dist_scr*sin(angle_screen) + LCD_HEIGHT/2;
	if (mode){
		LCD_drawLine(LCD_WIDTH/2, LCD_HEIGHT/2, x0, y0, WHITE);
	}
	else{
		LCD_drawLine(LCD_WIDTH/2, LCD_HEIGHT/2, x0, y0, BLACK);
	}
}
 
 
char angle_motor[10];
char x[10];
uint16_t angle = 0;
uint16_t anglePrev = 0;
 
void loop(){
	//Mapfield();
	UART_getLine(angle_motor, 10);
	
	angle = atoi(angle_motor);
	
    if(anglePrev != angle) {
		PingDistance();
	}
	//Update PWM and Print after Sonar Reading
	if(printVar) {
		//Distance calculation
		cmVal = elapsed / (58*2);
 
		
		//sprintf(x,"%d",angle);
		//strcat(x,"\n");
		//UART_putstring(angle_motor);
 
		//Reset for next Echo
		objectMap(angle, cmVal);
		angle1 = angle;
		//sprintf(x,"%d",angle);
		//strcat(x,"\n");
		//UART_putstring(x);
		//_delay_ms(100);
		//objectMap(angle, cmVal,0);
		
		sprintf(message,"%d",cmVal);
		strcat(message,"\n");
	    UART_putstring(message);
		
		if ((angle == 0)||(angle>=360)){
			Mapfield();
			//LCD_setScreen(BLACK);
		}
		printVar = 0;
		elapsed = 0;
	}
	anglePrev = angle;
	/*
	scanner(angle,1);
	_delay_ms(2);
	scanner(angle,0);
	*/
}
 
int main(void){
	UART_init(BAUD_PRESCALER);
	lcd_init();
	LCD_resetScreen();
	Mapfield();
	init();

	while(1) loop();
}