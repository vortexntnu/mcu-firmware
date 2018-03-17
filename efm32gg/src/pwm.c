#include "pwm.h"

uint32_t compareValue_1 = 0;
int k_1 = 1;

// TIMER ISR executes at 500 Hz
void TIMER0_IRQHandler(void)
{


	if(++compareValue_1 >= 300)
	{
		compareValue_1 = 0;

		if(k_1==1)
		{
			TIMER_CompareBufSet(TIMER0, 0, us_to_comparevalue(1500));
			k_1 = 0;
		}
		else
		{
			TIMER_CompareBufSet(TIMER0, 0, us_to_comparevalue(1400));
			k_1 = 1;
		}
	}


}

void TIMER1_IRQHandler(void)
{

}

void TIMER2_IRQHandler(void)
{

}

void TIMER3_IRQHandler(void)
{

}

uint32_t us_to_comparevalue(uint32_t us)
{

	uint32_t hz_to_us = 1000000 / THRUSTER_PWM_FREQ;

	if((us < 1100) || (us > 1900))
	{
		return ((CMU_ClockFreqGet(cmuClock_HFPER) / THRUSTER_PWM_FREQ) * THRUSTER_START_PULSE_WIDTH_US) / hz_to_us;
	}

	return ((CMU_ClockFreqGet(cmuClock_HFPER) / THRUSTER_PWM_FREQ) * us) / hz_to_us;
}

void initPwm(void)
{

	// Enable clock for GPIO module
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Enable clock for TIMERn modules
	CMU_ClockEnable(cmuClock_TIMER0, true);
	CMU_ClockEnable(cmuClock_TIMER1, true);
	CMU_ClockEnable(cmuClock_TIMER2, true);

	// Set compare channel pins for thruster PWM as output
	GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortD, 2, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortD, 3, gpioModePushPull, 0);

	GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortD, 7, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortC, 4, gpioModePushPull, 0);

	GPIO_PinModeSet(gpioPortA, 12, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortA, 13, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortA, 14, gpioModePushPull, 0);

	GPIO_PinModeSet(gpioPortE, 0, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortE, 1, gpioModePushPull, 0);
	GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0);

	// Setup and initialize timers
	initTimer(TIMER0, THRUSTER_PWM_FREQ, THRUSTER_START_PULSE_WIDTH_US, TIMER0_CC_LOCATION);
	initTimer(TIMER1, THRUSTER_PWM_FREQ, THRUSTER_START_PULSE_WIDTH_US, TIMER1_CC_LOCATION);
	initTimer(TIMER2, THRUSTER_PWM_FREQ, THRUSTER_START_PULSE_WIDTH_US, TIMER2_CC_LOCATION);
	initTimer(TIMER3, LED_PWM_FREQ, 	 LED_START_PULSE_WIDTH_US,		TIMER3_CC_LOCATION);

	// Enable TIMERn interrupt vector in NVIC
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);
	NVIC_EnableIRQ(TIMER2_IRQn);
	NVIC_EnableIRQ(TIMER3_IRQn);

}

void initTimer(TIMER_TypeDef *timer, uint32_t pwm_freq, uint32_t pulse_width_us, uint32_t cc_location)
{
	// Reset timer
	TIMER_Reset(timer);

	TIMER_InitCC_TypeDef timerCCInit =
	{
		.eventCtrl  = timerEventEveryEdge,
		.edge       = timerEdgeBoth,
		.prsSel     = timerPRSSELCh0,
		.cufoa      = timerOutputActionNone,
		.cofoa      = timerOutputActionNone,
		.cmoa       = timerOutputActionToggle,
		.mode       = timerCCModePWM,
		.filter     = false,
		.prsInput   = false,
		.coist      = false,
		.outInvert  = false,
	};

	// Initialize CC channels 0-2
	TIMER_InitCC(timer, 0, &timerCCInit);
	TIMER_InitCC(timer, 1, &timerCCInit);
	TIMER_InitCC(timer, 2, &timerCCInit);

	// Route CCn to location and enable pin
	timer->ROUTE |= (TIMER_ROUTE_CC0PEN | cc_location);
	timer->ROUTE |= (TIMER_ROUTE_CC1PEN | cc_location);
	timer->ROUTE |= (TIMER_ROUTE_CC2PEN | cc_location);

	// Select timer parameters
	TIMER_Init_TypeDef timerInit =
	{
		.enable     = true,
		.debugRun   = true,
		.prescale   = timerPrescale1,
		.clkSel     = timerClkSelHFPerClk,
		.fallAction = timerInputActionNone,
		.riseAction = timerInputActionNone,
		.mode       = timerModeUp,
		.dmaClrAct  = false,
		.quadModeX4 = false,
		.oneShot    = false,
		.sync       = false,
	};

	// Set Top Value
	TIMER_TopSet(timer, CMU_ClockFreqGet(cmuClock_HFPER)/pwm_freq);

	uint32_t compareValue = us_to_comparevalue(pulse_width_us);

	// Set initial compare value for channels 0-2
	TIMER_CompareBufSet(timer, 0, compareValue);
	TIMER_CompareBufSet(timer, 1, compareValue);
	TIMER_CompareBufSet(timer, 2, compareValue);

	// Enable overflow interrupt
	TIMER_IntEnable(timer, TIMER_IF_OF);

	// Configure timer
	TIMER_Init(timer, &timerInit);
}
