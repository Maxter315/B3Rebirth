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

#define COSLEN       512
#define BUFLEN       2048

int16_t costable[COSLEN];
int16_t buffer[BUFLEN*2];

SemaphoreHandle_t i2s_complt = NULL;

#define AMP (2048.0)

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

	for(int i = 0; i<20; i++)
	{
		if(tmp & 1)
		{
			notes[i][0] = 1;
		}
		else
		{
			notes[i][0] = 0;
			notes[i][2] = 0;
		}

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




void synth_task(void const * argument)
{
	printf("synth task\r\n");
	fflush(stdout);

    fill_table();
    init_notes();
    fill_buffer();
    i2s_complt = xSemaphoreCreateBinary();
    xSemaphoreGive(i2s_complt);

    HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)buffer, BUFLEN*2);

    i2s_done++;

	while(1)
	{
		if( xSemaphoreTake( i2s_complt, ( TickType_t ) 999 ) == pdTRUE )
		{
			keys_to_notes(g_kb_state);
			oscillator();
		}
		else
		{
			osDelay(1);
		}
	}
}
