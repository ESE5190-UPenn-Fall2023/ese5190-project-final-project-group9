#include <avr/pgmspace.h>
#include "ST7735.h"
#include <avr/io.h>
#include <util/delay.h>

// SPI SUBROUTINES --------------
static void SPI_Init(void){
	SPCR = (1<<SPE) | (1<<MSTR);		
	SPSR = (1<<SPI2X);										
}
void SPI_ControllerTx(uint8_t data){
	LCD_PORT &= ~(1<<LCD_TFT_CS); //pulled low

	SPI_ControllerTx_stream(data); //load SPI buffer

	LCD_PORT |= (1<<LCD_TFT_CS); //pulled high
}

void SPI_ControllerTx_stream(uint8_t stream) {
	SPDR = stream;		//Place data in stream
	while(!(SPSR & (1<<SPIF))); //wait for transmission to complete
}

void SPI_ControllerTx_16bit(uint16_t data){
	uint8_t data_temp = data >> 8; //shift 16-bit data to placeholder on 8-bit uint
	LCD_PORT &= ~(1<<LCD_TFT_CS);	
	
	SPDR = data_temp;		//Place data in register
	while(!(SPSR & (1<<SPIF)));	//wait for transmission to complete
	SPDR = data;		
	while(!(SPSR & (1<<SPIF)));	
	
	LCD_PORT |= (1<<LCD_TFT_CS);	
}

void SPI_ControllerTx_16bit_stream(uint16_t data){ //similar to SPI_ControllerTx_16bit but CS is pulled low/high once
	uint8_t data_temp = (data >> 8);

	SPDR = data_temp;		//Place data to be sent on registers
	while(!(SPSR & (1<<SPIF)));	//wait for end of transmission
	SPDR = data;		//Place data to be sent on registers
	while(!(SPSR & (1<<SPIF)));	//wait for end of transmission
}

// LCD SUBROUTINES --------------
void lcd_init(void){
	LCD_DDR |= (1<<LCD_TFT_CS)|(1<<LCD_MOSI)|(1<<LCD_SCK);	//Set up output pins
	DDRC |= (1<<LCD_DC)|(1<<LCD_RST);
	LCD_LITE_DDR |= (1<<LCD_LITE);	//Set up output pins

	TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);	//Fast PWM: clear OC0A on match, set at bottom
	TCCR0B |= (1<<CS02);	//clk/1024/256=244Hz
	OCR0A = 127;	//Set starting PWM value

	_delay_ms(50);
	PORTC |= (1<<LCD_RST);
	SPI_Init();
	_delay_ms(5);

	static uint8_t ST7735_cmds[]  =
	{
		ST7735_SWRESET, 0, 150,       // Software reset
		ST7735_SLPOUT, 0, 255,       // Exit sleep mode
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
	LCD_PORT &= ~(1<<LCD_TFT_CS);	//CS pulled low to start communication

	while (numCommands--){
		PORTC &= ~(1<<LCD_DC);	//D/C pulled low for command
		SPI_ControllerTx_stream(*cmds++);
		numData = *cmds++;	
		PORTC |= (1<<LCD_DC);	
		while (numData--)	SPI_ControllerTx_stream(*cmds++);
		waitTime = *cmds++;   
		if (waitTime!=0) Delay_ms((waitTime==255 ? 500 : waitTime));
	}
	LCD_PORT |= (1<<LCD_TFT_CS);	
}

void LCD_setAddr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
	uint8_t ST7735_cmds[]  =
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
