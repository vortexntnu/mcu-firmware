#include <string.h>

#include "uart.h"
#include "pwm.h"
#include "vortex_msg.h"
#include "efm32gg990f1024.h"
#include "crc.h"



int main() {

	CHIP_Init();

	initUart();
	initPwm();

	uint32_t i;

	bool received_msg = false;

	uint8_t receive_data[VORTEX_MSG_MAX_SIZE] = {0};
	uint8_t *receive_data_ptr = &receive_data[0];

	uint8_t msg_type = MSG_TYPE_NOTYPE;

	char hello_world[] = "\n\rHello World!\n\r";

	for (i = 0; i < strlen(hello_world); i++)
	{
		USART_Tx(UART, hello_world[i]);
	}

	GPIO_PinModeSet(gpioPortE, 3, gpioModePushPull, 0);
	GPIO_PinOutSet(gpioPortE, 3);

	uint16_t crc_checksum_calc, crc_checksum_received = 0;

	while (1)
	{
		switch (receive_vortex_msg(receive_data_ptr))
		{
			case MSG_STATE_RECEIVE_OK:
				crc_checksum_calc = crc_16(&receive_data[VORTEX_MSG_START_DATA_INDEX], MAX_PAYLOAD_SIZE);
				crc_checksum_received = (uint16_t)((receive_data[VORTEX_MSG_CRC_BYTE_INDEX] << 8) & 0xFF00)
										| (uint16_t)((receive_data[VORTEX_MSG_CRC_BYTE_INDEX+1]) & 0x00FF);

				if (crc_checksum_calc == crc_checksum_received)
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
				break;

			case MSG_STATE_RECEIVE_FAIL:
				msg_type = MSG_TYPE_NOTYPE;
				break;

			default:
				msg_type = MSG_TYPE_NOTYPE;
				if (received_msg == true)
				{
					send_vortex_msg(MSG_TYPE_NOACK);
					received_msg = false;
				}
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

				break;

			default:
				break;
		}

		/*for (i = 1; i <= receive_data[0]; i++)
		{
			USART_Tx(UART, receive_data[i]);
		}*/

		/*for (i = 0; i < VORTEX_MSG_MAX_SIZE; i++)
		{
			receive_data[i] = 0;
		}*/
	}

}
