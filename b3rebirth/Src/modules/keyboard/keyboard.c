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
#include "synthesizer.h"
#include "keyboard.h"

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
uint32_t g_enc_value;
#define VREFINT_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFF7A2A))
uint32_t vrefCal;

int16_t limit_value(int16_t val, int16_t incr, uint16_t minv, uint16_t maxv)
{
	int16_t res;

	res = val + incr;

	if(res < minv) res = minv;
	if(res > maxv) res = maxv;

	return res;
}

uint8_t select_env = 0;

void update_envelope(void)
{
	static int16_t prev_enc = 0;
	static int16_t curr_enc = 0;
	static uint8_t prev_ok = 0;
	static uint8_t curr_ok = 0;
	int8_t inc;

	inc = (int8_t)(curr_enc - prev_enc);

	prev_ok = curr_ok;
	curr_ok = (g_kb_state >> 21) & 1;

	prev_enc = curr_enc;
	curr_enc = (int16_t)g_enc_value;

	if(curr_ok == 1)
	{
//		select_env++;
//		if(select_env > 5) select_env = 0;
		select_env = limit_value(select_env, inc, 0, 6);
	}
	else
	{
		switch(select_env)
		{
			case 0:
				env_delay = limit_value(env_delay, 5 * inc, ENV_DELAY_MIN, ENV_DELAY_MAX);
				break;

			case 1:
				env_attack = limit_value(env_attack, 5 * inc, ENV_ATTACK_MIN, ENV_ATTACK_MAX);
				break;

			case 2:
				env_hold = limit_value(env_hold, 5 * inc, ENV_HOLD_MIN, ENV_HOLD_MAX);
				break;

			case 3:
				env_decay = limit_value(env_decay, 5 * inc, ENV_DECAY_MIN, ENV_DECAY_MAX);
				break;

			case 4:
				env_sustain = limit_value(env_sustain, 2 * inc, ENV_SUSTAIN_MIN, ENV_SUSTAIN_MAX);
				break;

			case 5:
				env_release = limit_value(env_release, 5 * inc, ENV_RELEASE_MIN, ENV_RELEASE_MAX);
				break;

			case 6:
				gen_wave = limit_value(gen_wave, inc, 0, 3);
				break;

			default:
				break;
		}
	}
}

void keyboard_task(void const * argument)
{
	printf("KB start\n\r");

	while(1)
	{
		int adc;
		float adcf, resf;
		g_kb_state = get_keyboard();
		g_enc_value = get_enc()>>1;

		update_envelope();


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
