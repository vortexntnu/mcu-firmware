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

uint32_t us_to_comparevalue(uint32_t us)
{
	uint32_t hz_to_us = 1000000 / THRUSTER_PWM_FREQ;

	if((us < THRUSTER_MIN_PULSE_WIDTH_US) || (us > THRUSTER_MAX_PULSE_WIDTH_US))
	{
		return ((CMU_ClockFreqGet(cmuClock_HFPER) / THRUSTER_PWM_FREQ) * THRUSTER_START_PULSE_WIDTH_US) / hz_to_us;
	}

	return ((CMU_ClockFreqGet(cmuClock_HFPER) / THRUSTER_PWM_FREQ) * us) / hz_to_us;
}

uint8_t update_thruster_pwm(uint8_t *pwm_data_ptr)
{
	int i;
	uint16_t pwm_data[NUM_THRUSTERS] = {1500};

	for (i = 0; i < NUM_THRUSTERS; i++)
	{
		pwm_data[i] = (uint16_t)((*pwm_data_ptr << 8) & 0xFF00);
		pwm_data_ptr++;
		pwm_data[i] |= (uint16_t)((*pwm_data_ptr) & 0x00FF);
		pwm_data_ptr++;
	}

	int ch;

	for (ch = 0; ch < 3; ch++)
	{
		TIMER_CompareBufSet(TIMER0, ch, us_to_comparevalue(pwm_data[ch]));
		TIMER_CompareBufSet(TIMER1, ch, us_to_comparevalue(pwm_data[ch + 3]));
		TIMER_CompareBufSet(TIMER2, ch, us_to_comparevalue(pwm_data[ch + 5]));
	}

	return PWM_UPDATE_OK;
}

uint8_t update_led_pwm(uint8_t *pwm_data_ptr)
{
	return PWM_UPDATE_OK;
}

void initPwm(void)
{
	// Enable clock for GPIO module
	CMU_ClockEnable(cmuClock_GPIO, true);

	// Enable clock for TIMERn modules
	CMU_ClockEnable(cmuClock_TIMER0, true);
	CMU_ClockEnable(cmuClock_TIMER1, true);
	CMU_ClockEnable(cmuClock_TIMER2, true);
	CMU_ClockEnable(cmuClock_TIMER3, true);

	// Set compare channel pins as output
	// TIM0_CCn LOCATION #3
	GPIO_PinModeSet(THR0_PORT, THR0_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR1_PORT, THR1_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR2_PORT, THR2_PIN, gpioModePushPull, 0);

	// TIM1_CCn LOCATION #3
	GPIO_PinModeSet(THR3_PORT, THR3_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR4_PORT, THR4_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR5_PORT, THR5_PIN, gpioModePushPull, 0);

	// TIM2_CCn LOCATION #1
	GPIO_PinModeSet(THR6_PORT, THR6_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(THR7_PORT, THR7_PIN, gpioModePushPull, 0);
	GPIO_PinModeSet(LIGHT0_PORT, LIGHT0_PIN, gpioModePushPull, 0);

	// TIM3_CC0 LOCATION #0
	GPIO_PinModeSet(LIGHT1_PORT, LIGHT1_PIN, gpioModePushPull, 0);

	// Setup and initialize timers
	initTimer(TIMER0, THRUSTER_PWM_FREQ, THRUSTER_START_PULSE_WIDTH_US, TIMER0_CC_LOCATION, TIMER0_NUM_CHANNELS);
	initTimer(TIMER1, THRUSTER_PWM_FREQ, THRUSTER_START_PULSE_WIDTH_US, TIMER1_CC_LOCATION, TIMER1_NUM_CHANNELS);
	initTimer(TIMER2, THRUSTER_PWM_FREQ, THRUSTER_START_PULSE_WIDTH_US, TIMER2_CC_LOCATION, TIMER2_NUM_CHANNELS);
	initTimer(TIMER3, LED_PWM_FREQ, 	 LED_START_PULSE_WIDTH_US,		TIMER3_CC_LOCATION, TIMER3_NUM_CHANNELS);

	// Enable TIMERn interrupt vector in NVIC
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);
	NVIC_EnableIRQ(TIMER2_IRQn);
	NVIC_EnableIRQ(TIMER3_IRQn);
}

void initTimer(TIMER_TypeDef *timer, uint32_t pwm_freq, uint32_t pulse_width_us, uint32_t cc_location, int num_channels)
{
	// Reset timer
	TIMER_Reset(timer);

	CMU_ClockDivSet(cmuClock_HF, cmuClkDiv_2);

	// Start HFXO and wait until it is stable
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

	 // Select HFXO as clock source for HFPER
	CMU_ClockSelectSet(cmuClock_HFPER, cmuSelect_HFXO);

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
	TIMER_TopSet(timer, CMU_ClockFreqGet(cmuClock_HFPER)/pwm_freq);

	uint32_t compareValue = us_to_comparevalue(pulse_width_us);

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
