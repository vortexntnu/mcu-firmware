#include "pwm.h"
#include "uart.h"

// TIMER ISR executes at 500 Hz
void TIMER0_IRQHandler(void)
{
	TIMER_IntClear(TIMER0, TIMER_IF_OF);
}


void TIMER1_IRQHandler(void)
{
	TIMER_IntClear(TIMER1, TIMER_IF_OF);
}


void TIMER2_IRQHandler(void)
{
	TIMER_IntClear(TIMER2, TIMER_IF_OF);
}


void TIMER3_IRQHandler(void)
{
	TIMER_IntClear(TIMER3, TIMER_IF_OF);

}

void initPwm(void)
{
	// Set compare channel pins as output
	// TIM0_CCn LOCATION #?
	GPIO_PinModeSet(THR0_PORT, THR0_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR1_PORT, THR1_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR2_PORT, THR2_PIN, gpioModePushPull, 0);

	// TIM1_CCn LOCATION #?
	GPIO_PinModeSet(THR3_PORT, THR3_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR4_PORT, THR4_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR5_PORT, THR5_PIN, gpioModePushPull, 0);

	// TIM2_CCn LOCATION #?
	GPIO_PinModeSet(THR6_PORT, THR6_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR7_PORT, THR7_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(LIGHT_PORT, LIGHT_PIN, gpioModePushPull, 0);

	// TIM3_CC0 LOCATION #?
	GPIO_PinModeSet(EXTRA_PWM_PORT, EXTRA_PWM_PIN, gpioModePushPull, 0);

	// Setup and initialize timers
	initTimer(TIMER0, PWM_FREQ_SCALING, THRUSTER_START_PULSE_WIDTH_US, TIMER0_CC_LOCATION, TIMER0_NUM_CHANNELS);
	initTimer(TIMER1, PWM_FREQ_SCALING, THRUSTER_START_PULSE_WIDTH_US, TIMER1_CC_LOCATION, TIMER1_NUM_CHANNELS);
	initTimer(TIMER2, PWM_FREQ_SCALING, THRUSTER_START_PULSE_WIDTH_US, TIMER2_CC_LOCATION, TIMER2_NUM_CHANNELS);
	initTimer(TIMER3, LED_PWM_SCALING, 	LED_START_PULSE_WIDTH_US,	   TIMER3_CC_LOCATION, TIMER3_NUM_CHANNELS);

	// Enable TIMERn interrupt vector in NVIC
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);
	NVIC_EnableIRQ(TIMER2_IRQn);
	NVIC_EnableIRQ(TIMER3_IRQn);
}


void initTimer(TIMER_TypeDef *timer, uint32_t pwm_freq_scaling_factor, uint32_t pulse_width_us, uint32_t cc_location, int num_channels)
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

	// Initialize CCn channels
	int ch;

	for (ch = 0; ch < num_channels; ch++)
	{
		TIMER_InitCC(timer, ch, &timerCCInit);
	}

	// Route CCn to location and enable pin
	for (ch = 0; ch < num_channels; ch++)
	{
		timer->ROUTE |= (TIMER_ROUTE_CCPEN(ch) | cc_location);
	}

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
	TIMER_TopSet(timer, CMU_ClockFreqGet(cmuClock_HFPER)/pwm_freq_scaling_factor);

	uint32_t compareValue = us_to_comparevalue(pulse_width_us, timer);

	// Set initial compare value for compare channels
	for (ch = 0; ch < num_channels; ch++)
	{
		TIMER_CompareBufSet(timer, ch, compareValue);
	}

	// Enable overflow interrupt
	TIMER_IntEnable(timer, TIMER_IF_OF);

	// Configure timer
	TIMER_Init(timer, &timerInit);
}
