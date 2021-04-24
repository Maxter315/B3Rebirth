/*
 * synthesizer.c
 *
 *  Created on: Apr 21, 2021
 *      Author: Maxter
 */

#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "keyboard.h"
#include "string.h"
#include "math.h"
#include "i2s.h"
#include "stdint.h"


#define COSLEN       512
#define BUFLEN       1024

int16_t costable[COSLEN];
int16_t buffer[BUFLEN*2];

SemaphoreHandle_t i2s_complt = NULL;

#define AMP (2048.0)




typedef struct {
	uint8_t  on;
	uint8_t  key_pressed;
	int16_t  envelope_x;
	uint16_t increment;
	uint32_t accumulator;
}generator_t;

#define POLYPHONY   (10)
#define KEYNUM      (20)

#define DDS_TABLE_WIDTH   (9)   // 512 samples
#define DDS_ACC_WIDTH     (20)  // virtual width
#define DDS_IDX_SHIFT     (DDS_ACC_WIDTH - DDS_TABLE_WIDTH)

uint16_t K440_20[] = {7095,7517,7964,8437,8939,9470,10034,10630,11262,11932,12642,13393,14190,15034,15928,16875,17878,18941,20068,21261};

generator_t _gen[KEYNUM];

uint8_t generator_update(uint8_t idx, uint8_t on);
uint8_t generator_update(uint8_t idx, uint8_t on)
{
	if(!(_gen[idx].on))
	{
		_gen[idx].accumulator = 0;
		_gen[idx].envelope_x = 0;

		if(on)
		{
			_gen[idx].on = 1;
		}

	}

	_gen[idx].key_pressed = on;

	return 0;
}

void generator_init(void)
{
	int i;

	for(i = 0; i < KEYNUM; i++)
	{
		_gen[i].increment = K440_20[i];
	}
}


#define ENV_DELAY_MIN    (0)
#define ENV_DELAY_MAX    (2000)
#define ENV_ATTACK_MIN   (50)
#define ENV_ATTACK_MAX   (2000)
#define ENV_HOLD_MIN     (0)
#define ENV_HOLD_MAX     (1000)
#define ENV_DECAY_MIN    (50)
#define ENV_DECAY_MAX    (2000)
#define ENV_SUSTAIN_MIN  (0)
#define ENV_SUSTAIN_MAX  (256)
#define ENV_RELEASE_MIN  (0)
#define ENV_RELEASE_MAX  (5000)

uint16_t env_delay   = ENV_DELAY_MIN + 10;
uint16_t env_attack  = ENV_ATTACK_MIN + 80;
uint16_t env_hold    = ENV_HOLD_MIN + 60;
uint16_t env_decay   = ENV_DECAY_MIN + 100;
uint16_t env_sustain = 170;
uint16_t env_release = ENV_RELEASE_MIN + 300;

uint16_t envelope_gen(int8_t idx)
{
	int16_t x = _gen[idx].envelope_x;

	if (x < env_delay)
	{
		_gen[idx].envelope_x++;
		return 0;
	}

	int16_t x_att = x - env_delay;
	if(x_att < env_attack)
	{
		_gen[idx].envelope_x++;
		return ((256 * x_att) / env_attack);
	}

	int16_t x_hld = x_att - env_attack;
	if(x_hld < env_hold)
	{
		_gen[idx].envelope_x++;
		return 256;
	}

	int16_t x_dec = x_hld - env_hold;
	if(x_dec < env_decay)
	{
		_gen[idx].envelope_x++;
		return (256 - ((256 - env_sustain) * x_dec / env_decay));
	}

	int16_t x_rel = x_dec - env_decay;
	if(x_rel < env_release)
	{
		if(_gen[idx].key_pressed)
		{
			_gen[idx].envelope_x = env_delay + env_attack + env_hold + env_decay;
			return env_sustain;
		}
		else
		{
			_gen[idx].envelope_x++;
			return (env_sustain - (env_sustain * x_rel / env_release));
		}
	}

	_gen[idx].on = 0;

	return 0;
}

void fill_table(void)
{
	for(int i = 0; i < 512; i++)
	{
		costable[i] = (int16_t)(AMP * cos(2 * 3.1415 * ((float)i) / 512.0) + 0*AMP);
	}
}

volatile unsigned int volume;

void fill_buffer(void)
{
	//volume = (get_enc()/2);
	memset(buffer, 0, BUFLEN*2*2);
//	for(int i = 0; i < BUFLEN; i++)
//	{
//		buffer[2 * i + 0] = costable[i & 0x1ff];// >> (15 - volume);
//		buffer[2 * i + 1] = costable[i & 0x1ff];// >> (15 - volume);
//	}
}



volatile int cnt2;
volatile int i2s_done = 0;

int16_t * i2s_buffer = buffer;
static signed long xHigherPriorityTaskWoken;

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    //cnt2++;
	i2s_done++;
	i2s_buffer = &buffer[0];
	xSemaphoreGiveFromISR(i2s_complt, &xHigherPriorityTaskWoken);
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	i2s_done++;
	i2s_buffer = &buffer[BUFLEN];
	xSemaphoreGiveFromISR(i2s_complt, &xHigherPriorityTaskWoken);
}


uint16_t M440[13] = {653, 692, 733, 777, 823, 872, 924, 979, 1037, 1099, 1165, 1234, 1307};

uint16_t notes[20][3];

void init_notes(void)
{
	for(int i = 0; i < 13; i++)
	{
		notes[i][1] = M440[i];
	}
}

void keys_to_notes(uint32_t keys)
{
	uint32_t tmp = keys;

	for(int i = 0; i < KEYNUM; i++)
	{
		generator_update(i, (tmp&1));
		tmp >>= 1;
	}
}

int idx = 0;
int amp = 0;
void oscillator(void)
{
	//amp = get_enc()/2;

	memset(i2s_buffer, 0, BUFLEN*2);

	for(int i = 0; i < 20; i++)
	{
		if(notes[i][0])
		{
			for(int n = 0; n < BUFLEN/2; n++)
			{
				i2s_buffer[2*n + 0] += costable[ (notes[i][2])>>7 ];
				i2s_buffer[2*n + 1] += costable[ (notes[i][2])>>7 ];
				notes[i][2] += notes[i][1] ;//;+ (costable[n & 0x1ff]>>(12-amp));
			}
		}
	}
}

void generator_osc(void)
{
	uint16_t env_value = 0;

	memset(i2s_buffer, 0, BUFLEN*2);

	for(int i = 0; i < KEYNUM; i++)
	{
		if(_gen[i].on)
		{
			for(int t = 0; t < BUFLEN/2; t++)
			{
				if(!(t % 32))
				{
					env_value = envelope_gen(i);
				}

				idx = (_gen[i].accumulator >> DDS_IDX_SHIFT);
				i2s_buffer[2*t + 0] += (costable[ idx ] * env_value) >> 8;
				i2s_buffer[2*t + 1] += (costable[ idx ] * env_value) >> 8;
				_gen[i].accumulator += _gen[i].increment;
				_gen[i].accumulator &= 0xFFFFF;
			}
		}
	}
}


int profile_synth = 0;

void synth_task(void const * argument)
{
	//printf("synth task\r\n");
	//fflush(stdout);
	int val;

    fill_table();
    generator_init();
    //init_notes();
    fill_buffer();
    i2s_complt = xSemaphoreCreateBinary();

    HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)buffer, BUFLEN*2);


	while(1)
	{
		if( xSemaphoreTake( i2s_complt, ( TickType_t ) 999 ) == pdTRUE )
		{
			val = xTaskGetTickCount();
			keys_to_notes(g_kb_state);
			generator_osc();
			profile_synth = xTaskGetTickCount() - val;
		}
	}
}
