#include <string.h>

#include "em_rmu.h"
#include "em_letimer.h"
#include "efm32gg990f1024.h"

#include "uart.h"
#include "pwm.h"
#include "vortex_msg.h"
#include "crc.h"
#include "watchdog.h"

#define LETIMER_MS 2000 // how often LETIMER0_IRQHandler triggers in milliseconds

bool start_sequence_finished = false;
bool is_start_sequence_finished();

bool crc_passed(uint8_t * receive_data);
void start_sequence(void);
void initleTimer(void);

int main()
{
	CHIP_Init();
	initUart();
	initPwm();
	initWdog();

	unsigned long resetCause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();

	char startup_msg[50] = {0};
	char* startup_msg_ptr = &startup_msg[0];

	if (resetCause & RMU_RSTCAUSE_WDOGRST)
	{
		strcpy(startup_msg_ptr, "MCU reset by watchdog, start initialization...\n\r");
	}
	else
	{
		strcpy(startup_msg_ptr, "$ MCU reset normally, start initialization... @\n\r");
	}

	USART_PutData((uint8_t*)startup_msg_ptr, strlen(startup_msg));

	uint8_t receive_data[VORTEX_MSG_MAX_SIZE] = {0};
	uint8_t *receive_data_ptr = &receive_data[0];
	uint8_t msg_type = MSG_TYPE_NOTYPE;

	start_sequence();


	strcpy(&startup_msg[0], "$ MCU initialization finished... @\n\r");
	USART_PutData((uint8_t*)startup_msg_ptr, strlen(startup_msg));

	while (1)
	{
		switch (receive_vortex_msg(receive_data_ptr))
		{
			case MSG_STATE_RECEIVE_OK:
				if (crc_passed(&receive_data[0]) == true)
				{
					send_vortex_msg(MSG_TYPE_ACK);
					msg_type = receive_data[VORTEX_MSG_TYPE_INDEX];
				}
				else
				{
					send_vortex_msg(MSG_TYPE_NOACK);
					msg_type = MSG_TYPE_NOACK;
				}

				WDOGn_Feed(WDOG);
				break;

			case MSG_STATE_RECEIVE_FAIL:
				msg_type = MSG_TYPE_NOTYPE;
				break;

			default:
				msg_type = MSG_TYPE_NOTYPE;
				break;
		}

		switch(msg_type)
		{
			case MSG_TYPE_NOTYPE:
				// check how long since last heartbeat?
				break;

			case MSG_TYPE_THRUSTER:
				if (update_thruster_pwm(&receive_data[VORTEX_MSG_START_DATA_INDEX]) != PWM_UPDATE_OK)
				{
					//error handling
				}
				break;

			case MSG_TYPE_LED:
				if (update_led_pwm(&receive_data[VORTEX_MSG_START_DATA_INDEX]) != PWM_UPDATE_OK)
				{
					//error handling
				}
				break;

			case MSG_TYPE_HEARTBEAT:
				WDOGn_Feed(WDOG);
				break;

			default:
				break;
		}
	}
}


bool crc_passed(uint8_t * receive_data)
{
	uint16_t crc_checksum_calc = 0;
	uint16_t crc_checksum_received = 1;

	crc_checksum_calc = crc_16(&receive_data[VORTEX_MSG_START_DATA_INDEX], MAX_PAYLOAD_SIZE);
	crc_checksum_received = (uint16_t)((receive_data[VORTEX_MSG_CRC_BYTE_INDEX] << 8) & 0xFF00)
							| (uint16_t)((receive_data[VORTEX_MSG_CRC_BYTE_INDEX+1]) & 0x00FF);

	if (crc_checksum_calc == crc_checksum_received)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void start_sequence(void)
{
	initleTimer();

	// startup sequence for leds, thrusters are already being initialized

	GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
	GPIO_PinOutSet(gpioPortE, 3);

	while (!is_start_sequence_finished())
	{
		// wait for LETIMER_MS milliseconds
		WDOGn_Feed(WDOG);
	}
}

bool is_start_sequence_finished()
{
	return start_sequence_finished;
}


void LETIMER0_IRQHandler(void)
{
	// Clear LETIMER0 underflow interrupt flag
	start_sequence_finished = true;
	LETIMER_IntClear(LETIMER0, LETIMER_IF_UF);
	LETIMER0->CNT = LETIMER_MS;
	NVIC_DisableIRQ(LETIMER0_IRQn);
}


void initleTimer(void)
{
	const LETIMER_Init_TypeDef letimerInit =
	{
	  .enable         = false,             // Start counting when init completed */
	  .debugRun       = true,            // Counter shall not keep running during debug halt */
	  .comp0Top       = true,             // Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
	  .bufTop         = false,            // Don't load COMP1 into COMP0 when REP0 reaches 0 */
	  .out0Pol        = 0,                // Idle value for output 0 */
	  .out1Pol        = 0,                // Idle value for output 1 */
	  .ufoa0          = letimerUFOAToggle,// Toggle output on output 0 */
	  .ufoa1          = letimerUFOAToggle,// Toggle output on output 1 */
	  .repMode        = letimerRepeatFree // Count until stopped */
	};

	CMU_ClockEnable(cmuClock_CORELE, true);

	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);

	// The clock source of LETIMER0
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

	CMU_ClockEnable(cmuClock_LETIMER0, true);

	// Time out value configuration
	LETIMER_CompareSet(LETIMER0, 0, LETIMER_MS);

	// Initializing LETIMER0
	LETIMER_Init(LETIMER0, &letimerInit);
	LETIMER_Enable(LETIMER0, true);

	LETIMER_IntClear(LETIMER0, _LETIMER_IF_MASK);
	LETIMER_IntEnable(LETIMER0, LETIMER_IF_UF);
	NVIC_ClearPendingIRQ(LETIMER0_IRQn);
	NVIC_EnableIRQ(LETIMER0_IRQn);

}

