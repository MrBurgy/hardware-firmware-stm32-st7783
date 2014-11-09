/*
 * STM7783.c
 *
 *  Created on: 07.11.2014
 *      Author: pdickmann
 */

#include "ST7783.h"
#include "stm32f4xx_hal.h"

#define TFTWIDTH   240
#define TFTHEIGHT  320

// // Initialization command table for LCD controller
 #define TFTLCD_DELAY 0xFF

#define LCD_CS  GPIO_PIN_0	// PB0 -> A3 // Chip Select goes to Analog 3
#define LCD_CD  GPIO_PIN_4	// PA4 -> A2 // Command/Data goes to Analog 2
#define LCD_WR  GPIO_PIN_1	// PA1 -> A1 // LCD Write goes to Analog 1
#define LCD_RD  GPIO_PIN_0	// PA0 -> A0 // LCD Read goes to Analog 0
#define LCD_RST GPIO_PIN_1	// PC1 -> RESET


#define RD_ACTIVE  HAL_GPIO_WritePin(GPIOA, LCD_RD, GPIO_PIN_RESET)
#define RD_IDLE    HAL_GPIO_WritePin(GPIOA, LCD_RD, GPIO_PIN_SET)
#define WR_ACTIVE  HAL_GPIO_WritePin(GPIOA, LCD_WR, GPIO_PIN_RESET)
#define WR_IDLE    HAL_GPIO_WritePin(GPIOA, LCD_WR, GPIO_PIN_SET)
#define CD_COMMAND HAL_GPIO_WritePin(GPIOA, LCD_CD, GPIO_PIN_RESET)
#define CD_DATA    HAL_GPIO_WritePin(GPIOA, LCD_CD, GPIO_PIN_SET)
#define CS_ACTIVE  HAL_GPIO_WritePin(GPIOB, LCD_CS, GPIO_PIN_RESET)
#define CS_IDLE    HAL_GPIO_WritePin(GPIOB, LCD_CS, GPIO_PIN_SET)
#define RST_ACTIVE HAL_GPIO_WritePin(GPIOC, LCD_RST, GPIO_PIN_RESET)
#define RST_IDLE   HAL_GPIO_WritePin(GPIOC, LCD_RST, GPIO_PIN_SET)


#define WR_STROBE { WR_ACTIVE; WR_IDLE; }

static const uint16_t ST7781_regValues[] = {
	0x0001,0x0100,
	0x0002,0x0700,
	0x0003,0x1030,
	0x0008,0x0302,
	0x0009,0x0000,
	0x000A,0x0008,
	//*******POWER CONTROL REGISTER INITIAL*******//
	0x0010,0x0790,
	0x0011,0x0005,
	0x0012,0x0000,
	0x0013,0x0000,
	//delayms(50,
	//********POWER SUPPPLY STARTUP 1 SETTING*******//
	0x0010,0x12B0,
	// delayms(50,
	0x0011,0x0007,
	//delayms(50,
	//********POWER SUPPLY STARTUP 2 SETTING******//
	0x0012,0x008C,
	0x0013,0x1700,
	0x0029,0x0022,
	// delayms(50,
	//******GAMMA CLUSTER SETTING******//
	0x0030,0x0000,
	0x0031,0x0505,
	0x0032,0x0205,
	0x0035,0x0206,
	0x0036,0x0408,
	0x0037,0x0000,
	0x0038,0x0504,
	0x0039,0x0206,
	0x003C,0x0206,
	0x003D,0x0408,
	// -----------DISPLAY WINDOWS 240*320-------------//
	0x0050,0x0000,
	0x0051,0x00EF,
	0x0052,0x0000,
	0x0053,0x013F,
	//-----FRAME RATE SETTING-------//
	0x0060,0xA700,
	0x0061,0x0001,
	0x0090,0x0033, //RTNI setting
	//-------DISPLAY ON------//
	0x0007,0x0133,    0x0001,0x0100,
	0x0002,0x0700,
	0x0003,0x1030,
	0x0008,0x0302,
	0x0009,0x0000,
	0x000A,0x0008,
	//*******POWER CONTROL REGISTER INITIAL*******//
	0x0010,0x0790,
	0x0011,0x0005,
	0x0012,0x0000,
	0x0013,0x0000,
	//delayms(50,
	//********POWER SUPPPLY STARTUP 1 SETTING*******//
	0x0010,0x12B0,
	// delayms(50,
	0x0011,0x0007,
	// delayms(50,
	//********POWER SUPPLY STARTUP 2 SETTING******//
	0x0012,0x008C,
	0x0013,0x1700,
	0x0029,0x0022,
	// delayms(50,
	//******GAMMA CLUSTER SETTING******//
	0x0030,0x0000,
	0x0031,0x0505,
	0x0032,0x0205,
	0x0035,0x0206,
	0x0036,0x0408,
	0x0037,0x0000,
	0x0038,0x0504,
	0x0039,0x0206,
	0x003C,0x0206,
	0x003D,0x0408,
	// -----------DISPLAY WINDOWS 240*320-------------//
	0x0050,0x0000,
	0x0051,0x00EF,
	0x0052,0x0000,
	0x0053,0x013F,
	//-----FRAME RATE SETTING-------//
	0x0060,0xA700,
	0x0061,0x0001,
	0x0090,0x0033, //RTNI setting
	//-------DISPLAY ON------//
	0x0007,0x0133,
};

static uint8_t m_rotation = 0;

void delay(unsigned int t)
{
  unsigned char t1;
  while(t--)
  for ( t1=10; t1 > 0; t1-- )
  {
    __asm("nop");
  }
}


void LCD_Begin(void)
{
	uint8_t i = 0;



//	while(1)
//	{
//		LCD_Write8(0);
//	HAL_Delay(1000);
//		LCD_Write8(8);
//	HAL_Delay(1000);
//	}

	LCD_Reset();


	uint16_t a, d;
	//    driver = ID_932X;
	CS_ACTIVE;
	while(i < sizeof(ST7781_regValues) / sizeof(uint16_t)) {
		a = ST7781_regValues[i++];
		d = ST7781_regValues[i++];
		if(a == TFTLCD_DELAY) {
			delay(d);
		} else {
			LCD_WriteRegister16(a, d);
		}
	}
	//setRotation(m_rotation);
	LCD_SetAddrWindow(0, 0, TFTWIDTH-1, TFTHEIGHT-1);
}

void LCD_Reset(void)
{


	CS_IDLE;
	WR_IDLE;
	RD_IDLE;


	RST_ACTIVE;
	delay(100);
	RST_IDLE;


//	if(_reset) {
//	digitalWrite(_reset, LOW);
//	delay(2);
//	digitalWrite(_reset, HIGH);
//	}

	// Data transfer sync
	CS_ACTIVE;

	CD_COMMAND;
	LCD_Write8(0x00);
	for(uint8_t i=0; i<3; i++) WR_STROBE; // Three extra 0x00s
	CS_IDLE;
}

void LCD_Write8(uint8_t data)
{

//	GPIOA->ODR = (GPIOA->ODR & !GPIO_PIN_9)  | (data & 0x01);		// LCD_D0 -> PA9
//	GPIOA->ODR = (GPIOC->ODR & !GPIO_PIN_7)  | (data & 0x02);		// LCD_D1 -> PC7
//	GPIOA->ODR = (GPIOA->ODR & !GPIO_PIN_10) | (data & 0x04);		// LCD_D2 -> PA10
//	GPIOA->ODR = (GPIOB->ODR & !GPIO_PIN_3)  | (data & 0x08);		// LCD_D3 -> PB3
//	GPIOA->ODR = (GPIOB->ODR & !GPIO_PIN_5)  | (data & 0x10);		// LCD_D4 -> PB5
//	GPIOA->ODR = (GPIOB->ODR & !GPIO_PIN_4)  | (data & 0x20);		// LCD_D5 -> PB4
//	GPIOA->ODR = (GPIOB->ODR & !GPIO_PIN_10) | (data & 0x40);		// LCD_D6 -> PB10
//	GPIOA->ODR = (GPIOA->ODR & !GPIO_PIN_8)  | (data & 0x80);		// LCD_D7 -> PA8
//
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, (data & 0x01) !=0);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, (data & 0x02) !=0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,(data & 0x04) !=0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (data & 0x08) !=0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (data & 0x10) !=0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (data & 0x20) !=0);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,(data & 0x40) !=0);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, (data & 0x80) !=0);

//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, (data & 0x01) ==0);
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, (data & 0x02) ==0);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,(data & 0x04) ==0);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (data & 0x08) ==0);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (data & 0x10) ==0);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (data & 0x20) ==0);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10,(data & 0x40) ==0);
//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, (data & 0x80) ==0);

}

void LCD_WriteRegister8(uint8_t a, uint8_t d)
{
	CD_COMMAND; LCD_Write8(a); CD_DATA; LCD_Write8(d);
}

void LCD_WriteRegister16(uint16_t a, uint16_t d)
{
  uint8_t hi, lo;
  hi = (a) >> 8; lo = (a); CD_COMMAND; LCD_Write8(hi); LCD_Write8(lo);
  hi = (d) >> 8; lo = (d); CD_DATA   ; LCD_Write8(hi); LCD_Write8(lo);
}

// Sets the LCD address window (and address counter, on 932X).
// Relevant to rect/screen fills and H/V lines.  Input coordinates are
// assumed pre-sorted (e.g. x2 >= x1).
void LCD_SetAddrWindow(int x1, int y1, int x2, int y2)
{

	CS_ACTIVE;

	// Values passed are in current (possibly rotated) coordinate
	// system.  932X requires hardware-native coords regardless of
	// MADCTL, so rotate inputs as needed.  The address counter is
	// set to the top-left corner -- although fill operations can be
	// done in any direction, the current screen rotation is applied
	// because some users find it disconcerting when a fill does not
	// occur top-to-bottom.
	int x, y, t;
	switch(m_rotation) {
		default:
			x  = x1;
			y  = y1;
			break;
		case 1:
			t  = y1;
			y1 = x1;
			x1 = TFTWIDTH  - 1 - y2;
			y2 = x2;
			x2 = TFTWIDTH  - 1 - t;
			x  = x2;
			y  = y1;
			break;
		case 2:
			t  = x1;
			x1 = TFTWIDTH  - 1 - x2;
			x2 = TFTWIDTH  - 1 - t;
			t  = y1;
			y1 = TFTHEIGHT - 1 - y2;
			y2 = TFTHEIGHT - 1 - t;
			x  = x2;
			y  = y2;
			break;
		case 3:
			t  = x1;
			x1 = y1;
			y1 = TFTHEIGHT - 1 - x2;
			x2 = y2;
			y2 = TFTHEIGHT - 1 - t;
			x  = x1;
			y  = y2;
			break;
	}

	LCD_WriteRegister16(0x0050, x1); // Set address window
	LCD_WriteRegister16(0x0051, x2);
	LCD_WriteRegister16(0x0052, y1);
	LCD_WriteRegister16(0x0053, y2);
	LCD_WriteRegister16(0x0020, x ); // Set address counter to top left
	LCD_WriteRegister16(0x0021, y );


	CS_IDLE;
}

// Fast block fill operation for fillScreen, fillRect, H/V line, etc.
// Requires setAddrWindow() has previously been called to set the fill
// bounds.  'len' is inclusive, MUST be >= 1.
void LCD_Flood(uint16_t color, uint32_t len)
{
  uint16_t blocks;
  uint8_t  i, hi = color >> 8,
              lo = color;

	CS_ACTIVE;
	CD_COMMAND;
	LCD_Write8(0x00); // High byte of GRAM register...
	LCD_Write8(0x22); // Write data to GRAM

	// Write first pixel normally, decrement counter by 1
	CD_DATA;
	LCD_Write8(hi);
	LCD_Write8(lo);
	len--;

	blocks = (uint16_t)(len / 64); // 64 pixels/block
	if(hi == lo) {
		// High and low bytes are identical.  Leave prior data
		// on the port(s) and just toggle the write strobe.
		while(blocks--) {
			i = 16; // 64 pixels/block / 4 pixels/pass
			do {
				WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // 2 bytes/pixel
				WR_STROBE; WR_STROBE; WR_STROBE; WR_STROBE; // x 4 pixels
			} while(--i);
		}
				// Fill any remaining pixels (1 to 64)
		for(i = (uint8_t)len & 63; i--; ) {
			WR_STROBE;
			WR_STROBE;
		}
	} else {
		while(blocks--) {
				i = 16; // 64 pixels/block / 4 pixels/pass
			do {
				LCD_Write8(hi); LCD_Write8(lo); LCD_Write8(hi); LCD_Write8(lo);
				LCD_Write8(hi); LCD_Write8(lo); LCD_Write8(hi); LCD_Write8(lo);
			} while(--i);
		}
		for(i = (uint8_t)len & 63; i--; ) {
			LCD_Write8(hi);
			LCD_Write8(lo);
		}
	}
	CS_IDLE;
}

void LCD_FillScreen(uint16_t color)
{
	// For the 932X, a full-screen address window is already the default
	// state, just need to set the address pointer to the top-left corner.
	// Although we could fill in any direction, the code uses the current
	// screen rotation because some users find it disconcerting when a
	// fill does not occur top-to-bottom.
	uint16_t x, y;
	switch(m_rotation) {
		default: x = 0            ; y = 0            ; break;
		case 1 : x = TFTWIDTH  - 1; y = 0            ; break;
		case 2 : x = TFTWIDTH  - 1; y = TFTHEIGHT - 1; break;
		case 3 : x = 0            ; y = TFTHEIGHT - 1; break;
	}
	CS_ACTIVE;
	LCD_WriteRegister16(0x0020, x);
	LCD_WriteRegister16(0x0021, y);

	LCD_Flood(color, (long)TFTWIDTH * (long)TFTHEIGHT);
}

void LCD_DrawPixel(int16_t x, int16_t y, uint16_t color) {

  // Clip
  if((x < 0) || (y < 0) || (x >= TFTWIDTH/*_width*/) || (y >= TFTHEIGHT/*_height*/)) return;

  CS_ACTIVE;

    int16_t t;
    switch(m_rotation) {
     case 1:
      t = x;
      x = TFTWIDTH  - 1 - y;
      y = t;
      break;
     case 2:
      x = TFTWIDTH  - 1 - x;
      y = TFTHEIGHT - 1 - y;
      break;
     case 3:
      t = x;
      x = y;
      y = TFTHEIGHT - 1 - t;
      break;
    }
    LCD_WriteRegister16(0x0020, x);
    LCD_WriteRegister16(0x0021, y);
    LCD_WriteRegister16(0x0022, color );

  CS_IDLE;
}

void LCD_DrawFastHLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
  int16_t x2;

  // Initial off-screen clipping
  if((length <= 0     ) ||
     (y      <  0     ) || ( y                  >= TFTHEIGHT/*_height*/) ||
     (x      >= TFTWIDTH/*_width*/) || ((x2 = (x+length-1)) <  0      )) return;

  if(x < 0) {        // Clip left
    length += x;
    x       = 0;
  }
  if(x2 >= TFTWIDTH/*_width*/) { // Clip right
    x2      = TFTWIDTH/*_width*/ - 1;
    length  = x2 - x + 1;
  }

  LCD_SetAddrWindow(x, y, x2, y);
  LCD_Flood(color, length);
  LCD_SetAddrWindow(0, 0, TFTWIDTH/*_width*/ - 1, TFTHEIGHT/*_height*/ - 1);

}
