#include "LCD_GFX.h"
#include "ST7735.h"

#include <stdio.h>
#include <math.h>

uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue){ // convert from 24-bit color to 16-bit color
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}


void LCD_drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color){
	for(int x = -radius+x0; x < radius+x0; x++) { //start from one end and go to the other
		int y1 = pow(radius*radius - (x-x0)*(x-x0),0.5) + y0;
		int y2 = -1*pow(radius*radius - (x-x0)*(x-x0),0.5) + y0;
		LCD_drawLine(x,y1,x,y2,color);
		
	}
}

void LCD_drawHollowCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color){
	for(int x = -radius+x0; x <= radius+x0; x++) { //start from one end and go to the other
		int y1 = pow(radius*radius - (x-x0)*(x-x0),0.5) + y0;
		int y2 = -1*pow(radius*radius - (x-x0)*(x-x0),0.5) + y0;
		LCD_drawPixel(x,y1,color);
		LCD_drawPixel(x,y2,color);
	}
	
	for(int y = -radius+y0; y <= radius+y0; y++) { //start from one end and go to the other
		int x1 = pow(radius*radius - (y-y0)*(y-y0),0.5) + x0;
		int x2 = -1*pow(radius*radius - (y-y0)*(y-y0),0.5) + x0;
		LCD_drawPixel(x1,y,color);
		LCD_drawPixel(x2,y,color);
	}

}

void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t color){ // Bresenham's line algorithm
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;


    while (1) {
	    //LCD_drawPixel(x0, y0, color); // Replace setPixel with your drawing function
		LCD_setAddr(x0,y0,x0,y0);
		SPI_ControllerTx_16bit(color);
		
	    if (x0 == x1 && y0 == y1) {
		    break;
	    }

	    int e2 = 2 * err;
	    if (e2 >= dy) {
		    err += dy;
		    x0 += sx;
	    }
	    if (e2 <= dx) {
		    err += dx;
		    y0 += sy;
	    }
    }
	
}


void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color){
	LCD_setAddr(x0,y0,x1,y1);
	LCD_PORT &= ~(1<<LCD_TFT_CS); //pull CS low
	
	if ((x0>x1)&(y0>y1)){
		int tempx = x0;
		x1 = x0;
		x1 = tempx;
		
		int tempy = y0;
		y1 = y0;
		y1 = tempy;
		
	}

	for (int x = x0; x<=x1; ++x){
		for (int y = y0; y<=y1; ++y){
			SPI_ControllerTx_16bit_stream(color);
		}
	}
	LCD_PORT |= (1<<LCD_TFT_CS); //pull CS high
}

void LCD_setScreen(uint16_t color) {
	LCD_setAddr(0,0,LCD_WIDTH,LCD_HEIGHT); 
	LCD_PORT &= ~(1<<LCD_TFT_CS); //pull CS low

	for (int x = 0; x<LCD_WIDTH; ++x){
		for (int y = 0; y<LCD_HEIGHT; ++y){
		SPI_ControllerTx_16bit_stream(color);	
		}
	}
	LCD_PORT |= (1<<LCD_TFT_CS); //pull CS high
}

void LCD_resetScreen(){
	LCD_setScreen(0x0000);
}
