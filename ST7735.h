#include <avr/io.h>

#define ADAFRUIT358_SPI_NO_ERR                 0 //! No error
#define ADAFRUIT358_SPI_ERR                    1 //! General or an unknown error
#define ADAFRUIT358_SPI_ERR_RESP_TIMEOUT       2 //! Timeout during command
#define ADAFRUIT358_SPI_ERR_RESP_BUSY_TIMEOUT  3 //! Timeout on busy signal of R1B response
#define ADAFRUIT358_SPI_ERR_READ_TIMEOUT       4 //! Timeout during read operation
#define ADAFRUIT358_SPI_ERR_WRITE_TIMEOUT      5 //! Timeout during write operation
#define ADAFRUIT358_SPI_ERR_RESP_CRC           6 //! Command CRC error
#define ADAFRUIT358_SPI_ERR_READ_CRC           7 //! CRC error during read operation
#define ADAFRUIT358_SPI_ERR_WRITE_CRC          8 //! CRC error during write operation
#define ADAFRUIT358_SPI_ERR_ILLEGAL_COMMAND    9 //! Command not supported
#define ADAFRUIT358_SPI_ERR_WRITE             10 //! Error during write operation
#define ADAFRUIT358_SPI_ERR_OUT_OF_RANGE      11 //! Data access out of range

#ifndef ST7735_H_
#define ST7735_H_

#define LCD_PORT		PORTB
#define LCD_DDR			DDRB
#define LCD_DC			PORTC2
#define LCD_RST			PORTC0
#define LCD_TFT_CS		PORTB2
#define LCD_MOSI		PORTB3
#define LCD_SCK			PORTB5

#define LCD_LITE_PORT	PORTD
#define LCD_LITE_DDR	DDRD
#define LCD_LITE		PORTD6

#define LCD_WIDTH 160
#define LCD_HEIGHT 128
#define LCD_SIZE  LCD_WIDTH * LCD_HEIGHT

// ST7735 registers (based on Lab 4 code)
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09
#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E
#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD
#define ST7735_PWCTR6  0xFC
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void lcd_init(void); // initialize LCD
void sendCommands (const uint8_t *cmds, uint8_t length); //send command
void LCD_setAddr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1); // access pixels on screen 
void SPI_ControllerTx(uint8_t data);
void SPI_ControllerTx_stream(uint8_t stream);
void SPI_ControllerTx_16bit(uint16_t data);
void SPI_ControllerTx_16bit_stream(uint16_t data);

#endif /* ST7735_H_ */
