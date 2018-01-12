#include "em_device.h"
#include "em_cmu.h"
#include "em_chip.h"            // required for CHIP_Init() function
#include "em_gpio.h"
#include "em_timer.h"

#define TOP_VAL_CU_TIMER 1000
#define TOP_VAL_PWM 1000        // sets PWM frequency to 1kHz (1MHz timer clock)
#define TOP_VAL_GP_TIMER 1000   // sets general purpose timer overflow frequency to 1kHz (1MHz timer clock)
#define UPDATE_PERIOD 250       // update compare value, toggle LED1 every 1/4 second (250ms)
#define INC_VAL (TOP_VAL_PWM/4) // adjust compare value amount

uint16_t ms_counter = 0;        // global variable to count general purpose timer overflow events

// TIMER ISR executes every ms
void TIMER0_IRQHandler(void) {
  TIMER_IntClear(TIMER0, TIMER_IF_OF); // Clear interrupt source
  ms_counter++;                        // Increment counter
}

int main() {

  uint32_t compare_val = 0;               // Initial PWM duty cycle is 0% (LED0 off)
  uint8_t inc = 1;                        // Increment = true

  CHIP_Init();                            // This function addresses some chip errata and should be called at the start of every EFM32 application (need em_system.c)
  CMU_HFRCOBandSet(cmuHFRCOBand_1MHz);    // Set HF oscillator to 1MHz and use as system source
  CMU_ClockEnable(cmuClock_GPIO, true);   // Start GPIO peripheral clock
  CMU_ClockEnable(cmuClock_TIMER0, true); // Start TIMER0 peripheral clock
  CMU_ClockEnable(cmuClock_TIMER3, true); // Start TIMER3 peripheral clock

  GPIO_PinModeSet(gpioPortE, 2, gpioModePushPull, 0); // set LED0 pin as push-pull output
  GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 1); // set LED0 pin as push-pull output

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

  //Setup Timer Configuration for general purpose use
  TIMER_Init_TypeDef timerGPInit =
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

  TIMER_TopSet(TIMER0, TOP_VAL_CU_TIMER);      // GP Timer period will be 1ms = 1kHz freq
  TIMER_TopSet(TIMER3, TOP_VAL_PWM);           // PWM period will be 1ms = 1kHz freq

  TIMER_CounterSet(TIMER0, 0);                 // Start counter at 0 (up-count mode)
  TIMER_CounterSet(TIMER3, 0);                 // Start counter at 0 (up-count mode)

  TIMER_CompareSet(TIMER3, 2, compare_val);    // Set CC2 compare value (0% duty)
  TIMER_CompareBufSet(TIMER3, 2, compare_val); // Set CC2 compare buffer value (0% duty)

  TIMER_IntEnable(TIMER0, TIMER_IF_OF);        // Enable Timer0 overflow interrupt
  NVIC_EnableIRQ(TIMER0_IRQn);                 // Enable Timer0 interrupt vector in NVIC

  TIMER_InitCC(TIMER3, 2, &timerCCInit);       // apply channel configuration to Timer3 channel 2
  TIMER3->ROUTE = (1 << 16) |(1 << 2);         // connect PWM output (timer3, channel 2) to PE2 (LED0). See EFM32GG990 datasheet for details.

  TIMER_Init(TIMER0, &timerGPInit);            // apply general purpose configuration to timer0
  TIMER_Init(TIMER3, &timerPWMInit);           // apply PWM configuration to timer3

  while(1) {
    if(ms_counter == UPDATE_PERIOD) {
      if(inc) {                                    // If increment = true
        compare_val += INC_VAL;                    // Increase the compare value
      }else{                                       // increment = false
        compare_val -= INC_VAL;                    // Decrease the compare value
      }
      TIMER_CompareBufSet(TIMER3, 2, compare_val); // Write new value to compare buffer
      GPIO_PinOutToggle(gpioPortE, 3);             // Toggle LED1
      ms_counter = 0;                              // Reset counter
    }
    if(compare_val > (TOP_VAL_PWM-1)) { inc = 0; } // If compare value is at max, start decrementing
    if(compare_val < 1) { inc = 1; }               // If compare value is at min, start incrementing
  }
}
