/***************************************************************************//**
 * @file
 * @brief FreeRTOS Blink Demo for Energy Micro EFM32GG_STK3700 Starter Kit
 * @version 5.2.2
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"

#include "em_chip.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "em_device.h"
#include "bsp.h"
#include "bsp_trace.h"

#include "sleep.h"
#include "efm32gg990f1024.h"

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define TOP_VAL_PWM 1000        // sets PWM frequency to 1kHz (1MHz timer clock)
#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define LED_TASK_PRIORITY      (tskIDLE_PRIORITY + 1)

// Setup Timer Configuration for PWM
  TIMER_Init_TypeDef timerPWMInit =
  {
    .enable     = true,                 // start timer upon configuration
    .debugRun   = true,                 // run timer in debug mode
    .prescale   = timerPrescale1,       // set prescaler to /1
    .clkSel     = timerClkSelHFPerClk,  // set clock source as HFPERCLK
    .fallAction = timerInputActionNone, // no action from inputs
    .riseAction = timerInputActionNone, // no action from inputs
    .mode       = timerModeUp,          // use up-count mode
    .dmaClrAct  = false,                // not using DMA
    .quadModeX4 = false,                // not using Quad Dec. mode
    .oneShot    = false,                // not using one shot mode
    .sync       = false,                // not syncronizing timer3 with other timers
  };

  // Setup Timer Channel Configuration for PWM
  TIMER_InitCC_TypeDef timerCCInit = {
    .eventCtrl  = timerEventEveryEdge,    // This value will be ignored since we aren't using input capture
    .edge       = timerEdgeNone,          // This value will be ignored since we aren't using input capture
    .prsSel     = timerPRSSELCh0,         // This value will be ignored since we aren't using PRS
    .cufoa      = timerOutputActionNone,  // No action on underflow (up-count mode)
    .cofoa      = timerOutputActionSet,   // On overflow, we want the output to go high, but in PWM mode this should happen automatically
    .cmoa       = timerOutputActionClear, // On compare match, we want output to clear, but in PWM mode this should happen automatically
    .mode       = timerCCModePWM,         // Set timer channel to run in PWM mode
    .filter     = false,                  // Not using input, so don't need a filter
    .prsInput   = false,                  // Not using PRS
    .coist      = false,                  // Initial state for PWM is high when timer is enabled
    .outInvert  = false,                  // non-inverted output
  };

uint16_t ms_counter = 0;


/* Structure with parameters for LedBlink */
typedef struct {
  /* Delay between blink of led */
  portTickType delay;
  /* Number of led */
  int          ledNo;
} TaskParams_t;

/***************************************************************************//**
 * @brief Simple task which is blinking led
 * @param *pParameters pointer to parameters passed to the function
 ******************************************************************************/
static void LedBlink(void *pParameters)
{
  TaskParams_t     * pData = (TaskParams_t*) pParameters;
  const portTickType delay = pData->delay;

  for (;; ) {
	  	  if(CHECK_BIT(TIMER0->STATUS,0)){
	      BSP_LedToggle(pData->ledNo);
	      vTaskDelay(delay);
	  	  }
  	  }

  }

void TIMER0_IRQHandler(void)
{
  TIMER0->IFC = 1;                              // Clear overflow flag
  ms_counter++;									// Increment counter
}


/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{

	uint32_t compare_val = 0;

	/* Chip errata */
	CHIP_Init();
	CMU_HFRCOBandSet(cmuHFRCOBand_1MHz);    // Set HF oscillator to 1MHz and use as system source
	CMU_ClockEnable(cmuClock_TIMER0, true); // Start TIMER0 peripheral clock

	/* If first word of user data page is non-zero, enable eA Profiler trace */
	BSP_TraceProfilerSetup();

	/* Initialize LED driver */
	BSP_LedsInit();
	/* Setting state of leds*/
	BSP_LedSet(0);
	BSP_LedSet(1);

	TIMER_TopSet(TIMER0, TOP_VAL_PWM);
	TIMER_Init(TIMER0, &timerPWMInit);

	TIMER_InitCC(TIMER3, 2, &timerCCInit);       // apply channel configuration to Timer3 channel 2
	TIMER3->ROUTE = (1 << 16) |(1 << 2);
	TIMER_CounterSet(TIMER0, 0);
	TIMER_CompareSet(TIMER0, 2, compare_val);    // Set CC2 compare value (0% duty)
	TIMER_CompareBufSet(TIMER0, 2, compare_val); // Set CC2 compare buffer value (0% duty)
	NVIC_EnableIRQ(TIMER0_IRQn);



	/* Initialize SLEEP driver, no callbacks are used */
	SLEEP_Init(NULL, NULL);
	#if (configSLEEP_MODE < 3)
	/* do not let to sleep deeper than define */
	SLEEP_SleepBlockBegin((SLEEP_EnergyMode_t)(configSLEEP_MODE + 1));
	#endif

	/* Parameters value for tasks*/
	static TaskParams_t parametersToTask1 = { pdMS_TO_TICKS(250), 0 };
	static TaskParams_t parametersToTask2 = { pdMS_TO_TICKS(500), 1 };

	/*Create two task for blinking leds*/
	xTaskCreate(LedBlink, (const char *) "LedBlink1", STACK_SIZE_FOR_TASK, &parametersToTask1, LED_TASK_PRIORITY, NULL);
	xTaskCreate(LedBlink, (const char *) "LedBlink2", STACK_SIZE_FOR_TASK, &parametersToTask2, LED_TASK_PRIORITY, NULL);
	//xTaskCreate(TimerInit, (const char *) "TimerInit", STACK_SIZE_FOR_TASK, NULL, LED_TASK_PRIORITY, NULL);


	/*Start FreeRTOS Scheduler*/
	vTaskStartScheduler();
	return 0;
}
