#ifndef PWM_H
#define PWM_H


#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"

#define TOP_VAL_PWM 100000     // sets PWM frequency to 1kHz (1MHz timer clock)
#define TOP_VAL_GP_TIMER 1000   // sets general purpose timer overflow frequency to 1kHz (1MHz timer clock)
#define UPDATE_PERIOD 5       // update compare value, toggle LED1 every 1/4 second (250ms)
#define INC_VAL (TOP_VAL_PWM/4) // adjust compare value amount


void init_Pwm(void);
void TIMER0_IRQHandler(void);
void generate_PWM(void);
char getCounter();

#endif //PWM_H_
