/*
 * testlcd.c
 *
 *  Created on: 11 окт. 2020 г.
 *      Author: Maxter
 */

//#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#include "stdio.h"
#include "main.h"
#include "tim.h"
#include "cmsis_os.h"

extern "C" void update_disp(void);

unsigned int tt0, tt1, ttx;
unsigned long testFillScreen()
{

  tft.fillScreen(BLACK);
//
  //ttx = get_clock();
  HAL_Delay(1000);
  //tt0 = get_clock();
  tft.fillScreen(RED);
  //tt1 = get_clock();
  printf("RED: %d [%d]\r\n", tt1-tt0, tt0 - ttx);
  HAL_Delay(1000);
  tft.fillScreen(GREEN);
  HAL_Delay(1000);
  tft.fillScreen(BLUE);
  HAL_Delay(1000);
  tft.fillScreen(BLACK);
  HAL_Delay(1000);
  tft.fillScreen(YELLOW);
  HAL_Delay(1000);
  tft.fillScreen(MAGENTA);

  //for(int i = 0; i<200; i++)
  {
	  //HAL_Delay(100);
	  //tft.fillScreen(((i & 0xF1)<<8)|((i & 0xfc)<<3)|(i>>3));
  }

  return 0;
}

void test_delay(void)
{
	unsigned int te;

	//set_clock(0);
	HAL_Delay(1000);
	//te = get_clock();
	printf("HAL1000 [%d]\t", te);

	//set_clock(0);
	delay_us(1000);
	//te = get_clock();
	printf("us1000 [%d]\t\n\r", te);

}

extern "C" void lcd_init(void)
{
	uint32_t ID = 0;
	ID = tft.readID();
	printf("ID = [%.8x]  res[%d, %d]\n\r", (unsigned int)ID, 10, 20);
	tft.begin(ID);
	tft.fillScreen(MAGENTA);
	tft.setRotation(3);
}

extern "C" void test_lcd(void)
{
  uint32_t ID = 0;

  //printf("Begin testing\n\r");

  //uint8_t var;
  
  //ID = tft.readID();
  //setResolution(240, 400);

  //printf("ID = [%.8x]  res[%d, %d]\n\r", (unsigned int)ID, 10, 20);

  //test_delay();

  //printf("RED:asdasdas\r\n");
  //tft.begin(ID);
  testFillScreen();

  update_disp();
  HAL_Delay(10000);
}



void fill_template1(int x0, int y0, int w, int h);
void draw_knob(void);
int cnt = 0;
extern int i2s_done;
extern int profile_synth;

extern "C" void update_disp(void)
{
	//tft.fillScreen(BLACK);
	//tft.fillRect(0, 0, 150, 50, BLACK);
	tft.setCursor(0,0);
	tft.println("Hello world");
	tft.print("synth ms: ");
	tft.printdec(profile_synth);
	tft.print("\n");
	tft.setTextColor(WHITE, BLACK);
	tft.print("Kakes: ");
	tft.printdec(cnt);
	tft.print("\n");
	tft.print("\n");
	cnt++;

//	for(int i = 0; i<200; i++)
//	{
//		tft.print("LOL: ");
//		tft.printdec(i * 10);
//		tft.print("\r");
//		delay_us(5000);
//		//HAL_Delay(500);
//	}

//	for(int x = 0; x < 50; x+=10)
//	{
//		for(int y = 100; y < 150; y+=10)
//		{
//			fill_template1(x, y, 50, 100);
//		}
//	}


//	tft.fillScreen(BLACK);
//
//	draw_knob();
//	HAL_Delay(5000);

//	int x = 0, y = 0;
//	uint16_t color = 0;
//	for(int r = 0; r < 256; r+=2)
//	{
//		for(int g = 0; g < 256; g+=4)
//		{
//			for(int b = 0; b < 256; b+=4)
//			{
//				color = ((r & 0xF1)<<8)|((g & 0xfc)<<3)|(b>>3);
//				tft.fillRect(x, y, 3, 3, color);
//				x+=3;
//			}
//			y+=3;
//			x=0;
//		}
//		//HAL_Delay(50);
//		y = 0;
//	}


	//fill_template1(0,0,0,0);

//	for(int x = 0; x < 240; x+=20)
//	{
//		for(int y = 0; y < 400; y+=20)
//		{
//			tft.drawPixel(x, y, WHITE);
//			tft.drawLine(x, y, 120, 200, GREEN);
//		}
//	}



}

void fill_template1(int x0, int y0, int w, int h)
{
	uint16_t color;
	//for(int x = 0; x < 240; )

	__asm__("nop");
	for(unsigned int y = 0; y < 256; y++)
	{
		color = (0xF100) | ((y & 0xfc)<<3) | (0xf);

		tft.drawLine(0, y, 100, y, color);
		//HAL_Delay(100);
	}

	//tft.drawRect(x0, y0, w, h, WHITE);
}

void draw_knob(void)
{
	tft.drawRect(0, 0, 48, 48, WHITE);

	tft.setTextColor(WHITE, BLACK);
	tft.setCursor(16,16);
	tft.print("-100");


//	tft.drawLine(0, 0, 5, 0, WHITE);
//	tft.drawLine(0, 0, 0, 5, WHITE);
//
//	tft.drawLine(0, 0, 5, 0, WHITE);
//	tft.drawLine(0, 0, 5, 0, WHITE);
}


extern "C" void disptask(void)
{
	printf("Display Start\n\r");

	lcd_init();

	while(1)
	{
		update_disp();
		osDelay(1000);
	}
}

