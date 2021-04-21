/*
 * keyboard.h
 *
 *  Created on: Mar 26, 2021
 *      Author: Maxter
 */

#ifndef MODULES_KEYBOARD_KEYBOARD_H_
#define MODULES_KEYBOARD_KEYBOARD_H_

#include "stdint.h"

void keyboard_task(void const * argument);
extern uint32_t g_kb_state;

#endif /* MODULES_KEYBOARD_KEYBOARD_H_ */
