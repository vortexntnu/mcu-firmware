#ifndef PWM_H
#define PWM_H

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

#define PWM_OUTPUT_PORT gpioPortD
#define TIMER0_CC_LOCATION TIMER_ROUTE_LOCATION_LOC3
#define TIMER1_CC_LOCATION TIMER_ROUTE_LOCATION_LOC4
#define TIMER2_CC_LOCATION TIMER_ROUTE_LOCATION_LOC1
#define TIMER3_CC_LOCATION TIMER_ROUTE_LOCATION_LOC1

#define LED_PWM_FREQ 500
#define LED_START_PULSE_WIDTH_US 1500

#define THRUSTER_PWM_FREQ 500
#define THRUSTER_START_PULSE_WIDTH_US 1500

#define NUM_THRUSTERS 8

void initPwm(void);
void initTimer(TIMER_TypeDef *timer, uint32_t pwm_freq, uint32_t pulse_width_freq, uint32_t cc_location);
uint32_t us_to_comparevalue(uint32_t us);

void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
void TIMER2_IRQHandler(void);
void TIMER3_IRQHandler(void);

#endif //PWM_H_
