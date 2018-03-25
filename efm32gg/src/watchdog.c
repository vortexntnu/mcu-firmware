#include "watchdog.h"

void initWdog(void)
{

	CMU_ClockEnable(cmuClock_CORELE, true);

	CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);

	WDOG_Init_TypeDef wdogInit =
	{
		.enable     = true,                // Start watchdog when init done
		.debugRun   = true,                // WDOG counting during debug halt
		.em2Run     = true,                // WDOG counting when in EM2
		.em3Run     = false,               // WDOG counting when in EM3
		.em4Block   = false,               // EM4 can be entered
		.swoscBlock = false,               // Do not block disabling LFRCO/LFXO in CMU
		.lock       = true ,               // Lock WDOG configuration (if locked, reset needed to unlock)
		.clkSel     = wdogClkSelLFRCO,	   // Select 32kHz WDOG oscillator
		.perSel     = wdogPeriod_64k,
	};

	WDOGn_Init(WDOG, &wdogInit);

	WDOGn_Enable(WDOG, true);

	WDOGn_Lock(WDOG);

	WDOGn_Feed(WDOG);

}
