/*
 * synthesizer.h
 *
 *  Created on: Apr 21, 2021
 *      Author: Maxter
 */

#ifndef MODULES_INCLUDE_SYNTHESIZER_H_
#define MODULES_INCLUDE_SYNTHESIZER_H_

void synth_task(void const * argument);

#define ENV_DELAY_MIN    (0)
#define ENV_DELAY_MAX    (2000)
#define ENV_ATTACK_MIN   (20)
#define ENV_ATTACK_MAX   (2000)
#define ENV_HOLD_MIN     (0)
#define ENV_HOLD_MAX     (1000)
#define ENV_DECAY_MIN    (20)
#define ENV_DECAY_MAX    (2000)
#define ENV_SUSTAIN_MIN  (0)
#define ENV_SUSTAIN_MAX  (256)
#define ENV_RELEASE_MIN  (0)
#define ENV_RELEASE_MAX  (5000)

extern uint16_t env_delay;
extern uint16_t env_attack;
extern uint16_t env_hold;
extern uint16_t env_decay;
extern uint16_t env_sustain;
extern uint16_t env_release;
extern uint16_t gen_wave;

#endif /* MODULES_INCLUDE_SYNTHESIZER_H_ */
