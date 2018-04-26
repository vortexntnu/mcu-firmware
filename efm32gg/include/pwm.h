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

#define TIMER1_CC_LOCATION TIMER_ROUTE_LOCATION_LOC3
#define TIMER1_NUM_CHANNELS 3

#define TIMER2_CC_LOCATION TIMER_ROUTE_LOCATION_LOC1
#define TIMER2_NUM_CHANNELS 3

#define TIMER3_CC_LOCATION TIMER_ROUTE_LOCATION_LOC0
#define TIMER3_NUM_CHANNELS 1

void initPwm(void);
void initTimer(TIMER_TypeDef *timer,
				uint32_t pwm_freq_scaling_factor,
				uint32_t pulse_width_freq,
				uint32_t cc_location,
				int num_channels);

void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);


#endif //PWM_H_
