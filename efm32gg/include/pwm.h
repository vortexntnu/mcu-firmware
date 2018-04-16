#ifndef PWM_H
#define PWM_H

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

#include "rov_utilities.h"

enum pwm_states
{
	PWM_UPDATE_FAIL,
	PWM_UPDATE_OK,
};

#define TIMER_ROUTE_CCPEN(ch) (0x1UL << ch)

#define TIMER0_CC_LOCATION TIMER_ROUTE_LOCATION_LOC3
#define TIMER0_NUM_CHANNELS 3

#define THR0_PORT 	gpioPortD
#define THR0_PIN	1

#define THR1_PORT	gpioPortD
#define THR1_PIN	2

#define THR2_PORT	gpioPortD
#define THR2_PIN	3

#define THR3_PORT	gpioPortB
#define THR3_PIN	7

#define THR4_PORT	gpioPortB
#define THR4_PIN	8

#define THR5_PORT	gpioPortB
#define THR5_PIN	11

#define THR6_PORT	gpioPortA
#define THR6_PIN	12

#define THR7_PORT	gpioPortA
#define THR7_PIN	13

#define LIGHT0_PORT	gpioPortA
#define LIGHT0_PIN	14

#define LIGHT1_PORT	gpioPortE
#define LIGHT1_PIN	14

#define TIMER1_CC_LOCATION TIMER_ROUTE_LOCATION_LOC3
#define TIMER1_NUM_CHANNELS 3

#define TIMER2_CC_LOCATION TIMER_ROUTE_LOCATION_LOC1
#define TIMER2_NUM_CHANNELS 3

#define TIMER3_CC_LOCATION TIMER_ROUTE_LOCATION_LOC0
#define TIMER3_NUM_CHANNELS 1

void initPwm(void);
void initTimer(TIMER_TypeDef *timer,
				uint32_t pwm_freq,
				uint32_t pulse_width_freq,
				uint32_t cc_location,
				int num_channels);

void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);


#endif //PWM_H_
