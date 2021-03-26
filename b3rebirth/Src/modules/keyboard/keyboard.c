/*
 * keyboard.c
 *
 *  Created on: Mar 26, 2021
 *      Author: Maxter
 */

#include "stdio.h"
#include "main.h"
#include "tim.h"
#include "cmsis_os.h"

uint32_t get_keyboard(void)
{
	uint32_t var;
	uint32_t res;

	LL_GPIO_WriteOutputPort(GPIOE, 0xE);
	delay_us(10);
	var = (0x1f0 & (~LL_GPIO_ReadInputPort(GPIOE))) >> 4;
	delay_us(10);
	res = var;

	LL_GPIO_WriteOutputPort(GPIOE, 0xD);
	delay_us(10);
	var = (0x1f0 & (~LL_GPIO_ReadInputPort(GPIOE))) >> 4;
	delay_us(10);
	res |= var<<5;

	LL_GPIO_WriteOutputPort(GPIOE, 0xB);
	delay_us(10);
	var = (0x1f0 & (~LL_GPIO_ReadInputPort(GPIOE))) >> 4;
	delay_us(10);
	res |= var<<10;

	LL_GPIO_WriteOutputPort(GPIOE, 0x7);
	delay_us(10);
	var = (0x1f0 & (~LL_GPIO_ReadInputPort(GPIOE))) >> 4;
	delay_us(10);
	res |= var<<15;

	var = (0x400 & (~LL_GPIO_ReadInputPort(GPIOE))) << 11;
	res |= var;

	return res;
}


uint32_t g_kb_state;

void keyboard_task(void const * argument)
{

	for(;;)
	{
		g_kb_state = get_keyboard();

		printf("kb: %.8x\r", (unsigned int)g_kb_state);

		osDelay(10);
	}
}
