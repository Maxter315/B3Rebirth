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
#include "adc.h"

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
#define VREFINT_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFF7A2A))
uint32_t vrefCal;

void keyboard_task(void const * argument)
{
	printf("KB start\n\r");

	while(1)
	{
		int adc;
		float adcf, resf;
		g_kb_state = get_keyboard();

		/*
		//printf("kb: %.8x\r", (unsigned int)g_kb_state);
        HAL_ADC_Start(&hadc1); // запускаем преобразование сигнала АЦП
        HAL_ADC_PollForConversion(&hadc1, 100); // ожидаем окончания преобразования
        adc = HAL_ADC_GetValue(&hadc1); // читаем полученное значение в переменную adc
        HAL_ADC_Stop(&hadc1); // останавливаем АЦП (не обязательно)

        adcf = adc;
        vrefCal = *VREFINT_CAL_ADDR;
        resf = (3.3 * ((1510)/adcf));

		printf("ADC: %d        %d\r\n", (int)(resf*1000), vrefCal);
		fflush(stdout);
		*/
		osDelay(20);
	}
}
