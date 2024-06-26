#include <avr/pgmspace.h>
#include "ST7735.h"
#include <avr/io.h>
#include <util/delay.h>

// SPI SUBROUTINES --------------
static void SPI_Init(void){
	SPCR = (1<<SPE) | (1<<MSTR); // set the SPI enable and master bit	
	SPSR = (1<<SPI2X); // set the prescalar to /2										
}
void SPI_ControllerTx(uint8_t data){ // load 8-bit data to send
	LCD_PORT &= ~(1<<LCD_TFT_CS); // pulled CS low

	SPI_ControllerTx_stream(data); // load SPI buffer

	LCD_PORT |= (1<<LCD_TFT_CS); // pulled CS high
}

void SPI_ControllerTx_stream(uint8_t stream) { // load 8-bit data to send continuously
	SPDR = stream;		// store data into SPI Data register
	while(!(SPSR & (1<<SPIF))); // when data transfer is complete, the SPIF flag is set
}

void SPI_ControllerTx_16bit(uint16_t data){ // load 16-bit data to send
	uint8_t data_temp = data >> 8; // shift 16-bit data to placeholder on 8-bit uint
	LCD_PORT &= ~(1<<LCD_TFT_CS); // pulled CS low	
	
	SPDR = data_temp;		// place data in register
	while(!(SPSR & (1<<SPIF)));	// wait for transmission to complete
	SPDR = data;		
	while(!(SPSR & (1<<SPIF)));	// when data transfer is complete, the SPIF flag is set
	
	LCD_PORT |= (1<<LCD_TFT_CS); // pulled CS high	
}

void SPI_ControllerTx_16bit_stream(uint16_t data){ // load 16-bit data to send continuously
	uint8_t data_temp = (data >> 8);

	SPDR = data_temp;		// place data to be sent on registers
	while(!(SPSR & (1<<SPIF)));	// wait for end of transmission
	SPDR = data;		// place data to be sent on registers
	while(!(SPSR & (1<<SPIF)));	// wait for end of transmission
}

// LCD SUBROUTINES --------------
void lcd_init(void){
	LCD_DDR |= (1<<LCD_TFT_CS)|(1<<LCD_MOSI)|(1<<LCD_SCK);	// set CS, MOSI, and Clk as output pins
	DDRC |= (1<<LCD_DC)|(1<<LCD_RST); // set DC and reset as output pins
	LCD_LITE_DDR |= (1<<LCD_LITE);	// set lite as output pins

	TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00); // set as Fast PWM
	TCCR0B |= (1<<CS02); // set /256 prescalar
	OCR0A = 127;

	PORTC |= (1<<LCD_RST); //reset
	SPI_Init();

	static uint8_t ST7735_cmds[]  = //initialization command (based on Lab 4 code)
	{
		ST7735_SWRESET, 0, 150,       
		ST7735_SLPOUT, 0, 255,      
		ST7735_FRMCTR1, 3, 0x01, 0x2C, 0x2D, 0,  
		ST7735_FRMCTR2, 3, 0x01, 0x2C, 0x2D, 0,  
		ST7735_FRMCTR3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, 0,  
		ST7735_INVCTR, 1, 0x07, 0,	
		ST7735_PWCTR1, 3, 0x0A, 0x02, 0x84, 5, 
		ST7735_PWCTR2, 1, 0xC5, 5,      
		ST7735_PWCTR3, 2, 0x0A, 0x00, 5,	
		ST7735_PWCTR4, 2, 0x8A, 0x2A, 5,	
		ST7735_PWCTR5, 2, 0x8A, 0xEE, 5,	
		ST7735_VMCTR1, 1, 0x0E, 0, 
		ST7735_INVOFF, 0, 0,	
		ST7735_MADCTL, 1, 0xC8, 0,	
		ST7735_COLMOD, 1, 0x05, 0,	
		ST7735_CASET, 4, 0x00, 0x00, 0x00, 0x7F, 0,		
		ST7735_RASET, 4, 0x00, 0x00, 0x00, 0x9F, 0,		
		ST7735_GMCTRP1, 16, 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D,
		0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, 0, 
		ST7735_GMCTRN1, 16, 0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29,0x2D,
		0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, 0, 
		ST7735_NORON, 0, 10,	
		ST7735_DISPON, 0, 100,           
		ST7735_MADCTL, 1, MADCTL_MX | MADCTL_MV | MADCTL_RGB, 10		
	};

	sendCommands(ST7735_cmds, 22);
}

void sendCommands (const uint8_t *cmds, uint8_t len){
	uint8_t numCommands, numData, waitTime;
	numCommands = len;	// # of commands to send
	LCD_PORT &= ~(1<<LCD_TFT_CS);	// CS pulled low to start communication

	while (numCommands--){
		PORTC &= ~(1<<LCD_DC);	// D/C pulled low for command
		SPI_ControllerTx_stream(*cmds++); // sends command from array one at a time
		numData = *cmds++;	
		PORTC |= (1<<LCD_DC);	
		while (numData--)	SPI_ControllerTx_stream(*cmds++);
		waitTime = *cmds++;   
		if (waitTime!=0) Delay_ms((waitTime==255 ? 500 : waitTime));
	}
	LCD_PORT |= (1<<LCD_TFT_CS); // CS pulled high	
}

void LCD_setAddr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
	uint8_t ST7735_cmds[]  = // command responsible for accessing row and column of pixels (based on lab 4)
	{
		ST7735_CASET, 4, 0x00, x0, 0x00, x1, 0,		// Column
		ST7735_RASET, 4, 0x00, y0, 0x00, y1, 0,		// Page
		ST7735_RAMWR, 0, 5				// Into RAM
	};
	sendCommands(ST7735_cmds, 3);
}

void Delay_ms(unsigned int n){
	while (n--) _delay_ms(1);
}
