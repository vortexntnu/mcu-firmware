#include <string.h>

#include "em_rmu.h"
#include "em_letimer.h"
#include "efm32gg942f1024.h"

#include "uart.h"
#include "pwm.h"
#include "crc.h"
#include "watchdog.h"
#include "rov_utilities.h"

int main()
{
	CHIP_Init();

	initUart();
	initPwm();
	initWdog();
	initLeTimer();

	GPIO_PinModeSet(LED1_PORT, LED1_PIN, gpioModePushPullDrive, 1);
	GPIO_PinModeSet(LED2_PORT, LED2_PIN, gpioModePushPullDrive, 1);

	unsigned long resetCause = RMU_ResetCauseGet();
	RMU_ResetCauseClear();

	char uart_msg[50] = {0};
	char* uart_msg_ptr = &uart_msg[0];

	if (resetCause & RMU_RSTCAUSE_WDOGRST)
	{
		strcpy(uart_msg_ptr, "Please pet the watchdog, start initialization...\n\r");
	}
	else
	{
		strcpy(uart_msg_ptr, "MCU reset normally, start initialization...\n\r");
	}

	USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));

	uint8_t receive_data[VORTEX_MSG_MAX_SIZE] = {0};
	uint8_t *receive_data_ptr = &receive_data[0];
	uint8_t msg_type = MSG_TYPE_NOTYPE;

	start_sequence();

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
					switch (receive_data[VORTEX_MSG_TYPE_INDEX])
					{
						case MSG_TYPE_HEARTBEAT:
							msg_type = MSG_TYPE_HEARTBEAT;
							break;

						case MSG_TYPE_ARM:
							msg_type = MSG_TYPE_ARM;
							break;

						case MSG_TYPE_DISARM:
							msg_type = MSG_TYPE_DISARM;
							break;

						default:
							msg_type = MSG_TYPE_NOACK;
							send_vortex_msg(MSG_TYPE_NOACK);
							break;
					}
				}
				break;

			case MSG_STATE_RECEIVE_FAIL:
				msg_type = MSG_TYPE_NOTYPE;
				break;

			default:
				msg_type = MSG_TYPE_NOTYPE;
				break;
		}

		switch (msg_type)
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

			case MSG_TYPE_ARM:
				arm_sequence();
				break;

			case MSG_TYPE_DISARM:
				disarm_sequence();
				break;

			case MSG_TYPE_HEARTBEAT:
				strcpy(uart_msg_ptr, "HEARTBEAT RECEIVED, PETTING WATCHDOG\n\r");
				USART_PutData((uint8_t*)uart_msg_ptr, strlen(uart_msg));
				WDOGn_Feed(WDOG);
				break;

			default:
				break;
		}
	}
}
