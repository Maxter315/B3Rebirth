#ifndef MCUFRIEND_KBV_H_
#define MCUFRIEND_KBV_H_   

//#define ILI9327_SPECIAL 
//#define USE_KEIL
//#define USE_SERIAL

#include "Adafruit_GFX.h"
#include "gpio.h"
#include "tim.h"


#define TFTWIDTH  240
#define TFTHEIGHT 320

// Color definitions
#define BLACK       0x0000      /*   0,   0,   0 */
#define NAVY        0x000F      /*   0,   0, 128 */
#define DARKGREEN   0x03E0      /*   0, 128,   0 */
#define DARKCYAN    0x03EF      /*   0, 128, 128 */
#define MAROON      0x7800      /* 128,   0,   0 */
#define PURPLE      0x780F      /* 128,   0, 128 */
#define OLIVE       0x7BE0      /* 128, 128,   0 */
#define LIGHTGRAY   0xC618      /* 192, 192, 192 */
#define DARKGRAY    0x7BEF      /* 128, 128, 128 */
#define BLUE        0x001F      /*   0,   0, 255 */
#define GREEN       0x07E0      /*   0, 255,   0 */
#define CYAN        0x07FF      /*   0, 255, 255 */
#define RED         0xF800      /* 255,   0,   0 */
#define MAGENTA     0xF81F      /* 255,   0, 255 */
#define YELLOW      0xFFE0      /* 255, 255,   0 */
#define WHITE       0xFFFF      /* 255, 255, 255 */
#define ORANGE      0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define PINK        0xF81F
#define GRAY        0x5AEB



#if 1
#define TFT_DATA       GPIOA
#define TFT_D0         LL_GPIO_PIN_0
#define TFT_D1         LL_GPIO_PIN_1
#define TFT_D2         LL_GPIO_PIN_2
#define TFT_D3         LL_GPIO_PIN_3
#define TFT_D4         LL_GPIO_PIN_4
#define TFT_D5         LL_GPIO_PIN_5
#define TFT_D6         LL_GPIO_PIN_6
#define TFT_D7         LL_GPIO_PIN_7
//Port data |D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0 |
//Pin stm32 |PA7|PA6|PA5|PA4|PA3|PA2|PA1|PA0|
#endif



#define TFT_CNTRL      GPIOC
#define LL_LOW(LL_GPIO_PIN)  LL_GPIO_WriteOutputPort(TFT_CNTRL, (LL_GPIO_ReadOutputPort(TFT_CNTRL) & ~(LL_GPIO_PIN)))
#define LL_HIGH(LL_GPIO_PIN) LL_GPIO_WriteOutputPort(TFT_CNTRL, (LL_GPIO_ReadOutputPort(TFT_CNTRL) | LL_GPIO_PIN))

// Note:
// PA15 PB3 PB4 is assigned to JTAG debug port by default on some boards.
// Therefore, it may not be available by default.
#define TFT_RD         LL_GPIO_PIN_4 // Px0
#define TFT_WR         LL_GPIO_PIN_2 // Px1
#define TFT_RS         LL_GPIO_PIN_3 // Px5
#define TFT_CS         LL_GPIO_PIN_1 // Px6
#define TFT_RST        LL_GPIO_PIN_5 // Px7

//#define DELAY          delayMicroseconds(10);
#define DELAY          (void)0  // NOP

#define RD_ACTIVE      LL_LOW(TFT_RD)
#define RD_IDLE        LL_HIGH(TFT_RD)
#define WR_ACTIVE      LL_LOW(TFT_WR)
#define WR_IDLE        LL_HIGH(TFT_WR)
#define CD_COMMAND     LL_LOW(TFT_RS)
#define CD_DATA        LL_HIGH(TFT_RS)
#define CS_ACTIVE      LL_LOW(TFT_CS)
#define CS_IDLE        LL_HIGH(TFT_CS)
#define RST_ACTIVE     LL_LOW(TFT_RST)
#define RST_IDLE       LL_HIGH(TFT_RST)

#define RD_STROBE      {RD_ACTIVE; RD_IDLE;} // Not use
#define WR_STROBE      {WR_ACTIVE; WR_IDLE;} // Not use


#define PROGMEM
#define delay(x)   HAL_Delay(x)

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

#define NOP     __asm__("nop")
#define NOP5    NOP; NOP

#define write8(x)     { write_8(x); WR_ACTIVE; NOP5; WR_IDLE; NOP5;}
#define write16(x)    { uint8_t h = (x)>>8, l = x; write8(h); write8(l); }

#define READ_8(dst)   { RD_ACTIVE; delay_us(2); dst = read8(); RD_IDLE; delay_us(2);}
#define READ_16(dst)  { uint8_t hi; READ_8(hi); READ_8(dst); dst |= (hi << 8); }

#define WriteCmd(x)  { CD_COMMAND; write16(x); }
#define WriteData(x) { CD_DATA; write16(x); }

class MCUFRIEND_kbv : public Adafruit_GFX {

	public:

	MCUFRIEND_kbv(int CS=TFT_CS, int RS=TFT_RS, int WR=TFT_WR, int RD = TFT_RD, int RST=TFT_RST);

	void     reset(void);                                       // you only need the constructor
	void     begin(uint16_t ID = 0x9341);                       // you only need the constructor
	virtual void     drawPixel(int16_t x, int16_t y, uint16_t color);  // and these three
	void     WriteCmdData(uint16_t cmd, uint16_t dat);                 // ?public methods !!!
    void     pushCommand(uint16_t cmd, uint8_t * block, int8_t N);
	uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3); }
	uint16_t readID(void);
	virtual void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	virtual void     drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) { fillRect(x, y, 1, h, color); }
	virtual void     drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) { fillRect(x, y, w, 1, color); }
	virtual void     fillScreen(uint16_t color)                                     { fillRect(0, 0, _width, _height, color); }
	virtual void     setRotation(uint8_t r);
    virtual void     invertDisplay(boolean i);

	uint16_t readReg(uint16_t reg);
	uint32_t readReg32(uint16_t reg);
	int16_t  readGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h);
	uint16_t readPixel(int16_t x, int16_t y) { uint16_t color; readGRAM(x, y, &color, 1, 1); return color; }
	void     setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1);
	void     pushColors(uint16_t *block, int16_t n, bool first);
	void     pushColors(uint8_t *block, int16_t n, bool first);
	void     pushColors(const uint8_t *block, int16_t n, bool first);
	void     vertScroll(int16_t top, int16_t scrollines, int16_t offset);
	//void write_8(uint8_t bytes);
	//uint8_t read8(void);
	//void setWriteDir(void);
	//void setReadDir(void);
	//void WriteCmd(uint16_t c);
	void println(const char * ptr);
	void print(const char * ptr);
	void printdec(int var);

	uint8_t flag_write_bmp;

	private:
	uint16_t _lcd_ID, _lcd_rev, _lcd_madctl, _lcd_drivOut, _MC, _MP, _MW, _SC, _EC, _SP, _EP;
    uint16_t  _lcd_xor, _lcd_capable;
};

#endif
