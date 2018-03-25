#include <string.h>

#include "uart.h"
#include "pwm.h"
#include "vortex_msg.h"
#include "efm32gg990f1024.h"
#include "crc.h"
#include "watchdog.h"

bool crc_passed(uint8_t * receive_data);

int main() {

	CHIP_Init();

	initUart();
	initPwm();
	initWdog();

	uint32_t i;

	bool received_msg = false;

	uint8_t receive_data[VORTEX_MSG_MAX_SIZE] = {0};
	uint8_t *receive_data_ptr = &receive_data[0];

	uint8_t msg_type = MSG_TYPE_NOTYPE;

	char startup_msg[] = "$START UP EFM32GG@";

	for (i = 0; i < strlen(startup_msg); i++)
	{
		USART_Tx(UART, startup_msg[i]);
	}

	GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
	GPIO_PinOutSet(gpioPortE, 3);

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
				received_msg = true;
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
